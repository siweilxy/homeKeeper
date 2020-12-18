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
#include "common.h"

enum deviceType
{
	TCP_SERVER = 0,
	TCP_CLIENT,
	UDP_SERVER,
	UDP_SERVER_BROADCAST,
	UDP_CLIENT,
	UDP_CLIENT_BROADCAST
};

typedef struct device_info_s
{
    int type;
    int sock;
    char ip[30];
} device_info_t;

class device
{
private:

	std::vector<homeKeeper_t> msgs;

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
		INFO("udp Server broadcast");

		std::string buf = "test";

		device_info_t* info = (device_info_t*)para;

		int sock = info->sock;
		int sockfd;

		struct sockaddr_in remoteAddr;
		socklen_t addrlen = sizeof(remoteAddr);

		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		{
			ERROR("fail to socket error:[%s]",strerror(errno));
			return nullptr;
		}

		remoteAddr.sin_family = AF_INET;
		remoteAddr.sin_port = htons(sock);
		INFO("SEND SOCK IS [%d]",sock);
		int on = 1;

		if(info->type == UDP_SERVER_BROADCAST)
		{
			remoteAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
			INFO("开启广播模式");
			if(setsockopt(sockfd, SOL_SOCKET,SO_BROADCAST, &on, sizeof(on)) < 0)
			{
				ERROR("fail to setsockopt");
			}
		}else
		{
			remoteAddr.sin_addr.s_addr = inet_addr(info->ip);
		}

		while(1)
		{
			sleep(1);
			if(sendto(sockfd, buf.c_str(), buf.size(), 0,(struct sockaddr *)&remoteAddr, addrlen) < 0)
			{
				ERROR("fail to sendto error:[%s]",strerror(errno));
			}else
			{
				INFO("SEND SUCCESS");
			}
		}
		close(sockfd);
		return nullptr;

		return 0;
	}

	static void* udpClient(void* para)
	{
		INFO("udp client");
		device_info_t* info = (device_info_t*)para;
		int sock = info->sock;
		int sockfd;
		struct sockaddr_in selfAddr, addr;
		socklen_t addrlen = sizeof(selfAddr);
		char buff[10240]={0};
		//第一步:创建套接字

		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		{
			ERROR("fail to socket");
			return nullptr;
		}

		      //第二步:填充广播网络信息结构体
		      //inet_addr:将点分十进制ip地址转化为网络字节序的整型数据
		      //htons:将主机字节序转化为网络字节序
		      //atoi:将数字型字符串转化为整型数据
		selfAddr.sin_family = AF_INET;
		if(info->type == UDP_CLIENT_BROADCAST)
		{
			selfAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
		}else
		{
			selfAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		}
		selfAddr.sin_port = htons(sock);

		INFO("RECV SOCK IS [%d]",sock);

		//第三步:将套接字与服务器网络信息结构体绑定
		if(bind(sockfd, (struct sockaddr *)&selfAddr, addrlen) < 0)
		{
			ERROR("fail to bind [%s]",strerror(errno));
            return nullptr;
		}

		INFO("bind succss");

		ssize_t bytes;

		while(1)
		{
			if((bytes = recvfrom(sockfd, buff, 10240, 0,(struct sockaddr *)&addr, &addrlen)) < 0)
			{
				ERROR("fail to recvfrom");
			}
			else
			{
				INFO("ip: %s, port: %d broadcast:%s",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port),buff);
			}
		}

		close(sockfd);
		return nullptr;
	}

    int type;
    int sock;
    std::string ip;
	device_info_t info;
public:
	device (int type,int socket,std::string ip = ""):type(type),sock(socket),ip(ip)
	{
		info.type = type;
		info.sock = sock;
		snprintf(info.ip,sizeof(info.ip),"%s",ip.c_str());
	}

	~device()
	{
		INFO("~device");
	}

	int init()
	{
	    if(type == TCP_SERVER)
	    {
	    	INFO("TCP_SERVER");
			threadPool::setFunction("TCP_SERVER", this->tcpServer,(void*)&info,sizeof(info));
	    }else if(type == TCP_CLIENT)
	    {
	    	INFO("TCP_CLIENT");
			threadPool::setFunction("TCP_CLIENT", this->tcpClient,(void*)&sock,sizeof(info));
	    }else if(type == UDP_SERVER)
	    {
	    	INFO("UDP_SERVER");
			threadPool::setFunction("UDP_SERVER", this->udpServer,(void*)&sock,sizeof(info));
	    }
	    else if(type == UDP_SERVER_BROADCAST)
	    {
	    	INFO("UDP_SERVER_BROADCAST");
			threadPool::setFunction("UDP_SERVER_BROADCAST", this->udpServer,(void*)&info,sizeof(info));
	    }
	    else if(type == UDP_CLIENT)
	    {
	    	INFO("UDP_CLIENT");
			threadPool::setFunction("UDP_CLIENT", this->udpClient,(void*)&sock,sizeof(sock));
	    }else if(type == UDP_CLIENT_BROADCAST)
	    {
            INFO("UDP_CLIENT_BROADCAST");
            threadPool::setFunction("UDP_CLIENT_BROADCAST", this->udpClient,(void*)&sock,sizeof(sock));
	    }
	    else
	    {
	    	ERROR("TYPE ERROR [%d]",type);
	    }

		INFO("INIT END");
		return 0;
	}
};



#endif /* INCLUDE_DEVICE_HPP_ */
