/*
 * keeper.cpp
 *
 *  Created on: Jan 2, 2020
 *      Author: siwei
 */

#include "keeper.h"
#include "log.h"
#include <stdio.h>

int keeper::start()
{
    int ret = 0;
    ret = log::getInstance().init();
    if(ret != 0)
    {
        printf("log init error");
    }
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

