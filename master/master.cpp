/*
 * master.cpp
 *
 *  Created on: 2020年11月29日
 *      Author: siwei
 */

#include "hmLog.hpp"

int main()
{
    int ret = 0;
	ret = hmLog::getInstance().init(0);
    if(ret != 0)
    {
        printf("log init error");
        return -1;
    }

    ERROR("master started");
}
