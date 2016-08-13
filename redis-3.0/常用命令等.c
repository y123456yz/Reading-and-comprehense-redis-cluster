/*
/*

? 

APPEND  key value  

Append a value to a key

? 

AUTH  password  

Authenticate to the server

? 

BGREWRITEAOF  

Asynchronously rewrite the append-only file

? 

BGSAVE  

Asynchronously save the dataset to disk

? 

BITCOUNT  key [start end]  

Count set bits in a string

? 

BITOP  operation destkey key [key ...]  

Perform bitwise operations between strings

? 

BITPOS  key bit [start] [end]  

Find first bit set or clear in a string

? 

BLPOP  key [key ...] timeout  

Remove and get the first element in a list, or block until one is available

? 

BRPOP  key [key ...] timeout  

Remove and get the last element in a list, or block until one is available

? 

BRPOPLPUSH  source destination timeout  

Pop a value from a list, push it to another list and return it; or block until one is available

? 

CLIENT KILL  [ip:port] [ID client-id] [TYPE normal|slave|pubsub] [ADDR ip:port] [SKIPME yes/no]  

Kill the connection of a client

? 

CLIENT LIST  

Get the list of client connections

? 

CLIENT GETNAME  

Get the current connection name

? 

CLIENT PAUSE  timeout  

Stop processing commands from clients for some time

? 

CLIENT SETNAME  connection-name  

Set the current connection name

? 

CLUSTER ADDSLOTS  slot [slot ...]  

Assign new hash slots to receiving node

? 

CLUSTER COUNT-FAILURE-REPORTS  node-id  

Return the number of failure reports active for a given node

? 

CLUSTER COUNTKEYSINSLOT  slot  

Return the number of local keys in the specified hash slot

? 

CLUSTER DELSLOTS  slot [slot ...]  

Set hash slots as unbound in receiving node

? 

CLUSTER FAILOVER  [FORCE|TAKEOVER]  

Forces a slave to perform a manual failover of its master.

? 

CLUSTER FORGET  node-id  

Remove a node from the nodes table

? 

CLUSTER GETKEYSINSLOT  slot count  

Return local key names in the specified hash slot

? 

CLUSTER INFO  

Provides info about Redis Cluster node state

? 

CLUSTER KEYSLOT  key  

Returns the hash slot of the specified key

? 

CLUSTER MEET  ip port  

Force a node cluster to handshake with another node

? 

CLUSTER NODES  

Get Cluster config for the node

? 

CLUSTER REPLICATE  node-id  

Reconfigure a node as a slave of the specified master node

? 

CLUSTER RESET  [HARD|SOFT]  

Reset a Redis Cluster node

? 

CLUSTER SAVECONFIG  

Forces the node to save cluster state on disk

? 

CLUSTER SET-CONFIG-EPOCH  config-epoch  

Set the configuration epoch in a new node

? 

CLUSTER SETSLOT  slot IMPORTING|MIGRATING|STABLE|NODE [node-id]  

Bind an hash slot to a specific node

? 

CLUSTER SLAVES  node-id  

List slave nodes of the specified master node

? 

CLUSTER SLOTS  

Get array of Cluster slot to node mappings

? 

COMMAND  

Get array of Redis command details

? 

COMMAND COUNT  

Get total number of Redis commands

? 

COMMAND GETKEYS  

Extract keys given a full Redis command

? 

COMMAND INFO  command-name [command-name ...]  

Get array of specific Redis command details

? 

CONFIG GET  parameter  

Get the value of a configuration parameter

? 

CONFIG REWRITE  

Rewrite the configuration file with the in memory configuration

? 

CONFIG SET  parameter value  

Set a configuration parameter to the given value

? 

CONFIG RESETSTAT  

Reset the stats returned by INFO

? 

DBSIZE  

Return the number of keys in the selected database

? 

DEBUG OBJECT  key  

Get debugging information about a key

? 

DEBUG SEGFAULT  

Make the server crash

? 

DECR  key  

Decrement the integer value of a key by one

? 

DECRBY  key decrement  

Decrement the integer value of a key by the given number

? 

DEL  key [key ...]  

Delete a key

? 

DISCARD  

Discard all commands issued after MULTI

? 

DUMP  key  

Return a serialized version of the value stored at the specified key.

? 

ECHO  message  

Echo the given string

? 

EVAL  script numkeys key [key ...] arg [arg ...]  

Execute a Lua script server side

? 

EVALSHA  sha1 numkeys key [key ...] arg [arg ...]  

Execute a Lua script server side

? 

EXEC  

Execute all commands issued after MULTI

? 

EXISTS  key [key ...]  

Determine if a key exists

? 

EXPIRE  key seconds  

Set a key's time to live in seconds

? 

EXPIREAT  key timestamp  

Set the expiration for a key as a UNIX timestamp

? 

FLUSHALL  

Remove all keys from all databases

? 

FLUSHDB  

Remove all keys from the current database

? 

GEOADD  key longitude latitude member [longitude latitude member ...]  

Add one or more geospatial items in the geospatial index represented using a sorted set

? 

GEOHASH  key member [member ...]  

Returns members of a geospatial index as standard geohash strings

? 

GEOPOS  key member [member ...]  

Returns longitude and latitude of members of a geospatial index

? 

GEODIST  key member1 member2 [unit]  

Returns the distance between two members of a geospatial index

? 

GEORADIUS  key longitude latitude radius m|km|ft|mi [WITHCOORD] [WITHDIST] [WITHHASH] [COUNT count]  

Query a sorted set representing a geospatial index to fetch members matching a given maximum distance from a point

? 

GEORADIUSBYMEMBER  key member radius m|km|ft|mi [WITHCOORD] [WITHDIST] [WITHHASH] [COUNT count]  

Query a sorted set representing a geospatial index to fetch members matching a given maximum distance from a member

? 

GET  key  

Get the value of a key

? 

GETBIT  key offset  

Returns the bit value at offset in the string value stored at key

? 

GETRANGE  key start end  

Get a substring of the string stored at a key

? 

GETSET  key value  

Set the string value of a key and return its old value

? 

HDEL  key field [field ...]  

Delete one or more hash fields

? 

HEXISTS  key field  

Determine if a hash field exists

? 

HGET  key field  

Get the value of a hash field

? 

HGETALL  key  

Get all the fields and values in a hash

? 

HINCRBY  key field increment  

Increment the integer value of a hash field by the given number

? 

HINCRBYFLOAT  key field increment  

Increment the float value of a hash field by the given amount

? 

HKEYS  key  

Get all the fields in a hash

? 

HLEN  key  

Get the number of fields in a hash

? 

HMGET  key field [field ...]  

Get the values of all the given hash fields

? 

HMSET  key field value [field value ...]  

Set multiple hash fields to multiple values

? 

HSET  key field value  

Set the string value of a hash field

? 

HSETNX  key field value  

Set the value of a hash field, only if the field does not exist

? 

HSTRLEN  key field  

Get the length of the value of a hash field

? 

HVALS  key  

Get all the values in a hash

? 

INCR  key  

Increment the integer value of a key by one

? 

INCRBY  key increment  

Increment the integer value of a key by the given amount

? 

INCRBYFLOAT  key increment  

Increment the float value of a key by the given amount

? 

INFO  [section]  

Get information and statistics about the server

? 

KEYS  pattern  

Find all keys matching the given pattern

? 

LASTSAVE  

Get the UNIX time stamp of the last successful save to disk

? 

LINDEX  key index  

Get an element from a list by its index

? 

LINSERT  key BEFORE|AFTER pivot value  

Insert an element before or after another element in a list

? 

LLEN  key  

Get the length of a list

? 

LPOP  key  

Remove and get the first element in a list

? 

LPUSH  key value [value ...]  

Prepend one or multiple values to a list

? 

LPUSHX  key value  

Prepend a value to a list, only if the list exists

? 

LRANGE  key start stop  

Get a range of elements from a list

? 

LREM  key count value  

Remove elements from a list

? 

LSET  key index value  

Set the value of an element in a list by its index

? 

LTRIM  key start stop  

Trim a list to the specified range

? 

MGET  key [key ...]  

Get the values of all the given keys

? 

MIGRATE  host port key destination-db timeout [COPY] [REPLACE]  

Atomically transfer a key from a Redis instance to another one.

? 

MONITOR  

Listen for all requests received by the server in real time

? 

MOVE  key db  

Move a key to another database

? 

MSET  key value [key value ...]  

Set multiple keys to multiple values

? 

MSETNX  key value [key value ...]  

Set multiple keys to multiple values, only if none of the keys exist

? 

MULTI  

Mark the start of a transaction block

? 

OBJECT  subcommand [arguments [arguments ...]]  

Inspect the internals of Redis objects

? 

PERSIST  key  

Remove the expiration from a key

? 

PEXPIRE  key milliseconds  

Set a key's time to live in milliseconds

? 

PEXPIREAT  key milliseconds-timestamp  

Set the expiration for a key as a UNIX timestamp specified in milliseconds

? 

PFADD  key element [element ...]  

Adds the specified elements to the specified HyperLogLog.

? 

PFCOUNT  key [key ...]  

Return the approximated cardinality of the set(s) observed by the HyperLogLog at key(s).

? 

PFMERGE  destkey sourcekey [sourcekey ...]  

Merge N different HyperLogLogs into a single one.

? 

PING  

Ping the server

? 

PSETEX  key milliseconds value  

Set the value and expiration in milliseconds of a key

? 

PSUBSCRIBE  pattern [pattern ...]  

Listen for messages published to channels matching the given patterns

? 

PUBSUB  subcommand [argument [argument ...]]  

Inspect the state of the Pub/Sub subsystem

? 

PTTL  key  

Get the time to live for a key in milliseconds

? 

PUBLISH  channel message  

Post a message to a channel

? 

PUNSUBSCRIBE  [pattern [pattern ...]]  

Stop listening for messages posted to channels matching the given patterns

? 

QUIT  

Close the connection

? 

RANDOMKEY  

Return a random key from the keyspace

? 

READONLY  

Enables read queries for a connection to a cluster slave node

? 

READWRITE  

Disables read queries for a connection to a cluster slave node

? 

RENAME  key newkey  

Rename a key

? 

RENAMENX  key newkey  

Rename a key, only if the new key does not exist

? 

RESTORE  key ttl serialized-value [REPLACE]  

Create a key using the provided serialized value, previously obtained using DUMP.

? 

ROLE  

Return the role of the instance in the context of replication

? 

RPOP  key  

Remove and get the last element in a list

? 

RPOPLPUSH  source destination  

Remove the last element in a list, prepend it to another list and return it

? 

RPUSH  key value [value ...]  

Append one or multiple values to a list

? 

RPUSHX  key value  

Append a value to a list, only if the list exists

? 

SADD  key member [member ...]  

Add one or more members to a set

? 

SAVE  

Synchronously save the dataset to disk

? 

SCARD  key  

Get the number of members in a set

? 

SCRIPT EXISTS  script [script ...]  

Check existence of scripts in the script cache.

? 

SCRIPT FLUSH  

Remove all the scripts from the script cache.

? 

SCRIPT KILL  

Kill the script currently in execution.

? 

SCRIPT LOAD  script  

Load the specified Lua script into the script cache.

? 

SDIFF  key [key ...]  

Subtract multiple sets

? 

SDIFFSTORE  destination key [key ...]  

Subtract multiple sets and store the resulting set in a key

? 

SELECT  index  

Change the selected database for the current connection

? 

SET  key value [EX seconds] [PX milliseconds] [NX|XX]  

Set the string value of a key

? 

SETBIT  key offset value  

Sets or clears the bit at offset in the string value stored at key

? 

SETEX  key seconds value  

Set the value and expiration of a key

? 

SETNX  key value  

Set the value of a key, only if the key does not exist

? 

SETRANGE  key offset value  

Overwrite part of a string at key starting at the specified offset

? 

SHUTDOWN  [NOSAVE] [SAVE]  

Synchronously save the dataset to disk and then shut down the server

? 

SINTER  key [key ...]  

Intersect multiple sets

? 

SINTERSTORE  destination key [key ...]  

Intersect multiple sets and store the resulting set in a key

? 

SISMEMBER  key member  

Determine if a given value is a member of a set

? 

SLAVEOF  host port  

Make the server a slave of another instance, or promote it as master

? 

SLOWLOG  subcommand [argument]  

Manages the Redis slow queries log

? 

SMEMBERS  key  

Get all the members in a set

? 

SMOVE  source destination member  

Move a member from one set to another

? 

SORT  key [BY pattern] [LIMIT offset count] [GET pattern [GET pattern ...]] [ASC|DESC] [ALPHA] [STORE destination]  

Sort the elements in a list, set or sorted set

? 

SPOP  key [count]  

Remove and return one or multiple random members from a set

? 

SRANDMEMBER  key [count]  

Get one or multiple random members from a set

? 

SREM  key member [member ...]  

Remove one or more members from a set

? 

STRLEN  key  

Get the length of the value stored in a key

? 

SUBSCRIBE  channel [channel ...]  

Listen for messages published to the given channels

? 

SUNION  key [key ...]  

Add multiple sets

? 

SUNIONSTORE  destination key [key ...]  

Add multiple sets and store the resulting set in a key

? 

SYNC  

Internal command used for replication

? 

TIME  

Return the current server time

? 

TTL  key  

Get the time to live for a key

? 

TYPE  key  

Determine the type stored at key

? 

UNSUBSCRIBE  [channel [channel ...]]  

Stop listening for messages posted to the given channels

? 

UNWATCH  

Forget about all watched keys

? 

WAIT  numslaves timeout  

Wait for the synchronous replication of all the write commands sent in the context of the current connection

? 

WATCH  key [key ...]  

Watch the given keys to determine execution of the MULTI/EXEC block

? 

ZADD  key [NX|XX] [CH] [INCR] score member [score member ...]  

Add one or more members to a sorted set, or update its score if it already exists

? 

ZCARD  key  

Get the number of members in a sorted set

? 

ZCOUNT  key min max  

Count the members in a sorted set with scores within the given values

? 

ZINCRBY  key increment member  

Increment the score of a member in a sorted set

? 

ZINTERSTORE  destination numkeys key [key ...] [WEIGHTS weight [weight ...]] [AGGREGATE SUM|MIN|MAX]  

Intersect multiple sorted sets and store the resulting sorted set in a new key

? 

ZLEXCOUNT  key min max  

Count the number of members in a sorted set between a given lexicographical range

? 

ZRANGE  key start stop [WITHSCORES]  

Return a range of members in a sorted set, by index

? 

ZRANGEBYLEX  key min max [LIMIT offset count]  

Return a range of members in a sorted set, by lexicographical range

? 

ZREVRANGEBYLEX  key max min [LIMIT offset count]  

Return a range of members in a sorted set, by lexicographical range, ordered from higher to lower strings.

? 

ZRANGEBYSCORE  key min max [WITHSCORES] [LIMIT offset count]  

Return a range of members in a sorted set, by score

? 

ZRANK  key member  

Determine the index of a member in a sorted set

? 

ZREM  key member [member ...]  

Remove one or more members from a sorted set

? 

ZREMRANGEBYLEX  key min max  

Remove all members in a sorted set between the given lexicographical range

? 

ZREMRANGEBYRANK  key start stop  

Remove all members in a sorted set within the given indexes

? 

ZREMRANGEBYSCORE  key min max  

Remove all members in a sorted set within the given scores

? 

ZREVRANGE  key start stop [WITHSCORES]  

Return a range of members in a sorted set, by index, with scores ordered from high to low

? 

ZREVRANGEBYSCORE  key max min [WITHSCORES] [LIMIT offset count]  

Return a range of members in a sorted set, by score, with scores ordered from high to low

? 

ZREVRANK  key member  

Determine the index of a member in a sorted set, with scores ordered from high to low

? 

ZSCORE  key member  

Get the score associated with the given member in a sorted set

? 

ZUNIONSTORE  destination numkeys key [key ...] [WEIGHTS weight [weight ...]] [AGGREGATE SUM|MIN|MAX]  

Add multiple sorted sets and store the resulting sorted set in a new key

? 

SCAN  cursor [MATCH pattern] [COUNT count]  

Incrementally iterate the keys space

? 

SSCAN  key cursor [MATCH pattern] [COUNT count]  

Incrementally iterate Set elements

? 

HSCAN  key cursor [MATCH pattern] [COUNT count]  

Incrementally iterate hash fields and associated values

? 

ZSCAN  key cursor [MATCH pattern] [COUNT count]  

Incrementally iterate sorted sets elements and associated scores





















Redis命令大全 


?本文主要内容 字符串命令
?列表命令和集合命令
?散列命令和有序集合命令
?发布命令与订阅命令
?其他命令 

3.1 字符串

本书在第1章和第2章曾经说过，Redis的字符串就是一个由字节组成的序列，它们和很多编程语言里面的字符串没有什么显著的不同，跟C或者C++风格的字符数组也相去不远。
在Redis里面，字符串可以存储以下3种类型的值。

字节串（byte string）。
整数。
浮点数。

用户可以通过给定一个任意的数值，对存储着整数或者浮点数的字符串执行自增（increment）或者自减（decrement）操作，在有需要的时候，Redis还会将整数转换成浮点数。
整数的取值范围和系统的长整数（long integer）的取值范围相同（在32位系统上，整数就是32位有符号整数，在64位系统上，整数就是64位有符号整数），而浮点数的取
值范围和精度则与IEEE 754标准的双精度浮点数（double）相同。Redis明确地区分字节串、整数和浮点数的做法是一种优势，比起只能够存储字节串的做法，Redis的做
法在数据表现方面具有更大的灵活性。

本节将对Redis里面最简单的结构—字符串进行讨论，介绍基本的数值自增和自减操作，以及二进制位（bit）和子串（substring）处理命令，读者可能会惊讶地发现，Redis里
面最简单的结构居然也有如此强大的作用。

命令				用例和描述
INCR				INCR key-name—将键存储的值加上1
DECR   				DECR key-name—将键存储的值减去1
INCRBY   			INCRBY key-name amount—将键存储的值加上整数amount
DECRBY			    DECRBY key-name amount—将键存储的值减去整数amount
INCRBYFLOAT			INCRBYFLOAT key-name amount—将键存储的值加上浮点数amount，这个命令在Redis 2.6或以上的版本可用
 

当用户将一个值存储到Redis字符串里面的时候，如果这个值可以被解释（interpret）为十进制整数或者浮点数，那么Redis会察觉到这一点，并允许用户对这个字符串
执行各种INCR和DECR操作。如果用户对一个不存在的键或者一个保存了空串的键执行自增或者自减操作，那么Redis在执行操作时会将这个键的值当作是0来处理。如果用
户尝试对一个值无法被解释为整数或者浮点数的字符串键执行自增或者自减操作，那么Redis将向用户返回一个错误。


在读完本书其他章节之后，读者可能会发现本书只调用了 incr()，这是因为 Python的Redis库在内部使用INCRBY命令来实现incr()方法，并且这个方法的第二个参数是可选的：
如果用户没有为这个可选参数设置值，那么这个参数就会使用默认值1。在编写本书的时候，Python的Redis客户端库支持Redis 2.6的所有命令，这个库通过incrbyfloat()
方法来实现INCRBYFLOAT命令，并且incrbyfloat()方法也有类似于incr()方法的可选参数特性。

除了自增操作和自减操作之外，Redis还拥有对字节串的其中一部分内容进行读取或者写入的操作（这些操作也可以用于整数或者浮点数，但这种用法并不常见），本书
在第9章将展示如何使用这些操作来高效地将结构化数据打包（pack）存储到字符串键里面。表3-2展示了用来处理字符串子串和二进制位的命令。

供Redis处理子串和二进制位的命令

命令    用例和描述

APPEND		APPEND key-name value—将值value追加到给定键key-name当前存储的值的末尾
GETRANGE	GETRANGE key-name start end—获取一个由偏移量start至偏移量end范围内所有字符组成的子串，包括start和end在内
SETRANGE	SETRANGE key-name offset value—将从start偏移量开始的子串设置为给定值
GETBIT		GETBIT key-name offset—将字节串看作是二进制位串（bit string），并返回位串中偏移量为offset的二进制位的值
SETBIT		SETBIT key-name offset value—将字节串看作是二进制位串，并将位串中偏移量为offset的二进制位的值设置为value
BITCOUNT	BITCOUNT key-name [start end]—统计二进制位串里面值为1的二进制位的数量，如果给定了可选的start偏移量和end偏移量，那么只对偏移量指定范围内的二进制位进行统计
BITOP		BITOP operation dest-key key-name [key-name ...]—对一个或多个二进制位串执行包括并（AND）、或（OR）、异或（XOR）、非（NOT）在内的任意一种按位运算操作（bitwise operation），
			并将计算得出的结果保存在dest-key键里面
 

GETRANGE和SUBSTR Redis现在的GETRANGE命令是由以前的SUBSTR命令改名而来的，因此，Python客户端至今仍然可以使用substr()方法来获取子串，但如果读者使用的是2.6或以上版本的Redis，
那么最好还是使用getrange()方法来获取子串。

在使用SETRANGE或者SETBIT命令对字符串进行写入的时候，如果字符串当前的长度不能满足写入的要求，那么Redis会自动地使用空字节（null）来将字符串扩展至所需的长度，然后才执行
写入或者更新操作。在使用GETRANGE读取字符串的时候，超出字符串末尾的数据会被视为是空串，而在使用GETBIT读取二进制位串的时候，超出字符串末尾的二进制位会被视为是0。代码

很多键值数据库只能将数据存储为普通的字符串，并且不提供任何字符串处理操作，有一些键值数据库允许用户将字节追加到字符串的前面或者后面，但是却没办法像Redis一样
对字符串的子串进行读写。从很多方面来讲，即使Redis只支持字符串结构，并且只支持本节列出的字符串处理命令，Redis也比很多别的数据库要强大得多；通过使用子串操作和
二进制位操作，配合WATCH命令、MULTI命令和EXEC命令（本书的3.7.2节将对这3个命令进行初步的介绍，并在第4章对它们进行更深入的讲解），用户甚至可以自己动手去构建任
何他们想要的数据结构。第9章将介绍如何使用字符串去存储一种简单的映射，这种映射可以在某些情况下节省大量内存。

只要花些心思，我们甚至可以将字符串当作列表来使用，但这种做法能够执行的列表操作并不多，更好的办法是直接使用下一节介绍的列表结构，Redis为这种结构提供了丰富的列表操作命令。

3.2 列表

在第1章曾经介绍过，Redis的列表允许用户从序列的两端推入或者弹出元素，获取列表元素，以及执行各种常见的列表操作。除此之外，列表还可以用来存储任务信息、最近浏览过的文章或者常用联系人信息。

本节将对列表这个由多个字符串值组成的有序序列结构进行介绍，并展示一些最常用的列表处理命令，阅读本节可以让读者学会如何使用这些命令来处理列表。表3-3展示了其中一部分最常用的列表命令。

一些常用的列表命令
命令			用例和描述

RPUSH
RPUSH key-name value [value ...]—将一个或多个值推入列表的右端
 
LPUSH
LPUSH key-name value [value ...]—将一个或多个值推入列表的左端

RPOP
RPOP key-name—移除并返回列表最右端的元素
 
LPOP
LPOP key-name—移除并返回列表最左端的元素
 
LINDEX
LINDEX key-name offset—返回列表中偏移量为offset的元素
 

LRANGE
LRANGE key-name start end—返回列表从start偏移量到end偏移量范围内的所有元素，其中偏移量为start和偏移量为end的元素也会包含在被返回的元素之内
 
LTRIM
LTRIM key-name start end—对列表进行修剪，只保留从start偏移量到end偏移量范围内的元素，其中偏移量为start和偏移量为end的元素也会被保留
 

 
阻塞式的列表弹出命令以及在列表之间移动元素的命令
命令		用例和描述
 


BLPOP
BLPOP key-name [key-name ...] timeout—从第一个非空列表中弹出位于最左端的元素，或者在timeout秒之内阻塞并等待可弹出的元素出现
 
BRPOP
BRPOP key-name [key-name ...] timeout—从第一个非空列表中弹出位于最右端的元素，或者在timeout秒之内阻塞并等待可弹出的元素出现
 
RPOPLPUSH
RPOPLPUSH source-key dest-key—从source-key列表中弹出位于最右端的元素，然后将这个元素推入dest-key列表的最左端，并向用户返回这个元素
 
BRPOPLPUSH
BRPOPLPUSH source-key dest-key timeout—从source-key列表中弹出位于最右端的元素，然后将这个元素推入dest-key列表的最左端，并向用户返回这个元素；如果source-key为空，那么在timeout秒之内阻塞并等待可弹出的元素出现
 
对于阻塞弹出命令和弹出并推入命令，最常见的用例就是消息传递（messaging）和任务队列（task queue），本书将在第6章对这两个主题进行介绍。

练习：通过列表来降低内存占用 在2.1节和2.5节中，我们使用了有序集合来记录用户最近浏览过的商品，并把用户浏览这些商品时的时间戳设置为分值，
从而使得程序可以在清理旧会话的过程中或是执行完购买操作之后，进行相应的数据分析。但由于保存时间戳需要占用相应的空间，所以如果分析操作并
不需要用到时间戳的话，那么就没有必要使用有序集合来保存用户最近浏览过的商品了。为此，请在保证语义不变的情况下，将update_token()函数里面
使用的有序集合替换成列表。提示：如果读者在解答这个问题时遇上困难的话，可以到6.1.1节中找找灵感。

列表的一个主要优点在于它可以包含多个字符串值，这使得用户可以将数据集中在同一个地方。Redis的集合也提供了与列表类似的特性，但集合只能保存各不相同
的元素。接下来的一节中就让我们来看看不能保存相同元素的集合都能做些什么。

3.3 集合

Redis的集合以无序的方式来存储多个各不相同的元素，用户可以快速地对集合执行添加元素操作、移除元素操作以及检查一个元素是否存在于集合里。

本节将对最常用的集合命令进行介绍，包括插入命令、移除命令、将元素从一个集合移动到另一个集合的命令，以及对多个集合执行交集运算、
并集运算和差集运算的命令。阅读本节也有助于读者更好地理解本书在

一些常用的集合命令

命令			用例和描述
 
SADD
SADD key-name item [item ...]—将一个或多个元素添加到集合里面，并返回被添加元素当中原本并不存在于集合里面的元素数量
 
SREM
SREM key-name item [item ...]—从集合里面移除一个或多个元素，并返回被移除元素的数量
 
SISMEMBER
SISMEMBER key-name item—检查元素item是否存在于集合key-name 里
 
SCARD
SCARD key-name—返回集合包含的元素的数量

SMEMBERS
SMEMBERS key-name—返回集合包含的所有元素
 
SRANDMEMBER
SRANDMEMBER key-name [count]—从集合里面随机地返回一个或多个元素。当count为正数时，命令返回的随机元素不会重复；当count为负数时，命令返回的随机元素可能会出现重复

SPOP
SPOP key-name—随机地移除集合中的一个元素，并返回被移除的元素
 
SMOVE
SMOVE source-key dest-key item—如果集合source-key包含元素item，那么从集合source-key里面移除元素item，并将元素item添加到集合dest-key中；如果item被成功移除，那么命令返回1，否则返回0
 

用于组合和处理多个集合的Redis命令



命令			用例和描述
 
SDIFF
SDIFF key-name [key-name ...]—返回那些存在于第一个集合、但不存在于其他集合中的元素（数学上的差集运算）
 
SDIFFSTORE
SDIFFSTORE dest-key key-name [key-name ...]—将那些存在于第一个集合但并不存在于其他集合中的元素（数学上的差集运算）存储到dest-key键里面
 
SINTER
SINTER key-name [key-name ...]—返回那些同时存在于所有集合中的元素（数学上的交集运算）
 
SINTERSTORE
SINTERSTORE dest-key key-name [key-name ...]—将那些同时存在于所有集合的元素（数学上的交集运算）存储到dest-key键里面
 
SUNION
SUNION key-name [key-name ...]—返回那些至少存在于一个集合中的元素（数学上的并集计算）
 
SUNIONSTORE
SUNIONSTORE dest-key key-name [key-name ...]—将那些至少存在于一个集合中的元素（数学上的并集计算）存储到dest-key键里面
 

这些命令分别是并集运算、交集运算和差集运算这3个基本集合操作的“返回结果”版本和“存储结果”版本

3.4 散列

第1章提到过，Redis的散列可以让用户将多个键值对存储到一个Redis键里面。从功能上来说，Redis为散列值提供了一些与字符串值相同的特性，使得散列非常适用于
将一些相关的数据存储在一起。我们可以把这种数据聚集看作是关系数据库中的行，或者文档数据库中的文档。

本节将对最常用的散列命令进行介绍：其中包括添加和删除键值对的命令、获取所有键值对的命令，以及对键值对的值进行自增或者自减操作的命令。阅读这一节可以让
读者学习到如何将数据存储到散列里面，以及这样做的好处是什么。表3-7展示了一部分常用的散列命令。

用于添加和删除键值对的散列操作

命令			用例和描述
 


HMGET
HMGET key-name key [key ...]—从散列里面获取一个或多个键的值

HMSET
HMSET key-name key value [key value ...]—为散列里面的一个或多个键设置值
 
HDEL
HDEL key-name key [key ...]—删除散列里面的一个或多个键值对，返回成功找到并删除的键值对数量
 
HLEN
HLEN key-name—返回散列包含的键值对数量
 

Redis散列的更高级特性



命令				用例和描述
 
HEXISTS
HEXISTS key-name key—检查给定键是否存在于散列中

HKEYS
HKEYS key-name—获取散列包含的所有键
 
HVALS
HVALS key-name—获取散列包含的所有值
 
HGETALL
HGETALL key-name—获取散列包含的所有键值对
 
HINCRBY
HINCRBY key-name key increment—将键key保存的值加上整数increment
 
HINCRBYFLOAT
HINCRBYFLOAT key-name key increment—将键key保存的值加上浮点数increment
 

尽管有HGETALL存在，但HKEYS和HVALUES也是非常有用的：如果散列包含的值非常大，那么用户可以先使用HKEYS取出散列包含的所有键，然后再使用HGET一个
接一个地取出键的值，从而避免因为一次获取多个大体积的值而导致服务器阻塞。

HINCRBY和HINCRBYFLOAT可能会让读者回想起用于处理字符串的INCRBY和INCRBYFLOAT，这两对命令拥有相同的语义，它们的不同在于HINCRBY和HINCRBYFLOAT处理
的是散列，而不是字符串。代码清单3-8展示了这些命令的使用方法。

正如前面所说，在对散列进行处理的时候，如果键值对的值的体积非常庞大，那么用户可以先使用HKEYS获取散列的所有键，然后通过只获取必要的值来减少需要
传输的数据量。除此之外，用户还可以像使用SISMEMBER检查一个元素是否存在于集合里面一样，使用HEXISTS检查一个键是否存在于散列里面。另外第1章也用到
了本节刚刚回顾过的HINCRBY来记录文章被投票的次数。

在接下来的一节中，我们要了解的是之后的章节里面会经常用到的有序集合结构。

3.5 有序集合

和散列存储着键与值之间的映射类似，有序集合也存储着成员与分值之间的映射，并且提供了分值处理命令，以及根据分值大小有序地获取（fetch）或扫描（scan）成
员和分值的命令。本书曾在第1章使用有序集合实现过基于发表时间排序的文章列表和基于投票数量排序的文章列表，还在第2章使用有序集合存储过cookie的过期时间。

本节将对操作有序集合的命令进行介绍，其中包括向有序集合添加新元素的命令、更新已有元素的命令，以及对有序集合进行交集运算和并集运算的命令。阅读本节可
以加深读者对有序集合的认识，从而帮助读者更好地理解本书在第1章、第5章、第6章和第7章展示的有序集合示例。


一些常用的有序集合命令
命令				用例和描述

ZADD
ZADD key-name score member [score member ...]—将带有给定分值的成员添加到有序集合里面
 
ZREM
ZREM key-name member [member ...]—从有序集合里面移除给定的成员，并返回被移除成员的数量
 
ZCARD
ZCARD key-name—返回有序集合包含的成员数量
 
ZINCRBY
ZINCRBY key-name increment member—将member成员的分值加上increment
 
ZCOUNT
ZCOUNT key-name min max—返回分值介于min和max之间的成员数量
 
ZRANK
ZRANK key-name member—返回成员member在有序集合中的排名
 
ZSCORE
ZSCORE key-name member—返回成员member的分值

ZRANGE
ZRANGE key-name start stop [WITHSCORES]—返回有序集合中排名介于start和stop之间的成员，如果给定了可选的WITHSCORES选项，那么命令会将成员的分值也一并返回
 


有序集合的范围型数据获取命令和范围型数据删除命令，以及并集命令和交集命令

命令			用例和描述

ZREVRANK
ZREVRANK key-name member—返回有序集合里成员member所处的位置，成员按照分值从大到小排列
 
ZREVRANGE
ZREVRANGE key-name start stop [WITHSCORES]—返回有序集合给定排名范围内的成员，成员按照分值从大到小排列
 
ZRANGEBYSCORE
ZRANGEBYSCORE key min max [WITHSCORES] [LIMIT offset count]—返回有序集合中，分值介于min和max之间的所有成员
 
ZREVRANGEBYSCORE
ZREVRANGEBYSCORE key max min [WITHSCORES] [LIMIT offset count]—获取有序集合中分值介于min和max之间的所有成员，并按照分值从大到小的顺序来返回它们
 
ZREMRANGEBYRANK
ZREMRANGEBYRANK key-name start stop—移除有序集合中排名介于start和stop之间的所有成员
 
ZREMRANGEBYSCORE
ZREMRANGEBYSCORE key-name min max—移除有序集合中分值介于min和max之间的所有成员
 
ZINTERSTORE
ZINTERSTORE dest-key key-count key [key ...] [WEIGHTS weight [weight ...]] [AGGREGATE SUM（这里是竖线）MIN（这里是竖线）MAX]—对给定的有序集合执行类似于集合的交集运算

ZUNIONSTORE
ZUNIONSTORE dest-key key-count key [key ...] [WEIGHTS weight [weight ...]] [AGGREGATE SUM（这里是竖线）IN（这里是竖线）MAX]—对给定的有序集合执行类似于集合的并集运算
 

发布与订阅

如果你因为想不起来本书在前面的哪个章节里面介绍过发布与订阅而困惑，那么大可不必—这是本书目前为止第一次介绍发布与订阅。一般来说，发布
与订阅（又称pub/sub）的特点是订阅者（listener）负责订阅频道（channel），发送者（publisher）负责向频道发送二进制字符串消息（binary string message）。
每当有消息被发送至给定频道时，频道的所有订阅者都会收到消息。我们也可以把频道看作是电台，其中订阅者可以同时收听多个电台，而发送者则可以在任何电台发送消息。

本节将对发布与订阅的相关操作进行介绍，阅读这一节可以让读者学会怎样使用发布与订阅的相关命令，并了解到为什么本书在之后的章节里面会使用其他相似的解决方案来代替Redis提供的发布与订阅。

Redis提供的发布与订阅命令

命令			用例和描述
 

SUBSCRIBE
SUBSCRIBE channel [channel ...]—订阅给定的一个或多个频道
 
UNSUBSCRIBE
UNSUBSCRIBE [channel [channel ...]]—退订给定的一个或多个频道，如果执行时没有给定任何频道，那么退订所有频道
 
PUBLISH 
PUBLISH channel message—向给定频道发送消息
 
PSUBSCRIBE
PSUBSCRIBE pattern [pattern ...]—订阅与给定模式相匹配的所有频道
 
PUNSUBSCRIBE
PUNSUBSCRIBE [pattern [pattern ...]]—退订给定的模式，如果执行时没有给定任何模式，那么退订所有模式
 

考虑到PUBLISH命令和SUBSCRIBE命令在Python客户端的实现方式，一个比较简单的演示发布与订阅的方法，就是像代码清单3-11那样使用辅助线程（helper thread）来执行PUBLISH命令。



3.7.1 排序

Redis的排序操作和其他编程语言的排序操作一样，都可以根据某种比较规则对一系列元素进行有序的排列。负责执行排序操作的SORT命令可以根据字符串、列表、集合、有序集合、
散列这5种键里面存储着的数据，对列表、集合以及有序集合进行排序。如果读者之前曾经使用过关系数据库的话，那么可以将SORT命令看作是SQL语言里的order by子句。表3-12展示了SORT命令的定义。

SORT命令的定义

命令			用例和描述
 

SORT
SORT source-key [BY pattern] [LIMIT offset count] [GET pattern [GET pattern ...]] [ASC（这里是竖线）DESC] [ALPHA] [STORE dest-key]—根据给定的选项，
对输入列表、集合或者有序集合进行排序，然后返回或者存储排序的结果
 
使用SORT命令提供的选项可以实现以下功能：根据降序而不是默认的升序来排序元素；将元素看作是数字来进行排序，或者将元素看作是二进制字符串来进行排序（比如
排序字符串'110'和'12'的结果就跟排序数字110和12的结果不一样）；使用被排序元素之外的其他值作为权重来进行排序，甚至还可以从输入的列表、集合、有序集合以外的其他地方进行取值。


3.7.2 基本的Redis事务

有时候为了同时处理多个结构，我们需要向Redis发送多个命令。尽管Redis有几个可以在两个键之间复制或者移动元素的命令，但却没有那种可以在两个不同类型之间移动元素的命令（虽然可以使用ZUNIONSTORE命令将元素从一个集合复制到一个有序集合）。为了对相同或者不同类型的多个键执行操作，Redis有5个命令可以让用户在不被打断（interruption）的情况下对多个键执行操作，它们分别是WATCH、MULTI、EXEC、UNWATCH和DISCARD。

这一节只介绍最基本的Redis事务用法，即使用MULTI命令和EXEC命令。如果读者想看看使用WATCH、MULTI、EXEC和UNWATCH等多个命令的事务是什么样子的，可以阅读4.4节，其中解释了为什么需要在使用MULTI和EXEC的同时使用WATCH和UNWATCH。

什么是Redis的基本事务

Redis的基本事务（basic transaction）需要用到MULTI命令和EXEC命令，这种事务可以让一个客户端在不被其他客户端打断的情况下执行多个命令。和关系数据库那种可以在执行的过程中进行回滚（rollback）的事务不同，在Redis里面，被MULTI命令和EXEC命令包围的所有命令会一个接一个地执行，直到所有命令都执行完毕为止。当一个事务执行完毕之后，Redis才会处理其他客户端的命令。

要在Redis里面执行事务，我们首先需要执行MULTI命令，然后输入那些我们想要在事务里面执行的命令，最后再执行EXEC命令。当Redis从一个客户端那里接收到MULTI命令时，Redis会将这个客户端之后发送的所有命令都放入到一个队列里面，直到这个客户端发送EXEC命令为止，然后Redis就会在不被打断的情况下，一个接一个地执行存储在队列里面的命令。从语义上来说，Redis事务在Python客户端上面是由流水线（pipeline）实现的：对连接对象调用piepline()方法将创建一个事务，在一切正常的情况下，客户端会自动地使用MULTI和EXEC包裹起用户输入的多个命令。此外，为了减少Redis与客户端之间的通信往返次数，提升执行多个命令时的性能，Python的Redis客户端会存储起事务包含的多个命令，然后在事务执行时一次性地将所有命令都发送给Redis。

跟介绍PUBLISH命令和SUBSCRIBE命令时的情况一样，要展示事务执行结果，最简单的方法就是将事务放到线程里面执行。代码清单3-13展示了在没有使用事务的情况下，执行并行（parallel）自增操作的结果。

代码清单3-13 在并行执行命令时，缺少事务可能会引发的问题

..\01\3-13.tif

因为没有使用事务，所以3个线程都可以在执行自减操作之前，对notrans:计数器执行自增操作。虽然代码清单里面通过休眠100毫秒的方式来放大了潜在的问题，但如果我们确实需要在不受其他命令干扰的情况下，对计数器执行自增操作和自减操作，那么我们就不得不解决这个潜在的问题。代码清单3-14展示了如何使用事务来执行相同的操作。

代码清单3-14 使用事务来处理命令的并行执行问题

..\01\3-14.tif

..\01\3-14b.tif

可以看到，尽管自增操作和自减操作之间有一段延迟时间，但通过使用事务，各个线程都可以在不被其他线程打断的情况下，执行各自队列里面的命令。记住，Redis要在接收到EXEC命令之后，才会执行那些位于MULTI和EXEC之间的入队命令。

使用事务既有利也有弊，本书的4.4节将对这个问题进行讨论。

练习：移除竞争条件 正如前面的代码清单3-13所示，MULTI和EXEC事务的一个主要作用是移除竞争条件。第1章展示的articlevote()函数包含一个竞争条件以及一个因为竞争条件而出现的bug。函数的竞争条件可能会造成内存泄漏，而函数的bug则可能会导致不正确的投票结果出现。尽管article vote()函数的竞争条件和bug出现的机会都非常少，但为了防范于未然，你能想个办法修复它们么？提示：如果你觉得很难理解竞争条件为什么会导致内存泄漏，那么可以在分析第1章的post_article()函数的同时，阅读一下6.2.5节。

练习：提高性能 在Redis里面使用流水线的另一个目的是提高性能（详细的信息会在之后的4.4节至4.6节中介绍）。在执行一连串命令时，减少Redis与客户端之间的通信往返次数可以大幅降低客户端等待回复所需的时间。第1章的get_articles()函数在获取整个页面的文章时，需要在Redis与客户端之间进行26次通信往返，这种做法简直低效得令人发指，你能否想个办法将get_articles()函数的往返次数从26次降低为2次呢？ |

在使用Redis存储数据的时候，有些数据仅在一段很短的时间内有用，虽然我们可以在数据的有效期过了之后手动删除无用的数据，但更好的办法是使用Redis提供的键过期操作来自动删除无用数据。

3.7.3 键的过期时间

在使用Redis存储数据的时候，有些数据可能在某个时间点之后就不再有用了，用户可以使用DEL命令显式地删除这些无用数据，也可以通过Redis的过期时间（expiration）特性来让一个键在给定的时限（timeout）之后自动被删除。当我们说一个键“带有生存时间（time to live）”或者一个键“会在特定时间之后过期（expire）”时，我们指的是Redis会在这个键的过期时间到达时自动删除该键。

虽然过期时间特性对于清理缓存数据非常有用，不过如果读者翻一下本书的其他章节，就会发现除了6.2节、7.1节和7.2节之外，本书使用过期时间特性的情况并不多，这主要和本
书使用的结构类型有关。在本书常用的命令当中，只有少数几个命令可以原子地为键设置过期时间，并且对于列表、集合、散列和有序集合这样的容器（container）来说，键过
期命令只能为整个键设置过期时间，而没办法为键里面的单个元素设置过期时间（为了解决这个问题，本书在好几个地方都使用了存储时间戳的有序集合来实现针对单个元素的过期操作）。

本节将对那些可以在给定时限之后或者给定时间之后自动删除过期键的Redis命令进行介绍，阅读本节可以让读者学习到使用过期操作来自动删除过期数据并降低Redis内存占用的方法。

表3-13列出了Redis提供的用于为键设置过期时间的命令，以及查看键的过期时间的命令。

表3-13 用于处理过期时间的Redis命令



命令
 

示例和描述
PERSIST
PERSIST key-name—移除键的过期时间
 

TTL
TTL key-name—查看给定键距离过期还有多少秒
 
EXPIRE
EXPIRE key-name seconds—让给定键在指定的秒数之后过期

EXPIREAT
EXPIREAT key-name timestamp—将给定键的过期时间设置为给定的UNIX时间戳
 

PTTL
PTTL key-name—查看给定键距离过期时间还有多少毫秒，这个命令在Redis 2.6或以上版本可用
 

PEXPIRE
PEXPIRE key-name milliseconds—让键给定键在指定的毫秒数之后过期，这个命令在Redis 2.6或以上版本可用
 

PEXPIREAT
PEXPIREAT key-name timestamp-milliseconds—将一个毫秒级精度的UNIX时间戳设置为给定键的过期时间，这个命令在Redis 2.6或以上版本可用
 






http://redisdoc.com/

?Key（键） ?DEL
?DUMP
?EXISTS
?EXPIRE
?EXPIREAT
?KEYS
?MIGRATE
?MOVE
?OBJECT
?PERSIST
?PEXPIRE
?PEXPIREAT
?PTTL
?RANDOMKEY
?RENAME
?RENAMENX
?RESTORE
?SORT
?TTL
?TYPE
?SCAN

 
?String（字符串） ?APPEND
?BITCOUNT
?BITOP
?DECR
?DECRBY
?GET
?GETBIT
?GETRANGE
?GETSET
?INCR
?INCRBY
?INCRBYFLOAT
?MGET
?MSET
?MSETNX
?PSETEX
?SET
?SETBIT
?SETEX
?SETNX
?SETRANGE
?STRLEN

 
?Hash（哈希表） ?HDEL
?HEXISTS
?HGET
?HGETALL
?HINCRBY
?HINCRBYFLOAT
?HKEYS
?HLEN
?HMGET
?HMSET
?HSET
?HSETNX
?HVALS
?HSCAN

 
?List（列表） ?BLPOP
?BRPOP
?BRPOPLPUSH
?LINDEX
?LINSERT
?LLEN
?LPOP
?LPUSH
?LPUSHX
?LRANGE
?LREM
?LSET
?LTRIM
?RPOP
?RPOPLPUSH
?RPUSH
?RPUSHX

 

?Set（集合） ?SADD
?SCARD
?SDIFF
?SDIFFSTORE
?SINTER
?SINTERSTORE
?SISMEMBER
?SMEMBERS
?SMOVE
?SPOP
?SRANDMEMBER
?SREM
?SUNION
?SUNIONSTORE
?SSCAN

 
?SortedSet（有序集合） ?ZADD
?ZCARD
?ZCOUNT
?ZINCRBY
?ZRANGE
?ZRANGEBYSCORE
?ZRANK
?ZREM
?ZREMRANGEBYRANK
?ZREMRANGEBYSCORE
?ZREVRANGE
?ZREVRANGEBYSCORE
?ZREVRANK
?ZSCORE
?ZUNIONSTORE
?ZINTERSTORE
?ZSCAN
?ZRANGEBYLEX
?ZLEXCOUNT
?ZREMRANGEBYLEX

 
?HyperLogLog ?PFADD
?PFCOUNT
?PFMERGE

 
?GEO（地理位置） ?GEOADD
?GEOPOS
?GEODIST
?GEORADIUS
?GEORADIUSBYMEMBER
?GEOHASH

 

?Pub/Sub（发布/订阅） ?PSUBSCRIBE
?PUBLISH
?PUBSUB
?PUNSUBSCRIBE
?SUBSCRIBE
?UNSUBSCRIBE

 
?Transaction（事务） ?DISCARD
?EXEC
?MULTI
?UNWATCH
?WATCH

 
?Script（脚本） ?EVAL
?EVALSHA
?SCRIPT EXISTS
?SCRIPT FLUSH
?SCRIPT KILL
?SCRIPT LOAD

 
?Connection（连接） ?AUTH
?ECHO
?PING
?QUIT
?SELECT

 

?Server（服务器） ?BGREWRITEAOF
?BGSAVE
?CLIENT GETNAME
?CLIENT KILL
?CLIENT LIST
?CLIENT SETNAME
?CONFIG GET
?CONFIG RESETSTAT
?CONFIG REWRITE
?CONFIG SET
?DBSIZE
?DEBUG OBJECT
?DEBUG SEGFAULT
?FLUSHALL
?FLUSHDB
?INFO
?LASTSAVE
?MONITOR
?PSYNC
?SAVE
?SHUTDOWN
?SLAVEOF
?SLOWLOG
?SYNC
?TIME












yang add
列表对象的编码可以是 ziplist 或者 linkedlist 。
编码转换

当列表对象可以同时满足以下两个条件时， 列表对象使用 ziplist 编码：
1.列表对象保存的所有字符串元素的长度都小于 64 字节；
2.列表对象保存的元素数量小于 512 个；

不能满足这两个条件的列表对象需要使用 linkedlist 编码。

注意
以上两个条件的上限值是可以修改的， 具体请看配置文件中关于 list-max-ziplist-value 选项和 list-max-ziplist-entries 选项的说明。

列表命令的实现

因为列表键的值为列表对象， 所以用于列表键的所有命令都是针对列表对象来构建的， 列出了其中一部分列表键命令， 以及这些命令在不同编码的列表对象下的实现方法。


列表命令的实现


命令            ziplist 编码的实现方法                  linkedlist 编码的实现方法


LPUSH   调用 ziplistPush 函数， 将新元素推入到压缩列表的表头。                  调用 listAddNodeHead 函数， 将新元素推入到双端链表的表头。 
RPUSH   调用 ziplistPush 函数， 将新元素推入到压缩列表的表尾。                  调用 listAddNodeTail 函数， 将新元素推入到双端链表的表尾。 
LPOP    调用 ziplistIndex 函数定位压缩列表的表头节点， 在向用户
        返回节点所保存的元素之后， 调用 ziplistDelete 函数删除表头节点。        调用 listFirst 函数定位双端链表的表头节点， 在向用户返回节点所保存的元素之后， 调用 listDelNode 函数删除表头节点。 
RPOP    调用 ziplistIndex 函数定位压缩列表的表尾节点， 在向用户返回节
        点所保存的元素之后， 调用 ziplistDelete 函数删除表尾节点。              调用 listLast 函数定位双端链表的表尾节点， 在向用户返回节点所保存的元素之后， 调用 listDelNode 函数删除表尾节点。 
LINDEX  调用 ziplistIndex 函数定位压缩列表中的指定节点， 然后返回节点
        所保存的元素。                                                          调用 listIndex 函数定位双端链表中的指定节点， 然后返回节点所保存的元素。 
LLEN    调用 ziplistLen 函数返回压缩列表的长度。                                调用 listLength 函数返回双端链表的长度。 
LINSERT 插入新节点到压缩列表的表头或者表尾时， 使用 ziplistPush 
        函数； 插入新节点到压缩列表的其他位置时， 使用 ziplistInsert 函数。     调用 listInsertNode 函数， 将新节点插入到双端链表的指定位置。 
LREM    遍历压缩列表节点， 并调用 ziplistDelete 函数删除包含了给定
        元素的节点。                                                            遍历双端链表节点， 并调用 listDelNode 函数删除包含了给定元素的节点。 
LTRIM   调用 ziplistDeleteRange 函数， 删除压缩列表中所有不在
        指定索引范围内的节点。                                                  遍历双端链表节点， 并调用 listDelNode 函数删除链表中所有不在指定索引范围内的节点。 
LSET    调用 ziplistDelete 函数， 先删除压缩列表指定索引上的现有节点， 然后调用 ziplistInsert 函数， 将一个包含给定元素的新节点插入到相同索引上面。 调用 listIndex 函数， 定位到双端链表指定索引上的节点， 然后通过赋值操作更新节点的值。 



ziplist 是节约内存但速度慢，而 hashtable 是速度快但耗内存，两者互补，按需要使用


编码转换

当哈希对象可以同时满足以下两个条件时， 哈希对象使用 ziplist 编码：
1.哈希对象保存的所有键值对的键和值的字符串长度都小于 64 字节；
2.哈希对象保存的键值对数量小于 512 个；

不能满足这两个条件的哈希对象需要使用 hashtable 编码。

注意
这两个条件的上限值是可以修改的， 具体请看配置文件中关于 hash-max-ziplist-value 选项和 hash-max-ziplist-entries 选项的说明。
对于使用 ziplist 编码的列表对象来说， 当使用 ziplist 编码所需的两个条件的任意一个不能被满足时， 对象的编码转换操作就会被执行： 
原本保存在压缩列表里的所有键值对都会被转移并保存到字典里面， 对象的编码也会从 ziplist 变为 hashtable 。

ziplist 是节约内存但速度慢，而 hashtable 是速度快但耗内存，两者互补，按需要使用

哈希命令的实现
因为哈希键的值为哈希对象， 所以用于哈希键的所有命令都是针对哈希对象来构建的， 一部分哈希键命令， 以及这些命令在不同编码的哈希对象下的实现方法。

哈希命令的实现
命令            ziplist 编码实现方法                    hashtable 编码的实现方法


HSET        首先调用 ziplistPush 函数， 将键推入到压缩列表的表尾， 然后再次调用 ziplistPush 函数， 将值推入到压缩列表的表尾。   调用 dictAdd 函数， 将新节点添加到字典里面。 
HGET        首先调用 ziplistFind 函数， 在压缩列表中查找指定键所对应的节点， 然后调用 ziplistNext 函数， 将指针移动到键节点旁边的值节点， 最后返回值节点。 调用 dictFind 函数， 在字典中查找给定键， 然后调用 dictGetVal 函数， 返回该键所对应的值。 
HEXISTS     调用 ziplistFind 函数， 在压缩列表中查找指定键所对应的节点， 如果找到的话说明键值对存在， 没找到的话就说明键值对不存在。 调用 dictFind 函数， 在字典中查找给定键， 如果找到的话说明键值对存在， 没找到的话就说明键值对不存在。 
HDEL        调用 ziplistFind 函数， 在压缩列表中查找指定键所对应的节点， 然后将相应的键节点、 以及键节点旁边的值节点都删除掉。 调用 dictDelete 函数， 将指定键所对应的键值对从字典中删除掉。 
HLEN        调用 ziplistLen 函数， 取得压缩列表包含节点的总数量， 将这个数量除以 2 ， 得出的结果就是压缩列表保存的键值对的数量。 调用 dictSize 函数， 返回字典包含的键值对数量， 这个数量就是哈希对象包含的键值对数量。 
HGETALL     遍历整个压缩列表， 用 ziplistGet 函数返回所有键和值（都是节点）。 遍历整个字典， 用 dictGetKey 函数返回字典的键， 用 dictGetVal 函数返回字典的值。 


集合对象的编码可以是 intset 或者 hashtable 。
编码的转换

当集合对象可以同时满足以下两个条件时， 对象使用 intset 编码：
1.集合对象保存的所有元素都是整数值；
2.集合对象保存的元素数量不超过 512 个；
不能满足这两个条件的集合对象需要使用 hashtable 编码。
注意
第二个条件的上限值是可以修改的， 具体请看配置文件中关于 set-max-intset-entries 选项的说明。


集合命令的实现

因为集合键的值为集合对象， 所以用于集合键的所有命令都是针对集合对象来构建的， 以及这些命令在不同编码的集合对象下的实现方法。

集合命令的实现方法


命令            intset 编码的实现方法                                                   hashtable 编码的实现方法


SADD        调用 intsetAdd 函数， 将所有新元素添加到整数集合里面。                  调用 dictAdd ， 以新元素为键， NULL 为值， 将键值对添加到字典里面。 
SCARD       调用 intsetLen 函数， 返回整数集合所包含的元素数量， 这个数量就是
            集合对象所包含的元素数量。                                              调用 dictSize 函数， 返回字典所包含的键值对数量， 这个数量就是集合对象所包含的元素数量。 
SISMEMBER   调用 intsetFind 函数， 在整数集合中查找给定的元素， 如果找到了说
            明元素存在于集合， 没找到则说明元素不存在于集合。                       调用 dictFind 函数， 在字典的键中查找给定的元素， 如果找到了说明元素存在于集合， 没找到则说明元素不存在于集合。 
SMEMBERS    遍历整个整数集合， 使用 intsetGet 函数返回集合元素。                    遍历整个字典， 使用 dictGetKey 函数返回字典的键作为集合元素。 
SRANDMEMBER 调用 intsetRandom 函数， 从整数集合中随机返回一个元素。                 调用 dictGetRandomKey 函数， 从字典中随机返回一个字典键。 
SPOP        调用 intsetRandom 函数， 从整数集合中随机取出一个元素， 在将这个
            随机元素返回给客户端之后， 调用 intsetRemove 函数， 将随机元素
            从整数集合中删除掉。                                                    调用 dictGetRandomKey 函数， 从字典中随机取出一个字典键， 在将这个随机字典键的值返回给客户端之后， 调用 dictDelete 函数， 从字典中删除随机字典键所对应的键值对。 
SREM        调用 intsetRemove 函数， 从整数集合中删除所有给定的元素。               调用 dictDelete 函数， 从字典中删除所有键为给定元素的键值对。 



为什么有序集合需要同时使用跳跃表和字典来实现？

在理论上来说， 有序集合可以单独使用字典或者跳跃表的其中一种数据结构来实现， 但无论单独使用字典还是跳跃表， 在性能上对
比起同时使用字典和跳跃表都会有所降低。

举个例子， 如果我们只使用字典来实现有序集合， 那么虽然以 O(1) 复杂度查找成员的分值这一特性会被保留， 但是， 因为字典以
无序的方式来保存集合元素， 所以每次在执行范围型操作 —— 比如 ZRANK 、 ZRANGE 等命令时， 程序都需要对字典保存的所有元
素进行排序， 完成这种排序需要至少 O(N \log N) 时间复杂度， 以及额外的 O(N) 内存空间 （因为要创建一个数组来保存排序后的元素）。

另一方面， 如果我们只使用跳跃表来实现有序集合， 那么跳跃表执行范围型操作的所有优点都会被保留， 但因为没有了字典， 所以
根据成员查找分值这一操作的复杂度将从 O(1) 上升为 O(\log N) 。
因为以上原因， 为了让有序集合的查找和范围型操作都尽可能快地执行， Redis 选择了同时使用字典和跳跃表两种数据结构来实现有序集合。



编码的转换

当有序集合对象可以同时满足以下两个条件时， 对象使用 ziplist 编码：
1.有序集合保存的元素数量小于 128 个；
2.有序集合保存的所有元素成员的长度都小于 64 字节；
不能满足以上两个条件的有序集合对象将使用 skiplist 编码。

注意
以上两个条件的上限值是可以修改的， 具体请看配置文件中关于 zset-max-ziplist-entries 选项和 zset-max-ziplist-value 选项的说明。

对于使用 ziplist 编码的有序集合对象来说， 当使用 ziplist 编码所需的两个条件中的任意一个不能被满足时， 程序就会执行编码转换操作， 将原本储存在压缩列表里面的所有集合元素转移到 zset 结构里面， 并将对象的编码从 ziplist 改为 skiplist 。


有序集合命令的实现

因为有序集合键的值为有序集合对象， 所以用于有序集合键的所有命令都是针对有序集合对象来构建的，

有序集合命令的实现方法


命令                        ziplist 编码的实现方法                              zset 编码的实现方法


ZADD        调用 ziplistInsert 函数， 将成员和分值作为两个节点分别插入到压缩列表。  先调用 zslInsert 函数， 将新元素添加到跳跃表， 然后调用 dictAdd 函数， 将新元素关联到字典。 
ZCARD       调用 ziplistLen 函数， 获得压缩列表包含节点的数量， 将这个数量除以 
            2 得出集合元素的数量。                                                  访问跳跃表数据结构的 length 属性， 直接返回集合元素的数量。 
ZCOUNT      遍历压缩列表， 统计分值在给定范围内的节点的数量。                       遍历跳跃表， 统计分值在给定范围内的节点的数量。 
ZRANGE      从表头向表尾遍历压缩列表， 返回给定索引范围内的所有元素。               从表头向表尾遍历跳跃表， 返回给定索引范围内的所有元素。 
ZREVRANGE   从表尾向表头遍历压缩列表， 返回给定索引范围内的所有元素。               从表尾向表头遍历跳跃表， 返回给定索引范围内的所有元素。 
ZRANK       从表头向表尾遍历压缩列表， 查找给定的成员， 沿途记录经
            过节点的数量， 当找到给定成员之后， 途经节点的数量就是
            该成员所对应元素的排名。                                                从表头向表尾遍历跳跃表， 查找给定的成员， 沿途记录经过节点的数量， 当找到给定成员之后， 途经节点的数量就是该成员所对应元素的排名。 
ZREVRANK    从表尾向表头遍历压缩列表， 查找给定的成员， 沿途记录经
            过节点的数量， 当找到给定成员之后， 途经节点的数量就是
            该成员所对应元素的排名。                                                从表尾向表头遍历跳跃表， 查找给定的成员， 沿途记录经过节点的数量， 当找到给定成员之后， 途经节点的数量就是该成员所对应元素的排名。 
ZREM        遍历压缩列表， 删除所有包含给定成员的节点， 以及被删除
            成员节点旁边的分值节点。                                                遍历跳跃表， 删除所有包含了给定成员的跳跃表节点。 并在字典中解除被删除元素的成员和分值的关联。 
ZSCORE      遍历压缩列表， 查找包含了给定成员的节点， 然后取出成员
            节点旁边的分值节点保存的元素分值。                                      直接从字典中取出给定成员的分值。 




RDB持久化原理:
    rdb是redis保存内存数据到磁盘数据的其中一种方式(另一种是AOF)。Rdb的主要原理就是在某个时间点把内存中的所有数据的快照保存一份
到磁盘上。在条件达到时通过fork一个子进程把内存中的数据写到一个临时文件中来实现保存数据快照。在所有数据写完后再把这个临时文
件用原子函数rename(2)重命名为目标rdb文件。这种实现方式充分利用fork的copy on write。



当过期键被惰性删除或者定期删除之后，程序会向AOF文件追加（append）一条DEL
命令，来显式地记录该键已被删除。
    举个例子，如果客户端使用GET message命令，试图访问过期的message键，那么
服务器将执行以下三个动作：
    1)从数据库中删除message键。
    2)追加一条DEL message命令到AOF文件。
    3)向执行GET命令的客户端返回空回复。






http://www.jb51.net/article/56448.htm  超强、超详细Redis数据库入门教程
redis提供了两种持久化的方式，分别是RDB（Redis DataBase）和AOF（Append Only File）。

常用命令大全及相关redis中文翻译参考:http://redisdoc.com/

日志文件默认路径/var/log/redis/redis.log 

该工程解决了source insight查看注释乱码问题，可以通过source insight查看注释信息

进一步完善一些关键点代码注释，例如客户端发送过来的命令字符串为key或者value 5M， redis解析处理过程详细分析
或者发送给客户端的命令格式字符串中的一个关键节点字符串为5M, redis是如何异步发送实现的。

多级redis主从服务器数据一致性保证代码分析研究注释


问题: set 和 setbit没有做标记区分都采用REDIS_STRING类，编码方式也一样。
如果set test abc,然后继续执行setbit test 0 1；是会成功的。会造成test键内容被无意修改。可以增加一种编码encoding方式来加以区分。


可以改造的地方:
改造点1:
    在应答客户端请求数据的时候，全是epoll采用epool write事件触发，这样不太好，每次发送数据前通过epoll_ctl来触发epoll write事件，即使发送
一个"+OK"字符串，也是这个流程。
    改造方法: 开始不把socket加入epoll，需要向socket写数据的时候，直接调用write或者send发送数据。如果返回EAGAIN，把socket加入epoll，在epoll的
驱动下写数据，全部数据发送完毕后，再移出epoll。
    这种方式的优点是：数据不多的时候可以避免epoll的事件处理，提高效率。 这个机制和nginx发送机制一致。
    后期有空来完成该优化。

改造点2:
    在对key-value对进行老化删除的过程中，采用从expire hash桶中随机取一个key-value节点，判断是否超时，超时则删除,这种方法每次采集点有很大
    不确定性，造成空转，最终由没有删除老化时间到的节点，浪费CPU资源。

    改造方法:在向expire hash桶中添加包含expire时间的key-value节点，在每个具体的table[i]桶对应的链中，可以按照时间从小到大排序，每次删除老化的时候
    直接取具体table[i]桶中的第一个节点接口判断出该桶中是否有老化时间到的节点，这样可以很准确的定位出那些具体table[i]桶中有老化节点，如果有则取出
    删除接口。

改造点3:
    主服务器同步rdb文件给从服务器的时候是采用直接读取文件，然后通过网络发送出去，首先需要把文件内容从内核读取到应用层，在通过网络应用程序从应用层
    到内核网络协议栈，这样有点浪费CPU资源和内存。

    改造方法:通过sendfile或者aio方式发送，避免多次内核与应用层交互，提高性能。
改造点4:主备同步过于麻烦，每次都要落地磁盘，然后在从磁盘读取
    改造方法:直接把内存中的KEY-VALUE对按照指定格式发给备，不用写磁盘落地和从磁盘读

改造点5:如果网络不好，被反复连接主，例如磁盘落地成功，并从磁盘读取成功后正坐在进行网络同步，然后网络断了?会造成主备同步反反复复，消耗性能

改造点6:建议自己写个redis中间件，避免所有操作有客户端来完成

需要增加获取数据库号的命令，因为多次操作数据库后，就忘了现在操作的是那个数据库号的数据库了。


发现可疑问题:
1.主从服务器之间通过ack进行链路探测，他们之间的tcp连接默认tcp-keepalive=0，也就是不开启内核自动keepalive保活，如果在链路不通的情况下
  (例如直接把主服务器网线拔掉或者直接关机)，从服务器长时间无法自动切换到主服务器来提高服务

修改方法:增加应用层保活超时检测，可以增加链路保活探测定时器，服务器长时间没有接收到彼此的保活报文，则直接主动断开TCP连接，从而让备切换为主服务器提高服务。



nodes.conf如果里面就一行空行，什么也没有，则redis起不来，这是clusterLoadConfig的问题，所以一般不要去操作nodes.conf文件，如果想让集群状态关系重新协商
则可以直接删掉nodes.conf来解决。nodes.conf有redis程序操作，不要手动操作该文件。

redis-trib.rb  create --replicas 1 127.0.0.1:7000 127.0.0.1:7001 127.0.0.1:7002 127.0.0.1:7003 127.0.0.1:7004 127.0.0.1:7005
redis-benchmark -h 10.23.22.240 -p 22121 -c 100 -t set -d 100 -l –q

redis-benchmark  -h 192.168.1.111 -p 22122 -c 100 -t set -d 100 

性能测试
这里使用redis自带的redis-benchmark进行简单的性能测试，测试结果如下:

Set测试：
通过twemproxy测试：
[root@COS1 src]# redis-benchmark -h 10.23.22.240 -p 22121 -c 100 -t set -d 100 -l –q
SET: 38167.94 requests per second
直接对后端redis测试：
[root@COS2 ~]# redis-benchmark -h 10.23.22.241 -p 6379 -c 100 -t set -d 100 -l –q
SET: 53191.49 requests per second
Get测试：
通过twemproxy测试：
[root@COS1 src]# redis-benchmark -h 10.23.22.240 -p 22121 -c 100 -t get -d 100 -l -q
GET: 37453.18 requests per second
直接对后端redis测试：
[root@COS2 ~]# redis-benchmark -h 10.23.22.241 -p 6379 -c 100 -t get -d 100 -l -q
GET: 62111.80 requests per second
查看键值分布：
[root@COS2 ~]# redis-cli info|grep db0
db0:keys=51483,expires=0,avg_ttl=0

[root@COS3 ~]# redis-cli info|grep db0
db0:keys=48525,expires=0,avg_ttl=0


日志优化，现在是每写一条日志都open打开文件，写入后关闭。日志多的时候效率底下，可以一直打开日志文件，不关闭

/*
[root@V172-16-3-44 autostartrediscluster]# redis-trib.rb  create --replicas 1 127.0.0.1:7000 127.0.0.1:7001 127.0.0.1:7002 127.0.0.1:7003 127.0.0.1:7004 127.0.0.1:7005
/usr/lib/ruby/site_ruby/1.8/rubygems/custom_require.rb:31:in `gem_original_require': no such file to load -- redis (LoadError)
        from /usr/lib/ruby/site_ruby/1.8/rubygems/custom_require.rb:31:in `require'
        from /usr/local/bin/redis-trib.rb:25
报错，执行yum install ruby  yum install redis即可


https://www.kernel.org/doc/Documentation/networking/ip-sysctl.txt   linux内核参数调优
config配置文件参考:http://my.oschina.net/u/568779/blog/308129

*/
*/

