/*
 * slave.cpp
 *
 *  Created on: Sep 10, 2019
 *      Author: siwei
 */

#include "log.h"
#include "logging.h"
#include <unistd.h>
int main()
{
    LOG(INFO)<<"slave启动";
    while (1)
    {
        sleep(1);
        //break;
    }
    return 0;
}
