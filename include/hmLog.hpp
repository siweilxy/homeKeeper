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
#include <thread>
#include <memory>
#include <mutex>
#include <time.h>

static void printLog();
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
		printThread->join();
		printf("hmLog 结束\n");
	}

	void lock()
	{
		logsMutex.lock();
	}

	void unlock()
	{
		logsMutex.unlock();
	}

	std::vector<log_t> logs;

private:
	hmLog()
	{
		printThread = std::make_shared<std::thread>(printLog);
		insertLog("hmLog 启动\n");
		printf("hmLog 启动\n");

	}

	void insertLog(std::string log)
	{
		log_t logIn;

        time(&now);
        tm_now = localtime(&now);

		snprintf(logIn.msg,sizeof(logIn.msg),"%s",log.c_str());
		snprintf(logIn.time,sizeof(logIn.time),"%d-%d-%d %d:%d:%d",
				tm_now->tm_year+1900,tm_now->tm_mon+1, tm_now->tm_mday,
				tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);

		logsMutex.lock();
		logs.push_back(logIn);

		logsMutex.unlock();
	}

	std::mutex logsMutex;
	std::shared_ptr<std::thread> printThread;
    time_t now ;
    struct tm *tm_now;
};

static void printLog()
{
	hmLog::getInstance().lock();

	auto logsTemp = std::move(hmLog::getInstance().logs);

	for(auto log :logsTemp)
	{
		printf("%s:%s:%s:%d:%s",log.time,__FILE__,__FUNCTION__,__LINE__,log.msg);
	}

	hmLog::getInstance().unlock();
}

#endif /* INCLUDE_HMLOG_HPP_ */
