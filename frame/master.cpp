/*
 * master.cpp
 *
 *  Created on: Sep 10, 2019
 *      Author: siwei
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <errno.h>
#include <map>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <wait.h>
#include <pthread.h>
#include <signal.h>
#include "json.hpp"
#include "common.h"
#include "logging.h"
#include "log.h"
using json = nlohmann::json;

std::map<pid_t, fd_t> pids;
int fd = 0;

char* cfgPath = nullptr;
char* binPath = nullptr;
pthread_t scanThread_t;
int button = 0;

static pthread_mutex_t scanMutex = PTHREAD_MUTEX_INITIALIZER;
char logbuf[512] =
{ 0 };
void * scanThread(void* param)
{
    int status = 0;
    while (1)
    {
        sleep(4);
        pthread_mutex_lock(&scanMutex);
        for (auto pid : pids)
        {
            int ret = kill(pid.first, 0);
            if (ret == -1)
            {
                memset(logbuf, 0, sizeof(logbuf));
                snprintf(logbuf, sizeof(logbuf),
                        "kill %d ret is %d,errno is %d:%s", pid.first, ret,
                        errno, strerror(errno));
                LOG(ERROR)<<logbuf;
                pids.erase(pid.first);
            } else
            {
                ret = waitpid(pid.first, &status, WNOHANG);
                memset(logbuf, 0, sizeof(logbuf));
                snprintf(logbuf, sizeof(logbuf),
                        "file is %s pid is %d,ret is %d,status is %d",
                        pid.second.file.c_str(), pid.first, ret, status);
                LOG(ERROR)<<logbuf;
            }
        }
        pthread_mutex_unlock(&scanMutex);
    }
    return nullptr;
}

int runWorker(std::string file, std::string number)
{
    std::string b = "/";
    auto total_file = binPath + b + file;
    int ret = execl(total_file.c_str(), total_file.c_str(), "1", NULL);
    if (ret == -1)
    {
        memset(logbuf, 0, sizeof(logbuf));
        snprintf(logbuf, sizeof(logbuf), "execl 失败,errno 为 %s,file 为 %s",
                strerror(errno), total_file.c_str());
        LOG(ERROR)<<logbuf;
        exit(0);
    }
    return 0;
}

int runWorkers(json j)
{
    auto workers = j["worker"];
    auto number = j["workerNo"];
    int ret = 0;
    pid_t pid = 0;
    for (int i = 0; i < number; i++)
    {
        std::string file = workers[i]["file"];
        std::string number = workers[i]["number"];
        LOG(INFO)<<"file is "<<file.c_str();
        LOG(INFO)<<"number is "<<number.c_str();
        pid = fork();
        if (pid == 0)
        {
            runWorker(file, number);
            LOG(INFO)<<"work exit";
            exit(0);
        } else
        {
            pids[pid]=
            {   0,file};
        }
    }

    pthread_create(&scanThread_t, nullptr, scanThread, nullptr);

    LOG(INFO)<<"worker 启动成功";
    return 0;
}

int initParams()
{
    cfgPath = getenv("cfg_path");
    if (cfgPath == nullptr)
    {
        LOG(ERROR)<<"cfgPath is nullptr";
        printf("cfgPath is nullptr\n");
        return -1;
    }
    printf("cfgPath is %s\n",cfgPath);
    LOG(INFO)<<"cfgPath is "<<cfgPath;

    binPath = getenv("BIN_PATH");
    if (binPath == nullptr)
    {
        LOG(ERROR)<<"binPath is nullptr";
        printf("binPath is nullptr\n");
        return -1;
    }
    printf("binPath is %s\n",binPath);
    LOG(INFO)<<"binPath is "<<binPath;

    return 0;
}

void cleanSources()
{
    LOG(INFO)<<__FUNCTION__<<" 开始";
    int status = 0;
    pthread_mutex_lock(&scanMutex);
    for (auto pid : pids)
    {
        int ret = kill(pid.first, 9);
        if (ret == -1)
        {
            memset(logbuf,0,sizeof(logbuf));
            snprintf(logbuf,sizeof(logbuf),"kill %d ret is %d,errno is %d:%s", pid.first, ret, errno,
                    strerror(errno));
            LOG(ERROR)<<logbuf;
        } else
        {

            ret = waitpid(pid.first, &status, 0);
            memset(logbuf,0,sizeof(logbuf));
            snprintf(logbuf,sizeof(logbuf),"file is %s pid is %d,ret is %d,status is %d",
                    pid.second.file.c_str(), pid.first, ret, status);
            LOG(INFO)<<logbuf;
        }
    }
    pthread_mutex_unlock(&scanMutex);

    close(fd);
    unlink(FIFO_NAME); //删除管道文件
    LOG(INFO)<<__FUNCTION__<<" 结束";
}

void cleanSource(std::string file)
{
    LOG(INFO)<<__FUNCTION__<<" 开始";
    int status = 0;
    pthread_mutex_lock(&scanMutex);
    for (auto pid : pids)
    {
        if(pid.second.file == file)
        {
            int ret = kill(pid.first, 9);
            if (ret == -1)
            {
                memset(logbuf,0,sizeof(logbuf));
                snprintf(logbuf,sizeof(logbuf),"kill %d ret is %d,errno is %d:%s", pid.first, ret, errno,
                        strerror(errno));
                LOG(ERROR)<<logbuf;
            } else
            {
                ret = waitpid(pid.first, &status, 0);
                memset(logbuf,0,sizeof(logbuf));
                snprintf(logbuf,sizeof(logbuf),"file is %s pid is %d,ret is %d,status is %d",
                        pid.second.file.c_str(), pid.first, ret, status);
                LOG(INFO)<<logbuf;
            }
        }
    }
    pthread_mutex_unlock(&scanMutex);

    LOG(INFO)<<__FUNCTION__<<" 结束";
}

int main()
{
    std::string result;
    int ret = 0;
    int len = 0;
    int size = 0;
    int status = 0;
    int epfd, event_cnt;
    struct epoll_event* ep_events;
    struct epoll_event event;
    char buf[10240] =
    { 0 };

    ret = log::getInstance().init();
    if (ret != 0)
    {
        printf("log init error");
    }

    internal_msg_t* msg;
    ret = initParams();
    if (ret != 0)
    {
        LOG(ERROR)<<"initParams failed";
        return -1;
    }

    ret = getfile(cfgPath, result);
    if (ret != 0)
    {
        LOG(ERROR)<<"read file error";
        return 1;
    }

    LOG(INFO)<<"getfile success";

    json j = json::parse(result);
    ret = runWorkers(j);

    LOG(INFO)<<"启动成功";

    U::pipe p(FIFO_NAME);
    fd = p.getFd();

    epfd = epoll_create(50);
    ep_events = (epoll_event*) malloc(sizeof(struct epoll_event) * 50);
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);

    while (button == 0)
    {
        event_cnt = epoll_wait(epfd, ep_events, 50, -1);
        if (event_cnt == -1)
        {
            printf("epoll_wait error\n");
            break;
        }

        for (int i = 0; i < event_cnt; i++)
        {
            while (button == 0)
            {
                memset(buf, 0, sizeof(buf));
                len = read(ep_events[i].data.fd, buf, sizeof(buf));
                if (len == 0)
                {
                    printf("len == 0!!!!!!!!!!\n");
                } else if (len < 0)
                {
                    if (errno == EAGAIN)
                    {
                        //printf("read EAGAIN,break\n");
                        break;
                    }
                } else
                {
                    printf("收到消息:%s 长度:%d\n", buf, len);
                    msg = (internal_msg_t*) buf;
                    switch (msg->type)
                    {
                    case ul_log:
                    {
                        log::getInstance().setLevel(msg->msg);
                        if (ret != 0)
                        {
                            LOG(ERROR)<<"log set error";
                        }
                        break;
                    }
                    case ul_end:
                    {
                        button = 1;
                        LOG(ERROR)<<"程序退出";
                        break;
                    }
                    case ul_process_success:
                    {
                        break;
                    }case ul_end_one:
                    {
                        cleanSource(msg->msg);
                        break;
                    }
                    default:
                    {
                        LOG(ERROR)<<"error msg type:", msg->type;
                    }
                }

            }
        }
    }
}

    cleanSources();
    return 0;
}
