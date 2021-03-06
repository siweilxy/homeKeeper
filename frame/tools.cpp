/*
 * tools.cpp
 *
 *  Created on: Sep 13, 2019
 *      Author: siwei
 */

#include "common.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "log.h"
#include "logging.h"
#include "common.h"

char logbuf[512]={0};

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printf("argc < 3,like this : tools msgType destName msg");
        return 0;
    }
    int ret = log::getInstance().init();
    if(ret != 0)
    {
        printf("log init error");
    }

    for (int i = 0; i < argc; i++)
    {
        snprintf(logbuf,sizeof(logbuf),"argc[%d] : %s", i, argv[i]);
        LOG(INFO)<<logbuf;
    }

    internal_msg_t msg;
    msg.type = (msgType)atoi(argv[1]);
    snprintf(msg.destName,sizeof(msg.destName),"%s",argv[2]);
    snprintf(msg.msg,sizeof(msg.msg),"%s",argv[3]);
    snprintf(msg.srcName,sizeof(msg.srcName),"%s","tools");

    int fd = 0;
    char buf[10240] =
    { 0 };
    int len = 0;
    memcpy(buf, &msg,sizeof(msg));
    U::pipe p(FIFO_NAME);

    p.senMsg(buf,sizeof(buf));

    close(fd);
}
