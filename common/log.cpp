/*
 * log.cpp
 *
 *  Created on: Jan 2, 2020
 *      Author: siwei
 */

#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "log.h"
#include "logging.h"

log::log()
{
    // TODO Auto-generated constructor stub

}

log::~log()
{
    // TODO Auto-generated destructor stub
}

log& log::getInstance()
{
    static log instance;
    return instance;
}

void log::setLevel(char* msg)
{
    if (strcmp(msg, "INFO") == 0)
    {
        FLAGS_stderrthreshold = 0;
        LOG(INFO)<<"INFO";
    } else if (strcmp(msg, "WARN") == 0)
    {
        FLAGS_stderrthreshold = 1;
        LOG(WARNING)<<"WARN";
    } else if (strcmp(msg, "ERROR") == 0)
    {
        FLAGS_stderrthreshold = 2;
        LOG(ERROR)<<"ERROR";
    } else if (strcmp(msg, "FATAL") == 0)
    {
        FLAGS_stderrthreshold = 3;
        LOG(FATAL)<<"INFO";
    } else
    {
        LOG(ERROR)<<"错误消息:"<<msg;
    }
}

int log::init(int stderrFlag)
{
    if(flag == 1)
    {
        LOG(ERROR)<<"log had inited";
        return 0;
    }
    char* name = nullptr;
    char* plevel = nullptr;
    int level = 0;
    name = getenv("PUBSUB_LOG_PATH");
    if (name == nullptr)
    {
        printf("PUBSUB_LOG_PATH 没有设置");
        return -1;
    }

    plevel = getenv("LOG_LEVEL");
    if (plevel == nullptr)
    {
        level = 0;
        printf("LOG_LEVEL 没有设置,使用默认设置INFO");
    } else
    {
        level = atoi(plevel);
    }
    FLAGS_stderrthreshold = level;
    FLAGS_logbufsecs = 0; //立即写入

    FLAGS_logtostderr = false;  //是否打印到控制台
    FLAGS_alsologtostderr = false;  //打印到日志同时是否打印到控制台

//    if(stderrFlag == 1)
//    {
//        FLAGS_stderrthreshold = 3;
//    }else
//    {
//        FLAGS_stderrthreshold = 0;
//    }

    FLAGS_colorlogtostderr = true; //标准输出带颜色

    std::string info_log = name;
    google::InitGoogleLogging(name);
    google::SetLogDestination(google::INFO, (info_log + "info-").c_str());
    google::SetLogDestination(google::WARNING, (info_log + "warn-").c_str());
    google::SetLogDestination(google::ERROR, (info_log + "error-").c_str());
    google::SetLogDestination(google::FATAL, (info_log + "fatal-").c_str());

    LOG(INFO)<< "INFO log path is "<<(info_log + "info");
    LOG(WARNING)<< "WARNING log path is "<<(info_log + "warn");
    LOG(ERROR)<< "ERROR log path is "<<(info_log + "error");

    flag = 1;
    return 0;
}

