/*
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "redis.h"
#include "cluster.h"

#include <signal.h>
#include <ctype.h>

void slotToKeyAdd(robj *key);
void slotToKeyDel(robj *key);
void slotToKeyFlush(void);

/*-----------------------------------------------------------------------------
 * C-level DB API
 *----------------------------------------------------------------------------*/

/*
 * 从数据库 db 中取出键 key 的值（对象）
 *
 * 如果 key 的值存在，那么返回该值；否则，返回 NULL 。
 */ //获取key对应的val
robj *lookupKey(redisDb *db, robj *key) {

    // 查找键空间
    dictEntry *de = dictFind(db->dict,key->ptr);

    // 节点存在
    if (de) {
        

        // 取出值
        robj *val = dictGetVal(de);

        /* Update the access time for the ageing algorithm.
         * Don't do it if we have a saving child, as this will trigger
         * a copy on write madness. */
        // 更新时间信息（只在不存在子进程时执行，防止破坏 copy-on-write 机制）
        if (server.rdb_child_pid == -1 && server.aof_child_pid == -1)
            val->lru = LRU_CLOCK();

        // 返回值
        return val;
    } else {

        // 节点不存在

        return NULL;
    }
}

/*
 * 为执行读取操作而取出键 key 在数据库 db 中的值。
 *
 * 并根据是否成功找到值，更新服务器的命中/不命中信息。
 *
 * 找到时返回值对象，没找到返回 NULL 。
 */
robj *lookupKeyRead(redisDb *db, robj *key) {
    robj *val;

    // 检查 key 释放已经过期
    expireIfNeeded(db,key);

    // 从数据库中取出键的值
    val = lookupKey(db,key);

    // 更新命中/不命中信息
    if (val == NULL)
        server.stat_keyspace_misses++;
    else
        server.stat_keyspace_hits++;

    // 返回值
    return val;
}

/*
 * 为执行写入操作而取出键 key 在数据库 db 中的值。
 *
 * 和 lookupKeyRead 不同，这个函数不会更新服务器的命中/不命中信息。
 *
 * 找到时返回值对象，没找到返回 NULL 。
 */
robj *lookupKeyWrite(redisDb *db, robj *key) {

    // 删除过期键
    expireIfNeeded(db,key);

    // 查找并返回 key 的值对象
    return lookupKey(db,key);
}

/*
 * 为执行读取操作而从数据库中查找返回 key 的值。
 *
 * 如果 key 存在，那么返回 key 的值对象。
 *
 * 如果 key 不存在，那么向客户端发送 reply 参数中的信息，并返回 NULL 。
 */
robj *lookupKeyReadOrReply(redisClient *c, robj *key, robj *reply) {

    // 查找
    robj *o = lookupKeyRead(c->db, key);

    // 决定是否发送信息
    if (!o) addReply(c,reply);

    return o;
}

/*
 * 为执行写入操作而从数据库中查找返回 key 的值。
 *
 * 如果 key 存在，那么返回 key 的值对象。
 *
 * 如果 key 不存在，那么向客户端发送 reply 参数中的信息，并返回 NULL 。
 */

robj *lookupKeyWriteOrReply(redisClient *c, robj *key, robj *reply) {

    robj *o = lookupKeyWrite(c->db, key);

    if (!o) addReply(c,reply);

    return o;
}

/* Add the key to the DB. It's up to the caller to increment the reference
 * counter of the value if needed.
 *
 * 尝试将键值对 key 和 val 添加到数据库中。
 *
 * 调用者负责对 key 和 val 的引用计数进行增加。
 *
 * The program is aborted if the key already exists. 
 *
 * 程序在键已经存在时会停止。
 */
void dbAdd(redisDb *db, robj *key, robj *val) {

    // 复制键名
    sds copy = sdsdup(key->ptr);

    // 尝试添加键值对
    int retval = dictAdd(db->dict, copy, val);

    // 如果键已经存在，那么停止
    redisAssertWithInfo(NULL,key,retval == REDIS_OK);

    // 如果开启了集群模式，那么将键保存到槽里面
    if (server.cluster_enabled) slotToKeyAdd(key);
 }

/* Overwrite an existing key with a new value. Incrementing the reference
 * count of the new value is up to the caller.
 *
 * 为已存在的键关联一个新值。
 *
 * 调用者负责对新值 val 的引用计数进行增加。
 *
 * This function does not modify the expire time of the existing key.
 *
 * 这个函数不会修改键的过期时间。
 *
 * The program is aborted if the key was not already present. 
 *
 * 如果键不存在，那么函数停止。
 */
void dbOverwrite(redisDb *db, robj *key, robj *val) {
    dictEntry *de = dictFind(db->dict,key->ptr);
    
    // 节点必须存在，否则中止
    redisAssertWithInfo(NULL,key,de != NULL);

    // 覆写旧值
    dictReplace(db->dict, key->ptr, val);
}

/* High level Set operation. This function can be used in order to set
 * a key, whatever it was existing or not, to a new object.
 *
 * 高层次的 SET 操作函数。
 *
 * 这个函数可以在不管键 key 是否存在的情况下，将它和 val 关联起来。
 *
 * 1) The ref count of the value object is incremented.
 *    值对象的引用计数会被增加
 *
 * 2) clients WATCHing for the destination key notified.
 *    监视键 key 的客户端会收到键已经被修改的通知
 *
 * 3) The expire time of the key is reset (the key is made persistent). 
 *    键的过期时间会被移除（键变为持久的）
 */
void setKey(redisDb *db, robj *key, robj *val) {

    // 添加或覆写数据库中的键值对
    if (lookupKeyWrite(db,key) == NULL) {
        dbAdd(db,key,val);
    } else {
        dbOverwrite(db,key,val);
    }

    incrRefCount(val);

    // 移除键的过期时间
    removeExpire(db,key);

    // 发送键修改通知
    signalModifiedKey(db,key);
}

/*
 * 检查键 key 是否存在于数据库中，存在返回 1 ，不存在返回 0 。
 */
int dbExists(redisDb *db, robj *key) {
    return dictFind(db->dict,key->ptr) != NULL;
}

/* Return a random key, in form of a Redis object.
 * If there are no keys, NULL is returned.
 *
 * 随机从数据库中取出一个键，并以字符串对象的方式返回这个键。
 *
 * 如果数据库为空，那么返回 NULL 。
 *
 * The function makes sure to return keys not already expired. 
 *
 * 这个函数保证被返回的键都是未过期的。
 */
robj *dbRandomKey(redisDb *db) {
    dictEntry *de;

    while(1) {
        sds key;
        robj *keyobj;

        // 从键空间中随机取出一个键节点
        de = dictGetRandomKey(db->dict);

        // 数据库为空
        if (de == NULL) return NULL;

        // 取出键
        key = dictGetKey(de);
        // 为键创建一个字符串对象，对象的值为键的名字
        keyobj = createStringObject(key,sdslen(key));
        // 检查键是否带有过期时间
        if (dictFind(db->expires,key)) {
            // 如果键已经过期，那么将它删除，并继续随机下个键
            if (expireIfNeeded(db,keyobj)) {
                decrRefCount(keyobj);
                continue; /* search for another key. This expired. */
            }
        }

        // 返回被随机到的键（的名字）
        return keyobj;
    }
}

/* Delete a key, value, and associated expiration entry if any, from the DB 
 *
 * 从数据库中删除给定的键，键的值，以及键的过期时间。
 *
 * 删除成功返回 1 ，因为键不存在而导致删除失败时，返回 0 。
 */
int dbDelete(redisDb *db, robj *key) {

    /* Deleting an entry from the expires dict will not free the sds of
     * the key, because it is shared with the main dictionary. */
    // 删除键的过期时间
    if (dictSize(db->expires) > 0) dictDelete(db->expires,key->ptr);

    // 删除键值对
    if (dictDelete(db->dict,key->ptr) == DICT_OK) {
        // 如果开启了集群模式，那么从槽中删除给定的键
        if (server.cluster_enabled) slotToKeyDel(key);
        return 1;
    } else {
        // 键不存在
        return 0;
    }
}

