/*
 * test.cpp
 *
 *  Created on: 2020年10月28日
 *      Author: siwei
 */

#include <hmLog.hpp>


void* test(void* para)
{
	hmLog::getInstance().setLevel(4);
	FATAL("test");
	INFO("%s %c %d ","a",'c',1);
	return nullptr;
}

int main()
{
	hmLog::getInstance().setLevel(0);
	INFO("test");
	INFO("%s %c %d ","a",'c',1);
	sleep(2);
	pthread_t p;
	pthread_create(&p,nullptr,test,nullptr);
	pthread_join(p,nullptr);
	return 0;
}


