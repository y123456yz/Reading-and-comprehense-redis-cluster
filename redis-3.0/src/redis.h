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

#ifndef __REDIS_H
#define __REDIS_H

#include "fmacros.h"
#include "config.h"

#if defined(__sun)
#include "solarisfixes.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <syslog.h>
#include <netinet/in.h>
#include <lua.h>
#include <signal.h>

#include "ae.h"      /* Event driven programming library */
#include "sds.h"     /* Dynamic safe strings */
#include "dict.h"    /* Hash tables */
#include "adlist.h"  /* Linked lists */
#include "zmalloc.h" /* total memory usage aware version of malloc/free */
#include "anet.h"    /* Networking the easy way */
#include "ziplist.h" /* Compact list data structure */
#include "intset.h"  /* Compact integer set structure */
#include "version.h" /* Version macro */
#include "util.h"    /* Misc functions useful in many places */

#define NGX_FUNC_LINE __FUNCTION__, __LINE__


/* Error codes */
#define REDIS_OK                0
#define REDIS_ERR               -1

/* Static server configuration */
/* 默认的服务器配置值 */
#define REDIS_DEFAULT_HZ        10      /* Time interrupt calls/sec. */
#define REDIS_MIN_HZ            1
#define REDIS_MAX_HZ            500 
#define REDIS_SERVERPORT        6379    /* TCP port */
#define REDIS_TCP_BACKLOG       511     /* TCP listen backlog */
#define REDIS_MAXIDLETIME       0       /* default client timeout: infinite */
#define REDIS_DEFAULT_DBNUM     16
#define REDIS_CONFIGLINE_MAX    1024
#define REDIS_DBCRON_DBS_PER_CALL 16
#define REDIS_MAX_WRITE_PER_EVENT (1024*64)
#define REDIS_SHARED_SELECT_CMDS 10
#define REDIS_SHARED_INTEGERS 10000
#define REDIS_SHARED_BULKHDR_LEN 32
#define REDIS_MAX_LOGMSG_LEN    1024 /* Default maximum length of syslog messages */
#define REDIS_AOF_REWRITE_PERC  100
#define REDIS_AOF_REWRITE_MIN_SIZE (64*1024*1024)

/*
 在实际中，为了避免在执行命令时造成客户端输入缓冲区溢出，重写程序在处理列表、
哈希表、集合、有序集合这四种可能会带有多个元素的键时，会先检查键所包含的元素数
量，如果元素的数量超过了redis．h/REDIS_AOF_REWRITE_ITEMS_PER_CMD常量的
值，那么重写程序将使用多条命令来记录键的值，而不单单使用一条命令。
    在目前版本中，REDIS_AOF_REWRITE_ITEMS_PER_CMD常量的值为64，这也就是
说，如果一个粟合键包含了超过64个元素，那么重写程序会用多条SADD命令来记录这个
集合，并且每条命令设置的元素数量也为64个：
SADD <set-key> <eleml> <elem2> _ <elem64>
SADD <set-key> <elem65> <elem66> ~ ~ ~ <elem128>
SADD <set-key> <elem129> <elem130>  ~ ~ ~  <elem192>
*/
#define REDIS_AOF_REWRITE_ITEMS_PER_CMD 64
#define REDIS_SLOWLOG_LOG_SLOWER_THAN 10000
#define REDIS_SLOWLOG_MAX_LEN 128
#define REDIS_MAX_CLIENTS 10000
#define REDIS_AUTHPASS_MAX_LEN 512
#define REDIS_DEFAULT_SLAVE_PRIORITY 100
#define REDIS_REPL_TIMEOUT 60
#define REDIS_REPL_PING_SLAVE_PERIOD 10
#define REDIS_RUN_ID_SIZE 40
#define REDIS_OPS_SEC_SAMPLES 16
#define REDIS_DEFAULT_REPL_BACKLOG_SIZE (1024*1024)    /* 1mb */
#define REDIS_DEFAULT_REPL_BACKLOG_TIME_LIMIT (60*60)  /* 1 hour */
#define REDIS_REPL_BACKLOG_MIN_SIZE (1024*16)          /* 16k */
#define REDIS_BGSAVE_RETRY_DELAY 5 /* Wait a few secs before trying again. */
#define REDIS_DEFAULT_PID_FILE "/var/run/redis.pid"
#define REDIS_DEFAULT_SYSLOG_IDENT "redis"
#define REDIS_DEFAULT_CLUSTER_CONFIG_FILE "nodes.conf"
#define REDIS_DEFAULT_DAEMONIZE 0
#define REDIS_DEFAULT_UNIX_SOCKET_PERM 0
#define REDIS_DEFAULT_TCP_KEEPALIVE 0
#define REDIS_DEFAULT_LOGFILE ""
#define REDIS_DEFAULT_SYSLOG_ENABLED 0
#define REDIS_DEFAULT_STOP_WRITES_ON_BGSAVE_ERROR 1
#define REDIS_DEFAULT_RDB_COMPRESSION 1
#define REDIS_DEFAULT_RDB_CHECKSUM 1
#define REDIS_DEFAULT_RDB_FILENAME "dump.rdb"
#define REDIS_DEFAULT_SLAVE_SERVE_STALE_DATA 1
#define REDIS_DEFAULT_SLAVE_READ_ONLY 1
#define REDIS_DEFAULT_REPL_DISABLE_TCP_NODELAY 0
#define REDIS_DEFAULT_MAXMEMORY 0
#define REDIS_DEFAULT_MAXMEMORY_SAMPLES 5
#define REDIS_DEFAULT_AOF_FILENAME "appendonly.aof"
#define REDIS_DEFAULT_AOF_NO_FSYNC_ON_REWRITE 0
#define REDIS_DEFAULT_ACTIVE_REHASHING 1
#define REDIS_DEFAULT_AOF_REWRITE_INCREMENTAL_FSYNC 1
#define REDIS_DEFAULT_MIN_SLAVES_TO_WRITE 0
#define REDIS_DEFAULT_MIN_SLAVES_MAX_LAG 10
#define REDIS_IP_STR_LEN INET6_ADDRSTRLEN
#define REDIS_PEER_ID_LEN (REDIS_IP_STR_LEN+32) /* Must be enough for ip:port */
#define REDIS_BINDADDR_MAX 16
#define REDIS_MIN_RESERVED_FDS 32

#define ACTIVE_EXPIRE_CYCLE_LOOKUPS_PER_LOOP 20 /* Loopkups per loop. */
#define ACTIVE_EXPIRE_CYCLE_FAST_DURATION 1000 /* Microseconds */
#define ACTIVE_EXPIRE_CYCLE_SLOW_TIME_PERC 25 /* CPU max % for keys collection */
#define ACTIVE_EXPIRE_CYCLE_SLOW 0
#define ACTIVE_EXPIRE_CYCLE_FAST 1

/* Protocol and I/O related defines */
#define REDIS_MAX_QUERYBUF_LEN  (1024*1024*1024) /* 1GB max query buffer. */
#define REDIS_IOBUF_LEN         (1024*16)  /* Generic I/O buffer size */
#define REDIS_REPLY_CHUNK_BYTES (16*1024) /* 16k output buffer */
#define REDIS_INLINE_MAX_SIZE   (1024*64) /* Max size of inline reads */
#define REDIS_MBULK_BIG_ARG     (1024*32)
#define REDIS_LONGSTR_SIZE      21          /* Bytes needed for long -> str */
// 指示 AOF 程序每累积这个量的写入数据
// 就执行一次显式的 fsync
#define REDIS_AOF_AUTOSYNC_BYTES (1024*1024*32) /* fdatasync every 32MB */
/* When configuring the Redis eventloop, we setup it so that the total number
 * of file descriptors we can handle are server.maxclients + RESERVED_FDS + FDSET_INCR
 * that is our safety margin. */
#define REDIS_EVENTLOOP_FDSET_INCR (REDIS_MIN_RESERVED_FDS+96)

/* Hash table parameters */
#define REDIS_HT_MINFILL        10      /* Minimal hash table fill 10% */

/* Command flags. Please check the command table defined in the redis.c file
 * for more information about the meaning of every flag. */
// 命令标志
#define REDIS_CMD_WRITE 1                   /* "w" flag */
#define REDIS_CMD_READONLY 2                /* "r" flag */
#define REDIS_CMD_DENYOOM 4                 /* "m" flag */
#define REDIS_CMD_NOT_USED_1 8              /* no longer used flag */
#define REDIS_CMD_ADMIN 16                  /* "a" flag */
#define REDIS_CMD_PUBSUB 32                 /* "p" flag */
#define REDIS_CMD_NOSCRIPT  64              /* "s" flag */
#define REDIS_CMD_RANDOM 128                /* "R" flag */
#define REDIS_CMD_SORT_FOR_SCRIPT 256       /* "S" flag */
#define REDIS_CMD_LOADING 512               /* "l" flag */
#define REDIS_CMD_STALE 1024                /* "t" flag */
#define REDIS_CMD_SKIP_MONITOR 2048         /* "M" flag */
#define REDIS_CMD_ASKING 4096               /* "k" flag */

/*
 * 对象类型   下面的取值存储在redisObject->type

 
对于 Redis 数据库保存的键值对来说， 键总是一个字符串对象， 而值则可以是字符串对象、列表对象、哈希对象、集合对象或者有序集合对象的其中一种， 因此：
当我们称呼一个数据库键为“字符串键”时， 我们指的是“这个数据库键所对应的值为字符串对象”；
当我们称呼一个键为“列表键”时， 我们指的是“这个数据库键所对应的值为列表对象”，
 
TYPE 命令的实现方式也与此类似， 当我们对一个数据库键执行 TYPE 命令时， 命令返回的结果为数据库键对应的值对象的类型， 而不是键对象的类型：
# 键为字符串对象，值为字符串对象
 
 redis> SET msg "hello world"
 OK
 
 redis> TYPE msg
 string
 
# 键为字符串对象，值为列表对象
 
 redis> RPUSH numbers 1 3 5
 (integer) 6
 
 redis> TYPE numbers
 list
 
# 键为字符串对象，值为哈希对象
 
 redis> HMSET profile name Tome age 25 career Programmer
 OK
 
 redis> TYPE profile
 hash
 
# 键为字符串对象，值为集合对象
 
 redis> SADD fruits apple banana cherry
 (integer) 3
 
 redis> TYPE fruits
 set
 
# 键为字符串对象，值为有序集合对象
 
 redis> ZADD price 8.5 apple 5.0 banana 6.0 cherry
 (integer) 3
 
 redis> TYPE price
 zset

 */

/*

类型                编码                    对象

REDIS_STRING    REDIS_ENCODING_INT 使用整数值实现的字符串对象。 
REDIS_STRING    REDIS_ENCODING_EMBSTR 使用 embstr 编码的简单动态字符串实现的字符串对象。 
REDIS_STRING    REDIS_ENCODING_RAW 使用简单动态字符串实现的字符串对象。 
REDIS_LIST      REDIS_ENCODING_ZIPLIST 使用压缩列表实现的列表对象。 
REDIS_LIST      REDIS_ENCODING_LINKEDLIST 使用双端链表实现的列表对象。 
REDIS_HASH      REDIS_ENCODING_ZIPLIST 使用压缩列表实现的哈希对象。 
REDIS_HASH      REDIS_ENCODING_HT 使用字典实现的哈希对象。 
REDIS_SET       REDIS_ENCODING_INTSET 使用整数集合实现的集合对象。 
REDIS_SET       REDIS_ENCODING_HT 使用字典实现的集合对象。 
REDIS_ZSET      REDIS_ENCODING_ZIPLIST 使用压缩列表实现的有序集合对象。 
REDIS_ZSET      REDIS_ENCODING_SKIPLIST 使用跳跃表和字典实现的有序集合对象。 
*/

/* Object types */
// 对象类型
/*
字符串对象的编码可以是 REDIS_ENCODING_RAW 或者 REDIS_ENCODING_EMBSTR 或者REDIS_ENCODING_INT 。 如果是字符串数字，则为REDIS_ENCODING_INT
如果字符串对象保存的是一个字符串值， 并且这个字符串值的长度大于 39 字节， 采用编码方式REDIS_ENCODING_EMBSTR，否则采用编码方式REDIS_ENCODING_RAW
*/
#define REDIS_STRING 0 //参考set命令函数setCommand函数执行流程

/*
列表对象的编码方式可以是REDIS_ENCODING_LINKEDLIST双向列表和REDIS_ENCODING_ZIPLIST压缩表，默认REDIS_ENCODING_ZIPLIST,当不满足下面条件时
将使用双向列表编码方式。

当列表对象可以同时满足以下两个条件时， 列表对象使用 ziplist 编码：
1.列表对象保存的所有字符串元素的长度都小于 64 字节；
2.列表对象保存的元素数量小于 512 个；

不能满足这两个条件的列表对象需要使用 linkedlist 编码。
*/
#define REDIS_LIST 1 //参考lpush命令函数lpushCommand函数执行流程

/*
集合对象编码方式支持REDIS_ENCODING_INTSET和REDIS_ENCODING_HT 

如果sadd后面是整数并且集合元素个数不超过512，则采用REDIS_ENCODING_INTSET  否则REDIS_ENCODING_HT

当集合对象可以同时满足以下两个条件时， 对象使用 intset 编码：
1.集合对象保存的所有元素都是整数值；
2.集合对象保存的元素数量不超过 512 个；

不能满足这两个条件的集合对象需要使用 hashtable 编码。

*/
#define REDIS_SET 2//参考sadd命令函数saddCommand函数执行流程

/*
有序集支持压缩表REDIS_ENCODING_ZIPLIST和跳跃表REDIS_ENCODING_SKIPLIST编码方式， 默认压缩表

编码的转换:

当有序集合对象可以同时满足以下两个条件时， 对象使用 ziplist 编码：
1.有序集合保存的元素数量小于 128 个；
2.有序集合保存的所有元素成员的长度都小于 64 字节；

不能满足以上两个条件的有序集合对象将使用 skiplist 编码。

*/
#define REDIS_ZSET 3//参考zadd命令函数zaddCommand函数执行流程

/*
哈希对象的编码可以是 ziplist(REDIS_ENCODING_ZIPLIST) 或者 hashtable(REDIS_ENCODING_HT) 。
编码转换: 默认使用REDIS_ENCODING_ZIPLIST编码方式，当满足如下条件改为用REDIS_ENCODING_HT编码方式。

当哈希对象可以同时满足以下两个条件时， 哈希对象使用 ziplist 编码：
1.哈希对象保存的所有键值对的键和值的字符串长度都小于 64 字节；
2.哈希对象保存的键值对数量小于 512 个；

不能满足这两个条件的哈希对象需要使用 hashtable 编码。
*/

#define REDIS_HASH 4

/*
对象的编码                  

   编码常量                         编码所对应的底层数据结构
 REDIS_ENCODING_INT                     long 类型的整数 
 REDIS_ENCODING_EMBSTR                  embstr 编码的简单动态字符串 
 REDIS_ENCODING_RAW                     简单动态字符串 
 REDIS_ENCODING_HT                      字典 
 REDIS_ENCODING_LINKEDLIST              双端链表 
 REDIS_ENCODING_ZIPLIST                 压缩列表 
 REDIS_ENCODING_INTSET                  整数集合 
 REDIS_ENCODING_SKIPLIST                跳跃表

 REDIS_ENCODING_EMBSTR和REDIS_ENCODING_RAW区别?
 REDIS_ENCODING_RAW:如果字符串对象保存的是一个字符串值， 并且这个字符串值的长度大于 39 字节， 那么字符串对象将使用一个简单动态字符串（SDS）
                    来保存这个字符串值， 并将对象的编码设置为 raw 。
 REDIS_ENCODING_EMBSTR:如果字符串对象保存的是一个字符串值， 并且这个字符串值的长度小于等于 39 字节， 那么字符串对象将使用 embstr 编码的方式来
                       保存这个字符串值。

    embstr 编码是专门用于保存短字符串的一种优化编码方式， 这种编码和 raw 编码一样， 都使用 redisObject 结构和 sdshdr 结构来表示字符串对象， 
 但 raw 编码会调用两次内存分配函数来分别创建 redisObject 结构和 sdshdr 结构， 而 embstr 编码则通过调用一次内存分配函数来分配一块连续的
 空间， 空间中依次包含 redisObject 和 sdshdr 两个结构。

 embstr 编码的字符串对象在执行命令时， 产生的效果和 raw 编码的字符串对象执行命令时产生的效果是相同的， 但使用 embstr 编码的字符串对象来保存短字符串值有以下好处：
 1.embstr 编码将创建字符串对象所需的内存分配次数从 raw 编码的两次降低为一次。
 2.释放 embstr 编码的字符串对象只需要调用一次内存释放函数， 而释放 raw 编码的字符串对象需要调用两次内存释放函数。
 3.因为 embstr 编码的字符串对象的所有数据都保存在一块连续的内存里面， 所以这种编码的字符串对象比起 raw 编码的字符串对象能够更好地利用缓存带来的优势。

    long double 类型表示的浮点数在 Redis 中也是作为字符串值来保存的： 如果我们要保存一个浮点数到字符串对象里面， 那么程序会
先将这个浮点数转换成字符串值， 然后再保存起转换所得的字符串值。

    举个例子， 执行以下代码将创建一个包含 3.14 的字符串表示 "3.14" 的字符串对象：
 redis> SET pi 3.14
 OK
 redis> OBJECT ENCODING pi
 "embstr"


 每种类型的对象都至少使用了两种不同的编码， 
不同类型和编码的对象
 
 类型                       编码                                对象
 REDIS_STRING           REDIS_ENCODING_INT              使用整数值实现的字符串对象。 
 REDIS_STRING           REDIS_ENCODING_EMBSTR           使用 embstr 编码的简单动态字符串实现的字符串对象。 
 REDIS_STRING           REDIS_ENCODING_RAW              使用简单动态字符串实现的字符串对象。 
 
 REDIS_LIST             REDIS_ENCODING_ZIPLIST          使用压缩列表实现的列表对象。 
 REDIS_LIST             REDIS_ENCODING_LINKEDLIST       使用双端链表实现的列表对象。 
 
 REDIS_HASH             REDIS_ENCODING_ZIPLIST          使用压缩列表实现的哈希对象。 
 REDIS_HASH             REDIS_ENCODING_HT               使用字典实现的哈希对象。
 
 REDIS_SET              REDIS_ENCODING_INTSET           使用整数集合实现的集合对象。 
 REDIS_SET              REDIS_ENCODING_HT               使用字典实现的集合对象。 
 
 REDIS_ZSET             REDIS_ENCODING_ZIPLIST          使用压缩列表实现的有序集合对象。 
 REDIS_ZSET             REDIS_ENCODING_SKIPLIST         使用跳跃表和字典实现的有序集合对象。 
 
 使用 OBJECT ENCODING 命令可以查看一个数据库键的值对象的编码：
 
 OBJECT ENCODING 对不同编码的输出:
 
 对象所使用的底层数据结构                   编码常量                                OBJECT ENCODING 命令输出
 
     整数                                   REDIS_ENCODING_INT                          "int" 
     embstr 编码的简单动态字符串（SDS）     REDIS_ENCODING_EMBSTR                       "embstr"  //2.9版本没有该编码方式了
     简单动态字符串                         REDIS_ENCODING_RAW                          "raw" 
     字典                                   REDIS_ENCODING_HT                           "hashtable" 
     双端链表                               REDIS_ENCODING_LINKEDLIST                   "linkedlist" 
     压缩列表                               REDIS_ENCODING_ZIPLIST                      "ziplist" 
     整数集合                               REDIS_ENCODING_INTSET                       "intset" 
     跳跃表和字典                           REDIS_ENCODING_SKIPLIST                     "skiplist" 

    通过 encoding 属性来设定对象所使用的编码， 而不是为特定类型的对象关联一种固定的编码， 极大地提升了 Redis 的灵活性和效率， 因为 
Redis 可以根据不同的使用场景来为一个对象设置不同的编码， 从而优化对象在某一场景下的效率。
 
举个例子， 在列表对象包含的元素比较少时， Redis 使用压缩列表作为列表对象的底层实现：
因为压缩列表比双端链表更节约内存， 并且在元素数量较少时， 在内存中以连续块方式保存的压缩列表比起双端链表可以更快被载入到缓存中；
随着列表对象包含的元素越来越多， 使用压缩列表来保存元素的优势逐渐消失时， 对象就会将底层实现从压缩列表转向功能更强、也更适合保存大量元素的双端链表上面；


 * 对象编码
 *
 * 像 String 和 Hash 这样的对象，可以有多种内部表示。
 * 对象的 encoding 属性可以设置为以下域的任意一种。
 */