/* Prepare the string object stored at 'key' to be modified destructively
 * to implement commands like SETBIT or APPEND.
 *
 * An object is usually ready to be modified unless one of the two conditions
 * are true:
 *
 * 1) The object 'o' is shared (refcount > 1), we don't want to affect
 *    other users.
 * 2) The object encoding is not "RAW".
 *
 * If the object is found in one of the above conditions (or both) by the
 * function, an unshared / not-encoded copy of the string object is stored
 * at 'key' in the specified 'db'. Otherwise the object 'o' itself is
 * returned.
 *
 * USAGE:
 *
 * The object 'o' is what the caller already obtained by looking up 'key'
 * in 'db', the usage pattern looks like this:
 *
 * o = lookupKeyWrite(db,key);
 * if (checkType(c,o,REDIS_STRING)) return;
 * o = dbUnshareStringValue(db,key,o);
 *
 * At this point the caller is ready to modify the object, for example
 * using an sdscat() call to append some data, or anything else.
 */
robj *dbUnshareStringValue(redisDb *db, robj *key, robj *o) {
    redisAssert(o->type == REDIS_STRING);
    if (o->refcount != 1 || o->encoding != REDIS_ENCODING_RAW) {
        robj *decoded = getDecodedObject(o);
        o = createRawStringObject(decoded->ptr, sdslen(decoded->ptr));
        decrRefCount(decoded);
        dbOverwrite(db,key,o);
    }
    return o;
}

/*
 * 清空服务器的所有数据。
 */ //把内存中的所有dbid(select id中的id)数据库key-value清楚，如果为-1，表示把说有数据库清除
long long emptyDb(void(callback)(void*)) {
    int j;
    long long removed = 0;

    // 清空所有数据库
    for (j = 0; j < server.dbnum; j++) {

        // 记录被删除键的数量
        removed += dictSize(server.db[j].dict);

        // 删除所有键值对
        dictEmpty(server.db[j].dict,callback);
        // 删除所有键的过期时间
        dictEmpty(server.db[j].expires,callback);
    }

    // 如果开启了集群模式，那么还要移除槽记录
    if (server.cluster_enabled) slotToKeyFlush();

    // 返回键的数量
    return removed;
}

/*
 * 将客户端的目标数据库切换为 id 所指定的数据库
 */
int selectDb(redisClient *c, int id) {

    // 确保 id 在正确范围内
    if (id < 0 || id >= server.dbnum)
        return REDIS_ERR;

    // 切换数据库（更新指针）
    c->db = &server.db[id];

    return REDIS_OK;
}

/*-----------------------------------------------------------------------------
 * Hooks for key space changes.
 *
 * 键空间改动的钩子。
 *
 * Every time a key in the database is modified the function
 * signalModifiedKey() is called.
 *
 * 每当数据库中的键被改动时， signalModifiedKey() 函数都会被调用。
 *
 * Every time a DB is flushed the function signalFlushDb() is called.
 *
 * 每当一个数据库被清空时， signalFlushDb() 都会被调用。
 *----------------------------------------------------------------------------*/

void signalModifiedKey(redisDb *db, robj *key) {
    touchWatchedKey(db,key);
}

void signalFlushedDb(int dbid) {
    touchWatchedKeysOnFlush(dbid);
}

/*-----------------------------------------------------------------------------
 * Type agnostic commands operating on the key space
 *
 * 与类型无关的数据库操作。
 *----------------------------------------------------------------------------*/

/*
 * 清空客户端指定的数据库
 */
void flushdbCommand(redisClient *c) {

    server.dirty += dictSize(c->db->dict);

    // 发送通知
    signalFlushedDb(c->db->id);

    // 清空指定数据库中的 dict 和 expires 字典
    dictEmpty(c->db->dict,NULL);
    dictEmpty(c->db->expires,NULL);

    // 如果开启了集群模式，那么还要移除槽记录
    if (server.cluster_enabled) slotToKeyFlush();

    addReply(c,shared.ok);
}

/*
 * 清空服务器中的所有数据库
 */
void flushallCommand(redisClient *c) {

    // 发送通知
    signalFlushedDb(-1);

    // 清空所有数据库
    server.dirty += emptyDb(NULL);
    addReply(c,shared.ok);

    // 如果正在保存新的 RDB ，那么取消保存操作
    if (server.rdb_child_pid != -1) {
        kill(server.rdb_child_pid,SIGUSR1);
        rdbRemoveTempFile(server.rdb_child_pid);
    }

    // 更新 RDB 文件
    if (server.saveparamslen > 0) {
        /* Normally rdbSave() will reset dirty, but we don't want this here
         * as otherwise FLUSHALL will not be replicated nor put into the AOF. */
        // rdbSave() 会清空服务器的 dirty 属性
        // 但为了确保 FLUSHALL 命令会被正常传播，
        // 程序需要保存并在 rdbSave() 调用之后还原服务器的 dirty 属性
        int saved_dirty = server.dirty;

        rdbSave(server.rdb_filename);

        server.dirty = saved_dirty;
    }

    server.dirty++;
}

/*
DEL

DEL key [key ...]

删除给定的一个或多个 key 。

不存在的 key 会被忽略。
可用版本：>= 1.0.0时间复杂度：

O(N)， N 为被删除的 key 的数量。


删除单个字符串类型的 key ，时间复杂度为O(1)。

删除单个列表、集合、有序集合或哈希表类型的 key ，时间复杂度为O(M)， M 为以上数据结构内的元素数量。
返回值：被删除 key 的数量。

#  删除单个 key

redis> SET name huangz
OK

redis> DEL name
(integer) 1


# 删除一个不存在的 key

redis> EXISTS phone
(integer) 0

redis> DEL phone # 失败，没有 key 被删除
(integer) 0


# 同时删除多个 key

redis> SET name "redis"
OK

redis> SET type "key-value store"
OK

redis> SET website "redis.com"
OK

redis> DEL name type website
(integer) 3


*/
void delCommand(redisClient *c) {
    int deleted = 0, j;

    // 遍历所有输入键
    for (j = 1; j < c->argc; j++) {

        // 先删除过期的键
        expireIfNeeded(c->db,c->argv[j]);

        // 尝试删除键
        if (dbDelete(c->db,c->argv[j])) {

            // 删除键成功，发送通知

            signalModifiedKey(c->db,c->argv[j]);
            notifyKeyspaceEvent(REDIS_NOTIFY_GENERIC,
                "del",c->argv[j],c->db->id);

            server.dirty++;

            // 成功删除才增加 deleted 计数器的值
            deleted++;
        }
    }

    // 返回被删除键的数量
    addReplyLongLong(c,deleted);
}

/*
EXISTS key

检查给定 key 是否存在。
可用版本：>= 1.0.0时间复杂度：O(1)返回值：若 key 存在，返回 1 ，否则返回 0 。

redis> SET db "redis"
OK

redis> EXISTS db
(integer) 1

redis> DEL db
(integer) 1

redis> EXISTS db
(integer) 0

*/
void existsCommand(redisClient *c) {

    // 检查键是否已经过期，如果已过期的话，那么将它删除
    // 这可以避免已过期的键被误认为存在
    expireIfNeeded(c->db,c->argv[1]);

    // 在数据库中查找
    if (dbExists(c->db,c->argv[1])) {
        addReply(c, shared.cone);
    } else {
        addReply(c, shared.czero);
    }
}

void selectCommand(redisClient *c) {
    long id;

    // 不合法的数据库号码
    if (getLongFromObjectOrReply(c, c->argv[1], &id,
        "invalid DB index") != REDIS_OK)
        return;

    if (server.cluster_enabled && id != 0) {
        addReplyError(c,"SELECT is not allowed in cluster mode");
        return;
    }

    // 切换数据库
    if (selectDb(c,id) == REDIS_ERR) {
        addReplyError(c,"invalid DB index");
    } else {
        addReply(c,shared.ok);
    }
}

/*
RANDOMKEY

从当前数据库中随机返回(不删除)一个 key 。
可用版本：>= 1.0.0时间复杂度：O(1)返回值：

当数据库不为空时，返回一个 key 。

当数据库为空时，返回 nil 。


# 数据库不为空

redis> MSET fruit "apple" drink "beer" food "cookies"   # 设置多个 key
OK

redis> RANDOMKEY
"fruit"

redis> RANDOMKEY
"food"

redis> KEYS *    # 查看数据库内所有key，证明 RANDOMKEY 并不删除 key
1) "food"
2) "drink"
3) "fruit"


# 数据库为空

redis> FLUSHDB  # 删除当前数据库所有 key
OK

redis> RANDOMKEY
(nil)


*/
void randomkeyCommand(redisClient *c) {
    robj *key;

    // 随机返回键
    if ((key = dbRandomKey(c->db)) == NULL) {
        addReply(c,shared.nullbulk);
        return;
    }

    addReplyBulk(c,key);
    decrRefCount(key);
}

