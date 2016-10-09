#ifndef __REDIS_CLUSTER_H
#define __REDIS_CLUSTER_H

/*-----------------------------------------------------------------------------
 * Redis cluster data structures, defines, exported API.
 *----------------------------------------------------------------------------*/

// 槽数量
#define REDIS_CLUSTER_SLOTS 16384  //对应16K，也就是2的14次方
// 集群在线
#define REDIS_CLUSTER_OK 0          /* Everything looks ok */
// 集群下线
#define REDIS_CLUSTER_FAIL 1        /* The cluster can't work */
// 节点名字的长度
#define REDIS_CLUSTER_NAMELEN 40    /* sha1 hex length */
// 集群的实际端口号 = 用户指定的端口号 + REDIS_CLUSTER_PORT_INCR
#define REDIS_CLUSTER_PORT_INCR 10000 /* Cluster port = baseport + PORT_INCR */

/* The following defines are amunt of time, sometimes expressed as
 * multiplicators of the node timeout value (when ending with MULT). 
 *
 * 以下是和时间有关的一些常量，
 * 以 _MULTI 结尾的常量会作为时间值的乘法因子来使用。
 */
// 默认节点超时时限
#define REDIS_CLUSTER_DEFAULT_NODE_TIMEOUT 15000
// 检验下线报告的乘法因子
#define REDIS_CLUSTER_FAIL_REPORT_VALIDITY_MULT 2 /* Fail report validity. */
// 撤销主节点 FAIL 状态的乘法因子
#define REDIS_CLUSTER_FAIL_UNDO_TIME_MULT 2 /* Undo fail if master is back. */
// 撤销主节点 FAIL 状态的加法因子
#define REDIS_CLUSTER_FAIL_UNDO_TIME_ADD 10 /* Some additional time. */
// 在检查从节点数据是否有效时使用的乘法因子
#define REDIS_CLUSTER_SLAVE_VALIDITY_MULT 10 /* Slave data validity. */
// 在执行故障转移之前需要等待的秒数，似乎已经废弃
#define REDIS_CLUSTER_FAILOVER_DELAY 5 /* Seconds */
// 未使用，似乎已经废弃
#define REDIS_CLUSTER_DEFAULT_MIGRATION_BARRIER 1
// 在进行手动的故障转移之前，需要等待的超时时间
#define REDIS_CLUSTER_MF_TIMEOUT 5000 /* Milliseconds to do a manual failover. */
// 未使用，似乎已经废弃
#define REDIS_CLUSTER_MF_PAUSE_MULT 2 /* Master pause manual failover mult. */

/* Redirection errors returned by getNodeByQuery(). */
/* 由 getNodeByQuery() 函数返回的转向错误。 */
// 节点可以处理这个命令
#define REDIS_CLUSTER_REDIR_NONE 0          /* Node can serve the request. */
// 键在其他槽
#define REDIS_CLUSTER_REDIR_CROSS_SLOT 1    /* Keys in different slots. */
// 键所处的槽正在进行 reshard
#define REDIS_CLUSTER_REDIR_UNSTABLE 2      /* Keys in slot resharding. */
// 需要进行 ASK 转向
#define REDIS_CLUSTER_REDIR_ASK 3           /* -ASK redirection required. */
// 需要进行 MOVED 转向
#define REDIS_CLUSTER_REDIR_MOVED 4         /* -MOVED redirection required. */

// 前置定义，防止编译错误
struct clusterNode;


//客户端想服务端发送meet后，客户端通过和服务端建立连接来记录服务端节点clusterNode->link在clusterCron
//服务端接收到连接后，通过clusterAcceptHandler建立客户端节点的clusterNode.link，见clusterAcceptHandler


