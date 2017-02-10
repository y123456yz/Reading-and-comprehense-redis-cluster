
echo ""
echo ""
echo ""
echo "========================================================================================================================================================"
echo "==========================================================       Welcome to Redis Cluster    ==========================================================="
echo "========================================================================================================================================================"
echo ""
echo "Starting Redis cluster.........."

killall -9 redis-server
ip=172.16.3.42

ip_cluster_str=cluster_create

for i in {1..10}
do
   node=`expr 7000 + $i`
   node_conf="nodes_""${node}"
   rm -rf /usr/local/cluster/$node
   mkdir -p /usr/local/cluster/$node
   
   
   cp ./src/redis-server  /usr/local/cluster/$node
   cp ./redis.conf  /usr/local/cluster/$node
   sed -i "s/6379/$node/g" /usr/local/cluster/$node/redis.conf   
done


for i in {1..10}
do
   node=`expr 7000 + $i`
   echo "Start Server $i at port $node >>>>>>>>>>>>>"
   cd /usr/local/cluster/$node
   ./redis-server /usr/local/cluster/$node/redis.conf &
done

sleep 2
ps -ef | grep redis

echo "===============================================================================Finish===================================================================="

