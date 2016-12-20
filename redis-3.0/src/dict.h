/* Hash Tables Implementation.
 *
 * This file implements in-memory hash tables with insert/del/replace/find/
 * get-random-element operations. Hash tables will auto-resize if needed
 * tables of power of two in size are used, collisions are handled by
 * chaining. See the source code for more information... :)
 *
 * 这个文件实现了一个内存哈希表，
 * 它支持插入、删除、替换、查找和获取随机元素等操作。
 *
 * 哈希表会自动在表的大小的二次方之间进行调整。
 *
 * 键的冲突通过链表来解决。
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
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

#include <stdint.h>

#ifndef __DICT_H
#define __DICT_H

/*
 * 字典的操作状态
 */
// 操作成功
#define DICT_OK 0
// 操作失败（或出错）
#define DICT_ERR 1

/* Unused arguments generate annoying warnings... */
// 如果字典的私有数据不使用时
// 用这个宏来避免编译器错误
#define DICT_NOTUSED(V) ((void) V)

/*
    首先命令行中的字符串先保存到进入xxxCommand(如setCommand)函数时，各个命令字符串单纯已经转换为redisObject保存到redisClient->argv[]中，
    然后在setKey等相关函数中把key-value转换为dictEntry节点的key和v(分别对应key和value)添加到dict->dictht->table[]  hash中,至于怎么添加到dictEntry
    节点的key和value中可以参考dict->type( type为xxxDictType 例如keyptrDictType等) ，见dictCreate
*/

/*
 * 哈希表节点
 */
typedef struct dictEntry { 
//该结构式字典dictht->table[]  hash中的成员结构，任何key - value键值对都会添加到转换为dictEntry结构添加到字典hash table中
    /*
  key 属性保存着键值对中的键， 
  而 v 属性则保存着键值对中的值， 其中键值对的值可以是一个指针， 或者是一个 uint64_t 整数， 又或者是一个 int64_t 整数。
     */
     /*
    首先命令行中的字符串先保存到进入xxxCommand(如setCommand)函数时，各个命令字符串单纯已经转换为redisObject保存到redisClient->argv[]中，
    然后在setKey等相关函数中把key-value转换为dictEntry节点的key和v(分别对应key和value)添加到dictht->table[]  hash中
    */
    // 键
    void *key; //对应一个robj
    
    /*
      key 属性保存着键值对中的键， 
      而 v 属性则保存着键值对中的值， 其中键值对的值可以是一个指针， 或者是一个 uint64_t 整数， 又或者是一个 int64_t 整数。
     */
     /*
    首先命令行中的字符串先保存到进入xxxCommand(如setCommand)函数时，各个命令字符串单纯已经转换为redisObject保存到redisClient->argv[]中，
    然后在setKey等相关函数中把key-value转换为dictEntry节点的key和v(分别对应key和value)添加到dictht->table[]  hash中
    */
    // 值
    union {
        void *val;
        uint64_t u64;
        int64_t s64;//一般记录的是过期键db->expires中每个键的过期时间  单位ms
    } v;//对应一个robj

    
    //next 属性是指向另一个哈希表节点的指针， 这个指针可以将多个哈希值相同的键值对连接在一次， 以此来解决键冲突（collision）的问题。
    // 链往后继节点
    // 指向下个哈希表节点，形成链表
    struct dictEntry *next;

} dictEntry;


/*
 * 字典类型特定函数
 */ //dictType主要由xxxDictType(dbDictType zsetDictType setDictType等)
typedef struct dictType {//函数privdata参数从dict->privdata中获取

    // 计算哈希值的函数 // 计算键的哈希值函数, 计算key在hash table中的存储位置，不同的dict可以有不同的hash function.
    unsigned int (*hashFunction)(const void *key);//dictHashKey中执行该函数

    // 复制键的函数
    void *(*keyDup)(void *privdata, const void *key);//dictSetKey

    // 复制值的函数 //也就是以key为键值计算出一个值来选出对应的hash桶，把key节点放入桶中，同时执行valdup来分配空间存储key对应的value
    void *(*valDup)(void *privdata, const void *obj); //dictSetVal  保存在dictEntry->v->value中，然后在

    // 对比键的函数
    int (*keyCompare)(void *privdata, const void *key1, const void *key2);//dictCompareKeys

    // 销毁键的函数
    void (*keyDestructor)(void *privdata, void *key);//dictFreeKey
    
    // 销毁值的函数 // 值的释构函数  dictFreeVal  删除hash中的key节点的时候会执行该函数进行删除value
    void (*valDestructor)(void *privdata, void *obj);//dictFreeVal

} dictType;