//server.cluster(clusterState)->clusterState.nodes(clusterNode)->clusterNode.link(clusterLink)
//redisClient结构和clusterLink结构都有自己的套接字描述法和输入 输出缓冲区，区别在于，redisClient用于客户端
//clusterLink用于集群中的连接节点
/* clusterLink encapsulates everything needed to talk with a remote node. */
// clusterLink 包含了与其他节点进行通讯所需的全部信息
typedef struct clusterLink { //clusterNode->link     集群数据交互接收的地方在clusterProcessPacket      
//clusterLink创建的地方在clusterAcceptHandler->createClusterLink
    //B节点连接到A节点，则A节点会创建一个clusterLink，并接收这个B节点相关的网络时间，其中的node就是B节点的clusterNode，fd为B连接A的时候的fd

    // 连接的创建时间
    mstime_t ctime;             /* Link creation time */

    // TCP 套接字描述符
    int fd;                     /* TCP socket file descriptor */

    // 输出缓冲区，保存着等待发送给其他节点的消息（message）。
    sds sndbuf;                 /* Packet send buffer */

    // 输入缓冲区，保存着从其他节点接收到的消息。见clusterReadHandler
    sds rcvbuf;                 /* Packet reception buffer */
 
    // 与这个连接相关联的节点，如果没有的话就为 NULL   
    //B节点连接到A节点，则A节点会创建一个clusterLink，并接收这个B节点相关的网络时间，其中的node就是B节点
    struct clusterNode *node;   /* Node related to this link if any, or NULL */

} clusterLink;

/*  一下这个标记赋值给clusterNode.flag */

/* Cluster node flags and macros. */
// 该节点为主节点  在集群情况下，在redis起来的时候如果发现配置是cluster模式则会设置本节点模式为nodes.conf中的配置，或者主备情况下主挂了后，被被
//其他的备节点被集群中的半数以上主节点选为主节点，则该节点变为主
#define REDIS_NODE_MASTER 1     /* The node is a master */
// 该节点为从节点
#define REDIS_NODE_SLAVE 2      /* The node is a slave */
// 该节点疑似下线，需要对它的状态进行确认
#define REDIS_NODE_PFAIL 4      /* Failure? Need acknowledge */
// 该节点已下线
#define REDIS_NODE_FAIL 8       /* The node is believed to be malfunctioning */
// 该节点是当前节点自身
#define REDIS_NODE_MYSELF 16    /* This node is myself */

//在敲cluster meet IP port的时候，在clusterStartHandshake中把节点状态置为REDIS_NODE_HANDSHAKE  REDIS_NODE_MEET ，或者从配置文件node.conf中读到的就是该状态
// 该节点还未与当前节点完成第一次 PING - PONG 通讯   只有接受到某个node的ping pong meet则会清除该状态
#define REDIS_NODE_HANDSHAKE 32 /* We have still to exchange the first ping */
// 该节点没有地址  clusterProcessPacket值置为该状态，或者从配置文件node.conf中读到的就是该状态
#define REDIS_NODE_NOADDR   64  /* We don't know the address of this node */

//在敲cluster meet IP port的时候，在clusterStartHandshake中把节点状态置为REDIS_NODE_HANDSHAKE  REDIS_NODE_MEET，或者从配置文件node.conf中读到的就是该状态

// 当前节点还未与该节点进行过接触
// 带有这个标识会让当前节点发送 MEET 命令而不是 PING 命令
#define REDIS_NODE_MEET 128     /* Send a MEET message to this node */
// 该节点被选中为新的主节点
#define REDIS_NODE_PROMOTED 256 /* Master was a slave propoted by failover */
// 空名字（在节点为主节点时，用作消息中的 slaveof 属性的值）
#define REDIS_NODE_NULL_NAME "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"

// 用于判断节点身份和状态的一系列宏
#define nodeIsMaster(n) ((n)->flags & REDIS_NODE_MASTER)
#define nodeIsSlave(n) ((n)->flags & REDIS_NODE_SLAVE)
#define nodeInHandshake(n) ((n)->flags & REDIS_NODE_HANDSHAKE)
#define nodeHasAddr(n) (!((n)->flags & REDIS_NODE_NOADDR))
#define nodeWithoutAddr(n) ((n)->flags & REDIS_NODE_NOADDR)
#define nodeTimedOut(n) ((n)->flags & REDIS_NODE_PFAIL)
#define nodeFailed(n) ((n)->flags & REDIS_NODE_FAIL)

/* This structure represent elements of node->fail_reports. */
// 每个 clusterNodeFailReport 结构保存了一条其他节点对目标节点的下线报告
// （认为目标节点已经下线）
struct clusterNodeFailReport {

    // 报告目标节点已经下线的节点
    struct clusterNode *node;  /* Node reporting the failure condition. */

    // 最后一次从 node 节点收到下线报告的时间
    // 程序使用这个时间戳来检查下线报告是否过期
    mstime_t time;             /* Time of the last report from this node. */

} typedef clusterNodeFailReport;

