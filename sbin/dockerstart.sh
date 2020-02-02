docker run -d -p 3306:3306 -v /home/siwei/mysql/data:/var/lib/mysql -e MYSQL_ROOT_PASSWORD="root" mysql
docker run -p 6379:6379 --name redis  -v /home/siwei/redis/data:/data  -d redis --appendonly yes --requirepass "789"