/* Objects encoding. Some kind of objects like Strings and Hashes can be
 * internally represented in multiple ways. The 'encoding' field of the object
 * is set to one of this fields for this object. */
// 对象编码


/* 参考5个对象REDIS_STRING，那边解释比较好 */


/*
如果字符串对象保存的是一个字符串值， 并且这个字符串值的长度大于 39 字节， 那么字符串对象将使用一个简单动态字符串（SDS）来保存这个字符串值， 并将对象的编码设置为 raw 。
*/
#define REDIS_ENCODING_RAW 0     /* Raw representation */ //REDIS_ENCODING_EMBSTR和REDIS_ENCODING_RAW编码方式区别见createStringObject
//如果key 或者value字符串长度小于39字节 REDIS_ENCODING_EMBSTR_SIZE_LIMIT，则使用该类型编码，见createStringObject
#define REDIS_ENCODING_EMBSTR 8  /* Embedded sds string encoding */  //REDIS_ENCODING_EMBSTR和REDIS_ENCODING_RAW编码方式区别见createStringObject


#define REDIS_ENCODING_INT 1     /* Encoded as integer */
#define REDIS_ENCODING_HT 2      /* Encoded as hash table */
//Redis引入了zipmap数据结构，保证在hashtable刚创建以及元素较少时，用更少的内存来存储，同时对查询的效率也不会受太大的影响
#define REDIS_ENCODING_ZIPMAP 3  /* Encoded as zipmap */

/*
编码转换

当列表对象可以同时满足以下两个条件时， 列表对象使用 ziplist 编码：
1.列表对象保存的所有字符串元素的长度都小于 64 字节；
2.列表对象保存的元素数量小于 512 个；

不能满足这两个条件的列表对象需要使用 linkedlist 编码。

lpush首先通过pushGenericCommand->createZiplistObject创建列表对象(默认编码方式REDIS_ENCODING_ZIPLIST)，然后在listTypePush->listTypeTryConversion中
会检查列表中节点数是否大于配置参数list_max_ziplist_value(默认64)，如果大于则将value列表值从压缩表改为双向链表编码方式REDIS_ENCODING_LINKEDLIST，见listTypePush->listTypeConvert
*/
#define REDIS_ENCODING_LINKEDLIST 4 /* Encoded as regular linked list */
/*
lpush首先通过pushGenericCommand->createZiplistObject创建列表对象(默认编码方式REDIS_ENCODING_ZIPLIST)，然后在listTypePush->listTypeTryConversion中
会检查列表中节点字符串长度是否大于配置参数list_max_ziplist_value(默认64)，如果大于则将value列表值从压缩表改为双向链表编码方式REDIS_ENCODING_LINKEDLIST，见listTypePush->listTypeConvert
*/
#define REDIS_ENCODING_ZIPLIST 5 /* Encoded as ziplist */



#define REDIS_ENCODING_INTSET 6  /* Encoded as intset */
//skiplist 编码的有序集合对象使用 zset 结构作为底层实现， 一个 zset 结构同时包含一个字典和一个跳跃表：
#define REDIS_ENCODING_SKIPLIST 7  /* Encoded as skiplist */

/* Defines related to the dump file format. To store 32 bits lengths for short
 * keys requires a lot of space, so we check the most significant 2 bits of
 * the first byte to interpreter the length:
 *
 * 00|000000 => if the two MSB are 00 the len is the 6 bits of this byte
 * 01|000000 00000000 =>  01, the len is 14 byes, 6 bits + 8 bits of next byte
 * 10|000000 [32 bit integer] => if it's 10, a full 32 bit len will follow
 * 11|000000 this means: specially encoded object will follow. The six bits
 *           number specify the kind of object that follows.
 *           See the REDIS_RDB_ENC_* defines.
 *
 * Lengths up to 63 are stored using a single byte, most DB keys, and may
 * values, will fit inside. */
#define REDIS_RDB_6BITLEN 0
#define REDIS_RDB_14BITLEN 1
#define REDIS_RDB_32BITLEN 2
#define REDIS_RDB_ENCVAL 3
#define REDIS_RDB_LENERR UINT_MAX

/* When a length of a string object stored on disk has the first two bits
 * set, the remaining two bits specify a special encoding for the object
 * accordingly to the following defines: */
#define REDIS_RDB_ENC_INT8 0        /* 8 bit signed integer */
#define REDIS_RDB_ENC_INT16 1       /* 16 bit signed integer */
#define REDIS_RDB_ENC_INT32 2       /* 32 bit signed integer */
#define REDIS_RDB_ENC_LZF 3         /* string compressed with FASTLZ */

/* AOF states */
#define REDIS_AOF_OFF 0             /* AOF is off */
#define REDIS_AOF_ON 1              /* AOF is on */  //需要创建进行AOF重写
#define REDIS_AOF_WAIT_REWRITE 2    /* AOF waits rewrite to start appending */

/* Client flags */
/*
在主从服务器进行复制操作时，主服务器会成为从服务器的客户端，而从服务器也会成为主服务器的客户端。REDIS_MASTER标志表示客户端代
表的是一个主服务器，REDIS—SLAVE标志表示客户端代表的是一个从服务器。
*/
//在monitorCommand  masterTryPartialResynchronization  syncCommand三个地方把redisClient->flag设置为REDIS_SLAVE
#define REDIS_SLAVE (1<<0)   /* This client is a slave server */
#define REDIS_MASTER (1<<1)  /* This client is a master server */ //标记这个客户端为主服务器，见readSyncBulkPayload
//标志表示客户端正在执行MONITOR命令。
#define REDIS_MONITOR (1<<2) /* This client is a slave monitor, see MONITOR */
//表示客户端正在执行事务。 //执行multi命令的时候，在multiCommand会设置该标记，表示后面会有多个命令输入，只有等exec命令到来在执行中间命令 multiCommand
#define REDIS_MULTI (1<<3)   /* This client is in a MULTI context */
//表示客户端正在被BRPOP、BLPOP等命令阻塞。
#define REDIS_BLOCKED (1<<4) /* The client is waiting in a blocking operation */
/*
REDIS_DIRTY_EXEC表示事务在命令人队时出现了错误，REDIS_DIRTY_CAS  REDIS_DIRTY_EXEC两个标志都表示事务的安全性已经被破坏，只要
这两个标记中的任意一个被打开，EXEC命令必然会执行失败。这两个标志只能在客户端打开了REDIS_MULTI标志的情况下使用。
*/
//监视机制触发见touchWatchedKey决定是否触发REDIS_DIRTY_CAS   取消事物函数更具watch的键是否有触发REDIS_DIRTY_CAS来决定是否继续执行事物中的命令，见execCommand
//表示事务使用WATCH命令监视的数据库键已经被修改，见touchWatchedKey  生效见execCommand //取消watch见unwatchAllKeys
#define REDIS_DIRTY_CAS (1<<5) /* Watched keys modified. EXEC will fail. */
/*
表示有用户对这个客户端执行了CLIENT KILL命令，或者客户端发送给服务器的命令请求中包含了错误的协议内容。服务器会将
客户端积存在输出缓冲区中的所有内容发送给客户端，煞后关闭客户端。
*/
#define REDIS_CLOSE_AFTER_REPLY (1<<6) /* Close after writing entire reply. */
//表示客户端已经从REDIS—BLOCKED标志所表示的阻塞状态中脱离出来，不再阻塞。REDIS—UNBLOCKED标志只能在REDIS—BLOCKED标志已经打开的情况下使用。
#define REDIS_UNBLOCKED (1<<7) /* This client was unblocked and is stored in
                                  server.unblocked_clients */
//REDIS_LUA_CLIENT标识表示客户端是专门用于处理Lua脚本里面包含的Redis命令的伪客户端。
#define REDIS_LUA_CLIENT (1<<8) /* This is a non connected client used by Lua */
//表示客户端向集群节点（运行在集群模式下的服务器）发送了ASKING命令。
#define REDIS_ASKING (1<<9)     /* Client issued the ASKING command */
/*
标志表示客户端的输出缓冲区大小超出了服务器允许的范围，服务器会在下一次执行serverCron函数时关闭这个客户端，以免服务器的稳定性
受到这个客户端影响。积存在输出缓冲区中的所有内容会直接被释放，不会返回给客户端。
*/
#define REDIS_CLOSE_ASAP (1<<10)/* Close this client ASAP */
//标志表示服务器使用UNIX套接字来连接客户端。
#define REDIS_UNIX_SOCKET (1<<11) /* Client connected via Unix domain socket */
/*
REDIS_DIRTY_EXEC表示事务在命令人队时出现了错误，REDIS_DIRTY_CAS  REDIS_DIRTY_EXEC两个标志都表示事务的安全性已经被破坏，只要这两个标记中的任意一个被打开，
EXEC命令必然会执行失败。这两个标志只能在客户端打开了REDIS_MULTI标志的情况下使用。
*/ //入队命令出错的时候置1，见flagTransaction,主要是没找到redisCommandTable中的命令
#define REDIS_DIRTY_EXEC (1<<12)  /* EXEC will fail for errors while queueing */
/*
在主从服务器进行命令传播期间，从服务器需要向主服务器发送REPLICATION ACK命令，在发送这个命令之前，从服务器必须打开主服务器对应的
客户端的REDIS_MASTER_FORCE_REPLY标志，否则发送操作会被拒绝执行。
*/
#define REDIS_MASTER_FORCE_REPLY (1<<13)  /* Queue replies even if is master */
/*
    通常情况下，Redis只会将那些对数据库进行了修改的命令写入到AOF文件，并复制到各个从服务器。如果一个命令没有对数据库进行任何修改，那么
它就会被认为是只读命令，这个命令不会被写入到AOF文伴，也不会被复制到从服务器。
    以上规则适用于绝大部分Redis命令，但PUBSUB命令和SCRIPT LOAD命令是其中的例外。PUBSUB命令虽然没有修改数据库，但PUBSUB命令向频道的
所有订阅者发送消息这一行为带有副作用，接收到消息的所有客户端的状态都会因为这个命令而改变。因此，服务器需要使用REDIS_FORCE_AOF标志，
强制将这个命令写入AOF文件，这样在将来载入AOF文件时，服务器就可以再次执行相同的PUBSUB命令，并产生相同的副作用。SCRIPT LOAD命令的情况
与PUBSUB命令类似
*/
#define REDIS_FORCE_AOF (1<<14)   /* Force AOF propagation of current cmd. */
/* 强制服务器将当前执行的命令写入到AOF文件里面，REDIS_FORCE_REPL标志强制主服务器将当前执行的命令复制给所有从服务器。
执行PUBSUB命令会使客户端打开REDIS_FORCE_AOF标志，执行SCRIPT LOAD命令会使客户端打开REDIS_FORCE_AOF标志和REDIS_FORCE_REPL标志。 */
#define REDIS_FORCE_REPL (1<<15)  /* Force replication of current cmd. */
//那么表示主服务器的版本低于 2.8 
// 无法使用 PSYNC ，所以需要设置相应的标识值
#define REDIS_PRE_PSYNC (1<<16)   /* Instance don't understand PSYNC. */
#define REDIS_READONLY (1<<17)    /* Cluster client is in read-only state. */

/* Client block type (btype field in client structure)
 * if REDIS_BLOCKED flag is set. */
#define REDIS_BLOCKED_NONE 0    /* Not blocked, no REDIS_BLOCKED flag set. */
#define REDIS_BLOCKED_LIST 1    /* BLPOP & co. */
#define REDIS_BLOCKED_WAIT 2    /* WAIT for synchronous replication. */

/* Client request types */
#define REDIS_REQ_INLINE 1 
#define REDIS_REQ_MULTIBULK 2 //一般客户端发送的请求中的第一个字节是*

/* Client classes for client limits, currently used only for
 * the max-client-output-buffer limit implementation. */
#define REDIS_CLIENT_LIMIT_CLASS_NORMAL 0
#define REDIS_CLIENT_LIMIT_CLASS_SLAVE 1
#define REDIS_CLIENT_LIMIT_CLASS_PUBSUB 2
#define REDIS_CLIENT_LIMIT_NUM_CLASSES 3

/* Slave replication state - from the point of view of the slave. */
#define REDIS_REPL_NONE 0 /* No active replication */
#define REDIS_REPL_CONNECT 1 /* Must connect to master */ //见replicationSetMaster  表示未连接状态
#define REDIS_REPL_CONNECTING 2 /* Connecting to master */ //见connectWithMaster
#define REDIS_REPL_RECEIVE_PONG 3 /* Wait for PING reply */ //从服务器连接主服务器成功后发送ping字符串给主服务器，然后进入该状态，表示等待主服务器应答PONG
#define REDIS_REPL_TRANSFER 4 /* Receiving .rdb from master */
#define REDIS_REPL_CONNECTED 5 /* Connected to master */ //已连接状态

/* Slave replication state - from the point of view of the master.
 * In SEND_BULK and ONLINE state the slave receives new updates
 * in its output queue. In the WAIT_BGSAVE state instead the server is waiting
 * to start the next background saving in order to send updates to it. */
#define REDIS_REPL_WAIT_BGSAVE_START 6 /* We need to produce a new RDB file. */
//表示已经创建子进程来进行bgsave了，等待写成功过程中
#define REDIS_REPL_WAIT_BGSAVE_END 7 /* Waiting RDB file creation to finish. */
//rdb文件重写成功后，开始进行网络传输，把rdb文件传输到slave节点，见updateSlavesWaitingBgsave
#define REDIS_REPL_SEND_BULK 8 /* Sending RDB file to slave. */ //开始发送bgsave保存的文件给从服务器端，见updateSlavesWaitingBgsave
#define REDIS_REPL_ONLINE 9 /* RDB file transmitted, sending just updates. */ //写rdb文件数据到客户端完成，见sendBulkToSlave

/* Synchronous read timeout - slave side */
#define REDIS_REPL_SYNCIO_TIMEOUT 5

/* List related stuff */
#define REDIS_HEAD 0
#define REDIS_TAIL 1

/* Sort operations */
#define REDIS_SORT_GET 0
#define REDIS_SORT_ASC 1
#define REDIS_SORT_DESC 2
#define REDIS_SORTKEY_MAX 1024

/* Log levels */
#define REDIS_DEBUG 0
#define REDIS_VERBOSE 1
#define REDIS_NOTICE 2
#define REDIS_WARNING 3
#define REDIS_LOG_RAW (1<<10) /* Modifier to log without timestamp */
#define REDIS_DEFAULT_VERBOSITY REDIS_NOTICE

/* Anti-warning macro... */
#define REDIS_NOTUSED(V) ((void) V)

#define ZSKIPLIST_MAXLEVEL 32 /* Should be enough for 2^32 elements */
#define ZSKIPLIST_P 0.25      /* Skiplist P = 1/4 */

/* Append only defines */
#define AOF_FSYNC_NO 0
#define AOF_FSYNC_ALWAYS 1
#define AOF_FSYNC_EVERYSEC 2
#define REDIS_DEFAULT_AOF_FSYNC AOF_FSYNC_EVERYSEC