/*
KEYS pattern

查找所有符合给定模式 pattern 的 key 。


KEYS * 匹配数据库中所有 key 。

KEYS h?llo 匹配 hello ， hallo 和 hxllo 等。

KEYS h*llo 匹配 hllo 和 heeeeello 等。

KEYS h[ae]llo 匹配 hello 和 hallo ，但不匹配 hillo 。

特殊符号用 \ 隔开



KEYS 的速度非常快，但在一个大的数据库中使用它仍然可能造成性能问题，如果你需要从一个数据集中查找特定的 key ，你最好还是用 Redis 的集合结构(set)来代替。
 可用版本：>= 1.0.0时间复杂度：O(N)， N 为数据库中 key 的数量。返回值：符合给定模式的 key 列表。

redis> MSET one 1 two 2 three 3 four 4  # 一次设置 4 个 key
OK

redis> KEYS *o*
1) "four"
2) "two"
3) "one"

redis> KEYS t??
1) "two"

redis> KEYS t[w]*
1) "two"

redis> KEYS *  # 匹配数据库内所有 key
1) "four"
2) "three"
3) "two"
4) "one"


*/
void keysCommand(redisClient *c) {
    dictIterator *di;
    dictEntry *de;

    // 模式
    sds pattern = c->argv[1]->ptr;

    int plen = sdslen(pattern), allkeys;
    unsigned long numkeys = 0;
    void *replylen = addDeferredMultiBulkLength(c);

    // 遍历整个数据库，返回（名字）和模式匹配的键
    di = dictGetSafeIterator(c->db->dict);
    allkeys = (pattern[0] == '*' && pattern[1] == '\0');
    while((de = dictNext(di)) != NULL) {
        sds key = dictGetKey(de);
        robj *keyobj;

        // 将键名和模式进行比对
        if (allkeys || stringmatchlen(pattern,plen,key,sdslen(key),0)) {

            // 创建一个保存键名字的字符串对象
            keyobj = createStringObject(key,sdslen(key));

            // 删除已过期键
            if (expireIfNeeded(c->db,keyobj) == 0) {
                addReplyBulk(c,keyobj);
                numkeys++;
            }

            decrRefCount(keyobj);
        }
    }
    dictReleaseIterator(di);

    setDeferredMultiBulkLength(c,replylen,numkeys);
}

/* This callback is used by scanGenericCommand in order to collect elements
 * returned by the dictionary iterator into a list. */
void scanCallback(void *privdata, const dictEntry *de) {
    void **pd = (void**) privdata;
    list *keys = pd[0];
    robj *o = pd[1];
    robj *key, *val = NULL;

    if (o == NULL) {
        sds sdskey = dictGetKey(de);
        key = createStringObject(sdskey, sdslen(sdskey));
    } else if (o->type == REDIS_SET) {
        key = dictGetKey(de);
        incrRefCount(key);
    } else if (o->type == REDIS_HASH) {
        key = dictGetKey(de);
        incrRefCount(key);
        val = dictGetVal(de);
        incrRefCount(val);
    } else if (o->type == REDIS_ZSET) {
        key = dictGetKey(de);
        incrRefCount(key);
        val = createStringObjectFromLongDouble(*(double*)dictGetVal(de));
    } else {
        redisPanic("Type not handled in SCAN callback.");
    }

    listAddNodeTail(keys, key);
    if (val) listAddNodeTail(keys, val);
}

/* Try to parse a SCAN cursor stored at object 'o':
 * if the cursor is valid, store it as unsigned integer into *cursor and
 * returns REDIS_OK. Otherwise return REDIS_ERR and send an error to the
 * client. */
int parseScanCursorOrReply(redisClient *c, robj *o, unsigned long *cursor) {
    char *eptr;

    /* Use strtoul() because we need an *unsigned* long, so
     * getLongLongFromObject() does not cover the whole cursor space. */
    errno = 0;
    *cursor = strtoul(o->ptr, &eptr, 10);
    if (isspace(((char*)o->ptr)[0]) || eptr[0] != '\0' || errno == ERANGE)
    {
        addReplyError(c, "invalid cursor");
        return REDIS_ERR;
    }
    return REDIS_OK;
}

/* This command implements SCAN, HSCAN and SSCAN commands.
 *
 * 这是 SCAN 、 HSCAN 、 SSCAN 命令的实现函数。
 *
 * If object 'o' is passed, then it must be a Hash or Set object, otherwise
 * if 'o' is NULL the command will operate on the dictionary associated with
 * the current database.
 *
 * 如果给定了对象 o ，那么它必须是一个哈希对象或者集合对象，
 * 如果 o 为 NULL 的话，函数将使用当前数据库作为迭代对象。
 *
 * When 'o' is not NULL the function assumes that the first argument in
 * the client arguments vector is a key so it skips it before iterating
 * in order to parse options.
 *
 * 如果参数 o 不为 NULL ，那么说明它是一个键对象，函数将跳过这些键对象，
 * 对给定的命令选项进行分析（parse）。
 *
 * In the case of a Hash object the function returns both the field and value
 * of every element on the Hash. 
 *
 * 如果被迭代的是哈希对象，那么函数返回的是键值对。
 */