//server.cluster(clusterState)->clusterState.nodes(clusterNode)->clusterNode.link(clusterLink)
// 节点状态    节点创建在createClusterNode
struct clusterNode { //clusterState->nodes结构  集群数据交互接收的地方在clusterProcessPacket

    // 创建节点的时间
    mstime_t ctime; /* Node object creation time. */

    // 节点的名字，由 40 个十六进制字符组成   见createClusterNode->getRandomHexChars
    // 例如 68eef66df23420a5862208ef5b1a7005b806f2ff
    char name[REDIS_CLUSTER_NAMELEN]; /* Node name, hex string, sha1-size */

    // 节点标识
    // 使用各种不同的标识值记录节点的角色（比如主节点或者从节点），
    // 以及节点目前所处的状态（比如在线或者下线）。  取值REDIS_NODE_MASTER  REDIS_NODE_PFAIL等
    int flags;      /* REDIS_NODE_... */ //取值可以参考clusterGenNodeDescription

    /* configepoch和currentepoch可以参考:Redis_Cluster的Failover设计.PPT */
    // 节点当前的配置纪元，用于实现故障转移 /* current epoch和cluster epoch可以参考http://redis.cn/topics/cluster-spec.html */
    //可以通过cluster set-config-epoch num来配置configEpoch,见clusterCommand
    //实际上各个节点自己的configEpoch通过报文交互，在clusterHandleConfigEpochCollision中进行设置，
    //也就是最终server.cluster->currentEpoch会和集群中节点node.configEpoch值最大的相同，如果没有配置set-config-epoch的话，
    //也就是集群节点数减去1(因为epoch从0开始)

    /*
    Current Epoch用于集群的epoch，代表集群的版本。
    Config Epoch，每个master都有config Epoch代表Master的版本
    每个新加入的节点，current Epoch初始为0，通过ping/pong消息交换的话，如果发送节点的epoch高于自己的，
    则将自己的currentEpoch更新为发送者的。经过N轮消息交换以后，每个节点的current epoch保持一致。
    clusterState.currentEpoch可以通过cluster info命令中的cluster_current_epoch获取到
    Current Epoch用于failover

    
    Redis提供了解决冲突的办法，节点之间消息交换过程中，会把自己的currentEpoch和configEpoch带过去，如果发现发送者
    的configEpoch和自己的configEpoch相同，则将自己的Epoch+1，经过N轮以后使每个master的configEpoch不一样
    Slave也有configEpoch，是通过master交互得到的Master的configEpoch。而currentEpoch是整个集群的版本号，所有节点该值相同
    
    每个master的configEpoch必须不同，当发生配置冲突以后，采用高版本的配置。通过多次交互后，集群中每个节点的configEpoch
    会不同，如果没有配置set-config-epoch的话，各个节点的clusterNode.configEpoch分别为0 - n,例如3个节点，则每个节点分别对应
    0 1 2，可以通过cluster node中connected前的数值查看
    */ //赋值见clusterHandleConfigEpochCollision
    uint64_t configEpoch; /* Last configEpoch observed for this node */

    // 由这个节点负责处理的槽
    // 一共有 REDIS_CLUSTER_SLOTS / 8 个字节长
    // 每个字节的每个位记录了一个槽的保存状态
    // 位的值为 1 表示槽正由本节点处理，值为 0 则表示槽并非本节点处理
    // 比如 slots[0] 的第一个位保存了槽 0 的保存情况
    // slots[0] 的第二个位保存了槽 1 的保存情况，以此类推     位图表示16384个槽位  记录该clusterNode节点处理的槽
    
    //clusterNode->slots记录本clusterNode节点处理的槽，clusterState->nodes记录了所有的clusterNode节点信息，各个节点的slots
    //指定了本节点处理的槽，因此clusterState->nodes可以获取到所有槽所属欲那个节点
    unsigned char slots[REDIS_CLUSTER_SLOTS/8]; /* slots handled by this node */

    // 该节点负责处理的槽数量
    int numslots;   /* Number of slots handled by this node */

    // 如果本节点是主节点，那么用这个属性记录从节点的数量
    int numslaves;  /* Number of slave nodes, if this is a master */

    // 指针数组，指向各个从节点
    struct clusterNode **slaves; /* pointers to slave nodes */