/* Zip structure related defaults */
#define REDIS_HASH_MAX_ZIPLIST_ENTRIES 512
#define REDIS_HASH_MAX_ZIPLIST_VALUE 64
#define REDIS_LIST_MAX_ZIPLIST_ENTRIES 512
#define REDIS_LIST_MAX_ZIPLIST_VALUE 64
#define REDIS_SET_MAX_INTSET_ENTRIES 512
#define REDIS_ZSET_MAX_ZIPLIST_ENTRIES 128
#define REDIS_ZSET_MAX_ZIPLIST_VALUE 64

/* HyperLogLog defines */
#define REDIS_DEFAULT_HLL_SPARSE_MAX_BYTES 3000

/* Sets operations codes */
#define REDIS_OP_UNION 0
#define REDIS_OP_DIFF 1
#define REDIS_OP_INTER 2

/* Redis maxmemory strategies */
#define REDIS_MAXMEMORY_VOLATILE_LRU 0
#define REDIS_MAXMEMORY_VOLATILE_TTL 1
#define REDIS_MAXMEMORY_VOLATILE_RANDOM 2
#define REDIS_MAXMEMORY_ALLKEYS_LRU 3
#define REDIS_MAXMEMORY_ALLKEYS_RANDOM 4
#define REDIS_MAXMEMORY_NO_EVICTION 5
#define REDIS_DEFAULT_MAXMEMORY_POLICY REDIS_MAXMEMORY_NO_EVICTION

/* Scripting */
#define REDIS_LUA_TIME_LIMIT 5000 /* milliseconds */

/* Units */
#define UNIT_SECONDS 0 //单位是秒
#define UNIT_MILLISECONDS 1 //单位是毫秒

/* SHUTDOWN flags */
#define REDIS_SHUTDOWN_SAVE 1       /* Force SAVE on SHUTDOWN even if no save
                                       points are configured. */
#define REDIS_SHUTDOWN_NOSAVE 2     /* Don't SAVE on SHUTDOWN. */

/* Command call flags, see call() function */
#define REDIS_CALL_NONE 0
#define REDIS_CALL_SLOWLOG 1 //如果某个命令的执行时间超过了一定时间，则会记录一条慢日志
#define REDIS_CALL_STATS 2
#define REDIS_CALL_PROPAGATE 4
#define REDIS_CALL_FULL (REDIS_CALL_SLOWLOG | REDIS_CALL_STATS | REDIS_CALL_PROPAGATE)

/* Command propagation flags, see propagate() function */
#define REDIS_PROPAGATE_NONE 0
#define REDIS_PROPAGATE_AOF 1
#define REDIS_PROPAGATE_REPL 2

/* Keyspace changes notification classes. Every class is associated with a
 * character for configuration purposes. */ //notifyKeyspaceEvent
#define REDIS_NOTIFY_KEYSPACE (1<<0)    /* K */
#define REDIS_NOTIFY_KEYEVENT (1<<1)    /* E */
#define REDIS_NOTIFY_GENERIC (1<<2)     /* g */ //表示这是一个通用类型通知
#define REDIS_NOTIFY_STRING (1<<3)      /* $ */
#define REDIS_NOTIFY_LIST (1<<4)        /* l */
#define REDIS_NOTIFY_SET (1<<5)         /* s */  //表示这是一个集合键通知
#define REDIS_NOTIFY_HASH (1<<6)        /* h */
#define REDIS_NOTIFY_ZSET (1<<7)        /* z */
#define REDIS_NOTIFY_EXPIRED (1<<8)     /* x */
#define REDIS_NOTIFY_EVICTED (1<<9)     /* e */
#define REDIS_NOTIFY_ALL (REDIS_NOTIFY_GENERIC | REDIS_NOTIFY_STRING | REDIS_NOTIFY_LIST | REDIS_NOTIFY_SET | REDIS_NOTIFY_HASH | REDIS_NOTIFY_ZSET | REDIS_NOTIFY_EXPIRED | REDIS_NOTIFY_EVICTED)      /* A */

/* Using the following macro you can run code inside serverCron() with the
 * specified period, specified in milliseconds.
 * The actual resolution depends on server.hz. */
 //表示多少ms这个条件满足，如果从现在开始经过ms毫秒，则满足条件  通过serverCron(100ms执行一次)循环次数来计算，参数ms一般是100的整数倍
#define run_with_period(_ms_) if ((_ms_ <= 1000/server.hz) || !(server.cronloops%((_ms_)/(1000/server.hz))))

/* We can print the stacktrace, so our assert is defined this way: */
#define redisAssertWithInfo(_c,_o,_e) ((_e)?(void)0 : (_redisAssertWithInfo(_c,_o,#_e,__FILE__,__LINE__),_exit(1)))
#define redisAssert(_e) ((_e)?(void)0 : (_redisAssert(#_e,__FILE__,__LINE__),_exit(1)))
#define redisPanic(_e) _redisPanic(#_e,__FILE__,__LINE__),_exit(1)

/*-----------------------------------------------------------------------------
 * Data types
 *----------------------------------------------------------------------------*/

typedef long long mstime_t; /* millisecond time type. */

/* A redis object, that is a type able to hold a string / list / set */

/* The actual Redis Object */
/*
 * Redis 对象
 */
#define REDIS_LRU_BITS 24
#define REDIS_LRU_CLOCK_MAX ((1<<REDIS_LRU_BITS)-1) /* Max value of obj->lru */
#define REDIS_LRU_CLOCK_RESOLUTION 1000 /* LRU clock resolution in ms */

/*
    首先命令行中的字符串先保存到进入xxxCommand(如setCommand)函数时，各个命令字符串单纯已经转换为redisObject保存到redisClient->argv[]中，
    然后在setKey等相关函数中把key-value转换为dictEntry节点的key和v(分别对应key和value)添加到dict->dictht->table[]  hash中,至于怎么添加到dictEntry
    节点的key和value中可以参考dict->type( type为xxxDictType 例如keyptrDictType等) ，见dictCreate
*/

typedef struct redisObject { 

    /*
     REDIS_STRING 字符串对象 
     REDIS_LIST 列表对象 
     REDIS_HASH 哈希对象 
     REDIS_SET 集合对象 
     REDIS_ZSET 有序集合对象 
     */
    // 类型
    unsigned type:4;

    // 编码 // 编码方式  encoding 属性记录了对象所使用的编码， 也即是说这个对象使用了什么数据结构作为对象的底层实现
    unsigned encoding:4;

    /*
    除了可以被 OBJECT IDLETIME 命令打印出来之外， 键的空转时长还有另外一项作用： 如果服务器打开了 maxmemory 选项， 并且服务器
    用于回收内存的算法为 volatile-lru 或者 allkeys-lru ， 那么当服务器占用的内存数超过了 maxmemory 选项所设置的上限值时， 空转
    时长较高的那部分键会优先被服务器释放， 从而回收内存。
     */
    // LRU 时间（相对于 server.lruclock） OBJECT IDLETIME 命令可以打印出给定键的空转时长， 这一空转时长就是通过将当前时间减去键的值对象的 lru 时间计算得出的
    // 对象最后一次被访问的时间  // 该属性记录了对象最后一次被命令程序访问的时间： 10s统计一次
    unsigned lru:REDIS_LRU_BITS; /* lru time (relative to server.lruclock) */

    /*
     incrRefCount 将对象的引用计数值增一。 
     decrRefCount 将对象的引用计数值减一， 当对象的引用计数值等于 0 时， 释放对象。 
     resetRefCount 将对象的引用计数值设置为 0 ， 但并不释放对象， 这个函数通常在需要重新设置对象的引用计数值时使用。 

     尽管共享更复杂的对象可以节约更多的内存， 但受到 CPU 时间的限制， Redis 只对包含整数值的字符串对象进行共享。
     */
    // 引用计数 程序可以通过跟踪对象的引用计数信息， 在适当的时候自动释放对象并进行内存回收。 
    // 引用计数
    int refcount;

    /*
       如果采用REDIS_ENCODING_INT编码方式，ptr指向一个int 4字节内存空间，来存储设置的int数据。
       如果是REDIS_ENCODING_RAW,则ptr指向的是一个sdshdr结构 
       其他表示对象的编码类型与这个类似
     */
    // 指向对象的值  对象的 ptr 指针指向对象的底层实现数据结构， 而这些数据结构由对象的 encoding 属性决定。
    // 指向实际值的指针  参考调用createObject的地方赋值
    //可以参考下createStringObject(ptr指向sdsnewlen)  createListObject(ptr指向list) 等等，每种type类型可以多种编码方式
    void *ptr;//如果是大于10000的字符串，则直接转换为REDIS_ENCODING_INT编码方式存储，见tryObjectEncoding，直接用这个指针存储对应的字符串地址

} robj;

/* Macro used to obtain the current LRU clock.
 * If the current resolution is lower than the frequency we refresh the
 * LRU clock (as it should be in production servers) we return the
 * precomputed value, otherwise we need to resort to a function call. */
#define LRU_CLOCK() ((1000/server.hz <= REDIS_LRU_CLOCK_RESOLUTION) ? server.lruclock : getLRUClock())

/* Macro used to initialize a Redis object allocated on the stack.
 * Note that this macro is taken near the structure definition to make sure
 * we'll update it when the structure is changed, to avoid bugs like
 * bug #85 introduced exactly in this way. */
#define initStaticStringObject(_var,_ptr) do { \
    _var.refcount = 1; \
    _var.type = REDIS_STRING; \
    _var.encoding = REDIS_ENCODING_RAW; \
    _var.ptr = _ptr; \
} while(0);

/* To improve the quality of the LRU approximation we take a set of keys
 * that are good candidate for eviction across freeMemoryIfNeeded() calls.
 *
 * Entries inside the eviciton pool are taken ordered by idle time, putting
 * greater idle times to the right (ascending order).
 *
 * Empty entries have the key pointer set to NULL. */
#define REDIS_EVICTION_POOL_SIZE 16
struct evictionPoolEntry {
    unsigned long long idle;    /* Object idle time. */
    sds key;                    /* Key name. */
};

/* Redis database representation. There are multiple databases identified
 * by integers from 0 (the default database) up to the max configured
 * database. The database number is the 'id' field in the structure. */
typedef struct redisDb { //初始化赋值见initServer  //键---值存储过程数据结构参考<redis涉及与实现9.3>

    // 数据库键空间，保存着数据库中的所有键值对 // key space，包括键值对象 redisDb结构的dict字典保存了数据库中的所有键值对，我们将这个字典称为键空间
    //server.db[j].dict = dictCreate(&dbDictType,NULL);
    dict *dict;                 /* The keyspace for this DB */

    /*
    redisDb结构的expires字典保存了数据库中所有键的过期时间，我们称这个字典为过期字典：
    过期字典的键是一个指针，这个指针指向键空间中的某个键对象（也即是某个数据库键）。
    过期字典的值是一个long long类型的整数，这个整数保存了键所指向的数据库键的过期时间——个毫秒精度的UNIX时间戳。
    键空间的键和过期字典的键都指向同一个键对象，所以不会出现任何重复对象，也不会浪费任何空间。
     */
    // 键的过期时间，字典的键为键，字典的值为过期事件 UNIX 时间戳 server.db[j].expires = dictCreate(&keyptrDictType,NULL);
    dict *expires;              /* Timeout of keys with a timeout set */

    // 正处于阻塞状态的键 server.db[j].blocking_keys = dictCreate(&keylistDictType,NULL);
    dict *blocking_keys;        /* Keys with clients waiting for data (BLPOP) */

    // 可以解除阻塞的键 server.db[j].ready_keys = dictCreate(&setDictType,NULL);
    dict *ready_keys;           /* Blocked keys that received a PUSH */

    /*
     每个Redis数据库都保存着一个watched_keys字典，这个字典的键是某个被WATCH命令监视的数据库键，而字典的值则是—个链表，
     链表中记录了所有监视相应数据库键的客户端
     */
    // 正在被 WATCH 命令监视的键 server.db[j].watched_keys = dictCreate(&keylistDictType,NULL);
    dict *watched_keys;         /* WATCHED keys for MULTI/EXEC CAS */

    struct evictionPoolEntry *eviction_pool;    /* Eviction pool of keys */

    // 数据库号码
    int id;                     /* Database ID */

    // 数据库的键的平均 TTL ，统计信息  相当于该数据库中所有键值对的平均过期时间，单位ms
    long long avg_ttl;          /* Average TTL, just for stats */

} redisDb;

/* Client MULTI/EXEC state */

/*
 * 事务命令
 */ 
//例如执行multi;  get xx; set tt xx; exec这四个命令，则multi和set tt xx两个命令就各自用一个multiCmd结构保存(commands数组中的两个成语结构)，count = 2
typedef struct multiCmd {

    // 参数
    robj **argv;

    // 参数数量
    int argc;

    // 命令指针
    struct redisCommand *cmd;

} multiCmd;

/*
 * 事务状态
 */ 
//例如执行multi;  get xx; set tt xx; exec这四个命令，则multi和set tt xx两个命令就各自用一个multiCmd结构保存(commands数组中的两个成语结构)，count = 2
typedef struct multiState {

    // 事务队列，FIFO 顺序
    multiCmd *commands;     /* Array of MULTI commands */

    // 已入队命令计数
    int count;              /* Total number of MULTI commands */
    int minreplicas;        /* MINREPLICAS for synchronous replication */
    time_t minreplicas_timeout; /* MINREPLICAS timeout as unixtime. */
} multiState; //数据结构源头在redisClient->mstate

/* This structure holds the blocking operation state for a client.
 * The fields used depend on client->btype. */
// 阻塞状态
typedef struct blockingState {

    /* Generic fields. */
    // 阻塞时限
    mstime_t timeout;       /* Blocking operation timeout. If UNIX current time
                             * is > timeout then the operation timed out. */

    /* REDIS_BLOCK_LIST */
    // 造成阻塞的键
    dict *keys;             /* The keys we are waiting to terminate a blocking
                             * operation such as BLPOP. Otherwise NULL. */
    // 在被阻塞的键有新元素进入时，需要将这些新元素添加到哪里的目标键
    // 用于 BRPOPLPUSH 命令
    robj *target;           /* The key that should receive the element,
                             * for BRPOPLPUSH. */

    /* REDIS_BLOCK_WAIT */
    // 等待 ACK 的复制节点数量
    int numreplicas;        /* Number of replicas we are waiting for ACK. */
    // 复制偏移量
    long long reploffset;   /* Replication offset to reach. */

} blockingState;

/* The following structure represents a node in the server.ready_keys list,
 * where we accumulate all the keys that had clients blocked with a blocking
 * operation such as B[LR]POP, but received new data in the context of the
 * last executed command.
 *
 * After the execution of every command or script, we run this list to check
 * if as a result we should serve data to clients blocked, unblocking them.
 * Note that server.ready_keys will not have duplicates as there dictionary
 * also called ready_keys in every structure representing a Redis database,
 * where we make sure to remember if a given key was already added in the
 * server.ready_keys list. */
// 记录解除了客户端的阻塞状态的键，以及键所在的数据库。
typedef struct readyList {
    redisDb *db;
    robj *key;
} readyList;


/*
get a ab 对应的报文为如下;

*3
$3
get
$1
a
$2
ab

第一个参数，也就是*开始相当于argc， 表示后续有几个对象，$后面的数字表示紧跟后面的字符串有几个字节。如get a ab，一共3个，分别是get ,a,b。 
$3表示后面的get是3个字节。
*/


/* With multiplexing we need to take per-client state.
 * Clients are taken in a liked list.
 *
   执行CLIENT list命令可以列出目前所有连接到服务器的普通客户端，命令输出中的fd域显示了服务器连接客户端所使用的套接字描述符：
    reciis> ClIENT list
    addr=127.0.0.1:53428  fd=6  name=  age=1242jdle=0…
    addr-127.0.0.1:53469  fd=7  name=  age=4  idle=4～
    
 * 因为 I/O 复用的缘故，需要为每个客户端维持一个状态。
 *
 * 多个客户端状态被服务器用链表连接起来。
 */ //创建及赋值见createClient