void scanGenericCommand(redisClient *c, robj *o, unsigned long cursor) {
    int rv;
    int i, j;
    char buf[REDIS_LONGSTR_SIZE];
    list *keys = listCreate();
    listNode *node, *nextnode;
    long count = 10;
    sds pat;
    int patlen, use_pattern = 0;
    dict *ht;

    /* Object must be NULL (to iterate keys names), or the type of the object
     * must be Set, Sorted Set, or Hash. */
    // 输入类型检查
    redisAssert(o == NULL || o->type == REDIS_SET || o->type == REDIS_HASH ||
                o->type == REDIS_ZSET);

    /* Set i to the first option argument. The previous one is the cursor. */
    // 设置第一个选项参数的索引位置
    // 0    1      2      3  
    // SCAN OPTION <op_arg>         SCAN 命令的选项值从索引 2 开始
    // HSCAN <key> OPTION <op_arg>  而其他 *SCAN 命令的选项值从索引 3 开始
    i = (o == NULL) ? 2 : 3; /* Skip the key argument if needed. */

    /* Step 1: Parse options. */
    // 分析选项参数
    while (i < c->argc) {
        j = c->argc - i;

        // COUNT <number>
        if (!strcasecmp(c->argv[i]->ptr, "count") && j >= 2) {
            if (getLongFromObjectOrReply(c, c->argv[i+1], &count, NULL)
                != REDIS_OK)
            {
                goto cleanup;
            }

            if (count < 1) {
                addReply(c,shared.syntaxerr);
                goto cleanup;
            }

            i += 2;

        // MATCH <pattern>
        } else if (!strcasecmp(c->argv[i]->ptr, "match") && j >= 2) {
            pat = c->argv[i+1]->ptr;
            patlen = sdslen(pat);

            /* The pattern always matches if it is exactly "*", so it is
             * equivalent to disabling it. */
            use_pattern = !(pat[0] == '*' && patlen == 1);

            i += 2;

        // error
        } else {
            addReply(c,shared.syntaxerr);
            goto cleanup;
        }
    }

    /* Step 2: Iterate the collection.
     *
     * Note that if the object is encoded with a ziplist, intset, or any other
     * representation that is not a hash table, we are sure that it is also
     * composed of a small number of elements. So to avoid taking state we
     * just return everything inside the object in a single call, setting the
     * cursor to zero to signal the end of the iteration. */
     // 如果对象的底层实现为 ziplist 、intset 而不是哈希表，
     // 那么这些对象应该只包含了少量元素，
     // 为了保持不让服务器记录迭代状态的设计
     // 我们将 ziplist 或者 intset 里面的所有元素都一次返回给调用者
     // 并向调用者返回游标（cursor） 0

    /* Handle the case of a hash table. */
    ht = NULL;
    if (o == NULL) {
        // 迭代目标为数据库
        ht = c->db->dict;
    } else if (o->type == REDIS_SET && o->encoding == REDIS_ENCODING_HT) {
        // 迭代目标为 HT 编码的集合
        ht = o->ptr;
    } else if (o->type == REDIS_HASH && o->encoding == REDIS_ENCODING_HT) {
        // 迭代目标为 HT 编码的哈希
        ht = o->ptr;
        count *= 2; /* We return key / value for this type. */
    } else if (o->type == REDIS_ZSET && o->encoding == REDIS_ENCODING_SKIPLIST) {
        // 迭代目标为 HT 编码的跳跃表
        zset *zs = o->ptr;
        ht = zs->dict;
        count *= 2; /* We return key / value for this type. */
    }

    if (ht) {
        void *privdata[2];

        /* We pass two pointers to the callback: the list to which it will
         * add new elements, and the object containing the dictionary so that
         * it is possible to fetch more data in a type-dependent way. */
        // 我们向回调函数传入两个指针：
        // 一个是用于记录被迭代元素的列表
        // 另一个是字典对象
        // 从而实现类型无关的数据提取操作
        privdata[0] = keys;
        privdata[1] = o;
        do {
            cursor = dictScan(ht, cursor, scanCallback, privdata);
        } while (cursor && listLength(keys) < count);
    } else if (o->type == REDIS_SET) {
        int pos = 0;
        int64_t ll;

        while(intsetGet(o->ptr,pos++,&ll))
            listAddNodeTail(keys,createStringObjectFromLongLong(ll));
        cursor = 0;
    } else if (o->type == REDIS_HASH || o->type == REDIS_ZSET) {
        unsigned char *p = ziplistIndex(o->ptr,0);
        unsigned char *vstr;
        unsigned int vlen;
        long long vll;

        while(p) {
            ziplistGet(p,&vstr,&vlen,&vll);
            listAddNodeTail(keys,
                (vstr != NULL) ? createStringObject((char*)vstr,vlen) :
                                 createStringObjectFromLongLong(vll));
            p = ziplistNext(o->ptr,p);
        }
        cursor = 0;
    } else {
        redisPanic("Not handled encoding in SCAN.");
    }

    /* Step 3: Filter elements. */
    node = listFirst(keys);
    while (node) {
        robj *kobj = listNodeValue(node);
        nextnode = listNextNode(node);
        int filter = 0;

        /* Filter element if it does not match the pattern. */
        if (!filter && use_pattern) {
            if (sdsEncodedObject(kobj)) {
                if (!stringmatchlen(pat, patlen, kobj->ptr, sdslen(kobj->ptr), 0))
                    filter = 1;
            } else {
                char buf[REDIS_LONGSTR_SIZE];
                int len;

                redisAssert(kobj->encoding == REDIS_ENCODING_INT);
                len = ll2string(buf,sizeof(buf),(long)kobj->ptr);
                if (!stringmatchlen(pat, patlen, buf, len, 0)) filter = 1;
            }
        }

        /* Filter element if it is an expired key. */
        if (!filter && o == NULL && expireIfNeeded(c->db, kobj)) filter = 1;

        /* Remove the element and its associted value if needed. */
        if (filter) {
            decrRefCount(kobj);
            listDelNode(keys, node);
        }

        /* If this is a hash or a sorted set, we have a flat list of
         * key-value elements, so if this element was filtered, remove the
         * value, or skip it if it was not filtered: we only match keys. */
        if (o && (o->type == REDIS_ZSET || o->type == REDIS_HASH)) {
            node = nextnode;
            nextnode = listNextNode(node);
            if (filter) {
                kobj = listNodeValue(node);
                decrRefCount(kobj);
                listDelNode(keys, node);
            }
        }
        node = nextnode;
    }

    /* Step 4: Reply to the client. */
    addReplyMultiBulkLen(c, 2);
    rv = snprintf(buf, sizeof(buf), "%lu", cursor);
    redisAssert(rv < sizeof(buf));
    addReplyBulkCBuffer(c, buf, rv);

    addReplyMultiBulkLen(c, listLength(keys));
    while ((node = listFirst(keys)) != NULL) {
        robj *kobj = listNodeValue(node);
        addReplyBulk(c, kobj);
        decrRefCount(kobj);
        listDelNode(keys, node);
    }

cleanup:
    listSetFreeMethod(keys,decrRefCountVoid);
    listRelease(keys);
}

/* The SCAN command completely relies on scanGenericCommand. */
void scanCommand(redisClient *c) {
    unsigned long cursor;
    if (parseScanCursorOrReply(c,c->argv[1],&cursor) == REDIS_ERR) return;
    scanGenericCommand(c,NULL,cursor);
}

void dbsizeCommand(redisClient *c) {
    addReplyLongLong(c,dictSize(c->db->dict));
}

void lastsaveCommand(redisClient *c) {
    addReplyLongLong(c,server.lastsave);
}

/*
TYPE key

返回 key 所储存的值的类型。
可用版本：>= 1.0.0时间复杂度：O(1)返回值：

none (key不存在)

string (字符串)

list (列表)

set (集合)

zset (有序集)

hash (哈希表)


# 字符串

redis> SET weather "sunny"
OK

redis> TYPE weather
string


# 列表

redis> LPUSH book_list "programming in scala"
(integer) 1

redis> TYPE book_list
list


# 集合

redis> SADD pat "dog"
(integer) 1

redis> TYPE pat
set


*/
void typeCommand(redisClient *c) {
    robj *o;
    char *type;

    o = lookupKeyRead(c->db,c->argv[1]);

    if (o == NULL) {
        type = "none";
    } else {
        switch(o->type) {
        case REDIS_STRING: type = "string"; break;
        case REDIS_LIST: type = "list"; break;
        case REDIS_SET: type = "set"; break;
        case REDIS_ZSET: type = "zset"; break;
        case REDIS_HASH: type = "hash"; break;
        default: type = "unknown"; break;
        }
    }

    addReplyStatus(c,type);
}

void shutdownCommand(redisClient *c) {
    int flags = 0;

    if (c->argc > 2) {
        addReply(c,shared.syntaxerr);
        return;
    } else if (c->argc == 2) {

        // 停机时不进行保存
        if (!strcasecmp(c->argv[1]->ptr,"nosave")) {
            flags |= REDIS_SHUTDOWN_NOSAVE;

        // 停机时进行保存
        } else if (!strcasecmp(c->argv[1]->ptr,"save")) {
            flags |= REDIS_SHUTDOWN_SAVE;

        } else {
            addReply(c,shared.syntaxerr);
            return;
        }
    }

    /* When SHUTDOWN is called while the server is loading a dataset in
     * memory we need to make sure no attempt is performed to save
     * the dataset on shutdown (otherwise it could overwrite the current DB
     * with half-read data).
     *
     * Also when in Sentinel mode clear the SAVE flag and force NOSAVE. */
    if (server.loading || server.sentinel_mode)
        flags = (flags & ~REDIS_SHUTDOWN_SAVE) | REDIS_SHUTDOWN_NOSAVE;

    if (prepareForShutdown(flags) == REDIS_OK) exit(0);

    addReplyError(c,"Errors trying to SHUTDOWN. Check logs.");
}

void renameGenericCommand(redisClient *c, int nx) {
    robj *o;
    long long expire;

    /* To use the same key as src and dst is probably an error */
    // 来源键和目标键不能相同
    if (sdscmp(c->argv[1]->ptr,c->argv[2]->ptr) == 0) {
        addReply(c,shared.sameobjecterr);
        return;
    }

    // 取出来源键
    if ((o = lookupKeyWriteOrReply(c,c->argv[1],shared.nokeyerr)) == NULL)
        return;

    // 增加引用计数，因为后面目标键也会引用这个对象
    // 如果不增加的话，当来源键被删除时，这个值对象也会被删除
    incrRefCount(o);

    // 取出来源键的过期时间
    expire = getExpire(c->db,c->argv[1]);

    // 检查目标键是否存在
    if (lookupKeyWrite(c->db,c->argv[2]) != NULL) {

        // 如果目标键存在，并且执行的是 RENAMENX ，那么直接返回
        if (nx) {
            decrRefCount(o);
            addReply(c,shared.czero);
            return;
        }

        // 如果执行的是 RENAME ，那么删除已有的目标键
        /* Overwrite: delete the old key before creating the new one
         * with the same name. */
        dbDelete(c->db,c->argv[2]);
    }

    // 将来源键的值对象和目标键进行关联
    dbAdd(c->db,c->argv[2],o);

    // 如果有过期时间，那么为目标键设置过期时间
    if (expire != -1) setExpire(c->db,c->argv[2],expire);

    // 删除来源键
    dbDelete(c->db,c->argv[1]);

    signalModifiedKey(c->db,c->argv[1]);
    signalModifiedKey(c->db,c->argv[2]);

    notifyKeyspaceEvent(REDIS_NOTIFY_GENERIC,"rename_from",
        c->argv[1],c->db->id);
    notifyKeyspaceEvent(REDIS_NOTIFY_GENERIC,"rename_to",
        c->argv[2],c->db->id);

    server.dirty++;

    addReply(c,nx ? shared.cone : shared.ok);
}

