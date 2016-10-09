
echo ""
echo ""
echo ""
echo "========================================================================================================================================================"
echo "==========================================================       Welcome to Redis Cluster    ==========================================================="
echo "========================================================================================================================================================"
echo ""
echo "Starting Redis cluster.........."

killall -9 redis-server

rm -rf /usr/local/cluster/7000
rm -rf /usr/local/cluster/7001
rm -rf /usr/local/cluster/7002
rm -rf /usr/local/cluster/7003

mkdir -p /usr/local/cluster/7000
mkdir -p /usr/local/cluster/7001
mkdir -p /usr/local/cluster/7002
mkdir -p /usr/local/cluster/7003


cp ./src/redis-server  /usr/local/cluster/7000
cp ./redis.conf  /usr/local/cluster/7000
sed -i "s/6379/7000/g" /usr/local/cluster/7000/redis.conf

cp ./src/redis-server  /usr/local/cluster/7001
cp ./redis.conf  /usr/local/cluster/7001
sed -i "s/6379/7001/g" /usr/local/cluster/7001/redis.conf

cp ./src/redis-server  /usr/local/cluster/7002
cp ./redis.conf  /usr/local/cluster/7002
sed -i "s/6379/7002/g" /usr/local/cluster/7002/redis.conf

cp ./src/redis-server  /usr/local/cluster/7003
cp ./redis.conf  /usr/local/cluster/7003
sed -i "s/6379/7003/g" /usr/local/cluster/7003/redis.conf

echo "Start Server 0 at port 7000 >>>>>>>>>>>>>"
cd /usr/local/cluster/7000
./redis-server /usr/local/cluster/7000/redis.conf &
sleep 1

cd /usr/local/cluster/7001
./redis-server /usr/local/cluster/7001/redis.conf &
echo "Start Server 1 at port 7001 >>>>>>>>>>>>>"
sleep 1

cd /usr/local/cluster/7002
./redis-server /usr/local/cluster/7002/redis.conf &
echo "Start Server 2 at port 7002 >>>>>>>>>>>>>"
sleep 1

cd /usr/local/cluster/7003
./redis-server /usr/local/cluster/7003/redis.conf &
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

#redis-trib.rb  create --replicas 1 127.0.0.1:7000 127.0.0.1:7001 127.0.0.1:7002 127.0.0.1:7003
