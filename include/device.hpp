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

	int tcpServer(void* para)
	{
		INFO("tcp server");
		return 0;
	}

	int tcpClient(void* para)
	{
		INFO("tcp client");
		return 0;
	}

	int udpServer(void* para)
	{
		INFO("udp server");
		return 0;
	}

	int udpServerBroadcast(void* para)
	{
		INFO("udp Server broadcast");
		return 0;
	}

	int udpClient(void* para)
	{
		INFO("udp client");
		return 0;
	}

	std::map<int ,std::function<int(void*)>> funcs;
	int type;
public:
	device (int type,int socket):type(type)
	{
	}

	int init()
	{
		funcs[TCP_SERVER] = this->tcpServer;
		funcs[TCP_CLIENT] = this->tcpClient;
		funcs[UDP_SERVER] = this->udpServer;
		funcs[UDP_SERVER_BROADCAST] = this->udpServerBroadcast;
		funcs[UDP_CLIENT] = this->udpClient;

		if(type >= funcs.size())
		{
			ERROR("TYPE ERROR");
			return -1;
		}

		return funcs[type]();;
	}
};



#endif /* INCLUDE_DEVICE_HPP_ */