/*
RENAME key newkey

将 key 改名为 newkey 。

当 key 和 newkey 相同，或者 key 不存在时，返回一个错误。

当 newkey 已经存在时， RENAME 命令将覆盖旧值。
可用版本：>= 1.0.0时间复杂度：O(1)返回值：改名成功时提示 OK ，失败时候返回一个错误。

# key 存在且 newkey 不存在

redis> SET message "hello world"
OK

redis> RENAME message greeting
OK

redis> EXISTS message               # message 不复存在
(integer) 0

redis> EXISTS greeting              # greeting 取而代之
(integer) 1


# 当 key 不存在时，返回错误

redis> RENAME fake_key never_exists
(error) ERR no such key


# newkey 已存在时， RENAME 会覆盖旧 newkey

redis> SET pc "lenovo"
OK

redis> SET personal_computer "dell"
OK

redis> RENAME pc personal_computer
OK

redis> GET pc
(nil)

redis:1> GET personal_computer      # 原来的值 dell 被覆盖了
"lenovo"


*/
void renameCommand(redisClient *c) {
    renameGenericCommand(c,0);
}

/*
RENAMENX key newkey

当且仅当 newkey 不存在时，将 key 改名为 newkey 。

当 key 不存在时，返回一个错误。
可用版本：>= 1.0.0时间复杂度：O(1)返回值：

修改成功时，返回 1 。

如果 newkey 已经存在，返回 0 。


# newkey 不存在，改名成功

redis> SET player "MPlyaer"
OK

redis> EXISTS best_player
(integer) 0

redis> RENAMENX player best_player
(integer) 1


# newkey存在时，失败

redis> SET animal "bear"
OK

redis> SET favorite_animal "butterfly"
OK

redis> RENAMENX animal favorite_animal
(integer) 0

redis> get animal
"bear"

redis> get favorite_animal
"butterfly"


*/
void renamenxCommand(redisClient *c) {
    renameGenericCommand(c,1);
}

/*
MOVE key db

将当前数据库的 key 移动到给定的数据库 db 当中。

如果当前数据库(源数据库)和给定数据库(目标数据库)有相同名字的给定 key ，或者 key 不存在于当前数据库，那么 MOVE 没有任何效果。

因此，也可以利用这一特性，将 MOVE 当作锁(locking)原语(primitive)。
可用版本：>= 1.0.0时间复杂度：O(1)返回值：移动成功返回 1 ，失败则返回 0 。

# key 存在于当前数据库

redis> SELECT 0                             # redis默认使用数据库 0，为了清晰起见，这里再显式指定一次。
OK

redis> SET song "secret base - Zone"
OK

redis> MOVE song 1                          # 将 song 移动到数据库 1
(integer) 1

redis> EXISTS song                          # song 已经被移走
(integer) 0

redis> SELECT 1                             # 使用数据库 1
OK

redis:1> EXISTS song                        # 证实 song 被移到了数据库 1 (注意命令提示符变成了"redis:1"，表明正在使用数据库 1)
(integer) 1


# 当 key 不存在的时候

redis:1> EXISTS fake_key
(integer) 0

redis:1> MOVE fake_key 0                    # 试图从数据库 1 移动一个不存在的 key 到数据库 0，失败
(integer) 0

redis:1> select 0                           # 使用数据库0
OK

redis> EXISTS fake_key                      # 证实 fake_key 不存在
(integer) 0


# 当源数据库和目标数据库有相同的 key 时

redis> SELECT 0                             # 使用数据库0
OK
redis> SET favorite_fruit "banana"
OK

redis> SELECT 1                             # 使用数据库1
OK
redis:1> SET favorite_fruit "apple"
OK

redis:1> SELECT 0                           # 使用数据库0，并试图将 favorite_fruit 移动到数据库 1
OK

redis> MOVE favorite_fruit 1                # 因为两个数据库有相同的 key，MOVE 失败
(integer) 0

redis> GET favorite_fruit                   # 数据库 0 的 favorite_fruit 没变
"banana"

redis> SELECT 1
OK

redis:1> GET favorite_fruit                 # 数据库 1 的 favorite_fruit 也是
"apple"


*/
void moveCommand(redisClient *c) {
    robj *o;
    redisDb *src, *dst;
    int srcid;

    if (server.cluster_enabled) {
        addReplyError(c,"MOVE is not allowed in cluster mode");
        return;
    }

    /* Obtain source and target DB pointers */
    // 源数据库
    src = c->db;
    // 源数据库的 id
    srcid = c->db->id;

    // 切换到目标数据库
    if (selectDb(c,atoi(c->argv[2]->ptr)) == REDIS_ERR) {
        addReply(c,shared.outofrangeerr);
        return;
    }

    // 目标数据库
    dst = c->db;

    // 切换回源数据库
    selectDb(c,srcid); /* Back to the source DB */

    /* If the user is moving using as target the same
     * DB as the source DB it is probably an error. */
    // 如果源数据库和目标数据库相等，那么返回错误
    if (src == dst) {
        addReply(c,shared.sameobjecterr);
        return;
    }

    /* Check if the element exists and get a reference */
    // 取出要移动的对象
    o = lookupKeyWrite(c->db,c->argv[1]);
    if (!o) {
        addReply(c,shared.czero);
        return;
    }

    /* Return zero if the key already exists in the target DB */
    // 如果键已经存在于目标数据库，那么返回
    if (lookupKeyWrite(dst,c->argv[1]) != NULL) {
        addReply(c,shared.czero);
        return;
    }

    // 将键添加到目标数据库中
    dbAdd(dst,c->argv[1],o);
    // 增加对对象的引用计数，避免接下来在源数据库中删除时 o 被清理
    incrRefCount(o);

    /* OK! key moved, free the entry in the source DB */
    // 将键从源数据库中返回
    dbDelete(src,c->argv[1]);

    server.dirty++;

    addReply(c,shared.cone);
}

/*-----------------------------------------------------------------------------
 * Expires API
 *----------------------------------------------------------------------------*/

/*
 * 移除键 key 的过期时间
 */
int removeExpire(redisDb *db, robj *key) {
    /* An expire may only be removed if there is a corresponding entry in the
     * main dict. Otherwise, the key will never be freed. */
    // 确保键带有过期时间
    redisAssertWithInfo(NULL,key,dictFind(db->dict,key->ptr) != NULL);

    // 删除过期时间
    return dictDelete(db->expires,key->ptr) == DICT_OK;
}

/*
 * 将键 key 的过期时间设为 when
 */
void setExpire(redisDb *db, robj *key, long long when) {

    dictEntry *kde, *de;

    /* Reuse the sds from the main dict in the expire dict */
    // 取出键
    kde = dictFind(db->dict,key->ptr);

    redisAssertWithInfo(NULL,key,kde != NULL);

    // 根据键取出键的过期时间
    de = dictReplaceRaw(db->expires,dictGetKey(kde));

    // 设置键的过期时间
    // 这里是直接使用整数值来保存过期时间，不是用 INT 编码的 String 对象
    dictSetSignedIntegerVal(de,when);
}

/* Return the expire time of the specified key, or -1 if no expire
 * is associated with this key (i.e. the key is non volatile) 
 *
 * 返回给定 key 的过期时间。
 *
 * 如果键没有设置过期时间，那么返回 -1 。
 */
long long getExpire(redisDb *db, robj *key) {
    dictEntry *de;

    /* No expire? return ASAP */
    // 获取键的过期时间
    // 如果过期时间不存在，那么直接返回
    if (dictSize(db->expires) == 0 ||
       (de = dictFind(db->expires,key->ptr)) == NULL) return -1;

    /* The entry was found in the expire dict, this means it should also
     * be present in the main dict (safety check). */
    redisAssertWithInfo(NULL,key,dictFind(db->dict,key->ptr) != NULL);

    // 返回过期时间
    return dictGetSignedIntegerVal(de);
}

/* Propagate expires into slaves and the AOF file.
 * When a key expires in the master, a DEL operation for this key is sent
 * to all the slaves and the AOF file if enabled.
 *
 * 将过期时间传播到附属节点和 AOF 文件。
 *
 * 当一个键在主节点中过期时，
 * 主节点会向所有附属节点和 AOF 文件传播一个显式的 DEL 命令。
 *
 * This way the key expiry is centralized in one place, and since both
 * AOF and the master->slave link guarantee operation ordering, everything
 * will be consistent even if we allow write operations against expiring
 * keys. 
 *
 * 这种做法使得对键的过期可以集中在一处处理，
 * 因为 AOF 以及主节点和附属节点之间的链接，都可以保证操作的执行顺序，
 * 所以即使有写操作对过期键执行，所有数据都还是 consistent 的。
 */