typedef struct redisClient {   //redisServer与redisClient对应

    /*
        伪客户端( fake client)的fd属性的值为-1:伪客户端处理的命令请求来源于AOF文件或者Lua脚本，而不是网络，所以这种客户端不需要套接
    字连接，自然也不需要记录套接字描述符。目前Redis服务器会在两个地方用到伪客户端，一个用于载人AOF文件并还原数据库状态，而另一个
    则用于执行Lua脚本中包含的Redis命令。
        普通客户端的fd属性的值为大于-1的整数：普通客户端使用套接字来与服务器进行通信，所以服务器会用fd属性来记录客户端套接字的描述符。
    因为合法的套接字描述符不能是-1，所以普通客户端的套接字描述符的值必然是
     */
    // 套接字描述符
    int fd;
    char cip[REDIS_IP_STR_LEN];
    int cport;
    
    // 指向当前目标数据库的指针 redisClient->db指针指向redisSerrer->db[]数组的其中一个元素，而被指向的元素就是客户端的目标数据库。
    // 当前正在使用的数据库  在接收客户端连接后，默认在createClient-> selectDb(c,0); //默认选择select 0
    redisDb *db;//客户端当前正在使用的数据库  通过select切换指向对应的数据库

    // 当前正在使用的数据库的 id （号码）
    int dictid;

    // 客户端的名字 默认情况下，一个连接到服务器的客户端是没有名字的。 可以通过CLIENT setname命令设置客户端名
    robj *name;             /* As set by CLIENT SETNAME */

    /*
    客户端状态的输入缓坤区用于保存客户端发送的命令请求：
    举个例子，如果客户端向服务器发送了以下命令请求：
    SET key value
    那么客户端状态的querybuf属性将是一个包含以下内容的SDS值：
    ·3 \r\n$ 3\r\nSET\r\n$3\ r\nkey\r\n$ 5\r\nvalue\r\n

    输入缓冲区的大小会根据输入内容动态地缩小或者扩大，但它的最大大小不能超过1GB(REDIS_MAX_QUERYBUF_LEN)，否则服务器将关闭这个客户端。
     */
    // 查询缓冲区  默认空间大小REDIS_IOBUF_LEN，见readQueryFromClient
    sds querybuf;//解析出的参数存入下面的argc和argv中   

    // 查询缓冲区长度峰值  querybuf读缓冲区中读取到的客户端最大数据长度   querybuf缓冲区内容长度的峰值
    size_t querybuf_peak;   /* Recent (100ms or more) peak of querybuf size */

    /*
    在服务器将客户端发送的命令请求保存到客户端状态的querybuf属性之后，服务器将对命令请求的内容进行分析，并将得出的命令参数以及
命令参数的个数分别保存到客户端状态的argv属性和argc属性：例如客户端命令是:set yang xxx，则argc=3,argv[0]为set，argv[1]为yang argv[2]为xxx
     */
    // 参数数量
    int argc;   //客户端命令解析见processMultibulkBuffer

    /*
    首先命令行中的字符串先保存到进入xxxCommand(如setCommand)函数时，各个命令字符串单纯已经转换为redisObject保存到redisClient->argv[]中，
    然后在setKey等相关函数中把key-value转换为dictEntry节点的key和v(分别对应key和value)添加到dict->dictht->table[]  hash中,至于怎么添加到dictEntry
    节点的key和value中可以参考dict->type( type为xxxDictType 例如keyptrDictType等) ，见dictCreate
*/
    // 参数对象数组  resetClient->freeClientArgv中释放空间
    robj **argv; //客户端命令解析见processMultibulkBuffer  创建空间和赋值见processMultibulkBuffer，有多少个参数数量multibulklen，就创建多少个robj(redisObject)存储参数的结构

    /*
    当程序在命令表中成功找到argv[0]所对应的redisCommand结构时，它会将客户端状态的cmd指针指向这个结构：
    //redisServer->orig_commands redisServer->commands(见populateCommandTable)命令表字典中的元素内容从redisCommandTable中获取，见 processCommand->lookupCommand查找
 //实际客户端命令的查找在函数lookupCommandOrOriginal中
     */
    // 记录被客户端执行的命令  processCommand->lookupCommand查找
    struct redisCommand *cmd, *lastcmd;

    // 请求的类型：内联命令还是多条命令
    int reqtype;

    // 剩余未读取的命令内容数量  参数数量，例如select tt,则multibulklen=2，set yang xxx，则则multibulklen=3
    int multibulklen;       /* number of multi bulk arguments left to read */ //没解析到一个set或者yang或者xxx，则该值减1，为0表示解析完毕，成功，见

    // 命令内容的长度
    long bulklen;           /* length of bulk argument in multi bulk request */

    /*
     执行命令所得的命令回复会被保存在客户端状态的输出缓冲区里面，每个客户端都有两个输出缓冲区可用，一个缓冲区的大小是固定的，
 另一个缓冲区的大小是可变的：口固定大小的缓冲区用于保存那些长度比较小的回复，比如OK、简短的字符串值、整数值、错误回复等等。
    可变大小的缓冲区用于保存那些长度比较大的回复，比如一个非常长的字符串值，一个由很多项组成的列表，一个包含了很多元素的集合等等。
    客户端的固定大小缓冲区由buf和bufpos两个属性组成： 动态缓冲区为edisClient->reply
     */
    // 回复链表
    list *reply;

    // 回复链表中对象的总大小  参考_addReplyObjectToList
    unsigned long reply_bytes; /* Tot bytes of objects in reply list */

    // 已发送字节，处理 short write 用
    int sentlen;            /* Amount of bytes already sent in the current
                               buffer or object being sent. */

    // 创建客户端的时间 ctime属性记录了创建客户端的时间，这个时间可以用来计算客户端与服务器已经连接了多少秒，CLIENT list命令的age域记录了这个秒数：
    time_t ctime;           /* Client creation time */

    // 客户端最后一次和服务器互动的时间
    time_t lastinteraction; /* time of the last interaction, used for timeout */

    /*
      输出缓冲区的时候提到过，可变大小缓冲区由一个链表和任意多个字符串对象组成，理论上来说，这个缓冲区可以保存任意长的命令回复。
  但是，为了避免客户端的回复过大，占用过多的服务器资源，服务器会时刻检查客户端的输出缓冲区的大小，并在缓冲区的大小超出范围时，
  执行相应的限制操作。
    服务器使用两种模式来限制客户端输出缓冲区的大小：
    硬性限制( hard limit)：如果输出缓冲区的大小超过了硬性限制所设置的大小，那么服务器立即关闭客户端。
    软性限制( soft limit)：如果输出缓冲区的大小超过了软性限制所设置的大小，但还没超过硬性限制，那么服务器将使用客户端状态结构的
    obuf_soft_limit_reached_time属性记录下客户端到达软性限制的起始时间；之后服务器会继续监视客户端，如果输出缓冲区的大小一直超出
    软性限制，并且持续时间超过服务器设定的时长，那么服务器将关闭客户端；相反地，如果输出缓冲区的大小在指定时间乏内，不再超出软性限制，
    那么客户端就不会被关闭，并且obuf_soft_limit_reached_time属性的值也会被清零。client-output-buffer-limit进行配置

     使用client-output-buffer-limit选项可以为普通客户端、从服务器客户端、执行发布与订阅功能的客户端分别设置不同的软性限制和硬性限制，该选项的格式为：
    client-output-buffer-limit <class><hard limit> <soft limit> <soft seconds>
    以下是三个设置示例：
    client-output-buffer-limit normal 0 0 0
    client-output-buffer-limit slave 256mb 64mb 60
    cliant-output-buffer-limit pubsub 32mb 8mb 60
    第一行设置将普通客户端的硬性限制和软性限制都设置为O，表示不限制客户端的输出缓冲区大小。
    第二行设置将从服务器客户端的硬性限制设置为256 MB，而软性限制设置为64 MB，软性限制的时长为60秒。
    第三行设置将执行发布与订阅功能的客户端的硬性限制设置为32 MB，软性限制设置为8 MB，软性限制的时长为60秒。
     */
    // 客户端的输出缓冲区超过软性限制的时间 记录了输出缓冲区第一次到达软性限制( soft limit)的时间
    time_t obuf_soft_limit_reached_time;

    // 客户端状态标志
    int flags;              /* REDIS_SLAVE | REDIS_MONITOR | REDIS_MULTI ... */

    // 当 server.requirepass 不为 NULL 时
    // 代表认证的状态
    // 0 代表未认证， 1 代表已认证    如果配置文件中requirepass 配置认证密码，则客户端需要AUTH 命令进行认证
    int authenticated;      /* when requirepass is non-NULL */

    // 复制状态  REDIS_REPL_WAIT_BGSAVE_END等
    int replstate;          /* replication state if this is a slave */
    // 用于保存主服务器传来的 RDB 文件的文件描述符
    int repldbfd;           /* replication DB file descriptor */

    // 读取主服务器传来的 RDB 文件的偏移量
    off_t repldboff;        /* replication DB file offset */
    // 主服务器传来的 RDB 文件的大小
    off_t repldbsize;       /* replication DB file size */

    //slave->replpreamble = sdscatprintf(sdsempty(),"$%lld\r\n", (unsigned long long) slave->repldbsize);
    //// 主服务器传来的 RDB 文件的大小字符串形式slave->repldbsize
    sds replpreamble;       /* replication DB preamble. */

    // 主服务器的复制偏移量  先前连接断开前接受到的主服务器数据量，下次再次连接到主服务器后，则把这个偏移量传过去，主服务器就知道应该从上面地方接着传
    //见slaveTryPartialResynchronization   //备服务器只有在接收到完整的主服务器的RDB文件后，才会更新该值，并且在实时数据过来的时候也需要更新该值。
    //如果该redis备启动后，正在同步rdb文件，同步一般RDB文件，这时候网络异常，则再次连接上后还是需要进行完整RDB同步，
    //因为只有同步了完整的RDB文件后才会更新偏移量reploff，见slaveTryPartialResynchronization 
    long long reploff;      /* replication offset if this is our master */
    // 从服务器最后一次发送 REPLCONF ACK 时的偏移量
    long long repl_ack_off; /* replication ack offset, if this is a slave */
    // 从服务器最后一次发送 REPLCONF ACK 的时间   赋值见replconfCommand
    long long repl_ack_time;/* replication ack time, if this is a slave */
    // 主服务器的 master run ID
    // 保存在客户端，用于执行部分重同步
    char replrunid[REDIS_RUN_ID_SIZE+1]; /* master run id if this is a master */
    // 从服务器的监听端口号  赋值见replconfCommand  从服务器连接主服务器的时候slaveof 

    //slave_listening_port的作用是在执行info replication命令时打印出从服务器的端口号。
    // 表示从服务器作为服务器端，等待redis-cli客户端连接的时候，服务器端的监听端口，见initServer listenToPort(server.port,server.ipfd,&server.ipfd_count) == REDIS_ERR)
    int slave_listening_port; /* As configured with: SLAVECONF listening-port */

    // 事务状态
    multiState mstate;      /* MULTI/EXEC state */

    // 阻塞类型
    int btype;              /* Type of blocking op if REDIS_BLOCKED. */
    // 阻塞状态
    blockingState bpop;     /* blocking state */

    // 最后被写入的全局复制偏移量
    long long woff;         /* Last write global replication offset. */

    // 被监视的键
    list *watched_keys;     /* Keys WATCHED for MULTI/EXEC CAS */

    // 这个字典记录了客户端所有订阅的频道
    // 键为频道名字，值为 NULL
    // 也即是，一个频道的集合
    dict *pubsub_channels;  /* channels a client is interested in (SUBSCRIBE) */

    // 链表，包含多个 pubsubPattern 结构
    // 记录了所有订阅频道的客户端的信息
    // 新 pubsubPattern 结构总是被添加到表尾
    list *pubsub_patterns;  /* patterns a client is interested in (SUBSCRIBE) */
    sds peerid;             /* Cached peer ID. */

    /*
     执行命令所得的命令回复会被保存在客户端状态的输出缓冲区里面，每个客户端都有两个输出缓冲区可用，一个缓冲区的大小是固定的，
 另一个缓冲区的大小是可变的：口固定大小的缓冲区用于保存那些长度比较小的回复，比如OK、简短的字符串值、整数值、错误回复等等。
    可变大小的缓冲区用于保存那些长度比较大的回复，比如一个非常长的字符串值，一个由很多项组成的列表，一个包含了很多元素的集合等等。
    客户端的固定大小缓冲区由buf和bufpos两个属性组成： 动态缓冲区为edisClient->reply
     */
    /* Response buffer */
    // 回复偏移量
    int bufpos;
    // 回复缓冲区
    char buf[REDIS_REPLY_CHUNK_BYTES];

} redisClient;

// 服务器的保存条件（BGSAVE 自动执行的条件）
struct saveparam {

    // 多少秒之内
    time_t seconds;

    // 发生多少次修改
    int changes;

};

// 通过复用来减少内存碎片，以及减少操作耗时的共享对象
struct sharedObjectsStruct { //对应字符串见createSharedObjects
    robj *crlf, *ok, *err, *emptybulk, *czero, *cone, *cnegone, *pong, *space,
    *colon, *nullbulk, *nullmultibulk, *queued,
    *emptymultibulk, *wrongtypeerr, *nokeyerr, *syntaxerr, *sameobjecterr,
    *outofrangeerr, *noscripterr, *loadingerr, *slowscripterr, *bgsaveerr,
    *masterdownerr, *roslaveerr, *execaborterr, *noautherr, *noreplicaserr,
    *busykeyerr, *oomerr, *plus, *messagebulk, *pmessagebulk, *subscribebulk,
    *unsubscribebulk, *psubscribebulk, *punsubscribebulk, *del, *rpop, *lpop,
    *lpush, *emptyscan, *minstring, *maxstring,
    *select[REDIS_SHARED_SELECT_CMDS],
    *integers[REDIS_SHARED_INTEGERS],
    *mbulkhdr[REDIS_SHARED_BULKHDR_LEN], /* "*<value>\r\n" */
    *bulkhdr[REDIS_SHARED_BULKHDR_LEN];  /* "$<value>\r\n" */
};

/* ZSETs use a specialized version of Skiplists */
/*
 * 跳跃表节点
 */
/*
跳跃表 API


函数                                            作用                                                时间复杂度


zslCreate               创建一个新的跳跃表。                                        O(1) 
zslFree                 释放给定跳跃表，以及表中包含的所有节点。                    O(N) ， N 为跳跃表的长度。 
zslInsert               将包含给定成员和分值的新节点添加到跳跃表中。                平均 O(\log N) ，最坏 O(N) ， N 为跳跃表长度。 
zslDelete               删除跳跃表中包含给定成员和分值的节点。                      平均 O(\log N) ，最坏 O(N) ， N 为跳跃表长度。 
zslGetRank              返回包含给定成员和分值的节点在跳跃表中的排位。              平均 O(\log N) ，最坏 O(N) ， N 为跳跃表长度。 
zslGetElementByRank     返回跳跃表在给定排位上的节点。                              平均 O(\log N) ，最坏 O(N) ， N 为跳跃表长度。 
zslIsInRange            给定一个分值范围（range）， 比如 0 到 15 ， 20 到 28 ，
                        诸如此类， 如果给定的分值范围包含在跳跃表的分值范围之内， 
                        那么返回 1 ，否则返回 0 。                                     通过跳跃表的表头节点和表尾节点， 这个检测可以用 O(1) 复杂度完成。 
zslFirstInRange         给定一个分值范围， 返回跳跃表中第一个符合这个范围的节点。      平均 O(\log N) ，最坏 O(N) 。 N 为跳跃表长度。 
zslLastInRange          给定一个分值范围， 返回跳跃表中最后一个符合这个范围的节点。    平均 O(\log N) ，最坏 O(N) 。 N 为跳跃表长度。 
zslDeleteRangeByScore   给定一个分值范围， 删除跳跃表中所有在这个范围之内的节点。      O(N) ， N 为被删除节点数量。 
zslDeleteRangeByRank    给定一个排位范围， 删除跳跃表中所有在这个范围之内的节点。      O(N) ， N 为被删除节点数量。 
*/

/* ZSETs use a specialized version of Skiplists */
/*
 * 跳跃表节点
 在同一个跳跃表中， 各个节点保存的成员对象必须是唯一的， 但是多个节点保存的分值却可以是相同的： 分值相同的节点将按照成员
 对象在字典序中的大小来进行排序， 成员对象较小的节点会排在前面（靠近表头的方向）， 而成员对象较大的节点则会排在后面（靠
 近表尾的方向）。
 */ //跳跃表可以参考http://blog.csdn.net/ict2014/article/details/17394259

typedef struct zskiplistNode {

     // 表头节点也有后退指针、分值和成员对象， 不过表头节点的这些属性都不会被用到
    // member 对象      节点所保存的成员对象。
    robj *obj;

    // 分值 // 分值 在跳跃表中，节点按各自所保存的分值从小到大排列。 跳跃表中的所有zskiplistNode节点都按分值从小到大来排序。
    double score;

    // 后退指针
    struct zskiplistNode *backward;

    // 层
    /*
     节点中用 L1 、 L2 、 L3 等字样标记节点的各个层， L1 代表第一层， L2 代表第二层，以此类推。每个层都带有两个属性：前进指针和跨度。

     跳跃表节点的 level 数组可以包含多个元素， 每个元素都包含一个指向其他节点的指针， 程序可以通过这些层来加快访问其他节点的
     速度， 一般来说， 层的数量越多， 访问其他节点的速度就越快。
     
     每次创建一个新跳跃表节点的时候， 程序都根据幂次定律 （power law，越大的数出现的概率越小） 随机生成一个介于 1 和 32 之间
     的值作为 level 数组的大小， 这个大小就是层的“高度”。
     */
    struct zskiplistLevel {

        // 前进指针
        // 前进指针 前进指针用于访问位于表尾方向的其他节点  当程序从表头向表尾进行遍历时，访问会沿着层的前进指针进行。
        struct zskiplistNode *forward;

        // 跨度
        // 这个层跨越的节点数量  跨度则记录了前进指针所指向节点和当前节点的距离。
        unsigned int span;

    } level[]; //创建新的zskiplistNode节点的时候，level层数数组[]大小时随机产生的，见zslInsert->zslRandomLevel

} zskiplistNode; //存储在zskiplist跳跃表结构中

/*
 * 跳跃表
 */
/*
header ：指向跳跃表的表头节点。
?tail ：指向跳跃表的表尾节点。
?level ：记录目前跳跃表内，层数最大的那个节点的层数（表头节点的层数不计算在内）。
?length ：记录跳跃表的长度，也即是，跳跃表目前包含节点的数量（表头节点不计算在内）。
*/ //跳跃表可以参考http://blog.csdn.net/ict2014/article/details/17394259
typedef struct zskiplist {

    // 表头节点和表尾节点 // 头节点，尾节点  注意在创建zskiplist的时候默认有创建一个头节点，见zslCreate
    struct zskiplistNode *header, *tail;

    // 表中节点的数量
    unsigned long length;

    // 目前表内节点的最大层数 level 属性则用于在 O(1) 复杂度内获取跳跃表中层高最大的那个节点的层数量， 注意表头节点的层高并不计算在内。
    //创建zskiplist的时候zslCreate中默认置1
    int level; //创建新的zskiplistNode节点的时候，level层数数组[]大小时随机产生的，见zslInsert->zslRandomLevel

} zskiplist;

/*
为什么有序集合需要同时使用跳跃表和字典来实现？

在理论上来说， 有序集合可以单独使用字典或者跳跃表的其中一种数据结构来实现， 但无论单独使用字典还是跳跃表， 在性能上对
比起同时使用字典和跳跃表都会有所降低。

举个例子， 如果我们只使用字典来实现有序集合， 那么虽然以 O(1) 复杂度查找成员的分值这一特性会被保留， 但是， 因为字典以
无序的方式来保存集合元素， 所以每次在执行范围型操作 —— 比如 ZRANK 、 ZRANGE 等命令时， 程序都需要对字典保存的所有元
素进行排序， 完成这种排序需要至少 O(N \log N) 时间复杂度， 以及额外的 O(N) 内存空间 （因为要创建一个数组来保存排序后的元素）。

另一方面， 如果我们只使用跳跃表来实现有序集合， 那么跳跃表执行范围型操作的所有优点都会被保留， 但因为没有了字典， 所以
根据成员查找分值这一操作的复杂度将从 O(1) 上升为 O(\log N) 。

因为以上原因， 为了让有序集合的查找和范围型操作都尽可能快地执行， Redis 选择了同时使用字典和跳跃表两种数据结构来实现有序集合。
*/
/*
 * 有序集
 */

