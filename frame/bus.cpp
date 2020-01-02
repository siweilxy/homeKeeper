
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
    log::getInstance().init();
    LOG(INFO)<<"bus启动";
    processFunc f = bus;
    worker w(f,f,f);

    return 0;
}