/*
解决键冲突

当有两个或以上数量的键被分配到了哈希表数组的同一个索引上面时， 我们称这些键发生了冲突（collision）。

Redis 的哈希表使用链地址法（separate chaining）来解决键冲突： 每个哈希表节点都有一个 next 指针， 多个哈希表节点可以用 next 
指针构成一个单向链表， 被分配到同一个索引上的多个节点可以用这个单向链表连接起来， 这就解决了键冲突的问题。

举个例子， 假设程序要将键值对 k2 和 v2 添加到图 4-6 所示的哈希表里面， 并且计算得出 k2 的索引值为 2 ， 那么键 k1 和 k2 将
产生冲突， 而解决冲突的办法就是使用 next 指针将键 k2 和 k1 所在的节点连接起来，

因为 dictEntry 节点组成的链表没有指向链表表尾的指针， 所以为了速度考虑， 程序总是将新节点添加到链表的表头位置（复杂度为 O(1)）， 
排在其他已有节点的前面。
*/

/*
rehash

随着操作的不断执行， 哈希表保存的键值对会逐渐地增多或者减少， 为了让哈希表的负载因子（load factor）维持在一个合理的范围之内， 
当哈希表保存的键值对数量太多或者太少时， 程序需要对哈希表的大小进行相应的扩展或者收缩。

扩展和收缩哈希表的工作可以通过执行 rehash （重新散列）操作来完成， Redis 对字典的哈希表执行 rehash 的步骤如下：
1.为字典的 ht[1] 哈希表分配空间， 这个哈希表的空间大小取决于要执行的操作， 以及 ht[0] 当前包含的键值对数量 （也即是 ht[0].used 
属性的值）：如果执行的是扩展操作， 那么 ht[1] 的大小为第一个大于等于 ht[0].used * 2 的 2^n （2 的 n 次方幂）；例如现在used是5，则
ht[1] hash表中的桶个数就是大于等于5 *2=10的最近的一个2的n次幂，也就是16，同理如果是8等，则为大于等于8*2=16最近的2的n次幂，就是16

如果执行的是收缩操作， 那么 ht[1] 的大小为第一个大于等于 ht[0].used 的 2^n 。例如5，则离他最近的2的n次幂为8，如果为8，则还是为8 ????????感觉这个有点问题，这不是扩容嘛，没收缩

2.将保存在 ht[0] 中的所有键值对 rehash 到 ht[1] 上面： rehash 指的是重新计算键的哈希值和索引值， 然后将键值对放置到 ht[1] 哈希表的指定位置上。
3.当 ht[0] 包含的所有键值对都迁移到了 ht[1] 之后 （ht[0] 变为空表）， 释放 ht[0] ， 将 ht[1] 设置为 ht[0] ， 并在 ht[1] 新创建一个空白哈希表， 为下一次 rehash 做准备。

举个例子， 假设程序要对字典的 ht[0] 进行扩展操作， 那么程序将执行以下步骤：
1.ht[0].used 当前的值为 4 ， 4 * 2 = 8 ， 而 8 （2^3）恰好是第一个大于等于 4 的 2 的 n 次方， 所以程序会将 ht[1] 哈希表的大小设置为 8 。 扩展后的大小参考_dictExpandIfNeeded
2.将 ht[0] 包含的四个键值对都 rehash 到 ht[1]
3.释放 ht[0] ，并将 ht[1] 设置为 ht[0] ，然后为 ht[1] 分配一个空白哈希表

至此， 对哈希表的扩展操作执行完毕， 程序成功将哈希表的大小从原来的 4 改为了现在的 8 。



哈希表的扩展与收缩

当以下条件中的任意一个被满足时， 程序会自动开始对哈希表执行扩展操作：
1.服务器目前没有在执行 BGSAVE 命令或者 BGREWRITEAOF 命令， 并且哈希表的负载因子大于等于 1 ；
2.服务器目前正在执行 BGSAVE 命令或者 BGREWRITEAOF 命令， 并且哈希表的负载因子大于等于 5 ；
另一方面， 当哈希表的负载因子小于 0.1 时， 程序自动开始对哈希表执行收缩操作。

其中哈希表的负载因子可以通过公式：


# 负载因子 = 哈希表已保存节点数量 / 哈希表大小
load_factor = ht[0].used / ht[0].size

计算得出。
比如说， 对于一个大小为 4 ， 包含 4 个键值对的哈希表来说， 这个哈希表的负载因子为：
load_factor = 4 / 4 = 1
又比如说， 对于一个大小为 512 ， 包含 256 个键值对的哈希表来说， 这个哈希表的负载因子为：
load_factor = 256 / 512 = 0.5

根据 BGSAVE 命令或 BGREWRITEAOF 命令是否正在执行， 服务器执行扩展操作所需的负载因子并不相同， 这是因为在执行 BGSAVE 命令
或 BGREWRITEAOF 命令的过程中， Redis 需要创建当前服务器进程的子进程， 而大多数操作系统都采用写时复制（copy-on-write）技术
来优化子进程的使用效率， 所以在子进程存在期间， 服务器会提高执行扩展操作所需的负载因子， 从而尽可能地避免在子进程存在期间
进行哈希表扩展操作， 这可以避免不必要的内存写入操作， 最大限度地节约内存。


另一方面， 当哈希表的负载因子小于 0.1 时， 程序自动开始对哈希表执行收缩操作。

1）总的元素个数 除 DICT桶的个数得到每个桶平均存储的元素个数(pre_num),如果 pre_num > dict_force_resize_ratio,就会触发dict 扩大操作。dict_force_resize_ratio = 5。

2）在总元素 * 10 < 桶的个数，也就是,填充率必须<10%, DICT便会进行收缩，让total / bk_num 接近 1:1。


rehash扩大调用关系调用过程:dictAddRaw->_dictKeyIndex->_dictExpandIfNeeded->dictExpand，这个函数调用关系是需要扩大dict的调用关系，
*/