void propagateExpire(redisDb *db, robj *key) { //键过期后需要让所有从redis数据库知道，并且需要些一个del到aOF
    robj *argv[2];

    // 构造一个 DEL key 命令
    argv[0] = shared.del;
    argv[1] = key;
    incrRefCount(argv[0]);
    incrRefCount(argv[1]);

    // 传播到 AOF 
    if (server.aof_state != REDIS_AOF_OFF)
        feedAppendOnlyFile(server.delCommand,db->id,argv,2);

    // 传播到所有附属节点
    replicationFeedSlaves(server.slaves,db->id,argv,2);

    decrRefCount(argv[0]);
    decrRefCount(argv[1]);
}

/*
 * 检查 key 是否已经过期，如果是的话，将它从数据库中删除。
 *
 * 返回 0 表示键没有过期时间，或者键未过期。
 *
 * 返回 1 表示键已经因为过期而被删除了。
 */
int expireIfNeeded(redisDb *db, robj *key) {

    // 取出键的过期时间
    mstime_t when = getExpire(db,key);
    mstime_t now;

    // 没有过期时间
    if (when < 0) return 0; /* No expire for this key */

    /* Don't expire anything while loading. It will be done later. */
    // 如果服务器正在进行载入，那么不进行任何过期检查
    if (server.loading) return 0;

    /* If we are in the context of a Lua script, we claim that time is
     * blocked to when the Lua script started. This way a key can expire
     * only the first time it is accessed and not in the middle of the
     * script execution, making propagation to slaves / AOF consistent.
     * See issue #1525 on Github for more information. */
    now = server.lua_caller ? server.lua_time_start : mstime();

    /* If we are running in the context of a slave, return ASAP:
     * the slave key expiration is controlled by the master that will
     * send us synthesized DEL operations for expired keys.
     *
     * Still we try to return the right information to the caller, 
     * that is, 0 if we think the key should be still valid, 1 if
     * we think the key is expired at this time. */
    // 当服务器运行在 replication 模式时
    // 附属节点并不主动删除 key
    // 它只返回一个逻辑上正确的返回值
    // 真正的删除操作要等待主节点发来删除命令时才执行
    // 从而保证数据的同步
    if (server.masterhost != NULL) return now > when;

    // 运行到这里，表示键带有过期时间，并且服务器为主节点

    /* Return when this key has not expired */
    // 如果未过期，返回 0
    if (now <= when) return 0;

    /* Delete the key */
    server.stat_expiredkeys++;

    // 向 AOF 文件和附属节点传播过期信息
    propagateExpire(db,key);

    // 发送事件通知
    notifyKeyspaceEvent(REDIS_NOTIFY_EXPIRED,
        "expired",key,db->id);

    // 将过期键从数据库中删除
    return dbDelete(db,key);
}

/*-----------------------------------------------------------------------------
 * Expires Commands
 *----------------------------------------------------------------------------*/

/* This is the generic command implementation for EXPIRE, PEXPIRE, EXPIREAT
 * and PEXPIREAT. Because the commad second argument may be relative or absolute
 * the "basetime" argument is used to signal what the base time is (either 0
 * for *AT variants of the command, or the current time for relative expires).
 *
 * 这个函数是 EXPIRE 、 PEXPIRE 、 EXPIREAT 和 PEXPIREAT 命令的底层实现函数。
 *
 * 命令的第二个参数可能是绝对值，也可能是相对值。
 * 当执行 *AT 命令时， basetime 为 0 ，在其他情况下，它保存的就是当前的绝对时间。
 *
 * unit is either UNIT_SECONDS or UNIT_MILLISECONDS, and is only used for
 * the argv[2] parameter. The basetime is always specified in milliseconds. 
 *
 * unit 用于指定 argv[2] （传入过期时间）的格式，
 * 它可以是 UNIT_SECONDS 或 UNIT_MILLISECONDS ，
 * basetime 参数则总是毫秒格式的。
 */
void expireGenericCommand(redisClient *c, long long basetime, int unit) {
    robj *key = c->argv[1], *param = c->argv[2];
    long long when; /* unix time in milliseconds when the key will expire. */

    // 取出 when 参数
    if (getLongLongFromObjectOrReply(c, param, &when, NULL) != REDIS_OK)
        return;

    // 如果传入的过期时间是以秒为单位的，那么将它转换为毫秒
    if (unit == UNIT_SECONDS) when *= 1000;
    when += basetime;

    /* No key, return zero. */
    // 取出键
    if (lookupKeyRead(c->db,key) == NULL) {
        addReply(c,shared.czero);
        return;
    }

    /* EXPIRE with negative TTL, or EXPIREAT with a timestamp into the past
     * should never be executed as a DEL when load the AOF or in the context
     * of a slave instance.
     *
     * 在载入数据时，或者服务器为附属节点时，
     * 即使 EXPIRE 的 TTL 为负数，或者 EXPIREAT 提供的时间戳已经过期，
     * 服务器也不会主动删除这个键，而是等待主节点发来显式的 DEL 命令。
     *
     * Instead we take the other branch of the IF statement setting an expire
     * (possibly in the past) and wait for an explicit DEL from the master. 
     *
     * 程序会继续将（一个可能已经过期的 TTL）设置为键的过期时间，
     * 并且等待主节点发来 DEL 命令。
     */
    if (when <= mstime() && !server.loading && !server.masterhost) {

        // when 提供的时间已经过期，服务器为主节点，并且没在载入数据

        robj *aux;

        redisAssertWithInfo(c,key,dbDelete(c->db,key));
        server.dirty++;

        /* Replicate/AOF this as an explicit DEL. */
        // 传播 DEL 命令
        aux = createStringObject("DEL",3);

        rewriteClientCommandVector(c,2,aux,key);
        decrRefCount(aux);

        signalModifiedKey(c->db,key);
        notifyKeyspaceEvent(REDIS_NOTIFY_GENERIC,"del",key,c->db->id);

        addReply(c, shared.cone);

        return;
    } else {

        // 设置键的过期时间
        // 如果服务器为附属节点，或者服务器正在载入，
        // 那么这个 when 有可能已经过期的
        setExpire(c->db,key,when);

        addReply(c,shared.cone);

        signalModifiedKey(c->db,key);
        notifyKeyspaceEvent(REDIS_NOTIFY_GENERIC,"expire",key,c->db->id);

        server.dirty++;

        return;
    }
}

/*
EXPIRE key seconds

为给定 key 设置生存时间，当 key 过期时(生存时间为 0 )，它会被自动删除。

在 Redis 中，带有生存时间的 key 被称为『易失的』(volatile)。

生存时间可以通过使用 DEL 命令来删除整个 key 来移除，或者被 SET 和 GETSET 命令覆写(overwrite)，这意味着，如果一个命令只是修改(alter)一个带生存时间的 key 的值而不是用一个新的 key 值来代替(replace)它的话，那么生存时间不会被改变。

比如说，对一个 key 执行 INCR 命令，对一个列表进行 LPUSH 命令，或者对一个哈希表执行 HSET 命令，这类操作都不会修改 key 本身的生存时间。

另一方面，如果使用 RENAME 对一个 key 进行改名，那么改名后的 key 的生存时间和改名前一样。

RENAME 命令的另一种可能是，尝试将一个带生存时间的 key 改名成另一个带生存时间的 another_key ，这时旧的 another_key (以及它的生存时间)会被删除，然后旧的 key 会改名为 another_key ，因此，新的 another_key 的生存时间也和原本的 key 一样。

使用 PERSIST 命令可以在不删除 key 的情况下，移除 key 的生存时间，让 key 重新成为一个『持久的』(persistent) key 。

更新生存时间

可以对一个已经带有生存时间的 key 执行 EXPIRE 命令，新指定的生存时间会取代旧的生存时间。

过期时间的精确度

在 Redis 2.4 版本中，过期时间的延迟在 1 秒钟之内 —— 也即是，就算 key 已经过期，但它还是可能在过期之后一秒钟之内被访问到，而在新的 Redis 2.6 版本中，延迟被降低到 1 毫秒之内。

Redis 2.1.3 之前的不同之处

在 Redis 2.1.3 之前的版本中，修改一个带有生存时间的 key 会导致整个 key 被删除，这一行为是受当时复制(replication)层的限制而作出的，现在这一限制已经被修复。
可用版本：>= 1.0.0时间复杂度：O(1)返回值：

设置成功返回 1 。

当 key 不存在或者不能为 key 设置生存时间时(比如在低于 2.1.3 版本的 Redis 中你尝试更新 key 的生存时间)，返回 0 。


redis> SET cache_page "www.google.com"
OK

redis> EXPIRE cache_page 30  # 设置过期时间为 30 秒
(integer) 1

redis> TTL cache_page    # 查看剩余生存时间
(integer) 23

redis> EXPIRE cache_page 30000   # 更新过期时间
(integer) 1

redis> TTL cache_page
(integer) 29996

*/
void expireCommand(redisClient *c) { //PERSIST key移除给定 key 的生存时间
    expireGenericCommand(c,mstime(),UNIT_SECONDS);
}


