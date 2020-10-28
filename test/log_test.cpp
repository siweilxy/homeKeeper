/*
 * test.cpp
 *
 *  Created on: 2020年10月28日
 *      Author: siwei
 */

#include <hmLog.hpp>


void* test(void* para)
{
	hmLog::getInstance().init(4);
	INFO("test");
	INFO("%s %c %d ","a",'c',1);
	return nullptr;
}

int main()
{
	INFO("test");
	INFO("%s %c %d ","a",'c',1);
	pthread_t p;
	pthread_create(&p,nullptr,test,nullptr);
	pthread_join(p,nullptr);
	return 0;
}


