/*
 * keeper.cpp
 *
 *  Created on: Jan 2, 2020
 *      Author: siwei
 */

#include "keeper.h"
#include <stdio.h>
#include "threadPool.hpp"
#include "workFunctions.hpp"
#include "hmLog.hpp"

int keeper::start()
{
    int ret = 0;
	ret = hmLog::getInstance().init(0);
    if(ret != 0)
    {
        printf("log init error");
        return -1;
    }
    threadPool pool;

    INFO( "funcMap.size() is [%d]",funcMap.size());


    for(auto iter:funcMap)
    {
        pool.setFunction(iter.first,iter.second);
    }

    pool.start();
    return 0;
}

keeper::keeper()
{
    // TODO Auto-generated constructor stub

}

keeper::~keeper()
{
    // TODO Auto-generated destructor stub
}

