/*
redis-trib.rb使用:
安装
yum install ruby
yum install rubygems
gem install redis
redis-trib.rb详见:http://blog.csdn.net/huwei2003/article/details/50973967  redis cluster管理工具redis-trib.rb详解


注意:
1.在迁移槽位期间，发送到原集群该槽位的set key value会做move，然后要求到目的节点做set key value，告诉客户端你应该把属于迁移槽的KV发送到目的节点
2.如果槽位n正在由A -> B节点，并且已经把A节点n槽位的key1迁移到B，现在正在迁移其他key,如果在A上get key1，
  则A会告诉客户端(error) ASK 2 B-IP:B-PORT，提示去B获取，这时候必须先向B发送asking，然后在get

(error) ASK 2 10.2.4.5:7001 说明2槽位正在迁移到10.2.4.5:7001节点，如果想set  get  2槽位的key信息，则需先asking ,然后在get   set
注意操作的key属于正在迁移的槽的时候，需要先发送asking   切记  切记     get  set  hset hmset等所有关于该正在迁移槽的key都需要先发送asking


把127.0.0.1:7001节点上面的4槽位迁移到127.0.0.1:7006上面命令执行过程:线通过中间工具获取到有哪些KV，然后一条一条的通知redis去进行迁移
可以参考http://blog.csdn.net/gqtcgq/article/details/51757755
1：向迁入节点发送” CLUSTER  SETSLOT  <slot>  IMPORTING  <node>”命令
2：向迁出节点发送” CLUSTER  SETSLOT  <slot>  MIGRATING  <node>”命令
3：向迁出节点发送”CLUSTER  GETKEYSINSLOT  <slot>  <count>”命令
4：向迁出节点发送”MIGRATE <target_host> <target_port> <key> <target_database> <timeout> replace”命令
5：向所有节点发送”CLUSTER  SETSLOT  <slot>  NODE  <nodeid>”命令



一  迁移前节点信息
127.0.0.1:7001> DBSIZE
(integer) 50217
127.0.0.1:7001> cluster nodes
f4a5287807d38083970f4b330fa78a1fe139c3ba 10.2.4.5:7000 slave a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 0 1481094817241 3 connected
a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 10.2.4.5:7005 master - 0 1481094822252 3 connected 10923-16383
b88af084e00308e7ab219f212617d121c92434e6 10.2.4.5:7002 slave 6f4e14557ff3ea0111ef802438bb612043f18480 0 1481094820248 5 connected
86ede7388b40eec6c2203155717f684afc016544 10.2.4.5:7003 master - 0 1481094821251 2 connected 5462-10922
6f4e14557ff3ea0111ef802438bb612043f18480 10.2.4.5:7001 myself,master - 0 0 5 connected 4-5461
4f3012ab3fcaf52d21d453219f6575cdf06d2ca6 10.2.4.5:7006 master - 0 1481094818244 6 connected 0-3
f1566757b261fd4305882f607df6586c3b944f52 10.2.4.5:7004 slave 86ede7388b40eec6c2203155717f684afc016544 0 1481094819248 4 connected

[root@s10-2-4-5 redis-3.0.6]# redis-cli -c -p 7006
127.0.0.1:7006> DBSIZE
(integer) 42
127.0.0.1:7006> quit   


二  迁移过程
步骤1: 通知源节点做好把4槽位迁移到127.0.0.1:7006(4f3012ab3fcaf52d21d453219f6575cdf06d2ca6)的准备
[root@s10-2-4-5 redis-3.0.6]# redis-cli -c -p 7001
127.0.0.1:7001> CLUSTER SETSLOT 4 migrating 4f3012ab3fcaf52d21d453219f6575cdf06d2ca6
OK
127.0.0.1:7001> quit

步骤2:通知目的127.0.0.1:7006节点做好接收127.0.0.1:7001节点中4槽位迁移到本节点的准备
[root@s10-2-4-5 redis-3.0.6]# redis-cli -c -p 7006
127.0.0.1:7006> CLUSTER SETSLOT 4 importing 6f4e14557ff3ea0111ef802438bb612043f18480
OK
127.0.0.1:7006> quit


步骤3: 通过redis-cli客户端获取到原集群节点中4槽位中的KV，然后一条一条的通知原集群节点把KV一条一条迁移到目的节点
[root@s10-2-4-5 redis-3.0.6]# redis-cli -c -p 7001
127.0.0.1:7001> CLUSTER GETKEYSINSLOT 4 20    先通过中间工具获取源节点有哪些KV，然后一条一条通知源节点迁移这些KV
1) "2xxxxxxxxxxxxxxxxxxxxxxx146073"
2) "2xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx149173"
3) "7xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx8565"
4) "7xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx38515"
127.0.0.1:7001> MIGRATE 127.0.0.1 7006 2xxxxxxxxxxxxxxxxxxxxxxx146073 0 10 replace
OK
127.0.0.1:7001> MIGRATE 127.0.0.1 7006 2xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx149173 0 10 replace
OK
127.0.0.1:7001> MIGRATE 127.0.0.1 7006 7xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx8565 0 10 replace
OK
127.0.0.1:7001> MIGRATE 127.0.0.1 7006 7xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx38515 0 10 replace
OK
127.0.0.1:7001> CLUSTER GETKEYSINSLOT 4 20
(empty list or set)
127.0.0.1:7001> cluster nodes
f4a5287807d38083970f4b330fa78a1fe139c3ba 10.2.4.5:7000 slave a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 0 1481095441547 3 connected
a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 10.2.4.5:7005 master - 0 1481095438539 3 connected 10923-16383
b88af084e00308e7ab219f212617d121c92434e6 10.2.4.5:7002 slave 6f4e14557ff3ea0111ef802438bb612043f18480 0 1481095435534 5 connected
86ede7388b40eec6c2203155717f684afc016544 10.2.4.5:7003 master - 0 1481095440545 2 connected 5462-10922
//源节点这里显示4槽位正在迁移给7006节点过程中
6f4e14557ff3ea0111ef802438bb612043f18480 10.2.4.5:7001 myself,master - 0 0 5 connected 4-5461 [4->-4f3012ab3fcaf52d21d453219f6575cdf06d2ca6]
4f3012ab3fcaf52d21d453219f6575cdf06d2ca6 10.2.4.5:7006 master - 0 1481095437537 6 connected 0-3
f1566757b261fd4305882f607df6586c3b944f52 10.2.4.5:7004 slave 86ede7388b40eec6c2203155717f684afc016544 0 1481095439541 4 connected
127.0.0.1:7001> quit
[root@s10-2-4-5 redis-3.0.6]# redis-cli -c -p 7006
127.0.0.1:7006> cluster nodes
f1566757b261fd4305882f607df6586c3b944f52 10.2.4.5:7004 slave 86ede7388b40eec6c2203155717f684afc016544 0 1481095452552 2 connected
b88af084e00308e7ab219f212617d121c92434e6 10.2.4.5:7002 slave 6f4e14557ff3ea0111ef802438bb612043f18480 0 1481095450549 5 connected
86ede7388b40eec6c2203155717f684afc016544 10.2.4.5:7003 master - 0 1481095454556 2 connected 5462-10922
//源节点这里显示4槽位正在从7006节点迁移到本7001过程中
4f3012ab3fcaf52d21d453219f6575cdf06d2ca6 10.2.4.5:7006 myself,master - 0 0 6 connected 0-3 [4-<-6f4e14557ff3ea0111ef802438bb612043f18480]
f4a5287807d38083970f4b330fa78a1fe139c3ba 10.2.4.5:7000 slave a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 0 1481095455558 3 connected
a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 10.2.4.5:7005 master - 0 1481095457561 3 connected 10923-16383
6f4e14557ff3ea0111ef802438bb612043f18480 10.2.4.5:7001 master - 0 1481095456559 5 connected 4-5461


步骤4: 步骤3迁移数据完成后，通知源节点数据迁移完毕，置位migrating_slots_to=NULL。如果不执行该命令，源节点就不知道迁移完成，cluster nodes还是会看到处于迁移过程中，打印[4-<-6f4e14557ff3ea0111ef802438bb612043f18480]
[root@s10-2-4-5 redis-3.0.6]# redis-cli -c -p 7001
127.0.0.1:7001> CLUSTER SETSLOT 4 node 4f3012ab3fcaf52d21d453219f6575cdf06d2ca6
OK
127.0.0.1:7001> cluster nodes
f4a5287807d38083970f4b330fa78a1fe139c3ba 10.2.4.5:7000 slave a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 0 1481095512698 3 connected
a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 10.2.4.5:7005 master - 0 1481095508689 3 connected 10923-16383
b88af084e00308e7ab219f212617d121c92434e6 10.2.4.5:7002 slave 6f4e14557ff3ea0111ef802438bb612043f18480 0 1481095511194 5 connected
86ede7388b40eec6c2203155717f684afc016544 10.2.4.5:7003 master - 0 1481095511695 2 connected 5462-10922
6f4e14557ff3ea0111ef802438bb612043f18480 10.2.4.5:7001 myself,master - 0 0 5 connected 5-5461
//从这里看出已经没有[4-<-6f4e14557ff3ea0111ef802438bb612043f18480]
4f3012ab3fcaf52d21d453219f6575cdf06d2ca6 10.2.4.5:7006 master - 0 1481095510693 6 connected 0-4
f1566757b261fd4305882f607df6586c3b944f52 10.2.4.5:7004 slave 86ede7388b40eec6c2203155717f684afc016544 0 1481095509690 4 connected
127.0.0.1:7001> quit


步骤5: 步骤3迁移数据完成后，通知目的节点迁移完成,置位importing_slots_from=NULL,同时向所有集群节点发送该命令
127.0.0.1:7006> CLUSTER SETSLOT 4 node 4f3012ab3fcaf52d21d453219f6575cdf06d2ca6
OK
127.0.0.1:7006> cluster nodes
f1566757b261fd4305882f607df6586c3b944f52 10.2.4.5:7004 slave 86ede7388b40eec6c2203155717f684afc016544 0 1481095495136 2 connected
b88af084e00308e7ab219f212617d121c92434e6 10.2.4.5:7002 slave 6f4e14557ff3ea0111ef802438bb612043f18480 0 1481095492632 5 connected
86ede7388b40eec6c2203155717f684afc016544 10.2.4.5:7003 master - 0 1481095494634 2 connected 5462-10922
//从这里看出已经没有[4->-4f3012ab3fcaf52d21d453219f6575cdf06d2ca6]
4f3012ab3fcaf52d21d453219f6575cdf06d2ca6 10.2.4.5:7006 myself,master - 0 0 6 connected 0-4
f4a5287807d38083970f4b330fa78a1fe139c3ba 10.2.4.5:7000 slave a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 0 1481095491628 3 connected
a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 10.2.4.5:7005 master - 0 1481095493634 3 connected 10923-16383
6f4e14557ff3ea0111ef802438bb612043f18480 10.2.4.5:7001 master - 0 1481095495638 5 connected 5-5461
127.0.0.1:7006> quit


步骤6:查看其它节点是否已经获取到了正确的节点槽位信息，看dbsize变化
[root@s10-2-4-5 redis-3.0.6]# redis-cli -c -p 7002
127.0.0.1:7002> cluster nodes
4f3012ab3fcaf52d21d453219f6575cdf06d2ca6 10.2.4.5:7006 master - 0 1481095518275 6 connected 0-4
6f4e14557ff3ea0111ef802438bb612043f18480 10.2.4.5:7001 master - 0 1481095522282 5 connected 5-5461
f4a5287807d38083970f4b330fa78a1fe139c3ba 10.2.4.5:7000 slave a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 0 1481095516271 3 connected
a3638eccb50fb018f51b5fa8a2d70cb09c38dc4d 10.2.4.5:7005 master - 0 1481095520278 3 connected 10923-16383
86ede7388b40eec6c2203155717f684afc016544 10.2.4.5:7003 master - 0 1481095519276 2 connected 5462-10922
b88af084e00308e7ab219f212617d121c92434e6 10.2.4.5:7002 myself,slave 6f4e14557ff3ea0111ef802438bb612043f18480 0 0 1 connected
f1566757b261fd4305882f607df6586c3b944f52 10.2.4.5:7004 slave 86ede7388b40eec6c2203155717f684afc016544 0 1481095521281 4 connected
127.0.0.1:7002> quit
[root@s10-2-4-5 redis-3.0.6]# 

[root@s10-2-4-5 redis-3.0.6]# redis-cli -c -p 7001
127.0.0.1:7001> DBSIZE
(integer) 50213       从50217变为了50213，因为迁移了4条给7006节点

[root@s10-2-4-5 redis-3.0.6]# redis-cli -c -p 7006
127.0.0.1:7006> DBSIZE
(integer) 46
127.0.0.1:7006> quit   从42变为了46，因为迁移了4条给7001节点
*/
