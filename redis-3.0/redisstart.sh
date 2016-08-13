
echo ""
echo ""
echo ""
echo "========================================================================================================================================================"
echo "==========================================================       Welcome to Redis Cluster    ==========================================================="
echo "========================================================================================================================================================"
echo ""
echo "Starting Redis cluster.........."

killall -9 redis-server

mkdir -p /usr/local/cluster
cd /usr/local/cluster/
mkdir 7000
mkdir 7001
mkdir 7002
mkdir 7003
mkdir 7004
mkdir 7005
mkdir 7006
mkdir 7007

cd /mnt/data/redisbackup.rdb/
mkdir 7000
mkdir 7001
mkdir 7002
mkdir 7003
mkdir 7004
mkdir 7005
mkdir 7006
mkdir 7007

mkdir -p /data/redislog
cd /data/redislog/
echo > redis_7000.log 
echo > redis_7001.log 
echo > redis_7002.log 
echo > redis_7003.log 
echo > redis_7004.log
echo > redis_7005.log  
echo > redis_7006.log
echo > redis_7007.log
rm -rf /usr/local/cluster/7000/nodes.conf 
rm -rf /usr/local/cluster/7001/nodes.conf 
rm -rf /usr/local/cluster/7002/nodes.conf 
rm -rf /usr/local/cluster/7003/nodes.conf
rm -rf /usr/local/cluster/7004/nodes.conf
rm -rf /usr/local/cluster/7005/nodes.conf   
rm -rf /usr/local/cluster/7006/nodes.conf
rm -rf /usr/local/cluster/7007/nodes.conf

cd /mnt/data/redisbackup.rdb/
cd ./7000/
rm -rf appendonly.aof 
rm -rf dump.rdb  
redis-server redis.conf
echo "Start Server 0 at port 7000 >>>>>>>>>>>>>"
sleep 1
cd ../7001
rm -rf appendonly.aof 
rm -rf dump.rdb 

cd ../7002
rm -rf appendonly.aof
rm -rf dump.rdb 

cd ../7003
rm -rf appendonly.aof
rm -rf dump.rdb 

cd ../7004
rm -rf appendonly.aof
rm -rf dump.rdb 

cd ../7005
rm -rf appendonly.aof
rm -rf dump.rdb 

cd ../7006
rm -rf appendonly.aof
rm -rf dump.rdb 

cd ../7007
rm -rf appendonly.aof
rm -rf dump.rdb 

cd /usr/local/cluster/7000
redis-server redis.conf
echo "Start Server 0 at port 7000 >>>>>>>>>>>>>"
sleep 1

cd ../7001
redis-server redis.conf
echo "Start Server 1 at port 7001 >>>>>>>>>>>>>"
sleep 1
cd ../7002
rm -rf appendonly.aof 
rm -rf dump.rdb 
redis-server redis.conf
echo "Start Server 2 at port 7002 >>>>>>>>>>>>>"
sleep 1
cd ../7003
rm -rf appendonly.aof 
rm -rf dump.rdb 
redis-server redis.conf
echo "Start Server 3 at port 7003 >>>>>>>>>>>>>"
sleep 1
cd ../7004
rm -rf appendonly.aof 
rm -rf dump.rdb 
redis-server redis.conf
echo "Start Server 4 at port 7004 >>>>>>>>>>>>>"
sleep 1
cd ../7005
rm -rf appendonly.aof 
rm -rf dump.rdb 
redis-server redis.conf
echo "Start Server 5 at port 7005 >>>>>>>>>>>>>"


sleep 1
cd ../7006
rm -rf appendonly.aof
rm -rf dump.rdb
#redis-server redis.conf
#echo "Start Server 4 at port 7006 >>>>>>>>>>>>>"
sleep 1
cd ../7007
rm -rf appendonly.aof
rm -rf dump.rdb
#redis-server redis.conf
#echo "Start Server 5 at port 7007 >>>>>>>>>>>>>"


sleep 2
echo ""
echo ""
echo "-------------------------------------------------Redis Status--------------------------------------------------------------------"
ps -ef | grep redis
sleep 2
echo ""
echo ""
echo "===============================================================================Finish===================================================================="
sleep 2
redis-trib.rb create --replicas 1 127.0.0.1:7000 127.0.0.1:7001 127.0.0.1:7002 127.0.0.1:7003 127.0.0.1:7004 127.0.0.1:7005 
#127.0.0.1:7006 127.0.0.1:7007
