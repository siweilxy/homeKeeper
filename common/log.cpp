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

int log::init()
{
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

    google::InitGoogleLogging(name);
    FLAGS_stderrthreshold = level;
    FLAGS_logbufsecs = 0; //立即写入
    std::string info_log = name;
    google::SetLogDestination(google::INFO,
            (info_log + "info-").c_str());
    google::SetLogDestination(google::WARNING,
            (info_log + "warn-").c_str());
    google::SetLogDestination(google::ERROR,
            (info_log + "error-").c_str());
    google::SetLogDestination(google::FATAL,
            (info_log + "fatal-").c_str());

    LOG(INFO)<< "INFO log path is "<<(info_log + "info");
    LOG(WARNING)<< "WARNING log path is "<<(info_log + "warn");
    LOG(ERROR)<< "ERROR log path is "<<(info_log + "error");
    return 0;
}

