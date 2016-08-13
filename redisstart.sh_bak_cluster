
echo ""
echo ""
echo ""
echo "========================================================================================================================================================"
echo "==========================================================       Welcome to Redis Cluster    ==========================================================="
echo "========================================================================================================================================================"
echo ""
echo "Starting Redis cluster.........."

rm -rf /usr/local/cluster/7000/nodes_7000.conf
rm -rf /usr/local/cluster/7001/nodes_7001.conf 
rm -rf /usr/local/cluster/7002/nodes_7002.conf 
rm -rf /usr/local/cluster/7003/nodes_7003.conf 

touch /usr/local/cluster/7000/nodes_7000.conf
touch /usr/local/cluster/7001/nodes_7001.conf
touch /usr/local/cluster/7002/nodes_7002.conf
touch /usr/local/cluster/7003/nodes_7003.conf

echo "Start Server 0 at port 7000 >>>>>>>>>>>>>"
./redis-server /usr/local/cluster/7000/redis.conf
sleep 1
./redis-server /usr/local/cluster/7001/redis.conf
echo "Start Server 1 at port 7001 >>>>>>>>>>>>>"
sleep 1
./redis-server /usr/local/cluster/7002/redis.conf
echo "Start Server 2 at port 7002 >>>>>>>>>>>>>"
sleep 1
./redis-server /usr/local/cluster/7003/redis.conf
echo "Start Server 3 at port 7003 >>>>>>>>>>>>>"
sleep 1
sleep 1
echo ""
echo ""
echo "-------------------------------------------------Redis Status--------------------------------------------------------------------"
ps -ef | grep redis
sleep 2
echo ""
echo ""
echo "===============================================================================Finish===================================================================="
