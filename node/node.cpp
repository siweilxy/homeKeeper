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
	device d(UDP_SERVER_BROADCAST,0);
	ret = d.init();
	if(ret != 0)
	{
		ERROR("d init failed");
	}

	return 0;
}

