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
#include <map>

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

void* getIp (void *para)
{
    std::string resNew = "new";
    int ret = 0;
    std::vector<ipInfo_t> infos;
    while (1)
    {
        tblIpInfo ipInfo;
        ret = ipInfo.init ();
        if (ret != SUCCESS)
        {
            LOG(ERROR) << "init failed";
            continue;
        }
        while (1)
        {
            sleep (2);
            resNew = curlUtil ("icanhazip.com");
            if(resNew == "failed")
            {
                LOG(ERROR) << "curlUtil failed";
                continue;
            }
            infos.clear ();
            auto ret = ipInfo.getRes (infos);
            if (ret != SUCCESS)
            {
                LOG(ERROR) << "ipInfo.getRes failed";
                continue;
            }

            if (infos.empty () || infos[0].ip != resNew)
            {
                ipInfo.insertToDb (resNew);
                LOG(ERROR) << "ip insert:" << resNew << "!";
            }
        }
    }
    return nullptr;
}

void* sendEmail (void *para)
{
    std::vector<ipInfo_t> infos;

    while (1)
    {
        tblEmailInfo emailInfo;
        int ret = emailInfo.init ();
        if (ret != SUCCESS)
        {
            LOG(ERROR) << "emailInfo.init () failed";
            continue;
        }

        tblIpInfo ipInfo;
        ret = ipInfo.init ();
        if (ret != SUCCESS)
        {
            LOG(ERROR) << "ipInfo.init () failed";
            continue;
        }

        while (1)
        {
            sleep (5);
            infos.clear ();
            auto ret = ipInfo.getRes (infos);
            bool result = true;
            if (infos.empty ())
            {
                continue;
            }
            else if (infos[0].send_flag == "0")
            {
                LOG(INFO) << "ip is " << infos[0].ip;

                auto res = emailInfo.getRes ();
                for (auto iter : res)
                {
                    EmailSender sendMail;
                    sendMail.SetSmtpServer (iter.userName, iter.passwd,
                                            iter.smtpServer);
                    sendMail.SetSendName (iter.mailFrom);
                    sendMail.SetSendMail (iter.mailFrom);
                    sendMail.AddRecvMail (iter.recipient);
                    sendMail.SetSubject ("ip changed");
                    sendMail.SetBodyContent (infos[0].ip + " url is " + infos[0].ip + ":19870");
                    //sendMail.AddAttachment("/home/siwei/github/homeKeeper/build/Makefile");
                    result = sendMail.SendMail () & result;
                }

                if (result)
                {
                    ipInfo.updateToDb (infos[0].rec_id);
                    LOG(WARNING) << "update " << infos[0].ip
                            << " send_flag to 1";
                }else
                {
                    break;
                }
            }
        }
    }
    return nullptr;
}

std::map<std::string, std::function<void* (void*)>> funcMap = {
        { "getIp", getIp }, { "sendEmail", sendEmail } };

#endif /* HOMEKEEPER_WORKFUNCTIONS_HPP_ */