/*
渐进式 rehash?

上一节说过， 扩展或收缩哈希表需要将 ht[0] 里面的所有键值对 rehash 到 ht[1] 里面， 但是， 这个 rehash 动作并不是一次性、
集中式地完成的， 而是分多次、渐进式地完成的。

这样做的原因在于， 如果 ht[0] 里只保存着四个键值对， 那么服务器可以在瞬间就将这些键值对全部 rehash 到 ht[1] ； 但是， 
如果哈希表里保存的键值对数量不是四个， 而是四百万、四千万甚至四亿个键值对， 那么要一次性将这些键值对全部 rehash 到 ht[1] 
的话， 庞大的计算量可能会导致服务器在一段时间内停止服务。

因此， 为了避免 rehash 对服务器性能造成影响， 服务器不是一次性将 ht[0] 里面的所有键值对全部 rehash 到 ht[1] ， 而是分多次、
渐进式地将 ht[0] 里面的键值对慢慢地 rehash 到 ht[1] 。

以下是哈希表渐进式 rehash 的详细步骤：
1.为 ht[1] 分配空间， 让字典同时持有 ht[0] 和 ht[1] 两个哈希表。
2.在字典中维持一个索引计数器变量 rehashidx ， 并将它的值设置为 0 ， 表示 rehash 工作正式开始。rehashidx表示的是hash桶标记，
    现在操作的是那个hash桶table[i]
3.在 rehash 进行期间， 每次对字典执行添加、删除、查找或者更新操作时， 程序除了执行指定的操作以外， 还会顺带将 ht[0] 哈希表在 
  rehashidx 索引上的所有键值对 rehash 到 ht[1] ， 当 rehash 工作完成之后， 程序将 rehashidx 属性的值增一。也就是rehash操作是由对该
  hash具体桶table[i]的增加 删除 操作 跟新等操作触发的，触发到该桶，则把该桶中数据放入ht[1] hash表总
4.随着字典操作的不断执行， 最终在某个时间点上， ht[0] 的所有键值对都会被 rehash 至 ht[1] ， 这时程序将 rehashidx 属性的值设为 -1 ， 表示 rehash 操作已完成。

渐进式 rehash 的好处在于它采取分而治之的方式， 将 rehash 键值对所需的计算工作均滩到对字典的每个添加、删除、查找和更新操作上， 
从而避免了集中式 rehash 而带来的庞大计算量。


渐进式 rehash 执行期间的哈希表操作?

因为在进行渐进式 rehash 的过程中， 字典会同时使用 ht[0] 和 ht[1] 两个哈希表， 所以在渐进式 rehash 进行期间， 字典的删除（delete）、
查找（find）、更新（update）等操作会在两个哈希表上进行： 比如说， 要在字典里面查找一个键的话， 程序会先在 ht[0] 里面进行查找， 
如果没找到的话， 就会继续到 ht[1] 里面进行查找， 诸如此类。

另外， 在渐进式 rehash 执行期间， 新添加到字典的键值对一律会被保存到 ht[1] 里面， 而 ht[0] 则不再进行任何添加操作： 
这一措施保证了 ht[0] 包含的键值对数量会只减不增， 并随着 rehash 操作的执行而最终变成空表。


 rehash扩大调用关系调用过程:dictAddRaw->_dictKeyIndex->_dictExpandIfNeeded(这里绝对是否需要扩容)->dictExpand 
//缩减hash过程:serverCron->tryResizeHashTables->dictResize(这里绝对缩减后的桶数)->dictExpand 

实际从ht[0]到ht[1]的过程:每步 rehash 都会移动哈希表数组内某个索引上的整个链表节点，所以从 ht[0] 迁移到 ht[1] 的key 
        可能不止一个。见函数dictRehash
*/



