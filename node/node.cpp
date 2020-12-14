/*
 * raft.cpp
 *
 *  Created on: 7 Jun 2020
 *      Author: siwei
 */
#include <unistd.h>
#include <stdio.h>
#include "hmLog.hpp"
#include "device.hpp"

int main()
{
	int ret = 0;
	ret = hmLog::getInstance().init(0);
    if(ret != 0)
    {
        printf("log init error");
        return -1;
    }

	INFO("node 启动");
	device* d_s = new device(UDP_SERVER_BROADCAST,57950);
	ret = d_s->init();
	if(ret != 0)
	{
		ERROR("d_s init failed");
	}

	INFO("UDP_SERVER_BROADCAST SUCCESS");

	device* d_c = new device(UDP_CLIENT,57590);
	ret = d_c->init();
	if(ret != 0)
	{
		ERROR("d_c init failed");
	}
	INFO("UDP_CLIENT SUCCESS");

	delete d_s;
	delete d_c;

	return 0;
}

