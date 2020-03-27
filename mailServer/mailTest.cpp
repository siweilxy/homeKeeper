/*
 * mailTest.cpp
 *
 *  Created on: 12 Mar 2020
 *      Author: siwei
 */

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
#define PASSWORD "HGIENKTKTIOBXVCV"
#define SMTPSERVER "smtp.163.com"
#define SMTPPORT ":25"
#define RECIPIENT "<251826184@qq.com>"
#define MAILFROM "<siweilxy@163.com>"

int main ()
{
    log::getInstance ().init (0);
    int ret = 0;
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

    resNew = curlUtil ("icanhazip.com");
    printf(" resNew is %s\n",resNew.c_str());

    EmailSender sendMail;
    sendMail.SetSmtpServer (USERNAME, PASSWORD, SMTPSERVER);
    sendMail.SetSendName (MAILFROM);
    sendMail.SetSendMail (MAILFROM);
    sendMail.AddRecvMail (RECIPIENT);
    sendMail.SetSubject ("ip changed");
    sendMail.SetBodyContent (resNew);
    sendMail.AddAttachment ("/home/siwei/work/github/homeKeeper/CMakeLists.txt");
    sendMail.SendMail ();

    return 0;

}