/* This is our hash table structure. Every dictionary has two of this as we
 * implement incremental rehashing, for the old to the new table. */
/*
 * 哈希表
 *
 * 每个字典都使用两个哈希表，从而实现渐进式 rehash 。
 */ 

/*
    首先命令行中的字符串先保存到进入xxxCommand(如setCommand)函数时，各个命令字符串单纯已经转换为redisObject保存到redisClient->argv[]中，
    然后在setKey等相关函数中把key-value转换为dictEntry节点的key和v(分别对应key和value)添加到dict->dictht->table[]  hash中,至于怎么添加到dictEntry
    节点的key和value中可以参考dict->type( type为xxxDictType 例如keyptrDictType等) ，见dictCreate
*/

 //初始化及赋值见dictExpand
typedef struct dictht {//dictht hash桶存在于dict结构中

    //每个具体table[i]中的节点数据类型是dictEntry 结构表示， 每个 dictEntry 结构都保存着一个键值对：
    // 哈希表节点指针数组（俗称桶，bucket）
    // 哈希表数组
    dictEntry **table;//table[idx]指向的是首个dictEntry节点，见dictAddRaw  创建初始化table桶见dictExpand

    // 哈希表大小
    unsigned long size;//表示该hash中实际桶的个数
    
    // 哈希表大小掩码，用于计算索引值
    // 总是等于 size - 1 // 指针数组的长度掩码，用于计算索引值   生效见_dictKeyIndex
    unsigned long sizemask; //sizemask = size-1 因为具体的桶是从0到size-1

    // 该哈希表已有节点的数量
    unsigned long used;

} dictht;

/*
字典 API

函数                                 作用                                         时间复杂度


dictCreate                      创建一个新的字典。                                  O(1) 
dictAdd                         将给定的键值对添加到字典里面。                      O(1) 
dictReplace                     将给定的键值对添加到字典里面， 如果键已经
                                存在于字典，那么用新值取代原有的值。                O(1) 
dictFetchValue                  返回给定键的值。                                    O(1) 
dictGetRandomKey                从字典中随机返回一个键值对。                        O(1) 
dictDelete                      从字典中删除给定键所对应的键值对。                  O(1) 
dictRelease                     释放给定字典，以及字典中包含的所有键值对。          O(N) ， N 为字典包含的键值对数量。 
_dictExpandIfNeeded             hash扩容大小在这里判断

*/


