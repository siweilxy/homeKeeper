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
		insertLog("hmLog 结束\n");
		pthread_join(printThread,nullptr);
		printf("hmLog 结束\n");
	}

	std::vector<log_t> logs;

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

private:
	hmLog()
	{
		pthread_create(&printThread,nullptr,printLog,nullptr);
		insertLog("hmLog 启动\n");
		printf("hmLog 启动\n");

	}

	void log_signal()
	{
		pthread_cond_signal(&logCond);
	}

	void insertLog(char* log)
	{
		log_t logIn;

        time(&now);
        tm_now = localtime(&now);

		snprintf(logIn.msg,sizeof(logIn.msg),"%s",log);
		snprintf(logIn.time,sizeof(logIn.time),"%d-%d-%d %d:%d:%d",
				tm_now->tm_year+1900,tm_now->tm_mon+1, tm_now->tm_mday,
				tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);

		lock();
		logs.push_back(logIn);
		log_signal();
		unlock();

	}

	pthread_mutex_t logsMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t logCond = PTHREAD_COND_INITIALIZER;
	pthread_t printThread;
    time_t now ;
    struct tm *tm_now;
};

void* printLog(void* para)
{
	while(1)
	{
		hmLog::getInstance().lock();
		hmLog::getInstance().log_wait();
		auto logsTemp = std::move(hmLog::getInstance().logs);

		for(auto log :logsTemp)
		{
			printf("%s:%s:%s:%d:%s",log.time,__FILE__,__FUNCTION__,__LINE__,log.msg);
		}

		hmLog::getInstance().unlock();
	}

}

#endif /* INCLUDE_HMLOG_HPP_ */
