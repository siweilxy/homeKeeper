
#include "log.h"

#include <stdio.h>
#include <unistd.h>

#include "worker.h"
#include "common.h"
#include "logging.h"
#include "log.h"
void bus()
{
    while (1)
    {
        sleep(1);
        LOG(INFO) << "bus 运行中";
        //break;
    }
}

int main()
{
    int ret = log::getInstance().init();
    if(ret != 0)
    {
        printf("log init error");
    }    LOG(INFO)<<"bus启动";
    processFunc f = bus;
    worker w(f,f,f);

    return 0;
}