typedef struct zset {

    // 字典，键为成员，值为分值
    // 用于支持 O(1) 复杂度的按成员取分值操作
    dict *dict;

    // 跳跃表，按分值排序成员
    // 用于支持平均复杂度为 O(log N) 的按分值定位成员操作
    // 以及范围操作
    zskiplist *zsl;

} zset;

// 客户端缓冲区限制
typedef struct clientBufferLimitsConfig {
    // 硬限制
    unsigned long long hard_limit_bytes;
    // 软限制
    unsigned long long soft_limit_bytes;
    // 软限制时限
    time_t soft_limit_seconds;
} clientBufferLimitsConfig;

// 限制可以有多个
extern clientBufferLimitsConfig clientBufferLimitsDefaults[REDIS_CLIENT_LIMIT_NUM_CLASSES];

/* The redisOp structure defines a Redis Operation, that is an instance of
 * a command with an argument vector, database ID, propagation target
 * (REDIS_PROPAGATE_*), and command pointer.
 *
 * redisOp 结构定义了一个 Redis 操作，
 * 它包含指向被执行命令的指针、命令的参数、数据库 ID 、传播目标（REDIS_PROPAGATE_*）。
 *
 * Currently only used to additionally propagate more commands to AOF/Replication
 * after the propagation of the executed command. 
 *
 * 目前只用于在传播被执行命令之后，传播附加的其他命令到 AOF 或 Replication 中。
 */
typedef struct redisOp {

    // 参数
    robj **argv;

    // 参数数量、数据库 ID 、传播目标
    int argc, dbid, target;

    // 被执行命令的指针
    struct redisCommand *cmd;

} redisOp;

/* Defines an array of Redis operations. There is an API to add to this
 * structure in a easy way.
 *
 * redisOpArrayInit();
 * redisOpArrayAppend();
 * redisOpArrayFree();
 */
typedef struct redisOpArray {
    redisOp *ops;
    int numops;
} redisOpArray;

/*-----------------------------------------------------------------------------
 * Global server state
 *----------------------------------------------------------------------------*/

struct clusterState;

/* 
redis服务器将所有数据库保存在服务器状态redis.h/redisserver结构的db数组中
*/
struct redisServer {//struct redisServer server; 

    /* General */

    // 配置文件的绝对路径
    char *configfile;           /* Absolute config file path, or NULL */

    // serverCron() 每秒调用的次数  默认为REDIS_DEFAULT_HZ  表示每秒运行serverCron多少次   可以由配置文件中的hz进行配置运行次数
    int hz;                     /* serverCron() calls frequency in hertz */

    // 数据库 selectDb赋值，每次和客户端读取交互的时候，都有对应的select x命令过来
    redisDb *db; //所有数据库都保存在该db数组中，每个redisDb代表一个数据库

    //命令表字典中的元素内容从redisCommandTable中获取，见populateCommandTable  实际客户端命令的查找在函数lookupCommandOrOriginal lookupCommand中
    // 命令表（受到 rename 配置选项的作用）  参考commandTableDictType可以看出该dict对应的key比较是不区分大小写的
    //如果是sentinel方式启动，则还会从sentinelcmds中获取，参考initSentinel
    dict *commands;             /* Command table */
    // 命令表（无 rename 配置选项的作用） 
    //redisServer->orig_commands redisServer->commands(见populateCommandTable)命令表字典中的元素内容从redisCommandTable中获取，见populateCommandTable
    dict *orig_commands;        /* Command table before command renaming. */

    // 事件状态
    aeEventLoop *el;

    // 最近一次使用时钟  getLRUClock();
    unsigned lruclock:REDIS_LRU_BITS; /* Clock for LRU eviction */

    // 关闭服务器的标识  每次serverCron函数运行时，程序都会对服务器状态的shutdown_asap属性进行检查，并根据属性的值决定是否关闭服务器：
    /* 
     关闭服务器的标识：值为1时，关闭服务器，值为0时，不做动作。
     */
    int shutdown_asap;          /* SHUTDOWN needed ASAP */

    // 在执行 serverCron() 时进行渐进式 rehash
    int activerehashing;        /* Incremental rehash in serverCron() */

    // 是否设置了密码 requirepass 配置认证密码
    char *requirepass;          /* Pass for AUTH command, or NULL */

    // PID 文件  进程号写入该文件中，见createPidFile
    char *pidfile;              /* PID file path */

    // 架构类型
    int arch_bits;              /* 32 or 64 depending on sizeof(long) */
 
    // serverCron() 函数的运行次数计数器  serverCron每运行一次就自增1
    int cronloops;              /* Number of times the cron function run */

    /*
    服务器运行ID: 除了复制偏移量和复制积压缓冲区之外，实现部分重同步还需要用到服务器运行ID
    1. 每个Redis服务器，不论主服务器还是从服务，都会有自己的运行ID。
    2. 运行rD在服务器启动时自动生成，由40个随机的十六进制字符组成，例如53b9b28df8042fdc9ab5e3fcbbbabffld5dce2b3。
    当从服务器对主服务器进行初次复制时，主服务器会将自己的运行ID传送给从服务器，而从服务器则会将这个运行ID保存起来。
    当从服务器断线并重新连上一个主服务器时，从服务器将向当前连接的主服务器发送之前保存的运行ID:
    1. 如果从服务器保存的运行ID和当前连接的主服务器的运行ID相同，那么说明从服务器断线之前复制的就是当前连接的这个主服务器，
    主服务器可以继续尝试执行部分重同步操作。
    2. 相反地，如果从服务器保存的运行ID和当前连接的主服务器的运行ID并不相同，那么说明从服务器断线之前复制的主服务器并不是
    当前连接的这个主服务器，主服务器将对从服务器执行完整重同步操作。

    举个例子，假设从服务器原本正在复制一个运行ID为53b9b28df8042fdc9ab5e3fcbbbabf fld5dce2b3的主服务器，那么在网络断开，从服务器
重新连接上主服务器之后，从服务器将向圭服务器发送这个运行ID，主服务器根据自己的运行ID是否53b9b28df8042fdc9ab5e3fcbbbabffld5dce2b3来
判断是执行部分重同步还是执行完整重同步。
     */
    // 本服务器的 RUN ID
    char runid[REDIS_RUN_ID_SIZE+1];  /* ID always different at every exec. */

    // 服务器是否运行在 SENTINEL 模式   见checkForSentinelMode
    int sentinel_mode;          /* True if this instance is a Sentinel. */


    /* Networking */

    //可以由启动redis的命令行赋值，默认REDIS_SERVERPORT   如果是Sentinel方式启动，则值为REDIS_SENTINEL_PORT
    // TCP 监听端口 port参数配置  表示作为服务器端，等待redis-cli客户端连接的时候，服务器端的监听端口，见initServer listenToPort(server.port,server.ipfd,&server.ipfd_count) == REDIS_ERR)
    int port;                   /* TCP listening port */

    int tcp_backlog;            /* TCP listen() backlog */

    // 地址
    char *bindaddr[REDIS_BINDADDR_MAX]; /* Addresses we should bind to */
    // 地址数量
    int bindaddr_count;         /* Number of addresses in server.bindaddr[] */

    // UNIX 套接字
    char *unixsocket;           /* UNIX socket path */
    mode_t unixsocketperm;      /* UNIX socket permission */

    // 描述符  对应的是redis服务器端listen的时候创建的套接字，服务器端可以bind多个端口和地址，有bind几个就有几个ipfd_count
    int ipfd[REDIS_BINDADDR_MAX]; /* TCP socket file descriptors */
    // 描述符数量 listenToPort(server.port,server.ipfd,&server.ipfd_count)中赋值
    int ipfd_count;             /* Used slots in ipfd[] */

    // UNIX 套接字文件描述符
    int sofd;                   /* Unix socket file descriptor */

    int cfd[REDIS_BINDADDR_MAX];/* Cluster bus listening socket */
    int cfd_count;              /* Used slots in cfd[] */

    // 一个链表，保存了所有客户端状态结构  createClient中把redisClient客户端添加到该服务端链表clients链表中  if (fd != -1) listAddNodeTail(server.clients,c);
    list *clients;               /* List of active clients */
    // 链表，保存了所有待关闭的客户端
    list *clients_to_close;     /* Clients to close asynchronously */

    // 链表，保存了所有从服务器，
    list *slaves, 
    //保存了所有连接该服务器并且执行了monitor的的客户端
    //服务器在每次处理命令请求乏前，都会调用replicationFeedMonitors函数，由这个函数将被处理的命令请求的相关信息发送给各个监视器。
    *monitors;    /* List of slaves and MONITORs */

    // 服务器的当前客户端，仅用于崩溃报告
    redisClient *current_client; /* Current client, only used on crash report */

    int clients_paused;         /* True if clients are currently paused */
    mstime_t clients_pause_end_time; /* Time when we undo clients_paused */

    // 网络错误
    char neterr[ANET_ERR_LEN];   /* Error buffer for anet.c */

    // MIGRATE 缓存
    dict *migrate_cached_sockets;/* MIGRATE cached sockets */


    /* RDB / AOF loading information */

    // 这个值为真时，表示服务器正在进行载入
    int loading;                /* We are loading data from disk if true */

    // 正在载入的数据的大小
    off_t loading_total_bytes;

    // 已载入数据的大小
    off_t loading_loaded_bytes;

    // 开始进行载入的时间
    time_t loading_start_time;
    off_t loading_process_events_interval_bytes;

    /* Fast pointers to often looked up command */
    // 常用命令的快捷连接
    struct redisCommand *delCommand, *multiCommand, *lpushCommand, *lpopCommand,
                        *rpopCommand;


    /* Fields used only for stats */

    // 服务器启动时间
    time_t stat_starttime;          /* Server start time */

    // 已处理命令的数量  当前总的指向命令总量  trackOperationsPerSecond
    long long stat_numcommands;     /* Number of processed commands */

    // 服务器接到的连接请求数量
    long long stat_numconnections;  /* Number of connections received */

    // 已过期的键数量
    long long stat_expiredkeys;     /* Number of expired keys */

    // 因为回收内存而被释放的过期键的数量
    long long stat_evictedkeys;     /* Number of evicted keys (maxmemory) */

    // 成功查找键的次数 //整个数据库的名字次数，可以通过info stats命令查看
    long long stat_keyspace_hits;   /* Number of successful lookups of keys */

    // 查找键失败的次数 //整个数据库的未名字次数，可以通过info stats命令查看
    long long stat_keyspace_misses; /* Number of failed lookups of keys */

    /*
    INFO memory命令的used memory_peak和used_memory_peak_human两个域分别以两种格式记录了服务器的内存峰值
     */
    // 已使用内存峰值  记录了服务器的内存峰值大小：
    size_t stat_peak_memory;        /* Max used memory record */

    // 最后一次执行 fork() 时消耗的时间
    long long stat_fork_time;       /* Time needed to perform latest fork() */

    // 服务器因为客户端数量过多而拒绝客户端连接的次数
    long long stat_rejected_conn;   /* Clients rejected because of maxclients */

    // 执行 full sync 的次数
    long long stat_sync_full;       /* Number of full resyncs with slaves. */

    // PSYNC 成功执行的次数
    long long stat_sync_partial_ok; /* Number of accepted PSYNC requests. */

    // PSYNC 执行失败的次数
    long long stat_sync_partial_err;/* Number of unaccepted PSYNC requests. */


    /* slowlog */

    // 保存了所有慢查询日志的链表  见slowlogPushEntryIfNeeded
    list *slowlog;                  /* SLOWLOG list of commands */ //链表节点类型slowlogEntry

    // 下一条慢查询日志的 ID  属性的初始值为O，每当创建一条新的慢查询日志时，这个属性的值就会用作新日志的id值，之后程序会对这个属性的值增一。
    long long slowlog_entry_id;     /* SLOWLOG current entry ID */

    /*
        slowlog-log-slower-than选项指定执行时间超过多少微秒（1秒等于1 000 0 0 0微秒）的命令请求会被记录到日志上。
    举个例子，如果这个选项的值为1 0 0，那么执行时间超过1 0 0微秒的命令就会被记录到慢查询日志
       */
    // 服务器配置 slowlog-log-slower-than 选项的值
    long long slowlog_log_slower_than; /* SLOWLOG time limit (to get logged) */

    /*
       lowlog-max-len选项指定服务器最多保存多少条慢查询日志。服务器使用先进先出的方式保存多条慢查询日志，当服务器存储的慢
       查询日志数量等于slowlog-max-len选项的值时，服务器在添加一条新的慢查询日志之前，会先将最旧的一条慢查询日志删除。
      */
    // 服务器配置 slowlog-max-len 选项的值
    unsigned long slowlog_max_len;     /* SLOWLOG max number of items logged */
    size_t resident_set_size;       /* RSS sampled in serverCron(). */
    /* The following two are used to track instantaneous "load" in terms
     * of operations per second. */
    // 最后一次进行抽样的时间 
    long long ops_sec_last_sample_time; /* Timestamp of last sample (in ms) */
    // 最后一次抽样时，服务器已执行命令的数量 trackOperationsPerSecond
    long long ops_sec_last_sample_ops;  /* numcommands in last sample */
    // 抽样结果  大小（默认值为16）的环形数组，数组中的每个项都记录了一次抽样结果。
    long long ops_sec_samples[REDIS_OPS_SEC_SAMPLES];
    // 数组索引，用于保存抽样结果，并在需要时回绕到 0    每次抽样后将值自增一，在值等于16时重置为口， 构成一个环形数组。
    int ops_sec_idx; //每次抽样加1，见trackOperationsPerSecond


    /* Configuration */

    // 日志可见性  日志登记配置 loglevel debug等
    int verbosity;                  /* Loglevel in redis.conf */

    // 客户端最大空转时间  默认REDIS_MAXIDLETIME不检查
    int maxidletime;                /* Client timeout in seconds */

    // 是否开启 SO_KEEPALIVE 选项  tcp-keepalive 设置，默认不开启
    int tcpkeepalive;               /* Set SO_KEEPALIVE if non-zero. */
    //默认初始化为1
    int active_expire_enabled;      /* Can be disabled for testing purposes. */
    size_t client_max_querybuf_len; /* Limit for client query buffer length */ //REDIS_MAX_QUERYBUF_LEN
    /*
    默认情况下，Redis客户端的目标数据库为O号数据库，但客户端可以通过执行SELECT命令来切换目标数据库。
     */
    //初始化服务器的时候，根据dbnum来决定创建多少个数据库  默认16  databases可以配置
    int dbnum;                      /* Total number of configured DBs */
    int daemonize;                  /* True if running as a daemon */
    // 客户端输出缓冲区大小限制
    // 数组的元素有 REDIS_CLIENT_LIMIT_NUM_CLASSES 个
    // 每个代表一类客户端：普通、从服务器、pubsub，诸如此类
    clientBufferLimitsConfig client_obuf_limits[REDIS_CLIENT_LIMIT_NUM_CLASSES];


    /* AOF persistence */

    // AOF 状态（开启/关闭/可写） //appendonly  yes | no配置  配置为no则不会参数aof文件appendonly.aof
    int aof_state;                  /* REDIS_AOF_(ON|OFF|WAIT_REWRITE) */

    // 所使用的 fsync 策略（每个写入/每秒/从不）
    int aof_fsync;                  /* Kind of fsync() policy */
    char *aof_filename;             /* Name of the AOF file */
    int aof_no_fsync_on_rewrite;    /* Don't fsync if a rewrite is in prog. */
    //默认100，可以用auto-aof-rewrite-percentage进行配置 
    //auto-aof-rewrite-percentage(100)：当AOF文件增长了这个比例（这里是增加了一倍），则后台rewrite自动运行
    int aof_rewrite_perc;           /* Rewrite AOF if % growth is > M and... */
    //auto-aof-rewrite-min-size(64mb)：进行后面rewrite要求的最小AOF文件大小。这两个选项共同决定了后面rewrite进程是否到达运行的时机
    off_t aof_rewrite_min_size;     /* the AOF file is at least N bytes. */

    // 最后一次执行 BGREWRITEAOF 时， AOF 文件的大小
    off_t aof_rewrite_base_size;    /* AOF size on latest startup or rewrite. */

    // AOF 文件的当前字节大小  BGREWRITEAOF后有新的命令到来会追加到文件末尾，所以aof_current_size比aof_rewrite_base_size大，
    off_t aof_current_size;         /* AOF current size. */
    /*
在服务器执行BGSA VE’命令的期间，如果客户端向服务器发来BGREWRITEAOF命令，那么服务器会将BGREWRITEAOF命令的执行时间延迟到BGSAVE命令执行完毕之后。
服务器的aof rewrite scheduled标识记录了服务器是否延迟了BGREWRITEAOF命令
每次serverCron函数执行时，函数都会检查BGSAVE命令或者BGREWIUTEAOF命令是否正在执行，如果这两个命令都没在执行，并且aof rewrite scheduled属性的
值为1，那么服务器就会执行之前被推延的BGREWRITEAOF命令
     */
    int aof_rewrite_scheduled;      /* Rewrite once BGSAVE terminates. */

    /*
     服务器状态使用rdb_child_pid属性和aof_child_pid属性记录执行BGSAVE命令和BGREWRITEAOF命令的子进程的ID，这两个属性也可以用于检查
     BGSAVE命令或者BGREWRITEAOF命令是否正在执行
     */
    // 负责进行 AOF 重写的子进程 ID  记录执行BGREWRITEAOF命令的子进程的ID:如果服务嚣没有在执行BGREWRITEAOF，那么这个属性的值为-1。
    pid_t aof_child_pid;            /* PID if rewriting process */

