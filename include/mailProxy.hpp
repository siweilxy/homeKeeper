/*
 * mailProxy.hpp
 *
 *  Created on: 2020年12月7日
 *      Author: siwei
 */

#ifndef INCLUDE_MAILPROXY_HPP_
#define INCLUDE_MAILPROXY_HPP_

#include <pthread.h>
#include "hmLog.hpp"
#include "threadPool.hpp"
class mailProxy
{
public:
	int start()
	{
	    int ret = 0;
		ret = hmLog::getInstance().init(0);
	    if(ret != 0)
	    {
	        printf("log init error");
	        return -1;
	    }
	    threadPool pool;
	    pool.setFunction("mailProxy",mailProxy);
	    pool.start();
		return 0;
	}
private:
	pthread_t mailThread;
    threadPool pool;
    void* mailProxy(void* para)
    {
        while (1)
        {
            tblEmailInfo emailInfo;
            int ret = emailInfo.init ();
            if (ret != SUCCESS)
            {
                ERROR("emailInfo.init () failed");
                continue;
            }

            while (1)
            {
                sleep (5);
                bool result = true;
                auto res = emailInfo.getRes ();
                for (auto iter : res)
                {
                    EmailSender sendMail;
                    sendMail.SetSmtpServer (iter.userName, iter.passwd,iter.smtpServer);
                    sendMail.SetSendName (iter.mailFrom);
                    sendMail.SetSendMail (iter.mailFrom);
                    sendMail.AddRecvMail (iter.recipient);
                    ERROR("[%s] sended", iter.recipient.c_str());
                    sendMail.SetSubject ("");
                    sendMail.SetBodyContent (infos[0].ip + " url is " + infos[0].ip + ":19870");
                    result = sendMail.SendMail ();
                    if(result != true)
                    {
                        ERROR("send mail failed");
                    }
                }
            }
        }
        return nullptr;
    };
};



#endif /* INCLUDE_MAILPROXY_HPP_ */
