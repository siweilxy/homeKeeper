/*
 * hmLog.hpp
 *
 *  Created on: 2020年10月28日
 *      Author: siwei
 */

#ifndef INCLUDE_HMLOG_HPP_
#define INCLUDE_HMLOG_HPP_

#include <stdio.h>
#include <vector>
#include <pthread.h>
#include <memory>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define INFO(...) hmLog::getInstance().insertLog(__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)

void* printLog(void* para);
typedef struct log_def
{
	int level;
	char time[255];
	char msg[1024];
}log_t;

class hmLog
{
public:
    static hmLog& getInstance()
    {
        static hmLog instance;
        return instance;
    }

	~hmLog()
	{
		printf("hmLog 结束\n");

		while(1)
		{
			lock();
			if(logs.empty() == false)
			{
				printf("logs:%ld\n",logs.size());
				log_signal();
				unlock();
			}else
			{
				unlock();
				break;
			}
		}
		pthread_cancel(printThread);
		pthread_join(printThread,nullptr);
	}

	std::vector<log_t> logs;
	int flag = 0;

	void lock()
	{
		pthread_mutex_lock(&logsMutex);
	}

	void unlock()
	{
		pthread_mutex_unlock(&logsMutex);
	}

	void log_wait()
	{
		pthread_cond_wait(&logCond,&logsMutex);
	}

	void insertLog(const char* fileName,const char* funcName,int line,const char* fmt,...)
	{
		log_t logIn;

	    time_t now ;
	    struct tm *tm_now;

        time(&now);
        tm_now = localtime(&now);


    	int d;
    	char *s;
    	char c;
    	char buf[40960] = {0};
    	char buffer[10];
    	va_list ap;
    	va_start(ap,fmt);

    	while(*fmt != '\0')
    	{
    		if(*fmt == '%')
    		{
    			fmt++;
    			if(*fmt == 's')
    			{
    				s = va_arg(ap,char *);
    				strcat(buf,s);
    			}
    			else if(*fmt == 'd')
    			{
    				d = va_arg(ap,int);
    				memset(buffer,0,10);
    				sprintf(buffer,"%d",d);
    				strcat(buf,buffer);
    			}
    		}
    		else
    		{
    			memset(buffer,0,10);
    			sprintf(buffer,"%c",*fmt);
    			strcat(buf,buffer);
    		}
    		fmt++;
    	}//end while

    	va_end(ap);

		snprintf(logIn.msg,sizeof(logIn.msg),":%s:%s:%d:%s",fileName,funcName,line,buf);
		snprintf(logIn.time,sizeof(logIn.time),"%d-%d-%d %d:%d:%d",
				tm_now->tm_year+1900,tm_now->tm_mon+1, tm_now->tm_mday,
				tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);

		lock();

		logs.push_back(logIn);

		log_signal();

		unlock();

	}

private:
	hmLog()
	{
		pthread_create(&printThread,nullptr,printLog,nullptr);
		printf("hmLog 启动完成\n");
	}

	void log_signal()
	{
		pthread_cond_signal(&logCond);
	}

	pthread_mutex_t logsMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t logCond = PTHREAD_COND_INITIALIZER;
	pthread_t printThread;
};

void* printLog(void* para)
{
	printf("print thread start\n");
	while(1)
	{
		pthread_testcancel();
		hmLog::getInstance().lock();
		hmLog::getInstance().log_wait();
		auto logsTemp = std::move(hmLog::getInstance().logs);

		for(auto log :logsTemp)
		{
			printf("%s:%s\n",log.time,log.msg);
		}

		hmLog::getInstance().unlock();
	}

}

#endif /* INCLUDE_HMLOG_HPP_ */