    // 如果这是一个从节点，那么指向主节点
    struct clusterNode *slaveof; /* pointer to the master node */

    //向该node节点最后一次发送ping消息的时间       本端ping对端，对端pong应答后会把该ping_pong置0，见clusterProcessPacket
    // 最后一次发送 PING 命令的时间   赋值见clusterSendPing
    mstime_t ping_sent;      /* Unix time we sent latest ping */

    // 最后一次接收 PONG 回复的时间戳
    mstime_t pong_received;  /* Unix time we received the pong */

    // 最后一次被设置为 FAIL 状态的时间
    mstime_t fail_time;      /* Unix time when FAIL flag was set */

    // 最后一次给某个从节点投票的时间
    mstime_t voted_time;     /* Last time we voted for a slave of this master */

    // 最后一次从这个节点接收到复制偏移量的时间
    mstime_t repl_offset_time;  /* Unix time we received offset for this node */

    // 这个节点的复制偏移量
    long long repl_offset;      /* Last known repl offset for this node. */

    // 节点的 IP 地址
    char ip[REDIS_IP_STR_LEN];  /* Latest known IP address of this node */

    // 节点的端口号
    int port;                   /* Latest known port of this node */

    
    //客户端想服务端发送meet后，客户端通过和服务端建立连接来记录服务端节点clusterNode->link在clusterCron
    //服务端接收到连接后，通过clusterAcceptHandler建立客户端节点的clusterNode.link，见clusterAcceptHandler
    
    // 保存连接节点所需的有关信息   link节点创建和赋值见clusterCron.createClusterLink
    clusterLink *link;          /* TCP/IP link with this node */

    // 一个链表，记录了所有其他节点对该节点的下线报告
    list *fail_reports;         /* List of nodes signaling this as failing */

};
typedef struct clusterNode clusterNode;


// 集群状态，每个节点都保存着一个这样的状态，记录了它们眼中的集群的样子。
// 另外，虽然这个结构主要用于记录集群的属性，但是为了节约资源，
// 有些与节点有关的属性，比如 slots_to_keys 、 failover_auth_count 
// 也被放到了这个结构里面。   集群数据交互接收的地方在clusterProcessPacket

