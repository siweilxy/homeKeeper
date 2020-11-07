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
#include "hmLog.hpp"
#include "EmailSender.hpp"
#include "curl.hpp"
#include "tblEmailInfo.hpp"

#define USERNAME "siweilxy@163.com"
#define PASSWORD "HGIENKTKTIOBXVCV"
#define SMTPSERVER "smtp.163.com"
#define SMTPPORT ":25"
#define RECIPIENT "<251826184@qq.com>"
#define MAILFROM "<siweilxy@163.com>"

int main ()
{
    int ret = 0;
    std::string resOld = "old";
    std::string resNew = "new";
    std::string userName = USERNAME;
    std::string passowrd = PASSWORD;
    std::string smtpServer = SMTPSERVER;
    std::string recipient = RECIPIENT;
    std::string mailFrom = MAILFROM;
	ret = hmLog::getInstance().init(0);
    if (ret != 0)
    {
        printf ("log init error");
    }

    while (1)
    {
        sleep (3);
            tblEmailInfo emailInfo;
            ret = emailInfo.init ();
            if(ret != SUCCESS)
            {
                ERROR("emailInfo.init () failed");
                return 0;
            }

            auto res = emailInfo.getRes();
            for(auto iter:res)
            {
                EmailSender sendMail;
                sendMail.SetSmtpServer (iter.userName, iter.passwd, iter.smtpServer);
                sendMail.SetSendName ( iter.mailFrom);
                sendMail.SetSendMail ( iter.mailFrom);
                sendMail.AddRecvMail ( iter.recipient);
                sendMail.SetSubject ("ip changed");
                sendMail.SetBodyContent (resNew);
                //sendMail.AddAttachment("/home/siwei/github/homeKeeper/build/Makefile");
                sendMail.SendMail ();
            }
            resOld = resNew;

    }
    return 0;

}
