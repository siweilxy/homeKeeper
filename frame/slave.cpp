/*
 * slave.cpp
 *
 *  Created on: Sep 10, 2019
 *      Author: siwei
 */

#include "log.h"
#include "logging.h"
#include <unistd.h>
#include <stdio.h>
#include "node.hpp"

int main()
{
    int ret = log::getInstance().init();
    if(ret != 0)
    {
        printf("log init error");
    }
    LOG(INFO)<<"slave启动";
    while (1)
    {
        sleep(1);
        LOG(INFO)<<"slave";
        //break;
    }
    return 0;
}
