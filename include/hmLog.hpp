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

static void printLog();

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
		printThread->join();
		printf("hmLog 结束\n");
		insertLog("hmLog 结束\n");
	}

	void lock()
	{
		logsMutex.lock();
	}

	void unlock()
	{
		logsMutex.unlock();
	}

	std::vector<std::string> logs;

private:
	hmLog()
	{
		printf("hmLog 启动\n");
		printThread = std::make_shared<std::thread>(printLog);
		insertLog("hmLog 启动\n");
	}

	void insertLog(std::string log)
	{
		logsMutex.lock();

		logs.push_back(log);

		logsMutex.unlock();
	}

	std::mutex logsMutex;
	std::shared_ptr<std::thread> printThread;
};

static void printLog()
{
	hmLog::getInstance().lock();

	auto logsTemp = std::move(hmLog::getInstance().logs);

	for(auto log :logsTemp)
	{
		printf("%s",log.c_str());
	}

	hmLog::getInstance().unlock();
}

#endif /* INCLUDE_HMLOG_HPP_ */