//server.cluster(clusterState)->clusterState.nodes(clusterNode)->clusterNode.link(clusterLink)
typedef struct clusterState { //数据源头在server.cluster   //集群相关配置加载在clusterLoadConfig

    // 指向当前节点的指针
    clusterNode *myself;  /* This node */

    /* current epoch和cluster epoch可以参考http://redis.cn/topics/cluster-spec.html */
    // 集群当前的配置纪元，用于实现故障转移   这个也就是集群中所有节点中最大currentEpoch 

    //实际上各个节点自己的configEpoch通过报文交互，在clusterHandleConfigEpochCollision中进行设置，
    //也就是最终server.cluster->currentEpoch会和集群中节点node.configEpoch值最大的相同，如果没有配置set-config-epoch的话，
    //也就是集群节点数减去1(因为epoch从0开始)
    //Epoch是一个只增的版本号。每当有事件发生，epoch向上增长。这里的事件是指节点加入、failover等
     /*
    Current Epoch用于集群的epoch，代表集群的版本。
    Config Epoch，每个master都有config Epoch代表Master的版本
    每个新加入的节点，current Epoch初始为0，通过ping/pong消息交换的话，如果发送节点的epoch高于自己的，
    则将自己的currentEpoch更新为发送者的。经过N轮消息交换以后，每个节点的current epoch保持一致。
    clusterState.currentEpoch可以通过cluster info命令中的cluster_current_epoch获取到
    Current Epoch用于failover

    
    Redis提供了解决冲突的办法，节点之间消息交换过程中，会把自己的currentEpoch和configEpoch带过去，如果发现发送者
    的configEpoch和自己的configEpoch相同，则将自己的Epoch+1，经过N轮以后使每个master的configEpoch不一样
    Slave也有configEpoch，是通过master交互得到的Master的configEpoch。而currentEpoch是整个集群的版本号，所有节点该值相同
    
    每个master的configEpoch必须不同，当发生配置冲突以后，采用高版本的配置。通过多次交互后，集群中每个节点的configEpoch
    会不同，如果没有配置set-config-epoch的话，各个节点的clusterNode.configEpoch分别为0 - n,例如3个节点，则每个节点分别对应
    0 1 2，可以通过cluster node中connected前的数值查看
    */ //currentEpoch可以通过cluster info命令中的cluster_current_epoch获取到   configEpoch可以通过cluster node中connected前的数值查看
    uint64_t currentEpoch; //在clusterHandleConfigEpochCollision中会自增

    // 集群当前的状态：是在线还是下线    在clusterUpdateState中更新集群状态
    int state;            /* REDIS_CLUSTER_OK, REDIS_CLUSTER_FAIL, ... */

    // 集群中至少处理着一个槽的节点的数量。  clusterUpdateState中跟新   在线并且正在处理至少一个槽的 master 的数量
    //注意:包括已下线的，因为已下线的node还是会在dict *nodes中 
    int size;             /* Num of master nodes with at least one slot */ //默认从1开始，而不是从0开始

    //节点B通过cluster meet A-IP A-PORT把B节点添加到A节点在集群的时候，A节点的nodes里面就会有B节点的信息
    //然后A节点应答pong给B，B收到后也会把A节点添加到自己的nodes中
    // 集群节点名单（包括 myself 节点）
    // 字典的键为节点的名字，字典的值为 clusterNode 结构

    //clusterNode->slots记录本clusterNode节点处理的槽，clusterState->nodes记录了所有的clusterNode节点信息，各个节点的slots
    //指定了本节点处理的槽，因此clusterState->nodes可以获取到所有槽所属欲那个节点

    //注意:如果加到集群中的某个节点下线了，这个主节点的clusterNode还是会在该nodes上面，只是cluster nodes的时候会把该节点标记为下线
    dict *nodes;          /* Hash table of name -> clusterNode structures */

    // 节点黑名单，用于 CLUSTER FORGET 命令
    // 防止被 FORGET 的命令重新被添加到集群里面
    // （不过现在似乎没有在使用的样子，已废弃？还是尚未实现？）
    dict *nodes_black_list; /* Nodes we don't re-add for a few seconds. */

    // 记录要从当前节点迁移到目标节点的槽，以及迁移的目标节点
    // migrating_slots_to[i] = NULL 表示槽 i 未被迁移
    // migrating_slots_to[i] = clusterNode_A 表示槽 i 要从本节点迁移至节点 A
    clusterNode *migrating_slots_to[REDIS_CLUSTER_SLOTS];

    // 记录要从源节点迁移到本节点的槽，以及进行迁移的源节点
    // importing_slots_from[i] = NULL 表示槽 i 未进行导入
    // importing_slots_from[i] = clusterNode_A 表示正从节点 A 中导入槽 i
    clusterNode *importing_slots_from[REDIS_CLUSTER_SLOTS];

    // 负责处理各个槽的节点
    // 例如 slots[i] = clusterNode_A 表示槽 i 由节点 A 处理
    clusterNode *slots[REDIS_CLUSTER_SLOTS];

    // 跳跃表，表中以槽作为分值，键作为成员，对槽进行有序排序
    // 当需要对某些槽进行区间（range）操作时，这个跳跃表可以提供方便
    // 具体操作定义在 db.c 里面

     //注意在从rdb文件或者aof文件中读取到key-value对的时候，如果启用了集群功能会在dbAdd->slotToKeyAdd(key);中把key和slot的对应关系添加到slots_to_keys
    //并在verifyClusterConfigWithData->clusterAddSlot中从而指派对应的slot，也就是本服务器中的rdb中的key-value对应的slot分配给本服务器
    zskiplist *slots_to_keys; 
   

    /* The following fields are used to take the slave state on elections. */
    // 以下这些域被用于进行故障转移选举

    // 上次执行选举或者下次执行选举的时间
    mstime_t failover_auth_time; /* Time of previous or next election. */

    // 节点获得的投票数量
    int failover_auth_count;    /* Number of votes received so far. */

    // 如果值为 1 ，表示本节点已经向其他节点发送了投票请求
    int failover_auth_sent;     /* True if we already asked for votes. */

    int failover_auth_rank;     /* This slave rank for current auth request. */

    uint64_t failover_auth_epoch; /* Epoch of the current election. */

    /* Manual failover state in common. */
    /* 共用的手动故障转移状态 */

    // 手动故障转移执行的时间限制    CLUSTER FAILOVER命令会触发进行手动故障转移，见clusterCommand
    mstime_t mf_end;            /* Manual failover time limit (ms unixtime).
                                   It is zero if there is no MF in progress. */
    /* Manual failover state of master. */
    /* 主服务器的手动故障转移状态 */
    clusterNode *mf_slave;      /* Slave performing the manual failover. */
    /* Manual failover state of slave. */
    /* 从服务器的手动故障转移状态 */
    long long mf_master_offset; /* Master offset the slave needs to start MF
                                   or zero if stil not received. */
    // 指示手动故障转移是否可以开始的标志值
    // 值为非 0 时表示各个主服务器可以开始投票
    int mf_can_start;           /* If non-zero signal that the manual failover
                                   can start requesting masters vote. */

    /* The followign fields are uesd by masters to take state on elections. */
    /* 以下这些域由主服务器使用，用于记录选举时的状态 */

    // 集群最后一次进行投票的纪元
    uint64_t lastVoteEpoch;     /* Epoch of the last vote granted. */

    // 在进入下个事件循环之前要做的事情，以各个 flag 来记录
    int todo_before_sleep; /* Things to do in clusterBeforeSleep(). */

    // 通过 cluster 连接发送的消息数量
    long long stats_bus_messages_sent;  /* Num of msg sent via cluster bus. */

    // 通过 cluster 接收到的消息数量   其他节点发往本节点的报文字节数
    long long stats_bus_messages_received; /* Num of msg rcvd via cluster bus.*/

} clusterState;

