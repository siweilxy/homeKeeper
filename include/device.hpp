/*
 * device.hpp
 *
 *  Created on: 2020年12月12日
 *      Author: siwei
 */

#ifndef INCLUDE_DEVICE_HPP_
#define INCLUDE_DEVICE_HPP_

#include "hmLog.hpp"
#include <map>
#include <functional>
#include <arpa/inet.h>
#include "threadPool.hpp"

enum deviceType
{
	TCP_SERVER = 0,
	TCP_CLIENT,
	UDP_SERVER,
	UDP_SERVER_BROADCAST,
	UDP_CLIENT
};

class device
{
private:
	static void* tcpServer(void* para)
	{
		INFO("tcp server");
		return 0;
	}

	static void* tcpClient(void* para)
	{
		INFO("tcp client");
		return 0;
	}

	static void* udpServer(void* para)
	{
		INFO("udp server");
		return 0;
	}

	static void* udpServerBroadcast(void* para)
	{
		INFO("udp Server broadcast");

		std::string buf = "test";

		struct sockaddr_in broadcastaddr;
		socklen_t addrlen = sizeof(broadcastaddr);

		if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		{
			ERROR("fail to socket error:[%s]",strerror(errno));
			return nullptr;
		}

		broadcastaddr.sin_family = AF_INET;
		broadcastaddr.sin_addr.s_addr = htonl (INADDR_ANY);
		broadcastaddr.sin_port = htons(sock);
		int on = 1;
		if(setsockopt(sock, SOL_SOCKET,SO_BROADCAST, &on, sizeof(on)) < 0)
		{
			ERROR("fail to setsockopt");
		}

		while(1)
		{
			sleep(1);
			if(sendto(sock, buf.c_str(), 10240, 0,(struct sockaddr *)&broadcastaddr, addrlen) < 0)
			{
				ERROR("fail to sendto");
			}
		}
		close(sock);
		return nullptr;

		return 0;
	}

	static void* udpClient(void* para)
	{
		INFO("udp client");

		int sockfd;
		struct sockaddr_in broadcastaddr, addr;
		socklen_t addrlen = sizeof(broadcastaddr);
		char buff[10240]={0};
		//第一步:创建套接字

		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		{
			ERROR("fail to socket");
		}

		      //第二步:填充广播网络信息结构体
		      //inet_addr:将点分十进制ip地址转化为网络字节序的整型数据
		      //htons:将主机字节序转化为网络字节序
		      //atoi:将数字型字符串转化为整型数据
		broadcastaddr.sin_family = AF_INET;
		broadcastaddr.sin_addr.s_addr = htonl (INADDR_ANY);
		broadcastaddr.sin_port = htons(sock);

		//第三步:将套接字与服务器网络信息结构体绑定
		if(bind(sockfd, (struct sockaddr *)&broadcastaddr, addrlen) < 0)
		{
			ERROR("fail to bind");
		}

		ssize_t bytes;
		while(1)
		{
			if((bytes = recvfrom(sockfd, buff, 10240, 0,(struct sockaddr *)&addr, &addrlen)) < 0)
			{
				ERROR("fail to recvfrom");
			}
			else
			{
				INFO("ip: %s, port: %d\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
		        INFO("broadcast : %s\n", buff);
			}
		}

		close(sockfd);
		return nullptr;

		return 0;
	}

	int type;
	static int sock;
public:
	device (int type,int socket):type(type),sock(socket)
	{
	}

	~device()
	{
	}

	int init()
	{
	    threadPool pool;

	    if(type == TCP_SERVER)
	    {
			threadPool::setFunction("TCP_SERVER", this->tcpServer);
	    }

	    if(type == TCP_CLIENT)
	    {
			threadPool::setFunction("TCP_CLIENT", this->tcpClient);
	    }

	    if(type == UDP_SERVER)
	    {
			threadPool::setFunction("UDP_SERVER", this->udpServer);
	    }

	    if(type == UDP_SERVER_BROADCAST)
	    {
			threadPool::setFunction("UDP_SERVER_BROADCAST", this->udpServerBroadcast);
	    }

	    if(type == UDP_CLIENT)
	    {
			threadPool::setFunction("UDP_CLIENT", this->udpClient);
	    }

		pool.start();
		return 0;
	}
};



#endif /* INCLUDE_DEVICE_HPP_ */
