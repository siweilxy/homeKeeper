/*
 * raft.cpp
 *
 *  Created on: 7 Jun 2020
 *      Author: siwei
 */
#include <unistd.h>
#include <stdio.h>
#include "hmLog.hpp"

int main()
{
	hmLog::getInstance().init(0);
	INFO("node 启动");
	return 0;
}