/*
EXPIREAT

EXPIREAT key timestamp

EXPIREAT 的作用和 EXPIRE 类似，都用于为 key 设置生存时间。

不同在于 EXPIREAT 命令接受的时间参数是 UNIX 时间戳(unix timestamp)。
可用版本：>= 1.2.0时间复杂度：O(1)返回值：

如果生存时间设置成功，返回 1 。

当 key 不存在或没办法设置生存时间，返回 0 。


redis> SET cache www.google.com
OK

redis> EXPIREAT cache 1355292000     # 这个 key 将在 2012.12.12 过期
(integer) 1

redis> TTL cache
(integer) 45081860
*/
void expireatCommand(redisClient *c) {//PERSIST key移除给定 key 的生存时间
    expireGenericCommand(c,0,UNIT_SECONDS);
}

/*
PEXPIRE key milliseconds

这个命令和 EXPIRE 命令的作用类似，但是它以毫秒为单位设置 key 的生存时间，而不像 EXPIRE 命令那样，以秒为单位。
可用版本：>= 2.6.0时间复杂度：O(1)返回值：

设置成功，返回 1

key 不存在或设置失败，返回 0


redis> SET mykey "Hello"
OK

redis> PEXPIRE mykey 1500
(integer) 1

redis> TTL mykey    # TTL 的返回值以秒为单位
(integer) 2

redis> PTTL mykey   # PTTL 可以给出准确的毫秒数
(integer) 1499




PEXPIREAT key milliseconds-timestamp

这个命令和 EXPIREAT 命令类似，但它以毫秒为单位设置 key 的过期 unix 时间戳，而不是像 EXPIREAT 那样，以秒为单位。
可用版本：>= 2.6.0时间复杂度：O(1)返回值：

如果生存时间设置成功，返回 1 。

当 key 不存在或没办法设置生存时间时，返回 0 。(查看 EXPIRE 命令获取更多信息)


redis> SET mykey "Hello"
OK

redis> PEXPIREAT mykey 1555555555005
(integer) 1

redis> TTL mykey           # TTL 返回秒
(integer) 223157079

redis> PTTL mykey          # PTTL 返回毫秒
(integer) 223157079318

*/

void pexpireatCommand(redisClient *c) {//PERSIST key移除给定 key 的生存时间
    expireGenericCommand(c,0,UNIT_MILLISECONDS);
}

void pexpireCommand(redisClient *c) {//PERSIST key移除给定 key 的生存时间
    expireGenericCommand(c,mstime(),UNIT_MILLISECONDS);
}


/*
 * 返回键的剩余生存时间。
 *
 * output_ms 指定返回值的格式：
 *
 *  - 为 1 时，返回毫秒
 *
 *  - 为 0 时，返回秒
 */
void ttlGenericCommand(redisClient *c, int output_ms) {
    long long expire, ttl = -1;

    /* If the key does not exist at all, return -2 */
    // 取出键
    if (lookupKeyRead(c->db,c->argv[1]) == NULL) {
        addReplyLongLong(c,-2);
        return;
    }

    /* The key exists. Return -1 if it has no expire, or the actual
     * TTL value otherwise. */
    // 取出过期时间
    expire = getExpire(c->db,c->argv[1]);

    if (expire != -1) {
        // 计算剩余生存时间
        ttl = expire-mstime();
        if (ttl < 0) ttl = 0;
    }

    if (ttl == -1) {
        // 键是持久的
        addReplyLongLong(c,-1);
    } else {
        // 返回 TTL 
        // (ttl+500)/1000 计算的是渐近秒数
        addReplyLongLong(c,output_ms ? ttl : ((ttl+500)/1000));
    }
}

/*
TTL key

以秒为单位，返回给定 key 的剩余生存时间(TTL, time to live)。
可用版本：>= 1.0.0时间复杂度：O(1)返回值：

当 key 不存在时，返回 -2 。

当 key 存在但没有设置剩余生存时间时，返回 -1 。

否则，以秒为单位，返回 key 的剩余生存时间。



在 Redis 2.8 以前，当 key 不存在，或者 key 没有设置剩余生存时间时，命令都返回 -1 。
 

# 不存在的 key

redis> FLUSHDB
OK

redis> TTL key
(integer) -2


# key 存在，但没有设置剩余生存时间

redis> SET key value
OK

redis> TTL key
(integer) -1


# 有剩余生存时间的 key

redis> EXPIRE key 10086
(integer) 1

redis> TTL key
(integer) 10084


*/
void ttlCommand(redisClient *c) {
    ttlGenericCommand(c, 0);
}

/*
PTTL key

这个命令类似于 TTL 命令，但它以毫秒为单位返回 key 的剩余生存时间，而不是像 TTL 命令那样，以秒为单位。
可用版本：>= 2.6.0复杂度：O(1)返回值：

当 key 不存在时，返回 -2 。

当 key 存在但没有设置剩余生存时间时，返回 -1 。

否则，以毫秒为单位，返回 key 的剩余生存时间。



在 Redis 2.8 以前，当 key 不存在，或者 key 没有设置剩余生存时间时，命令都返回 -1 。
 

# 不存在的 key

redis> FLUSHDB
OK

redis> PTTL key
(integer) -2


# key 存在，但没有设置剩余生存时间

redis> SET key value
OK

redis> PTTL key
(integer) -1


# 有剩余生存时间的 key

redis> PEXPIRE key 10086
(integer) 1

redis> PTTL key
(integer) 6179


*/
void pttlCommand(redisClient *c) {
    ttlGenericCommand(c, 1);
}

/*
PERSIST key

移除给定 key 的生存时间，将这个 key 从『易失的』(带生存时间 key )转换成『持久的』(一个不带生存时间、永不过期的 key )。
可用版本：>= 2.2.0时间复杂度：O(1)返回值：

当生存时间移除成功时，返回 1 .

如果 key 不存在或 key 没有设置生存时间，返回 0 。


redis> SET mykey "Hello"
OK

redis> EXPIRE mykey 10  # 为 key 设置生存时间
(integer) 1

redis> TTL mykey
(integer) 10

redis> PERSIST mykey    # 移除 key 的生存时间
(integer) 1

redis> TTL mykey
(integer) -1


*/
void persistCommand(redisClient *c) {
    dictEntry *de;

    // 取出键
    de = dictFind(c->db->dict,c->argv[1]->ptr);

    if (de == NULL) {
        // 键没有过期时间
        addReply(c,shared.czero);

    } else {

        // 键带有过期时间，那么将它移除
        if (removeExpire(c->db,c->argv[1])) {
            addReply(c,shared.cone);
            server.dirty++;

        // 键已经是持久的了
        } else {
            addReply(c,shared.czero);
        }
    }
}

/* -----------------------------------------------------------------------------
 * API to get key arguments from commands
 * ---------------------------------------------------------------------------*/

/* The base case is to use the keys position as given in the command table
 * (firstkey, lastkey, step). */
int *getKeysUsingCommandTable(struct redisCommand *cmd,robj **argv, int argc, int *numkeys) {
    int j, i = 0, last, *keys;
    REDIS_NOTUSED(argv);

    if (cmd->firstkey == 0) {
        *numkeys = 0;
        return NULL;
    }
    last = cmd->lastkey;
    if (last < 0) last = argc+last;
    keys = zmalloc(sizeof(int)*((last - cmd->firstkey)+1));
    for (j = cmd->firstkey; j <= last; j += cmd->keystep) {
        redisAssert(j < argc);
        keys[i++] = j;
    }
    *numkeys = i;
    return keys;
}