/*
    首先命令行中的字符串先保存到进入xxxCommand(如setCommand)函数时，各个命令字符串单纯已经转换为redisObject保存到redisClient->argv[]中，
    然后在setKey等相关函数中把key-value转换为dictEntry节点的key和v(分别对应key和value)添加到dict->dictht->table[]  hash中,至于怎么添加到dictEntry
    节点的key和value中可以参考dict->type( type为xxxDictType 例如keyptrDictType等) ，见dictCreate
*/

/*
 * 字典
 */
typedef struct dict {//dictCreate创建和初始化

    //type 属性是一个指向 dictType 结构的指针， 每个 dictType 结构保存了一簇用于操作特定类型键值对的函数， Redis 会为用途不
//同的字典设置不同的类型特定函数。
    // 类型特定函数
    dictType *type;

    // 私有数据 // 类型处理函数的私有数据  privdata 属性则保存了需要传给那些类型type特定函数的可选参数。
    void *privdata;

    /*
    ht 属性是一个包含两个项的数组， 数组中的每个项都是一个 dictht 哈希表， 一般情况下， 字典只使用 ht[0] 哈希表， ht[1] 哈希表只
    会在对 ht[0] 哈希表进行 rehash 时使用。
    
    除了 ht[1] 之外， 另一个和 rehash 有关的属性就是 rehashidx ： 它记录了 rehash 目前的进度， 如果目前没有在进行 rehash ， 
    那么它的值为 -1 。
    */

    // 哈希表
    dictht ht[2];//dictht hash桶初始化创建见dictExpand     

    // rehash 索引
    // 当 rehash 不在进行时，值为 -1  // 记录 rehash 进度的标志，值为-1 表示 rehash 未进行
    
    //判断是否需要rehash dictIsRehashing  _dictInit初始化-1 //dictRehash中子曾，dictExpand中置0，如果是迁移完毕置-1
    int rehashidx; /* rehashing not in progress if rehashidx == -1 */

    // 目前正在运行的安全迭代器的数量
    int iterators; /* number of iterators currently running */

} dict; //dict空间创建初始化在dictExpand，第一次是在_dictExpandIfNeededif->dictExpand(d, DICT_HT_INITIAL_SIZE);

/* If safe is set to 1 this is a safe iterator, that means, you can call
 * dictAdd, dictFind, and other functions against the dictionary even while
 * iterating. Otherwise it is a non safe iterator, and only dictNext()
 * should be called while iterating. */
/*
 * 字典迭代器
 *
 * 如果 safe 属性的值为 1 ，那么在迭代进行的过程中，
 * 程序仍然可以执行 dictAdd 、 dictFind 和其他函数，对字典进行修改。
 *
 * 如果 safe 不为 1 ，那么程序只会调用 dictNext 对字典进行迭代，
 * 而不对字典进行修改。
 */
typedef struct dictIterator {
        
    // 被迭代的字典
    dict *d;

    // table ：正在被迭代的哈希表号码，值可以是 0 或 1 。  dictht ht[2];中的哪一个
    // index ：迭代器当前所指向的哈希表索引位置。  对应具体的桶槽位号
    // safe ：标识这个迭代器是否安全
    int table, index, safe;

    // entry ：当前迭代到的节点的指针
    // nextEntry ：当前迭代节点的下一个节点
    //             因为在安全迭代器运作时， entry 所指向的节点可能会被修改，
    //             所以需要一个额外的指针来保存下一节点的位置，
    //             从而防止指针丢失
    dictEntry *entry, *nextEntry;

    long long fingerprint; /* unsafe iterator fingerprint for misuse detection */
} dictIterator;

typedef void (dictScanFunction)(void *privdata, const dictEntry *de);

/* This is the initial size of every hash table */
/*
 * 哈希表的初始大小
 */
#define DICT_HT_INITIAL_SIZE     4

/* ------------------------------- Macros ------------------------------------*/
// 释放给定字典节点的值
#define dictFreeVal(d, entry) \
    if ((d)->type->valDestructor) \
        (d)->type->valDestructor((d)->privdata, (entry)->v.val)