/* clusterState todo_before_sleep flags. */
// 以下每个 flag 代表了一个服务器在开始下一个事件循环之前
// 要做的事情
#define CLUSTER_TODO_HANDLE_FAILOVER (1<<0)
#define CLUSTER_TODO_UPDATE_STATE (1<<1)
#define CLUSTER_TODO_SAVE_CONFIG (1<<2)
#define CLUSTER_TODO_FSYNC_CONFIG (1<<3)

/* Redis cluster messages header */

/* Note that the PING, PONG and MEET messages are actually the same exact
 * kind of packet. PONG is the reply to ping, in the exact format as a PING,
 * while MEET is a special PING that forces the receiver to add the sender
 * as a node (if it is not already in the list). */

/* 下面这些赋值给clusterMsg.type   以下消息的处理统一在clusterReadHandler->clusterProcessPacket */
 
// 注意，PING 、 PONG 和 MEET 实际上是同一种消息。
// PONG 是对 PING 的回复，它的实际格式也为 PING 消息，
// 而 MEET 则是一种特殊的 PING 消息，用于强制消息的接收者将消息的发送者添加到集群中
// （如果节点尚未在节点列表中的话）
// PING  MEET消息和PING消息都在clusterCron中发送
#define CLUSTERMSG_TYPE_PING 0          /* Ping */
// PONG （回复 PING）
#define CLUSTERMSG_TYPE_PONG 1          /* Pong (reply to Ping) */
// 请求将某个节点添加到集群中   MEET消息和PING消息都在clusterCron中发送
#define CLUSTERMSG_TYPE_MEET 2          /* Meet "let's join" message */
// 将某个节点标记为 FAIL   通过clusterBuildMessageHdr组包发送
#define CLUSTERMSG_TYPE_FAIL 3          /* Mark node xxx as failing */
// 通过发布与订阅功能广播消息
#define CLUSTERMSG_TYPE_PUBLISH 4       /* Pub/Sub Publish propagation */
// 请求进行故障转移操作，要求消息的接收者通过投票来支持消息的发送者
#define CLUSTERMSG_TYPE_FAILOVER_AUTH_REQUEST 5 /* May I failover? */
// 消息的接收者同意向消息的发送者投票
#define CLUSTERMSG_TYPE_FAILOVER_AUTH_ACK 6     /* Yes, you have my vote */
// 槽布局已经发生变化，消息发送者要求消息接收者进行相应的更新    通过clusterBuildMessageHdr组包发送
#define CLUSTERMSG_TYPE_UPDATE 7        /* Another node slots configuration */
// 为了进行手动故障转移，暂停各个客户端
#define CLUSTERMSG_TYPE_MFSTART 8       /* Pause clients for manual failover */

/* Initially we don't know our "name", but we'll find it once we connect
 * to the first node, using the getsockname() function. Then we'll use this
 * address for all the next messages. */