    // AOF 重写缓存链表，链接着多个缓存块   
    //在进行AOF过程中，如果有产生新的写命令，则新的写命令会暂时保存到aof_rewrite_buf_blocks该链表中，然后在aofRewriteBufferWrite追加写入AOF文件末尾
    list *aof_rewrite_buf_blocks;   /* Hold changes during an AOF rewrite. */

    /* 当AOF持久化功能处于打开状态时，服务器在执行完一个写命令之后，会以协议格式
将被执行的写命令追加到服务器状态的aof buf缓冲区的末尾： */
    // AOF 缓冲区
    sds aof_buf;      /* AOF buffer, written before entering the event loop */

    // AOF 文件的描述符
    int aof_fd;       /* File descriptor of currently selected AOF file */

    // AOF 的当前目标数据库
    int aof_selected_db; /* Currently selected DB in AOF */

    // 推迟 write 操作的时间
    time_t aof_flush_postponed_start; /* UNIX time of postponed AOF flush */

    // 最后一直执行 fsync 的时间
    time_t aof_last_fsync;            /* UNIX time of last fsync() */
    time_t aof_rewrite_time_last;   /* Time used by last AOF rewrite run. */

    // AOF 重写的开始时间
    time_t aof_rewrite_time_start;  /* Current AOF rewrite start time. */

    // 最后一次执行 BGREWRITEAOF 的结果
    int aof_lastbgrewrite_status;   /* REDIS_OK or REDIS_ERR */

    // 记录 AOF 的 write 操作被推迟了多少次
    unsigned long aof_delayed_fsync;  /* delayed AOF fsync() counter */

    // 指示是否需要每写入一定量的数据，就主动执行一次 fsync()
    int aof_rewrite_incremental_fsync;/* fsync incrementally while rewriting? */
    int aof_last_write_status;      /* REDIS_OK or REDIS_ERR */
    int aof_last_write_errno;       /* Valid if aof_last_write_status is ERR */
    /* RDB persistence */

    /*
    服务器每次修改一个键之后，都会对脏（dirty）键计数器的值增l，这个计数器会触发服务器的持久化以及复制操作
     */
    // 自从上次 SAVE 执行以来，数据库被修改的次数（包括写入、删除、更新等操作）。 执行save bgsave命令后会从新置为0
    //当bgsave执行完毕后，会在backgroundSaveDoneHandler更新该值
    long long dirty;                /* Changes to DB from the last save */

    // BGSAVE 执行前的数据库被修改次数  //当bgsave执行完毕后，会在backgroundSaveDoneHandler更新该值
    long long dirty_before_bgsave;  /* Used to restore dirty on failed BGSAVE */

    /*
     服务器状态使用rdb_child_pid属性和aof_child_pid属性记录执行BGSAVE命令和BGREWRITEAOF命令的子进程的ID，这两个属性也可以用于检查
     BGSAVE命令或者BGREWRITEAOF命令是否正在执行
     */
    // 负责执行 BGSAVE 的子进程的 ID
    // 没在执行 BGSAVE 时，设为 -1  记录执行BGSA VE命令的子进程的ID：如果服务器没有在执行BGSA VE，那么这个属性的值为-l。
    pid_t rdb_child_pid;            /* PID of RDB saving child */

/*
当Redis服务器启动时，用户可以通过指定配置文件或者传人启动参数的方式设置save选项，如果用户没有主动设置save选项，那幺服务器会为save选项设置默认条件：
    save 900 1
    save 300 10
    save 60 10000
saveparams属性是一个数组，数组中的每个元素都是一个saveparam结构，每个saveparam结构都保存了一个save选项设置的保存条件
*/  //saveparams属性生效地方见serverCron  
    struct saveparam *saveparams;   /* Save points array for RDB */
    int saveparamslen;              /* Number of saving points */
    char *rdb_filename;             /* Name of RDB file */ //dbfilename XXX配置  默认REDIS_DEFAULT_RDB_FILENAME
    int rdb_compression;            /* Use compression in RDB? */ //rdbcompression  yes | off
    //默认1，见REDIS_DEFAULT_RDB_CHECKSUM
    int rdb_checksum;               /* Use RDB checksum? */

    // 最后一次完成 SAVE 的时间 lastsave属性是一个UNIX时间戳，记录了服务器上一次成功执行SA VE命令或者BGSAVE命令的时间。
    time_t lastsave;                /* Unix time of last successful save */ //当bgsave执行完毕后，会在backgroundSaveDoneHandler更新该值

    // 最后一次尝试执行 BGSAVE 的时间
    time_t lastbgsave_try;          /* Unix time of last attempted bgsave */

    // 最近一次 BGSAVE 执行耗费的时间
    time_t rdb_save_time_last;      /* Time used by last RDB save run. */

    // 数据库最近一次开始执行 BGSAVE 的时间
    time_t rdb_save_time_start;     /* Current RDB save start time. */

    // 最后一次执行 SAVE 的状态 //当bgsave执行完毕后，会在backgroundSaveDoneHandler更新该值
    int lastbgsave_status;          /* REDIS_OK or REDIS_ERR */
    int stop_writes_on_bgsave_err;  /* Don't allow writes if can't BGSAVE */


    /* Propagation of commands in AOF / replication */
    redisOpArray also_propagate;    /* Additional command to propagate. */


    /* Logging */
    char *logfile;                  /* Path of log file */
    int syslog_enabled;             /* Is syslog enabled? */
    char *syslog_ident;             /* Syslog ident */
    int syslog_facility;            /* Syslog facility */


    /* Replication (master) */
    int slaveseldb;                 /* Last SELECTed DB in replication output */
    // 全局复制偏移量（一个累计值）
    long long master_repl_offset;   /* Global replication offset */
    // 主服务器发送 PING 的频率
    int repl_ping_slave_period;     /* Master pings the slave every N seconds */


/*
    根据需要调整复制积压缓冲区的大小
        Redis为复制积压缓冲区设置的默认大小为l MB，如果主服务器需要执行大量写命令，又或者主从服务器断线后重连接所需的时间比较长，
    那么这个大小也许并不合适。
        如果复制积压缓冲区的大小设置得不恰当，那么PSYNC命令的复制重同步模式就不能正常发挥作用，因此，正确估算和设置复制积压缓冲区的大小非常重要。
        复制积压缓冲区的最小大小可以根据公式second * write_size_per_second来估算：
        其中second为从服务器断线后重新连接上主服务器所需的平均时阃（以秒计算):
        而write_size_per_second则是主服务器平均每秒产生的写命令数据量（协议格式的写命令的长度总和】。
        例如，如果主服务器平均每秒产生1MB的写数据，而从服务器断线之后平均要5秒才能重新连接上主服务器，那么复制积压缓冲区的大小就不能低于5 MB。
        为了安全起见，可以将复制积压缓冲区的大小设为2 * second * write_size_per_second．这样可以保证绝大部分断线情况都能用部分重同步来处理。



    复制积压缓冲区是由主服务器维护的一个固定长度（fixed-size）先进先出(FIFO)队列，默认大小为lMB。
    固定长度先进先出队列的入队和出队规则跟普通的先进先出队列一样：新元素从一边进入队列，而旧元素从另一边弹出队列。
    和普通先进先出队列随着元素的增加和减少而动态调整长度不同，固定长度先进先出队列的长度是固定的，当入队元素的数量大于队列长度时，
    最先入队的元素会被弹出，而新元隶会被放入队列。

当从服务器重新连上主服务器时，从服务器会通过PSYNC命令将自己的复制偏移量offset发送给主服务器，主服务器会根据这个复制偏移量来决
定对从服务器执行何种同步操作：
    如果offset偏移量之后的数据（也即是偏移量offset+l开始的数据）仍然存在于复制积压缓冲区里面，那么主服务器将对从服务器执行部分萤同步操作。
    相反，如果offset偏移量之后的数据已经不存在于复制积压缓冲区，那么主服务器将对从服务器执行完整重同步操作。

    当从服务器A断线之后，它立即重新连接主服务器，并向主服务器发送PSYNC命令，报告自己的复制偏移量为10086。
    主服务器收到从服务器发来的PSYNC命令以及偏移量10086之后，主服务器将检查偏移量1 0 0 8 6之后的数据是否存在于复制积压缓冲区里面，
        结果发现这些数据仍然存在，于是主服务器向从服务器发送"+CONTINUE"回复，表示数据同步将以部分重同步模式来进行。
    接着主服务器会将复制积压缓冲区10086偏移量之后的所有数据（偏移量为10087至10119）都发送给从服务器。
*/


    // backlog 本身   repl_backlog积压缓存区空间  repl_backlog_size积压缓冲区总大小  参考resizeReplicationBacklog
    char *repl_backlog;             /* Replication backlog for partial syncs */
    //repl_backlog积压缓存区空间  repl_backlog_size积压缓冲区总大小  参考resizeReplicationBacklog
    // backlog 的长度 //repl-backlog-size进行设置  表示在从服务器断开连接过程中，如果有很大写命令，则用该大小的积压缓冲区来存储这些新增的，下次再次连接上直接把积压缓冲区的内容发送给从服务器即可
    long long repl_backlog_size;    /* Backlog circular buffer size */
    // backlog 中数据的长度  repl_backlog_histlen 的最大值只能等于 repl_backlog_size
    long long repl_backlog_histlen; /* Backlog actual data length */
    // backlog 的当前索引
    long long repl_backlog_idx;     /* Backlog circular buffer current offset */
    // backlog 中可以被还原的第一个字节的偏移量
    long long repl_backlog_off;     /* Replication offset of first byte in the
                                       backlog buffer. */
    // backlog 的过期时间
    time_t repl_backlog_time_limit; /* Time without slaves after the backlog
                                       gets released. */

    // 距离上一次有从服务器的时间
    time_t repl_no_slaves_since;    /* We have no slaves since that time.
                                       Only valid if server.slaves len is 0. */

    /*
     辅助实现min-slaves配置选项
         Redis的min-slaves-to-write和min-slaves-max-lag两个选项可以防止主服务器在不安全的情况下执行写命令。举个例子，如果我们向主服务器提供以下设置：
       min-slaves-to-write 3
       min-slaves-max-lag 10
       那么在从服务器的数量少于3个，或者三个从服务器的延迟(lag)值都大于或等于10秒时，主服务器将拒绝执行写命令，这里的延迟值就是上面
       提到的INFO replication命令的lag值。
     */
    // 是否开启最小数量从服务器写入功能
    int repl_min_slaves_to_write;   /* Min number of slaves to write. */
    // 定义最小数量从服务器的最大延迟值
    int repl_min_slaves_max_lag;    /* Max lag of <count> slaves to write. */
    // 延迟良好的从服务器的数量
    int repl_good_slaves_count;     /* Number of slaves with lag <= max_lag. */


    /* Replication (slave) */
    // 主服务器的验证密码  masterauth配置
    char *masterauth;               /* AUTH with this password with master */
    //slaveof 10.2.2.2 1234 中的masterhost=10.2.2.2 masterport=1234  赋值见replicationSetMaster  建立连接在replicationCron
    // 主服务器的地址  如果这个部位空，则说明该参数指定的是该服务器对应的主服务器，也就是本服务器是从服务器

    //当从服务器执行slave no on，本服务器不在从属于某个主服务器了，replicationUnsetMaster中会把masterhost置为NULL
    char *masterhost;               /* Hostname of master */ //赋值见replicationSetMaster  建立连接在replicationCron
    // 主服务器的端口 //slaveof 10.2.2.2 1234 中的masterhost=10.2.2.2 masterport=1234
    int masterport;                 /* Port of master */ //赋值见replicationSetMaster  建立连接在replicationCron
    // 超时时间
    int repl_timeout;               /* Timeout after N seconds of master idle */

    /*
    从服务器将向主服务器发送PSYNC命令，执行同步操作，并将自己的数据库更新至主服务器数据库当前所处的状态。
  值得一提的是，在同步操作执行之前，只有从服务器是主服务器的客户端，但是在执行同步操作之后，主服务器也会成为从服务器的客户端：
    1. 如果PSYNC命令执行的是完整重同步操作，那么主服务器需要成为从服务器的客户端，才能将保存在缓冲区里面的写命令发送给从服务器执行。
    2. 如果PSYNC命令执行的是部分重同步操作，那么主服务器需要成为从服务器的客户端，才能向从服务器发送保存在复制积压缓冲区里面的写命令。
    因此，在同步操作执行之后，主从服务器双方都是对方的客户端，它们可以互相向对方发送命令请求，或者互相向对方返回命令回复
    正因为主服务器成为了从服务器的客户端，所以主服务器才可以通过发送写命令来改变从服务器的数掘库状态，不仅同步操作需要用到这一点，
    这也是主服务器对从服务器执行命令传播操作的基础
     */

    /*
    主备同步会专门创建一个repl_transfer_s套接字(connectWithMaster)来进行主备同步，同步完成后在replicationAbortSyncTransfer中关闭该套接字
    主备同步完成后，主服务器要向本从服务器发送实时KV，则需要一个模拟的redisClient,因为redis都是通过redisClient中的fd来接收客户端发送的KV,
    主备同步完成后的时候KV和主备心跳保活都是通过该master(redisClient)的fd来和主服务器通信的
    */
    
    // 主服务器所对应的客户端，见readSyncBulkPayload    replicationSetMaster中是否该master资源  也就是本服务器的主服务器对象，见readSyncBulkPayload
    //使用这个的母的是让主服务器发送命令给从服务器，因为命令一般都是从客户端发送给服务器端的，

    //当本节点是从节点的时候，就创建一个redisClient节点用来专门同步主节点发往本节点的实时KV对给本从服务器  
    //例如从服务器连接到本主服务器，客户端更新了KV，这个更新操作就通过该redisClient来和主服务器通信
    redisClient *master;     /* Client that is master for this slave */ //
    // 被缓存的主服务器，PSYNC 时使用  
//例如从服务器之前和主服务器连接上，并同步了数据，中途端了，则连接断了后会在replicationCacheMaster把server.cached_master = server.master;表示之前有连接到过服务器
    redisClient *cached_master; /* Cached master to be reused for PSYNC. */
    int repl_syncio_timeout; /* Timeout for synchronous I/O calls */
    // 复制的状态（服务器是从服务器时使用）  读取服务器端同步过来的rdb文件后，置为server.repl_state = REDIS_REPL_CONNECTED; 见readSyncBulkPayload
    int repl_state;          /* Replication status if the instance is a slave */
    // RDB 文件的总的大小
    off_t repl_transfer_size; /* Size of RDB to read from master during sync. */
    // 已读 RDB 文件内容的字节数  repl_transfer_size - repl_transfer_read就是还未读取的rdb文件大小
    off_t repl_transfer_read;  /* Amount of RDB read from master during sync. */
    // 最近一次执行 fsync 时的偏移量
    // 用于 sync_file_range 函数  写入磁盘的文件大小，见readSyncBulkPayload
    off_t repl_transfer_last_fsync_off; /* Offset when we fsync-ed last time. */

    /*
    主备同步会专门创建一个repl_transfer_s套接字(connectWithMaster)来进行主备同步，同步完成后在replicationAbortSyncTransfer中关闭该套接字
    主备同步完成后，主服务器要向本从服务器发送实时KV，则需要一个模拟的redisClient,因为redis都是通过redisClient中的fd来接收客户端发送的KV,
    主备同步完成后的时候KV和主备心跳保活都是通过该master(redisClient)的fd来和主服务器通信的
    */
   
    // 主服务器的套接字  见connectWithMaster  用于主备同步  
    //主备同步会专门创建一个repl_transfer_s套接字(connectWithMaster)来进行主备同步，同步完成后在replicationAbortSyncTransfer中关闭该套接字
    int repl_transfer_s;     /* Slave -> Master SYNC socket */
    // 保存 RDB 文件的临时文件的描述符
    int repl_transfer_fd;    /* Slave -> Master SYNC temp file descriptor */
    // 保存 RDB 文件的临时文件名字
    char *repl_transfer_tmpfile; /* Slave-> master SYNC temp file name */
    // 最近一次读入 RDB 内容的时间
    time_t repl_transfer_lastio; /* Unix time of the latest read, for timeout */
    int repl_serve_stale_data; /* Serve stale data when link is down? */
    // 是否只读从服务器？  从服务器默认是只读，不能进行相关写操作
    int repl_slave_ro;          /* Slave is read only? */
    // 连接断开的时长
    time_t repl_down_since; /* Unix time at which link with master went down */
    // 是否要在 SYNC 之后关闭 NODELAY ？
    int repl_disable_tcp_nodelay;   /* Disable TCP_NODELAY after SYNC? */
    // 从服务器优先级
    int slave_priority;             /* Reported in INFO and used by Sentinel. */
    // 本服务器（从服务器）当前主服务器的 RUN ID
    char repl_master_runid[REDIS_RUN_ID_SIZE+1];  /* Master run id for PSYNC. */
    // 初始化偏移量
    long long repl_master_initial_offset;         /* Master PSYNC offset. */


    /* Replication script cache. */
    // 复制脚本缓存
    // 字典
    dict *repl_scriptcache_dict;        /* SHA1 all slaves are aware of. */
    // FIFO 队列
    list *repl_scriptcache_fifo;        /* First in, first out LRU eviction. */
    // 缓存的大小
    int repl_scriptcache_size;          /* Max number of elements. */

    /* Synchronous replication. */
    list *clients_waiting_acks;         /* Clients waiting in WAIT command. */
    int get_ack_from_slaves;            /* If true we send REPLCONF GETACK. */
    /* Limits */
    int maxclients;                 /* Max number of simultaneous clients */
    //生效比较见freeMemoryIfNeeded，实际内存是算出来的  maxmemory参数进行配置
    unsigned long long maxmemory;   /* Max number of memory bytes to use */
    int maxmemory_policy;           /* Policy for key eviction */
    int maxmemory_samples;          /* Pricision of random sampling */


