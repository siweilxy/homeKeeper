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

#define USERNAME "siweilxy@163.com"
#define PASSWORD "sd982597"
#define SMTPSERVER "smtp.163.com"
#define SMTPPORT ":25"
#define RECIPIENT "<251826184@qq.com>"
#define MAILFROM "<siweilxy@163.com>"

int main()
{
    EmailSender sendMail;
    sendMail.SetSmtpServer(USERNAME, PASSWORD, SMTPSERVER);
    sendMail.SetSendName( MAILFROM);
    sendMail.SetSendMail( MAILFROM);
    sendMail.AddRecvMail( RECIPIENT);
    sendMail.SetSubject("mail send test");
    sendMail.SetBodyContent("this is a test!");
    //sendMail.AddAttachment("/home/siwei/github/homeKeeper/build/Makefile");
    sendMail.SendMail();
}