//clusterMsg是集群节点通信的消息头，消息体是结构clusterMsgData，
//clusterMsgData包括clusterMsgDataGossip、clusterMsgDataFail、clusterMsgDataPublish、clusterMsgDataUpdate
typedef struct {  //ping  pong meet消息用该结构，见clusterProcessPacket

    // 节点的名字
    // 在刚开始的时候，节点的名字会是随机的
    // 当 MEET 信息发送并得到回复之后，集群就会为节点设置正式的名字
    char nodename[REDIS_CLUSTER_NAMELEN];

    // 最后一次向该节点发送 PING 消息的时间戳
    uint32_t ping_sent;

    // 最后一次从该节点接收到 PONG 消息的时间戳
    uint32_t pong_received;

    // 节点的 IP 地址
    char ip[REDIS_IP_STR_LEN];    /* IP address last time it was seen */

    // 节点的端口号
    uint16_t port;  /* port last time it was seen */

    // 节点的标识值
    uint16_t flags;

    // 对齐字节，不使用
    uint32_t notused; /* for 64 bit alignment */

} clusterMsgDataGossip;

//clusterMsg是集群节点通信的消息头，消息体是结构clusterMsgData，
//clusterMsgData包括clusterMsgDataGossip、clusterMsgDataFail、clusterMsgDataPublish、clusterMsgDataUpdate
typedef struct {

    // 下线节点的名字
    char nodename[REDIS_CLUSTER_NAMELEN];

} clusterMsgDataFail;

//clusterMsg是集群节点通信的消息头，消息体是结构clusterMsgData，
//clusterMsgData包括clusterMsgDataGossip、clusterMsgDataFail、clusterMsgDataPublish、clusterMsgDataUpdate
typedef struct {

    // 频道名长度
    uint32_t channel_len;

    // 消息长度
    uint32_t message_len;

    // 消息内容，格式为 频道名+消息
    // bulk_data[0:channel_len-1] 为频道名
    // bulk_data[channel_len:channel_len+message_len-1] 为消息
    unsigned char bulk_data[8]; /* defined as 8 just for alignment concerns. */

} clusterMsgDataPublish;

//clusterMsg是集群节点通信的消息头，消息体是结构clusterMsgData，
//clusterMsgData包括clusterMsgDataGossip、clusterMsgDataFail、clusterMsgDataPublish、clusterMsgDataUpdate
typedef struct {

    // 节点的配置纪元  /* current epoch和cluster epoch可以参考http://redis.cn/topics/cluster-spec.html */
    uint64_t configEpoch; /* Config epoch of the specified instance. */

    // 节点的名字
    char nodename[REDIS_CLUSTER_NAMELEN]; /* Name of the slots owner. */

    // 节点的槽布局
    unsigned char slots[REDIS_CLUSTER_SLOTS/8]; /* Slots bitmap. */

} clusterMsgDataUpdate;

//clusterMsg是集群节点通信的消息头，消息体是结构clusterMsgData，
//clusterMsgData包括clusterMsgDataGossip、clusterMsgDataFail、clusterMsgDataPublish、clusterMsgDataUpdate
union clusterMsgData {//clusterMsg中的data字段

     /* PING, MEET and PONG */ /*
    因为MEET、PING、PONG三种消息都使用相同的消息正文，所以节点通过消息头的type属性来判断一条消息是MEET消息、PING消息还是PONG消息。
     */
    struct {
        /* Array of N clusterMsgDataGossip structures */
        // 每条消息都包含两个 clusterMsgDataGossip 结构     ?????????为什么这里可以存两个成员进来
        clusterMsgDataGossip gossip[1];  
    } ping;

    /* FAIL */
    struct {
        clusterMsgDataFail about;
    } fail;

    /* PUBLISH */
    struct {
        clusterMsgDataPublish msg;
    } publish;

    /* UPDATE */
    struct {
        clusterMsgDataUpdate nodecfg;
    } update;

};

//clusterMsg是集群节点通信的消息头，消息体是结构clusterMsgData，
//clusterMsgData包括clusterMsgDataGossip、clusterMsgDataFail、clusterMsgDataPublish、clusterMsgDataUpdate


