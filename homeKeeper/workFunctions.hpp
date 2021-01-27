/*
 * workFunctions.hpp
 *
 *  Created on: 30 Mar 2020
 *      Author: siwei
 */

#ifndef HOMEKEEPER_WORKFUNCTIONS_HPP_
#define HOMEKEEPER_WORKFUNCTIONS_HPP_

#include "curl.hpp"
#include "EmailSender.hpp"
#include "tblEmailInfo.hpp"
#include "file.hpp"
#include "tblIpInfo.hpp"
#include "downloadfile.hpp"
#include <map>
#include "hmLog.hpp"

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
            ERROR("init failed");
            continue;
        }
        while (1)
        {
            sleep (2);
            resNew = curlUtil ("icanhazip.com");
            if (resNew == "failed")
            {
                ERROR("curlUtil failed");
                continue;
            }

            DEBUG("ip[%s]",resNew.c_str());

            infos.clear ();
            auto ret = ipInfo.getRes (infos);
            if (ret != SUCCESS)
            {
                ERROR("ipInfo.getRes failed");
                continue;
            }

            if (infos.empty () || infos[0].ip != resNew)
            {
                ipInfo.insertToDb (resNew);
                ERROR("ip insert:%s", resNew.c_str());
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
            ERROR("emailInfo.init () failed");
            continue;
        }

        tblIpInfo ipInfo;
        ret = ipInfo.init ();
        if (ret != SUCCESS)
        {
            ERROR("ipInfo.init () failed");
            continue;
        }

        while (1)
        {
            sleep (5);
            infos.clear ();
            auto ret = ipInfo.getRes (infos);
            if (ret == FAILED)
            {
                ERROR("ipInfo.getRes failed");
                continue;
            }
            bool result = true;
            if (infos.empty ())
            {
                continue;
            }
            else if (infos[0].send_flag == "0")
            {
                ERROR("ip is [%s]", infos[0].ip.c_str());

                auto res = emailInfo.getRes ();
                for (auto iter : res)
                {
                    EmailSender sendMail;
                    sendMail.SetSmtpServer (iter.userName, iter.passwd,
                                            iter.smtpServer);
                    sendMail.SetSendName (iter.mailFrom);
                    sendMail.SetSendMail (iter.mailFrom);
                    sendMail.AddRecvMail (iter.recipient);
                    ERROR("[%s] sended", iter.recipient.c_str());
                    sendMail.SetSubject ("ip changed");
                    sendMail.SetBodyContent (
                            infos[0].ip + " url is " + infos[0].ip + ":19870");
                    //sendMail.AddAttachment("/home/siwei/github/homeKeeper/build/Makefile");
                    result = sendMail.SendMail () | result;
                }

                if (result)
                {
                    ipInfo.updateToDb (infos[0].rec_id);
                    WARN( "update  [%s] send_flag to 1" , infos[0].ip.c_str());
                }
                else
                {
                    break;
                }
            }
        }
    }
    return nullptr;
}

void* sendFile (void *para)
{
    std::vector<downloadFile_t> downloadFiles;

    while (1)
    {
        tblEmailInfo emailInfo;
        int ret = emailInfo.init ();
        if (ret != SUCCESS)
        {
            ERROR("emailInfo.init () failed");
            continue;
        }
        downloadFile dlfile;
        ret = dlfile.init ();
        if (ret != SUCCESS)
        {
            ERROR("dlfile.init () failed");
            continue;
        }

        while (1)
        {
            sleep (5);

            auto ret = dlfile.getRes (downloadFiles);
            if (ret == FAILED)
            {
                ERROR( "dlfile.getRes () failed");
                continue;
            }

            for (auto dlFile : downloadFiles)
            {
                WARN ( "get [%s]" , dlFile.fileName.c_str());
            }

            bool result = true;
            auto res = emailInfo.getRes ();
            for (auto iter : res)
            {
                EmailSender sendMail;
                sendMail.SetSmtpServer (iter.userName, iter.passwd,
                                        iter.smtpServer);
                sendMail.SetSendName (iter.mailFrom);
                sendMail.SetSendMail (iter.mailFrom);
                sendMail.AddRecvMail (iter.recipient);
                sendMail.SetSubject (downloadFiles[0].fileName);
                sendMail.AddAttachment (downloadFiles[0].path);
                result = sendMail.SendMail () & result;
            }

            if (result)
            {
                dlfile.updateToDb (downloadFiles[0].fileName);
                WARN ( "update [%s] send_flag to 1" , downloadFiles[0].fileName.c_str());
            }
            else
            {
                break;
            }
        }
        sleep (3600 * 24);
    }
    return nullptr;
}

std::map<std::string, std::function<void* (void*)>> funcMap =
        {
                { "getIp", getIp },
                { "sendEmail", sendEmail }
                //{ "sendFile", sendFile }
        };

#endif /* HOMEKEEPER_WORKFUNCTIONS_HPP_ */
