/*
 * workFunctions.hpp
 *
 *  Created on: 30 Mar 2020
 *      Author: siwei
 */

#ifndef HOMEKEEPER_WORKFUNCTIONS_HPP_
#define HOMEKEEPER_WORKFUNCTIONS_HPP_

#include "log.h"
#include "curl.hpp"

#define USERNAME "siweilxy@163.com"
#define PASSWORD "HGIENKTKTIOBXVCV"
#define SMTPSERVER "smtp.163.com"
#define SMTPPORT ":25"
#define RECIPIENT "<251826184@qq.com>"
#define MAILFROM "<siweilxy@163.com>"

std::string userName = USERNAME;
std::string passowrd = PASSWORD;
std::string smtpServer = SMTPSERVER;
std::string recipient = RECIPIENT;
std::string mailFrom = MAILFROM;

void* test (void *para)
{
    while (1)
    {
        sleep (1);
        LOG(INFO) << "test";
    }
    return nullptr;
}

void* getIp (void *para)
{
    std::string resNew = "new";
    std::string resOld = "old";
    while (1)
    {
        sleep (1);
        resNew = curlUtil ("icanhazip.com");
        if (resNew != resOld)
        {
            LOG(INFO) << "resNew is " << resNew << " resOld" << resOld;
            resOld = resNew;
        }
    }
    return nullptr;
}

void* sendEmail (void *para)
{

    while (1)
    {
        sleep (1);
    }
    return nullptr;
}

std::vector<std::function<void* (void*)>> funcArray = { getIp };

#endif /* HOMEKEEPER_WORKFUNCTIONS_HPP_ */
