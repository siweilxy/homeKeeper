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
#include <boost/asio.hpp>

#include "EmailSender.hpp"
#include "curl.hpp"

#define USERNAME "siweilxy@163.com"
#define PASSWORD "sd982597"
#define SMTPSERVER "smtp.163.com"
#define SMTPPORT ":25"
#define RECIPIENT "<251826184@qq.com>"
#define MAILFROM "<siweilxy@163.com>"

int
main ()
{
    std::string resOld="old";
    std::string resNew="new";
    while (1)
    {
        sleep(3);
        resNew = curlUtil ("icanhazip.com");
        if (resNew == resOld)
        {
            std::cout << "resNew == resOld" << std::endl;
        }
        else
        {
            std::cout << "resNew != resOld" <<" resNew is "<<resNew <<" resOld is "<<resOld<< std::endl;

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
