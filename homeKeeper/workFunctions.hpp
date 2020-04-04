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
#include "tblIpInfo.hpp"

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
    while (1)
    {
        sleep (2);
        resNew = curlUtil ("icanhazip.com");
        tblIpInfo ipInfo;
        ipInfo.init();
        auto info = ipInfo.getRes();
        if(info.empty() || info[0].ip != resNew)
        {
            ipInfo.insertToDb(resNew);
            LOG(WARNING)<<"ip insert:"<<resNew<<"!";
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
        sleep (5);
        tblIpInfo ipInfo;
        ipInfo.init();
        auto info = ipInfo.getRes();
        bool ret = true;
        if(info.empty())
        {
            continue;
        }else
        {
            if(info[0].send_flag == "0")
            {
                LOG(INFO) << "ip is " << info[0].ip;

                auto res = emailInfo.getRes();
                for(auto iter:res)
                {
                    EmailSender sendMail;
                    sendMail.SetSmtpServer (iter.userName, iter.passwd, iter.smtpServer);
                    sendMail.SetSendName ( iter.mailFrom);
                    sendMail.SetSendMail ( iter.mailFrom);
                    sendMail.AddRecvMail ( iter.recipient);
                    sendMail.SetSubject ("ip changed");
                    sendMail.SetBodyContent (info[0].ip);
                    //sendMail.AddAttachment("/home/siwei/github/homeKeeper/build/Makefile");
                    ret = sendMail.SendMail ()&ret;
                }

                if(ret)
                {
                    ipInfo.updateToDb(info[0].ip);
                    LOG(WARNING) << "update " << info[0].ip<<" send_flag to 1";
                }

            }
        }
    }
    return nullptr;
}

std::vector<std::function<void* (void*)>> funcArray = { getIp ,sendEmail};

#endif /* HOMEKEEPER_WORKFUNCTIONS_HPP_ */
