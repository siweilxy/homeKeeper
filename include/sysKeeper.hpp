/*
 * sysKeeper.hpp
 *
 *  Created on: 2020年12月7日
 *      Author: siwei
 */

#ifndef INCLUDE_SYSKEEPER_HPP_
#define INCLUDE_SYSKEEPER_HPP_

#include <vector>
#include <string>
#include <pthread.h>
#include "hmLog.hpp"
class NetWork
{

};

class Disk
{

};

class Cpu
{

};

class Mem
{

};

//监控系统信息以及对系统参数等进行修改
class SysKeeper
{
public:

private:
	pthread_t scanThread;
	NetWork net;
	Disk disk;
	Cpu cpu;
	Mem mem;
	std::string hostId;
};


#endif /* INCLUDE_SYSKEEPER_HPP_ */