// 设置给定字典节点的值
#define dictSetVal(d, entry, _val_) do { \
    if ((d)->type->valDup) \
        entry->v.val = (d)->type->valDup((d)->privdata, _val_); \
    else \
        entry->v.val = (_val_); \
} while(0)

// 将一个有符号整数设为节点的值
#define dictSetSignedIntegerVal(entry, _val_) \
    do { entry->v.s64 = _val_; } while(0)

// 将一个无符号整数设为节点的值
#define dictSetUnsignedIntegerVal(entry, _val_) \
    do { entry->v.u64 = _val_; } while(0)

//dictType主要由xxxDictType(dbDictType zsetDictType setDictType等)
// 释放给定字典节点的键
#define dictFreeKey(d, entry) \
    if ((d)->type->keyDestructor) \
        (d)->type->keyDestructor((d)->privdata, (entry)->key)

//dictType主要由xxxDictType(dbDictType zsetDictType setDictType等)

// 设置给定字典节点的键
#define dictSetKey(d, entry, _key_) do { \
    if ((d)->type->keyDup) \
        entry->key = (d)->type->keyDup((d)->privdata, _key_); \
    else \
        entry->key = (_key_); \
} while(0)

//dictType主要由xxxDictType(dbDictType zsetDictType setDictType等)

// 比对两个键
#define dictCompareKeys(d, key1, key2) \
    (((d)->type->keyCompare) ? \
        (d)->type->keyCompare((d)->privdata, key1, key2) : \
        (key1) == (key2))

// 计算给定键的哈希值
#define dictHashKey(d, key) (d)->type->hashFunction(key)
// 返回获取给定节点的键
#define dictGetKey(he) ((he)->key)
// 返回获取给定节点的值
#define dictGetVal(he) ((he)->v.val)
// 返回获取给定节点的有符号整数值
#define dictGetSignedIntegerVal(he) ((he)->v.s64)
// 返回给定节点的无符号整数值
#define dictGetUnsignedIntegerVal(he) ((he)->v.u64)
// 返回给定字典的大小   hash桶的个数
#define dictSlots(d) ((d)->ht[0].size+(d)->ht[1].size)
// 返回字典的已有节点数量  所有桶中节点之和
#define dictSize(d) ((d)->ht[0].used+(d)->ht[1].used)
// 查看字典是否正在 rehash
#define dictIsRehashing(ht) ((ht)->rehashidx != -1)

/* API */
dict *dictCreate(dictType *type, void *privDataPtr);
int dictExpand(dict *d, unsigned long size);
int dictAdd(dict *d, void *key, void *val);
dictEntry *dictAddRaw(dict *d, void *key);
int dictReplace(dict *d, void *key, void *val);
dictEntry *dictReplaceRaw(dict *d, void *key);
int dictDelete(dict *d, const void *key);
int dictDeleteNoFree(dict *d, const void *key);
void dictRelease(dict *d);
dictEntry * dictFind(dict *d, const void *key);
void *dictFetchValue(dict *d, const void *key);
int dictResize(dict *d);
dictIterator *dictGetIterator(dict *d);
dictIterator *dictGetSafeIterator(dict *d);
dictEntry *dictNext(dictIterator *iter);
void dictReleaseIterator(dictIterator *iter);
dictEntry *dictGetRandomKey(dict *d);
int dictGetRandomKeys(dict *d, dictEntry **des, int count);
void dictPrintStats(dict *d);
unsigned int dictGenHashFunction(const void *key, int len);
unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len);
void dictEmpty(dict *d, void(callback)(void*));
void dictEnableResize(void);
void dictDisableResize(void);
int dictRehash(dict *d, int n);
int dictRehashMilliseconds(dict *d, int ms);
void dictSetHashFunctionSeed(unsigned int initval);
unsigned int dictGetHashFunctionSeed(void);
unsigned long dictScan(dict *d, unsigned long v, dictScanFunction *fn, void *privdata);

/* Hash table types */
extern dictType dictTypeHeapStringCopyKey;
extern dictType dictTypeHeapStrings;
extern dictType dictTypeHeapStringCopyKeyValue;

#endif /* __DICT_H */
