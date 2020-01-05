/*
 * common.cpp
 *
 *  Created on: Sep 13, 2019
 *      Author: siwei
 */

#include "common.h"
#include "log.h"
#include "logging.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <sys/stat.h>

int getfile(const std::string& path, std::string& result)
{
    char buffer[256];
    std::ifstream in(path);
    if (!in.is_open())
    {
       // LOG(ERROR)<<"Error opening file:"<<path;
        return -1;
    }
    while (!in.eof())
    {
        in.getline(buffer, 10000);
        result = result + buffer;
    }

    //LOG(INFO)<<"result is "<<result.c_str();
    return 0;
}

namespace U
{
pipe::pipe(std::string fifoName)
{
    log::getInstance().init();
    fifo = fifoName;
    fd = open(fifoName.c_str(), O_RDWR);
    char logbuf[512]={0};
    if (fd < 0)
    {
        snprintf(logbuf,sizeof(logbuf),"open fd error,errno is %s,errno is %d", strerror(errno), errno);
        LOG(ERROR)<<logbuf;
        if (errno == 2)
        {
            LOG(ERROR)<<fifoName.c_str()<<" is not existed,created it";
            int ret = mkfifo(fifoName.c_str(), S_IFIFO | 0666);
            if (ret == -1)
            {
                LOG(ERROR)<<"mkfifo return -1,"<< fifoName.c_str()<<" created failed";
            } else
            {
                LOG(INFO)<<"mkfifo """<<fifoName.c_str()<<" successed";
                fd = open(fifoName.c_str(), O_RDWR);
                owner = 1;
            }
        }
    }
}

pipe::~pipe()
{
    close(fd);
    if (owner == 1)
    {
        LOG(INFO)<<fifo.c_str()<<" created by this,unlink it";
        unlink(fifo.c_str());
    }
    LOG(INFO)<<fifo.c_str()<<" pipe end success";
}

int pipe::senMsg(char* buf,int length)
{
    int len = write(fd, buf, length);
    if (len < 0)
    {
        LOG(ERROR)<<"write error,errno is "<<strerror(errno);
        return -1;
    }
    return 0;
}

int pipe::recvMsg(char* buff,int len)
{
    return read(fd, buff, len);
}

int pipe::getFd()
{
    return fd;
}
}

