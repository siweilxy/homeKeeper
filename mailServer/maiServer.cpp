/*
 * maiServer.cpp
 *
 *  Created on: Jan 18, 2020
 *      Author: siwei
 */

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include "file.hpp"
#include "log.h"
#include "EmailSender.hpp"
#include "curl.hpp"
#include "tblEmailInfo.hpp"

#define USERNAME "siweilxy@163.com"
#define PASSWORD "sd982597"
#define SMTPSERVER "smtp.163.com"
#define SMTPPORT ":25"
#define RECIPIENT "<251826184@qq.com>"
#define MAILFROM "<siweilxy@163.com>"

int main ()
{
    log::getInstance ().init (0);
    tblEmailInfo emailInfo;
    int ret = emailInfo.init ();
    if(ret != SUCCESS)
    {
        LOG(ERROR)<<"emailInfo.init () failed";
        return 0;
    }

    auto res = emailInfo.getRes();
    for(auto iter:res)
    {
        LOG(INFO)<<iter.mailFrom;
    }
    return 0;
    std::string resOld = "old";
    std::string resNew = "new";
    std::string userName = USERNAME;
    std::string passowrd = PASSWORD;
    std::string smtpServer = SMTPSERVER;
    std::string recipient = RECIPIENT;
    std::string mailFrom = MAILFROM;
    ret = log::getInstance ().init (0);
    if (ret != 0)
    {
        printf ("log init error");
    }

    while (1)
    {
        sleep (3);
        resNew = curlUtil ("icanhazip.com");
        if (resNew == resOld)
        {
            //LOG(INFO)<<"resNew == resOld";
        }
        else
        {
            LOG(INFO) << "resNew != resOld" << " resNew is " << resNew
                    << " resOld is " << resOld;

            EmailSender sendMail;
            sendMail.SetSmtpServer (USERNAME, PASSWORD, SMTPSERVER);
            sendMail.SetSendName ( MAILFROM);
            sendMail.SetSendMail ( MAILFROM);
            sendMail.AddRecvMail ( RECIPIENT);
            sendMail.SetSubject ("ip changed");
            sendMail.SetBodyContent (resNew);
            //sendMail.AddAttachment("/home/siwei/github/homeKeeper/build/Makefile");
            sendMail.SendMail ();
            resOld = resNew;
        }
    }
    return 0;

}