    /* Blocked clients */
    unsigned int bpop_blocked_clients; /* Number of clients blocked by lists */
    list *unblocked_clients; /* list of clients to unblock before next loop */
    list *ready_keys;        /* List of readyList structures for BLPOP & co */


    /* Sort parameters - qsort_r() is only available under BSD so we
     * have to take this state global, in order to pass it to sortCompare() */
    int sort_desc;
    int sort_alpha;
    int sort_bypattern;
    int sort_store;


    /* Zip structure config, see redis.conf for more information  */
    size_t hash_max_ziplist_entries;
    size_t hash_max_ziplist_value;
    size_t list_max_ziplist_entries;
    size_t list_max_ziplist_value; //默认=REDIS_LIST_MAX_ZIPLIST_VALUE，可以通过list-max-ziplist-value设置
    size_t set_max_intset_entries;
    size_t zset_max_ziplist_entries;
    size_t zset_max_ziplist_value;
    size_t hll_sparse_max_bytes;
    /*
     因为serverCron函数默认会以每100毫秒一次的频率更新unixtime属性和mstime属性，所以这两个属性记录的时间的精确度并不高：
     */
    //保存了秒级精度的系统当前UNIX时间戳  updateCachedTime中跟新
    time_t unixtime;        /* Unix time sampled every cron cycle. */
    //保存了毫秒级精度的系统当前UNIX时间戳 updateCachedTime中跟新
    long long mstime;       /* Like 'unixtime' but with milliseconds resolution. */


     /*
    当一个客户端执行SUBSCRIBE命令订阅某个或某些频道的时候，这个客户端与被订阅频道之间就建立起了一种订阅关系。
        Redis将所有频道的订阅关系都保存在服务器状态的pubsub_channels字典里面，这个字典的键是某个被订阅的频道，而键的值则是一个链表，
    链表里面记录了所有订阅这个频道的客户端：
        subscribe频道订阅关系保存到pubsub_channels，psubscribe模式订阅关系保存到pubsub_patterns里面

        客户端1订阅:psubscribe  aaa[12]c, 则其他客户端2publish aaa1c xxx或者publish aaa2c xxx的时候，客户端1都会受到这个信息
         subscribe  ---  unsubscribe   频道订阅   
         psubscribe ---  punsubscribe  模式订阅
    */
    
    /* Pubsub */
    // 字典，键为频道，值为链表
    // 链表中保存了所有订阅某个频道的客户端
    // 新客户端总是被添加到链表的表尾
    dict *pubsub_channels;  /* Map channels to list of subscribed clients */

    /*
     pubsub_patterns属性是一个链表，链表中的每个节点都包含着一个pubsub_pattern结构，这个结构的pattern属性记录了被订阅的模式，
     而client属性则记录了订阅模式的客户端
     */
    // 这个链表记录了客户端订阅的所有模式的名字  链表中的成员结构式pubsubPattern
    list *pubsub_patterns;  /* A list of pubsub_patterns */

    int notify_keyspace_events; /* Events to propagate via Pub/Sub. This is an
                                   xor of REDIS_NOTIFY... flags. */


    /* Cluster */

    int cluster_enabled;      /* Is cluster enabled? */
    //默认REDIS_CLUSTER_DEFAULT_NODE_TIMEOUT ms // 等待 PONG 回复的时长超过了限制值，将目标节点标记为 PFAIL （疑似下线）
    //起作用的地方见clusterCron
    mstime_t cluster_node_timeout; /* Cluster node timeout. */
    char *cluster_configfile; /* Cluster auto-generated config file name. */ //从nodes.conf中载入，见clusterLoadConfig
    //server.cluster = zmalloc(sizeof(clusterState));
    //集群相关配置加载在clusterLoadConfig,  server.cluster可能从nodes.conf配置文件中加载也可能如果没有nodes.conf配置文件或者配置文件空或者配置有误，则加载失败后
    //创建对应的cluster节点 clusterInit初始化创建空间
    struct clusterState *cluster;  /* State of the cluster */

    int cluster_migration_barrier; /* Cluster replicas migration barrier. */
    /* Scripting */

    // Lua 环境
    lua_State *lua; /* The Lua interpreter. We use just one for all clients */
    
    // 复制执行 Lua 脚本中的 Redis 命令的伪客户端
    //服务器会在初始化时创建负责执行Lua脚本中包含的Redis命令的伪客户端，并将这个伪客户端关联在服务器状态结构的lua_client属性中
    redisClient *lua_client;   /* The "fake client" to query Redis from Lua */

    // 当前正在执行 EVAL 命令的客户端，如果没有就是 NULL
    redisClient *lua_caller;   /* The client running EVAL right now, or NULL */

    // 一个字典，值为 Lua 脚本，键为脚本的 SHA1 校验和
    dict *lua_scripts;         /* A dictionary of SHA1 -> Lua scripts */
    // Lua 脚本的执行时限
    mstime_t lua_time_limit;  /* Script timeout in milliseconds */
    // 脚本开始执行的时间
    mstime_t lua_time_start;  /* Start time of script, milliseconds time */

    // 脚本是否执行过写命令
    int lua_write_dirty;  /* True if a write command was called during the
                             execution of the current script. */

    // 脚本是否执行过带有随机性质的命令
    int lua_random_dirty; /* True if a random command was called during the
                             execution of the current script. */

    // 脚本是否超时
    int lua_timedout;     /* True if we reached the time limit for script
                             execution. */

    // 是否要杀死脚本
    int lua_kill;         /* Kill the script if true. */


    /* Assert & bug reporting */

    char *assert_failed;
    char *assert_file;
    int assert_line;
    int bug_report_start; /* True if bug report header was already logged. */
    int watchdog_period;  /* Software watchdog period in ms. 0 = off */
};

/*
 * 记录订阅模式的结构
 */
typedef struct pubsubPattern { //是为psubscribe 模式订阅创建的相关结构，参考psubscribeCommand

    // 订阅模式的客户端
    redisClient *client;

    // 被订阅的模式
    robj *pattern;  //publish channel  message的时候需要用channel与pattern进行匹配判断，匹配过程参考stringmatchlen

} pubsubPattern;

typedef void redisCommandProc(redisClient *c);
typedef int *redisGetKeysProc(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);

/*
 * Redis 命令
 */
struct redisCommand { //redisCommandTable中使用

    // 命令名字
    char *name;

    // 实现函数  指向命令的实现函数
    redisCommandProc *proc; //命令执行在processCommand->call

/*  
命令参数的个数，用于检查命令请求的格式是否正确。如果这个值为负数-N，那么表示参数的数量大于等于N。注意
命令的名字本身也是一个参数，比如说SET msg hello world 命令的参数有三个 
*/
    // 参数个数
    int arity;

//字符串形式的标识值，这个值记录了命令的属性，比如这个命令是写命令还是读命令，这个命令是否允许在载人数据时使用，这个命令是否允许在Lua脚本中使用等等                   
    // 字符串表示的 FLAG
    char *sflags; /* Flags as string representation, one char per flag. */

//对sflags标识进行分析得出的二进制标识，由程序自动生成。服务器对命令标识进行检查时使用的都是flags属性而不是sflags属性，因为对二进制标识的检查可以方便地通过＆、^、-等操作完成
    // 实际 FLAG
    int flags;    /* The actual flags, obtained from the 'sflags' field. */

    /* Use a function to determine keys arguments in a command line.
     * Used for Redis Cluster redirect. */
    // 从命令中判断命令的键参数。在 Redis 集群转向时使用。
    redisGetKeysProc *getkeys_proc;

    /* What keys should be loaded in background when calling this command? */
    // 指定哪些参数是 key
    int firstkey; /* The first argument that's a key (0 = no keys) */
    int lastkey;  /* The last argument that's a key */
    int keystep;  /* The step between first and last key */

    // 统计信息
    // microseconds 记录了命令执行耗费的总毫微秒数
    // calls 是命令被执行的总次数
    long long microseconds,  //服务器执行这个命令所耗费的总时长 
    calls; //服务器总共执行了多少次这个命令
};

struct redisFunctionSym {
    char *name;
    unsigned long pointer;
};

/*
SORT命令为每个被排序的键都创建一个与键长度相同的数组，数组的每个项都是一个_redisSortObject结构，根据SORT命令使用的选项不同，
程序使用_redisSortObject结构的方式也不同，最后排序后的顺序就依据此数组实现，从数组开始到数组末尾是升序或者降序
*/
// 用于保存被排序值及其权重的结构
typedef struct _redisSortObject {

    // 被排序键的值
    robj *obj;

    // 权重
    union {

        // 排序数字值时使用
        double score;

        // 排序字符串时使用
        robj *cmpobj;

    } u;

} redisSortObject;

// 排序操作
typedef struct _redisSortOperation {

    // 操作的类型，可以是 GET 、 DEL 、INCR 或者 DECR
    // 目前只实现了 GET
    int type;

    // 用户给定的模式
    robj *pattern;

} redisSortOperation;

/* Structure to hold list iteration abstraction.
 *
 * 列表迭代器对象
 */
typedef struct {

    // 列表对象
    robj *subject;

    // 对象所使用的编码
    unsigned char encoding;

    // 迭代的方向
    unsigned char direction; /* Iteration direction */

    // ziplist 索引，迭代 ziplist 编码的列表时使用
    unsigned char *zi;

    // 链表节点的指针，迭代双端链表编码的列表时使用
    listNode *ln;

} listTypeIterator;

/* Structure for an entry while iterating over a list.
 *
 * 迭代列表时使用的记录结构，
 * 用于保存迭代器，以及迭代器返回的列表节点。
 */
typedef struct {

    // 列表迭代器
    listTypeIterator *li;

    // ziplist 节点索引
    unsigned char *zi;  /* Entry in ziplist */

    // 双端链表节点指针
    listNode *ln;       /* Entry in linked list */

} listTypeEntry;

/* Structure to hold set iteration abstraction. */
/*
 * 多态集合迭代器
 */
typedef struct {

    // 被迭代的对象
    robj *subject;

    // 对象的编码
    int encoding;

    // 索引值，编码为 intset 时使用
    int ii; /* intset iterator */

    // 字典迭代器，编码为 HT 时使用
    dictIterator *di;

} setTypeIterator;

/* Structure to hold hash iteration abstraction. Note that iteration over
 * hashes involves both fields and values. Because it is possible that
 * not both are required, store pointers in the iterator to avoid
 * unnecessary memory allocation for fields/values. */
/*
 * 哈希对象的迭代器
 */
typedef struct {

    // 被迭代的哈希对象
    robj *subject;

    // 哈希对象的编码
    int encoding;

    // 域指针和值指针
    // 在迭代 ZIPLIST 编码的哈希对象时使用
    unsigned char *fptr, *vptr;

    // 字典迭代器和指向当前迭代字典节点的指针
    // 在迭代 HT 编码的哈希对象时使用
    dictIterator *di;
    dictEntry *de;
} hashTypeIterator;

#define REDIS_HASH_KEY 1
#define REDIS_HASH_VALUE 2

/*-----------------------------------------------------------------------------
 * Extern declarations
 *----------------------------------------------------------------------------*/

extern struct redisServer server;
extern struct sharedObjectsStruct shared;
extern dictType setDictType;
extern dictType zsetDictType;
extern dictType clusterNodesDictType;
extern dictType clusterNodesBlackListDictType;
extern dictType dbDictType;
extern dictType shaScriptObjectDictType;
extern double R_Zero, R_PosInf, R_NegInf, R_Nan;
extern dictType hashDictType;
extern dictType replScriptCacheDictType;

/*-----------------------------------------------------------------------------
 * Functions prototypes
 *----------------------------------------------------------------------------*/

/* Utils */
long long ustime(void);
long long mstime(void);
void getRandomHexChars(char *p, unsigned int len);
uint64_t crc64(uint64_t crc, const unsigned char *s, uint64_t l);
void exitFromChild(int retcode);
size_t redisPopcount(void *s, long count);
void redisSetProcTitle(char *title);

/* networking.c -- Networking and Client related operations */
redisClient *createClient(int fd);
void closeTimedoutClients(void);
void freeClient(redisClient *c, const char *func, unsigned int line);
void freeClientAsync(redisClient *c);
void resetClient(redisClient *c);
void sendReplyToClient(aeEventLoop *el, int fd, void *privdata, int mask);
void addReply(redisClient *c, robj *obj);
void *addDeferredMultiBulkLength(redisClient *c);
void setDeferredMultiBulkLength(redisClient *c, void *node, long length);
void addReplySds(redisClient *c, sds s);
void processInputBuffer(redisClient *c);
void acceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void acceptUnixHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask);
void addReplyBulk(redisClient *c, robj *obj);
void addReplyBulkCString(redisClient *c, char *s);
void addReplyBulkCBuffer(redisClient *c, void *p, size_t len);
void addReplyBulkLongLong(redisClient *c, long long ll);
void acceptHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void addReply(redisClient *c, robj *obj);
void addReplySds(redisClient *c, sds s);
void addReplyError(redisClient *c, char *err);
void addReplyStatus(redisClient *c, char *status);
void addReplyDouble(redisClient *c, double d);
void addReplyLongLong(redisClient *c, long long ll);
void addReplyMultiBulkLen(redisClient *c, long length);
void copyClientOutputBuffer(redisClient *dst, redisClient *src);
void *dupClientReplyValue(void *o);
void getClientsMaxBuffers(unsigned long *longest_output_list,
                          unsigned long *biggest_input_buffer);
void formatPeerId(char *peerid, size_t peerid_len, char *ip, int port);
char *getClientPeerId(redisClient *client);
sds catClientInfoString(sds s, redisClient *client);
sds getAllClientsInfoString(void);
void rewriteClientCommandVector(redisClient *c, int argc, ...);
void rewriteClientCommandArgument(redisClient *c, int i, robj *newval);
unsigned long getClientOutputBufferMemoryUsage(redisClient *c);
void freeClientsInAsyncFreeQueue(void);
void asyncCloseClientOnOutputBufferLimitReached(redisClient *c);
int getClientLimitClassByName(char *name);
char *getClientLimitClassName(int class);
void flushSlavesOutputBuffers(void);
void disconnectSlaves(void);
int listenToPort(int port, int *fds, int *count);
void pauseClients(mstime_t duration);
int clientsArePaused(void);
int processEventsWhileBlocked(void);