/* Return all the arguments that are keys in the command passed via argc / argv.
 *
 * The command returns the positions of all the key arguments inside the array,
 * so the actual return value is an heap allocated array of integers. The
 * length of the array is returned by reference into *numkeys.
 *
 * 'cmd' must be point to the corresponding entry into the redisCommand
 * table, according to the command name in argv[0].
 *
 * This function uses the command table if a command-specific helper function
 * is not required, otherwise it calls the command-specific function. */
int *getKeysFromCommand(struct redisCommand *cmd, robj **argv, int argc, int *numkeys) {
    if (cmd->getkeys_proc) {
        return cmd->getkeys_proc(cmd,argv,argc,numkeys);
    } else {
        return getKeysUsingCommandTable(cmd,argv,argc,numkeys);
    }
}

/* Free the result of getKeysFromCommand. */
void getKeysFreeResult(int *result) {
    zfree(result);
}

/* Helper function to extract keys from following commands:
 * ZUNIONSTORE <destkey> <num-keys> <key> <key> ... <key> <options>
 * ZINTERSTORE <destkey> <num-keys> <key> <key> ... <key> <options> */
int *zunionInterGetKeys(struct redisCommand *cmd, robj **argv, int argc, int *numkeys) {
    int i, num, *keys;
    REDIS_NOTUSED(cmd);

    num = atoi(argv[2]->ptr);
    /* Sanity check. Don't return any key if the command is going to
     * reply with syntax error. */
    if (num > (argc-3)) {
        *numkeys = 0;
        return NULL;
    }

    /* Keys in z{union,inter}store come from two places:
     * argv[1] = storage key,
     * argv[3...n] = keys to intersect */
    keys = zmalloc(sizeof(int)*(num+1));

    /* Add all key positions for argv[3...n] to keys[] */
    for (i = 0; i < num; i++) keys[i] = 3+i;

    /* Finally add the argv[1] key position (the storage key target). */
    keys[num] = 1;
    *numkeys = num+1;  /* Total keys = {union,inter} keys + storage key */
    return keys;
}

/* Helper function to extract keys from the following commands:
 * EVAL <script> <num-keys> <key> <key> ... <key> [more stuff]
 * EVALSHA <script> <num-keys> <key> <key> ... <key> [more stuff] */
int *evalGetKeys(struct redisCommand *cmd, robj **argv, int argc, int *numkeys) {
    int i, num, *keys;
    REDIS_NOTUSED(cmd);

    num = atoi(argv[2]->ptr);
    /* Sanity check. Don't return any key if the command is going to
     * reply with syntax error. */
    if (num > (argc-3)) {
        *numkeys = 0;
        return NULL;
    }

    keys = zmalloc(sizeof(int)*num);
    *numkeys = num;

    /* Add all key positions for argv[3...n] to keys[] */
    for (i = 0; i < num; i++) keys[i] = 3+i;

    return keys;
}

/* Helper function to extract keys from the SORT command.
 *
 * SORT <sort-key> ... STORE <store-key> ...
 *
 * The first argument of SORT is always a key, however a list of options
 * follow in SQL-alike style. Here we parse just the minimum in order to
 * correctly identify keys in the "STORE" option. */
int *sortGetKeys(struct redisCommand *cmd, robj **argv, int argc, int *numkeys) {
    int i, j, num, *keys;
    REDIS_NOTUSED(cmd);

    num = 0;
    keys = zmalloc(sizeof(int)*2); /* Alloc 2 places for the worst case. */

    keys[num++] = 1; /* <sort-key> is always present. */

    /* Search for STORE option. By default we consider options to don't
     * have arguments, so if we find an unknown option name we scan the
     * next. However there are options with 1 or 2 arguments, so we
     * provide a list here in order to skip the right number of args. */
    struct {
        char *name;
        int skip;
    } skiplist[] = {
        {"limit", 2},
        {"get", 1},
        {"by", 1},
        {NULL, 0} /* End of elements. */
    };

    for (i = 2; i < argc; i++) {
        for (j = 0; skiplist[j].name != NULL; j++) {
            if (!strcasecmp(argv[i]->ptr,skiplist[j].name)) {
                i += skiplist[j].skip;
                break;
            } else if (!strcasecmp(argv[i]->ptr,"store") && i+1 < argc) {
                /* Note: we don't increment "num" here and continue the loop
                 * to be sure to process the *last* "STORE" option if multiple
                 * ones are provided. This is same behavior as SORT. */
                keys[num] = i+1; /* <store-key> */
                break;
            }
        }
    }
    *numkeys = num;
    return keys;
}

/* Slot to Key API. This is used by Redis Cluster in order to obtain in
 * a fast way a key that belongs to a specified hash slot. This is useful
 * while rehashing the cluster. */
// 将给定键添加到槽里面，
// 节点的 slots_to_keys 用跳跃表记录了 slot -> key 之间的映射
// 这样可以快速地处理槽和键的关系，在 rehash 槽时很有用。
void slotToKeyAdd(robj *key) {

    // 计算出键所属的槽
    unsigned int hashslot = keyHashSlot(key->ptr,sdslen(key->ptr));

    // 将槽 slot 作为分值，键作为成员，添加到 slots_to_keys 跳跃表里面
    zslInsert(server.cluster->slots_to_keys,hashslot,key);
    incrRefCount(key);
}

// 从槽中删除给定的键 key
void slotToKeyDel(robj *key) {
    unsigned int hashslot = keyHashSlot(key->ptr,sdslen(key->ptr));

    zslDelete(server.cluster->slots_to_keys,hashslot,key);
}

// 清空节点所有槽保存的所有键
void slotToKeyFlush(void) {
    zslFree(server.cluster->slots_to_keys);
    server.cluster->slots_to_keys = zslCreate();
}

// 记录 count 个属于 hashslot 槽的键到 keys 数组
// 并返回被记录键的数量
unsigned int getKeysInSlot(unsigned int hashslot, robj **keys, unsigned int count) {
    zskiplistNode *n;
    zrangespec range;
    int j = 0;

    range.min = range.max = hashslot;
    range.minex = range.maxex = 0;

    // 定位到第一个属于指定 slot 的键上面
    n = zslFirstInRange(server.cluster->slots_to_keys, &range);
    // 遍历跳跃表，并保存属于指定 slot 的键
    // n && n->score 检查当前键是否属于指定 slot
    // && count-- 用来计数
    while(n && n->score == hashslot && count--) {
        // 记录键
        keys[j++] = n->obj;
        n = n->level[0].forward;
    }
    return j;
}

/* Remove all the keys in the specified hash slot.
 * The number of removed items is returned. */
unsigned int delKeysInSlot(unsigned int hashslot) {
    zskiplistNode *n;
    zrangespec range;
    int j = 0;

    range.min = range.max = hashslot;
    range.minex = range.maxex = 0;

    n = zslFirstInRange(server.cluster->slots_to_keys, &range);
    while(n && n->score == hashslot) {
        robj *key = n->obj;
        n = n->level[0].forward; /* Go to the next item before freeing it. */
        incrRefCount(key); /* Protect the object while freeing it. */
        dbDelete(&server.db[0],key);
        decrRefCount(key);
        j++;
    }
    return j;
}

// 返回指定 slot 包含的键数量
unsigned int countKeysInSlot(unsigned int hashslot) {
    zskiplist *zsl = server.cluster->slots_to_keys;
    zskiplistNode *zn;
    zrangespec range;
    int rank, count = 0;

    range.min = range.max = hashslot;
    range.minex = range.maxex = 0;

    /* Find first element in range */
    // 定位到第一个在指定 slot 上的键
    zn = zslFirstInRange(zsl, &range);

    /* Use rank of first element, if any, to determine preliminary count */
    // 使用第一个指定 slot 键的排位减去最后一个指定 slot 键的排位
    // 这一方法来计算 slot 键的数量
    // 类似区间算法

    // 第一个在指定 slot 上的键存在
    if (zn != NULL) {
        // 获取第一个键的排位
        rank = zslGetRank(zsl, zn->score, zn->obj);
        count = (zsl->length - (rank - 1));

        /* Find last element in range */
        // 获取最后一个指定 slot 的键
        zn = zslLastInRange(zsl, &range);

        /* Use rank of last element, if any, to determine the actual count */
        // 最后一个键存在
        if (zn != NULL) {
            // 获取排位
            rank = zslGetRank(zsl, zn->score, zn->obj);
            // 计算数量
            count -= (zsl->length - rank);
        }
    }

    return count;
}