//clustermsg是集群节点通信的消息头，消息体是结构clusterMsgData
// 用来表示集群消息的结构（消息头，header）   clusterMsg在clusterBuildMessageHdr中进行组包
typedef struct { //内部通信直接通过该结构发送，解析该结构在clusterProcessPacket
    char sig[4];        /* Siganture "RCmb" (Redis Cluster message bus). */
    // 消息的长度（包括这个消息头的长度和消息正文的长度）
    uint32_t totlen;    /* Total length of this message */
    uint16_t ver;       /* Protocol version, currently set to 0. */
    uint16_t notused0;  /* 2 bytes not used. */

    /*
    因为MEET、PING、PONG三种消息都使用相同的消息正文，所以节点通过消息头的type属性来判断一条消息是MEET消息、PING消息还是PONG消息。
     */
    // 消息的类型  取值CLUSTERMSG_TYPE_PING等
    uint16_t type;      /* Message type */

    // 消息正文包含的节点信息数量
    // 只在发送 MEET 、 PING 和 PONG 这三种 Gossip 协议消息时使用
    uint16_t count;     /* Only used for some kind of messages. */ //代表携带的消息体个数，可以参考clusterSendPing

    //赋值来自于server.cluster->currentEpoch，见clusterBuildMessageHdr  
    // 消息发送者的配置纪元   也就是当前节点所在集群的版本号  /* current epoch和cluster epoch可以参考http://redis.cn/topics/cluster-spec.html */
    uint64_t currentEpoch;  /* The epoch accordingly to the sending node. */

    // 如果消息发送者是一个主节点，那么这里记录的是消息发送者的配置纪元
    // 如果消息发送者是一个从节点，那么这里记录的是消息发送者正在复制的主节点的配置纪元 
    /* current epoch和cluster epoch可以参考http://redis.cn/topics/cluster-spec.html */
    //见clusterBuildMessageHdr，当前节点的Epoch，每个节点自己的Epoch不一样，可以参考clusterNode->configEpoch
    uint64_t configEpoch;   /* The config epoch if it's a master, or the last
                               epoch advertised by its master if it is a
                               slave. */

    // 节点的复制偏移量
    uint64_t offset;    /* Master replication offset if node is a master or
                           processed replication offset if node is a slave. */

    /* currentEpoch、sender、myslots等属性记录了发送者自身的节点信息，接牧者会根据这些信息，在自己的clusterState．nodes字典里找到发送
者对应的clusterNode结构，并对结构进行更新。 */
    // 消息发送者的名字（ID）
    char sender[REDIS_CLUSTER_NAMELEN]; /* Name of the sender node */

    // 消息发送者目前的槽指派信息
    unsigned char myslots[REDIS_CLUSTER_SLOTS/8];

    // 如果消息发送者是一个从节点，那么这里记录的是消息发送者正在复制的主节点的名字
    // 如果消息发送者是一个主节点，那么这里记录的是 REDIS_NODE_NULL_NAME
    // （一个 40 字节长，值全为 0 的字节数组）
    char slaveof[REDIS_CLUSTER_NAMELEN];

    char notused1[32];  /* 32 bytes reserved for future usage. */

    // 消息发送者的端口号
    uint16_t port;      /* Sender TCP base port */

    // 消息发送者的标识值
    uint16_t flags;     /* Sender node flags */

    // 消息发送者所处集群的状态
    unsigned char state; /* Cluster state from the POV of the sender */

    // 消息标志   取值CLUSTERMSG_FLAG0_PAUSED等
    unsigned char mflags[3]; /* Message flags: CLUSTERMSG_FLAG[012]_... */

    // 消息的正文（或者说，内容）
    union clusterMsgData data;

} clusterMsg;

#define CLUSTERMSG_MIN_LEN (sizeof(clusterMsg)-sizeof(union clusterMsgData))

/* Message flags better specify the packet content or are used to
 * provide some information about the node state. */
#define CLUSTERMSG_FLAG0_PAUSED (1<<0) /* Master paused for manual failover. */
#define CLUSTERMSG_FLAG0_FORCEACK (1<<1) /* Give ACK to AUTH_REQUEST even if
                                            master is up. */

/* ---------------------- API exported outside cluster.c -------------------- */
clusterNode *getNodeByQuery(redisClient *c, struct redisCommand *cmd, robj **argv, int argc, int *hashslot, int *ask);

#endif /* __REDIS_CLUSTER_H */
