/*
 * node.hpp
 *
 *  Created on: 2020年11月26日
 *      Author: siwei
 */

#ifndef INCLUDE_NODE_HPP_
#define INCLUDE_NODE_HPP_

#include <string>
#include <unistd.h>
#include "common.h"
#include "hmLog.hpp"
class node
{
public:
	int init()
	{
	    int ret = 0;
		ret = hmLog::getInstance().init(0);
	    if(ret != 0)
	    {
	        printf("log init error");
	        return FAILED;
	    }

		char strProcessPath[1024] = {0};
		if(readlink("/proc/self/exe", strProcessPath,1024) <=0)
		{
			ERROR("get NodeName Failed");
			return FAILED;
		}
		char *strProcessName = strrchr(strProcessPath, '/');
		nodeName = strProcessName + 1;

		INFO("nodeName [%s]",nodeName.c_str());

		return SUCCESS;
	}

	std::string getNodeName()
	{
		return nodeName;
	}

private:
	std::string nodeName;
};


#endif /* INCLUDE_NODE_HPP_ */
