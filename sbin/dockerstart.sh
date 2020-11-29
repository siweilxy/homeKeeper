docker run -d -p 3306:3306 -v /root/work/mysql/data:/var/lib/mysql -e MYSQL_ROOT_PASSWORD="root" mysql
docker run -p 6379:6379 --name redis  -v /home/siwei/redis/data:/data  -d redis --appendonly yes --requirepass "789"
docker run -p 2181:2181 -p 9092:9092 -dit spotify/kafka:latest