#ifdef __GNUC__
void addReplyErrorFormat(redisClient *c, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
void addReplyStatusFormat(redisClient *c, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
void addReplyErrorFormat(redisClient *c, const char *fmt, ...);
void addReplyStatusFormat(redisClient *c, const char *fmt, ...);
#endif

/* List data type */
void listTypeTryConversion(robj *subject, robj *value);
void listTypePush(robj *subject, robj *value, int where);
robj *listTypePop(robj *subject, int where);
unsigned long listTypeLength(robj *subject);
listTypeIterator *listTypeInitIterator(robj *subject, long index, unsigned char direction);
void listTypeReleaseIterator(listTypeIterator *li);
int listTypeNext(listTypeIterator *li, listTypeEntry *entry);
robj *listTypeGet(listTypeEntry *entry);
void listTypeInsert(listTypeEntry *entry, robj *value, int where);
int listTypeEqual(listTypeEntry *entry, robj *o);
void listTypeDelete(listTypeEntry *entry);
void listTypeConvert(robj *subject, int enc);
void unblockClientWaitingData(redisClient *c);
void handleClientsBlockedOnLists(void);
void popGenericCommand(redisClient *c, int where);

/* MULTI/EXEC/WATCH... */
void unwatchAllKeys(redisClient *c);
void initClientMultiState(redisClient *c);
void freeClientMultiState(redisClient *c);
void queueMultiCommand(redisClient *c);
void touchWatchedKey(redisDb *db, robj *key);
void touchWatchedKeysOnFlush(int dbid);
void discardTransaction(redisClient *c);
void flagTransaction(redisClient *c);

/* Redis object implementation */
void decrRefCount(robj *o);
void decrRefCountVoid(void *o);
void incrRefCount(robj *o);
robj *resetRefCount(robj *obj);
void freeStringObject(robj *o);
void freeListObject(robj *o);
void freeSetObject(robj *o);
void freeZsetObject(robj *o);
void freeHashObject(robj *o);
robj *createObject(int type, void *ptr);
robj *createStringObject(char *ptr, size_t len);
robj *createRawStringObject(char *ptr, size_t len);
robj *createEmbeddedStringObject(char *ptr, size_t len);
robj *dupStringObject(robj *o);
int isObjectRepresentableAsLongLong(robj *o, long long *llongval);
robj *tryObjectEncoding(robj *o);
robj *getDecodedObject(robj *o);
size_t stringObjectLen(robj *o);
robj *createStringObjectFromLongLong(long long value);
robj *createStringObjectFromLongDouble(long double value);
robj *createListObject(void);
robj *createZiplistObject(void);
robj *createSetObject(void);
robj *createIntsetObject(void);
robj *createHashObject(void);
robj *createZsetObject(void);
robj *createZsetZiplistObject(void);
int getLongFromObjectOrReply(redisClient *c, robj *o, long *target, const char *msg);
int checkType(redisClient *c, robj *o, int type);
int getLongLongFromObjectOrReply(redisClient *c, robj *o, long long *target, const char *msg);
int getDoubleFromObjectOrReply(redisClient *c, robj *o, double *target, const char *msg);
int getLongLongFromObject(robj *o, long long *target);
int getLongDoubleFromObject(robj *o, long double *target);
int getLongDoubleFromObjectOrReply(redisClient *c, robj *o, long double *target, const char *msg);
char *strEncoding(int encoding);
int compareStringObjects(robj *a, robj *b);
int collateStringObjects(robj *a, robj *b);
int equalStringObjects(robj *a, robj *b);
unsigned long long estimateObjectIdleTime(robj *o);

#define sdsEncodedObject(objptr) \
    (objptr->encoding == REDIS_ENCODING_RAW || objptr->encoding == REDIS_ENCODING_EMBSTR)

/* Synchronous I/O with timeout */
ssize_t syncWrite(int fd, char *ptr, ssize_t size, long long timeout);
ssize_t syncRead(int fd, char *ptr, ssize_t size, long long timeout);
ssize_t syncReadLine(int fd, char *ptr, ssize_t size, long long timeout);

/* Replication */
void replicationFeedSlaves(list *slaves, int dictid, robj **argv, int argc);
void replicationFeedMonitors(redisClient *c, list *monitors, int dictid, robj **argv, int argc);
void updateSlavesWaitingBgsave(int bgsaveerr);
void replicationCron(void);
void replicationHandleMasterDisconnection(void);
void replicationCacheMaster(redisClient *c);
void resizeReplicationBacklog(long long newsize);
void replicationSetMaster(char *ip, int port);
void replicationUnsetMaster(void);
void refreshGoodSlavesCount(void);
void replicationScriptCacheInit(void);
void replicationScriptCacheFlush(void);
void replicationScriptCacheAdd(sds sha1);
int replicationScriptCacheExists(sds sha1);
void processClientsWaitingReplicas(void);
void unblockClientWaitingReplicas(redisClient *c);
int replicationCountAcksByOffset(long long offset);
void replicationSendNewlineToMaster(void);
long long replicationGetSlaveOffset(void);

/* Generic persistence functions */
void startLoading(FILE *fp);
void loadingProgress(off_t pos);
void stopLoading(void);

/* RDB persistence */
#include "rdb.h"

/* AOF persistence */
void flushAppendOnlyFile(int force);
void feedAppendOnlyFile(struct redisCommand *cmd, int dictid, robj **argv, int argc);
void aofRemoveTempFile(pid_t childpid);
int rewriteAppendOnlyFileBackground(void);
int loadAppendOnlyFile(char *filename);
void stopAppendOnly(void);
int startAppendOnly(void);
void backgroundRewriteDoneHandler(int exitcode, int bysignal);
void aofRewriteBufferReset(void);
unsigned long aofRewriteBufferSize(void);

/* Sorted sets data type */

/* Struct to hold a inclusive/exclusive range spec by score comparison. */
// 表示开区间/闭区间范围的结构
typedef struct {

    // 最小值和最大值
    double min, max;

    // 指示最小值和最大值是否*不*包含在范围之内
    // 值为 1 表示不包含，值为 0 表示包含
    int minex, maxex; /* are min or max exclusive? */
} zrangespec;

/* Struct to hold an inclusive/exclusive range spec by lexicographic comparison. */
typedef struct {
    robj *min, *max;  /* May be set to shared.(minstring|maxstring) */
    int minex, maxex; /* are min or max exclusive? */
} zlexrangespec;

zskiplist *zslCreate(void);
void zslFree(zskiplist *zsl);
zskiplistNode *zslInsert(zskiplist *zsl, double score, robj *obj);
unsigned char *zzlInsert(unsigned char *zl, robj *ele, double score);
int zslDelete(zskiplist *zsl, double score, robj *obj);
zskiplistNode *zslFirstInRange(zskiplist *zsl, zrangespec *range);
zskiplistNode *zslLastInRange(zskiplist *zsl, zrangespec *range);
double zzlGetScore(unsigned char *sptr);
void zzlNext(unsigned char *zl, unsigned char **eptr, unsigned char **sptr);
void zzlPrev(unsigned char *zl, unsigned char **eptr, unsigned char **sptr);
unsigned int zsetLength(robj *zobj);
void zsetConvert(robj *zobj, int encoding);
unsigned long zslGetRank(zskiplist *zsl, double score, robj *o);

/* Core functions */
int freeMemoryIfNeeded(void);
int processCommand(redisClient *c);
void setupSignalHandlers(void);
struct redisCommand *lookupCommand(sds name);
struct redisCommand *lookupCommandByCString(char *s);
struct redisCommand *lookupCommandOrOriginal(sds name);
void call(redisClient *c, int flags);
void propagate(struct redisCommand *cmd, int dbid, robj **argv, int argc, int flags);
void alsoPropagate(struct redisCommand *cmd, int dbid, robj **argv, int argc, int target);
void forceCommandPropagation(redisClient *c, int flags);
int prepareForShutdown();
#ifdef __GNUC__
void redisLog(int level, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
void redisLog(int level, const char *fmt, ...);
#endif
void redisLogRaw(int level, const char *msg);
void redisLogFromHandler(int level, const char *msg);
void usage();
void updateDictResizePolicy(void);
int htNeedsResize(dict *dict);
void oom(const char *msg);
void populateCommandTable(void);
void resetCommandTableStats(void);
void adjustOpenFilesLimit(void);
void closeListeningSockets(int unlink_unix_socket);
void updateCachedTime(void);
void resetServerStats(void);
unsigned int getLRUClock(void);

/* Set data type */
robj *setTypeCreate(robj *value);
int setTypeAdd(robj *subject, robj *value);
int setTypeRemove(robj *subject, robj *value);
int setTypeIsMember(robj *subject, robj *value);
setTypeIterator *setTypeInitIterator(robj *subject);
void setTypeReleaseIterator(setTypeIterator *si);
int setTypeNext(setTypeIterator *si, robj **objele, int64_t *llele);
robj *setTypeNextObject(setTypeIterator *si);
int setTypeRandomElement(robj *setobj, robj **objele, int64_t *llele);
unsigned long setTypeSize(robj *subject);
void setTypeConvert(robj *subject, int enc);

/* Hash data type */
void hashTypeConvert(robj *o, int enc);
void hashTypeTryConversion(robj *subject, robj **argv, int start, int end);
void hashTypeTryObjectEncoding(robj *subject, robj **o1, robj **o2);
robj *hashTypeGetObject(robj *o, robj *key);
int hashTypeExists(robj *o, robj *key);
int hashTypeSet(robj *o, robj *key, robj *value);
int hashTypeDelete(robj *o, robj *key);
unsigned long hashTypeLength(robj *o);
hashTypeIterator *hashTypeInitIterator(robj *subject);
void hashTypeReleaseIterator(hashTypeIterator *hi);
int hashTypeNext(hashTypeIterator *hi);
void hashTypeCurrentFromZiplist(hashTypeIterator *hi, int what,
                                unsigned char **vstr,
                                unsigned int *vlen,
                                long long *vll);
void hashTypeCurrentFromHashTable(hashTypeIterator *hi, int what, robj **dst);
robj *hashTypeCurrentObject(hashTypeIterator *hi, int what);
robj *hashTypeLookupWriteOrCreate(redisClient *c, robj *key);

/* Pub / Sub */
int pubsubUnsubscribeAllChannels(redisClient *c, int notify);
int pubsubUnsubscribeAllPatterns(redisClient *c, int notify);
void freePubsubPattern(void *p);
int listMatchPubsubPattern(void *a, void *b);
int pubsubPublishMessage(robj *channel, robj *message);

/* Keyspace events notification */
void notifyKeyspaceEvent(int type, char *event, robj *key, int dbid);
int keyspaceEventsStringToFlags(char *classes);
sds keyspaceEventsFlagsToString(int flags);

/* Configuration */
void loadServerConfig(char *filename, char *options);
void appendServerSaveParams(time_t seconds, int changes);
void resetServerSaveParams();
struct rewriteConfigState; /* Forward declaration to export API. */
void rewriteConfigRewriteLine(struct rewriteConfigState *state, char *option, sds line, int force);
int rewriteConfig(char *path);

/* db.c -- Keyspace access API */
int removeExpire(redisDb *db, robj *key);
void propagateExpire(redisDb *db, robj *key);
int expireIfNeeded(redisDb *db, robj *key);
long long getExpire(redisDb *db, robj *key);
void setExpire(redisDb *db, robj *key, long long when);
robj *lookupKey(redisDb *db, robj *key);
robj *lookupKeyRead(redisDb *db, robj *key);
robj *lookupKeyWrite(redisDb *db, robj *key);
robj *lookupKeyReadOrReply(redisClient *c, robj *key, robj *reply);
robj *lookupKeyWriteOrReply(redisClient *c, robj *key, robj *reply);
void dbAdd(redisDb *db, robj *key, robj *val);
void dbOverwrite(redisDb *db, robj *key, robj *val);
void setKey(redisDb *db, robj *key, robj *val);
int dbExists(redisDb *db, robj *key);
robj *dbRandomKey(redisDb *db);
int dbDelete(redisDb *db, robj *key);
robj *dbUnshareStringValue(redisDb *db, robj *key, robj *o);
long long emptyDb(void(callback)(void*));
int selectDb(redisClient *c, int id);
void signalModifiedKey(redisDb *db, robj *key);
void signalFlushedDb(int dbid);
unsigned int getKeysInSlot(unsigned int hashslot, robj **keys, unsigned int count);
unsigned int countKeysInSlot(unsigned int hashslot);
unsigned int delKeysInSlot(unsigned int hashslot);
int verifyClusterConfigWithData(void);
void scanGenericCommand(redisClient *c, robj *o, unsigned long cursor);
int parseScanCursorOrReply(redisClient *c, robj *o, unsigned long *cursor);

/* API to get key arguments from commands */
int *getKeysFromCommand(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);
void getKeysFreeResult(int *result);
int *zunionInterGetKeys(struct redisCommand *cmd,robj **argv, int argc, int *numkeys);
int *evalGetKeys(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);
int *sortGetKeys(struct redisCommand *cmd, robj **argv, int argc, int *numkeys);

/* Cluster */
void clusterInit(void);
unsigned short crc16(const char *buf, int len);
unsigned int keyHashSlot(char *key, int keylen);
void clusterCron(void);
void clusterPropagatePublish(robj *channel, robj *message);
void migrateCloseTimedoutSockets(void);
void clusterBeforeSleep(void);

/* Sentinel */
void initSentinelConfig(void);
void initSentinel(void);
void sentinelTimer(void);
char *sentinelHandleConfiguration(char **argv, int argc);
void sentinelIsRunning(void);

/* Scripting */
void scriptingInit(void);

/* Blocked clients */
void processUnblockedClients(void);
void blockClient(redisClient *c, int btype);
void unblockClient(redisClient *c);
void replyToBlockedClientTimedOut(redisClient *c);
int getTimeoutFromObjectOrReply(redisClient *c, robj *object, mstime_t *timeout, int unit);

/* Git SHA1 */
char *redisGitSHA1(void);
char *redisGitDirty(void);
uint64_t redisBuildId(void);

/* Commands prototypes */
void authCommand(redisClient *c);
void pingCommand(redisClient *c);
void echoCommand(redisClient *c);
void setCommand(redisClient *c);
void setnxCommand(redisClient *c);
void setexCommand(redisClient *c);
void psetexCommand(redisClient *c);
void getCommand(redisClient *c);
void delCommand(redisClient *c);
void existsCommand(redisClient *c);
void setbitCommand(redisClient *c);
void getbitCommand(redisClient *c);
void setrangeCommand(redisClient *c);
void getrangeCommand(redisClient *c);
void incrCommand(redisClient *c);
void decrCommand(redisClient *c);
void incrbyCommand(redisClient *c);
void decrbyCommand(redisClient *c);
void incrbyfloatCommand(redisClient *c);
void selectCommand(redisClient *c);
void randomkeyCommand(redisClient *c);
void keysCommand(redisClient *c);
void scanCommand(redisClient *c);
void dbsizeCommand(redisClient *c);
void lastsaveCommand(redisClient *c);
void saveCommand(redisClient *c);
void bgsaveCommand(redisClient *c);
void bgrewriteaofCommand(redisClient *c);
void shutdownCommand(redisClient *c);
void moveCommand(redisClient *c);
void renameCommand(redisClient *c);
void renamenxCommand(redisClient *c);
void lpushCommand(redisClient *c);
void rpushCommand(redisClient *c);
void lpushxCommand(redisClient *c);
void rpushxCommand(redisClient *c);
void linsertCommand(redisClient *c);
void lpopCommand(redisClient *c);
void rpopCommand(redisClient *c);
void llenCommand(redisClient *c);
void lindexCommand(redisClient *c);
void lrangeCommand(redisClient *c);
void ltrimCommand(redisClient *c);
void typeCommand(redisClient *c);
void lsetCommand(redisClient *c);
void saddCommand(redisClient *c);
void sremCommand(redisClient *c);
void smoveCommand(redisClient *c);
void sismemberCommand(redisClient *c);
void scardCommand(redisClient *c);
void spopCommand(redisClient *c);
void srandmemberCommand(redisClient *c);
void sinterCommand(redisClient *c);
void sinterstoreCommand(redisClient *c);
void sunionCommand(redisClient *c);
void sunionstoreCommand(redisClient *c);
void sdiffCommand(redisClient *c);
void sdiffstoreCommand(redisClient *c);
void sscanCommand(redisClient *c);
void syncCommand(redisClient *c);
void flushdbCommand(redisClient *c);
void flushallCommand(redisClient *c);
void sortCommand(redisClient *c);
void lremCommand(redisClient *c);
void rpoplpushCommand(redisClient *c);
void infoCommand(redisClient *c);
void mgetCommand(redisClient *c);
void monitorCommand(redisClient *c);
void expireCommand(redisClient *c);
void expireatCommand(redisClient *c);
void pexpireCommand(redisClient *c);
void pexpireatCommand(redisClient *c);
void getsetCommand(redisClient *c);
void ttlCommand(redisClient *c);
void pttlCommand(redisClient *c);
void persistCommand(redisClient *c);
void slaveofCommand(redisClient *c);
void debugCommand(redisClient *c);
void msetCommand(redisClient *c);
void msetnxCommand(redisClient *c);
void zaddCommand(redisClient *c);
void zincrbyCommand(redisClient *c);
void zrangeCommand(redisClient *c);
void zrangebyscoreCommand(redisClient *c);
void zrevrangebyscoreCommand(redisClient *c);
void zrangebylexCommand(redisClient *c);
void zrevrangebylexCommand(redisClient *c);
void zcountCommand(redisClient *c);
void zlexcountCommand(redisClient *c);
void zrevrangeCommand(redisClient *c);
void zcardCommand(redisClient *c);
void zremCommand(redisClient *c);
void zscoreCommand(redisClient *c);
void zremrangebyscoreCommand(redisClient *c);
void zremrangebylexCommand(redisClient *c);
void multiCommand(redisClient *c);
void execCommand(redisClient *c);
void discardCommand(redisClient *c);
void blpopCommand(redisClient *c);
void brpopCommand(redisClient *c);
void brpoplpushCommand(redisClient *c);
void appendCommand(redisClient *c);
void strlenCommand(redisClient *c);
void zrankCommand(redisClient *c);
void zrevrankCommand(redisClient *c);
void hsetCommand(redisClient *c);
void hsetnxCommand(redisClient *c);
void hgetCommand(redisClient *c);
void hmsetCommand(redisClient *c);
void hmgetCommand(redisClient *c);
void hdelCommand(redisClient *c);
void hlenCommand(redisClient *c);
void zremrangebyrankCommand(redisClient *c);
void zunionstoreCommand(redisClient *c);
void zinterstoreCommand(redisClient *c);
void zscanCommand(redisClient *c);
void hkeysCommand(redisClient *c);
void hvalsCommand(redisClient *c);
void hgetallCommand(redisClient *c);
void hexistsCommand(redisClient *c);
void hscanCommand(redisClient *c);
void configCommand(redisClient *c);
void hincrbyCommand(redisClient *c);
void hincrbyfloatCommand(redisClient *c);
void subscribeCommand(redisClient *c);
void unsubscribeCommand(redisClient *c);
void psubscribeCommand(redisClient *c);
void punsubscribeCommand(redisClient *c);
void publishCommand(redisClient *c);
void pubsubCommand(redisClient *c);
void watchCommand(redisClient *c);
void unwatchCommand(redisClient *c);
void clusterCommand(redisClient *c);
void restoreCommand(redisClient *c);
void migrateCommand(redisClient *c);
void askingCommand(redisClient *c);
void readonlyCommand(redisClient *c);
void readwriteCommand(redisClient *c);
void dumpCommand(redisClient *c);
void objectCommand(redisClient *c);
void clientCommand(redisClient *c);
void evalCommand(redisClient *c);
void evalShaCommand(redisClient *c);
void scriptCommand(redisClient *c);
void timeCommand(redisClient *c);
void bitopCommand(redisClient *c);
void bitcountCommand(redisClient *c);
void bitposCommand(redisClient *c);
void replconfCommand(redisClient *c);
void waitCommand(redisClient *c);
void pfselftestCommand(redisClient *c);
void pfaddCommand(redisClient *c);
void pfcountCommand(redisClient *c);
void pfmergeCommand(redisClient *c);
void pfdebugCommand(redisClient *c);

#if defined(__GNUC__)
void *calloc(size_t count, size_t size) __attribute__ ((deprecated));
void free(void *ptr) __attribute__ ((deprecated));
void *malloc(size_t size) __attribute__ ((deprecated));
void *realloc(void *ptr, size_t size) __attribute__ ((deprecated));
#endif

/* Debugging stuff */
void _redisAssertWithInfo(redisClient *c, robj *o, char *estr, char *file, int line);
void _redisAssert(char *estr, char *file, int line);
void _redisPanic(char *msg, char *file, int line);
void bugReportStart(void);
void redisLogObjectDebugInfo(robj *o);
void sigsegvHandler(int sig, siginfo_t *info, void *secret);
sds genRedisInfoString(char *section);
void enableWatchdog(int period);
void disableWatchdog(void);
void watchdogScheduleSignal(int period);
void redisLogHexDump(int level, char *descr, void *value, size_t len);

#define redisDebug(fmt, ...) \
    printf("DEBUG %s:%d > " fmt "\n", __FILE__, __LINE__, __VA_ARGS__)
#define redisDebugMark() \
    printf("-- MARK %s:%d --\n", __FILE__, __LINE__)

#endif
