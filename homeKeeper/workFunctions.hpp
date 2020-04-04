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
#include "EmailSender.hpp"
#include "tblEmailInfo.hpp"
#include "file.hpp"

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

std::string ip = "";
int sendFlag = 0;

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
            ip = resOld;
            sendFlag = 1;
        }
    }
    return nullptr;
}

void* sendEmail (void *para)
{
    tblEmailInfo emailInfo;
    int ret = emailInfo.init ();
    if(ret != SUCCESS)
    {
        LOG(ERROR)<<"emailInfo.init () failed";
        return nullptr;
    }
    while (1)
    {
        sleep (1);
        if(sendFlag == 1)
        {
            LOG(INFO) << "ip is " << ip;
            sendFlag = 0;

            auto res = emailInfo.getRes();
            for(auto iter:res)
            {
                EmailSender sendMail;
                sendMail.SetSmtpServer (iter.userName, iter.passwd, iter.smtpServer);
                sendMail.SetSendName ( iter.mailFrom);
                sendMail.SetSendMail ( iter.mailFrom);
                sendMail.AddRecvMail ( iter.recipient);
                sendMail.SetSubject ("ip changed");
                sendMail.SetBodyContent (ip);
                //sendMail.AddAttachment("/home/siwei/github/homeKeeper/build/Makefile");
                sendMail.SendMail ();
            }

        }
    }
    return nullptr;
}

std::vector<std::function<void* (void*)>> funcArray = { getIp ,sendEmail};

#endif /* HOMEKEEPER_WORKFUNCTIONS_HPP_ */
