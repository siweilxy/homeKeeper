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

private:
	hmLog()
	{
		printf("hmLog 启动\n");
		printThread =  new std::thread(printLog);
		insertLog("hmLog 启动\n");
	}

	void printLog()
	{
		logsMutex.lock();

		auto logsTemp = std::move(logs);

		for(auto log :logsTemp)
		{
			printf("%s",log.c_str());
		}

		logsMutex.unlock();
	}

	void insertLog(std::string log)
	{
		logsMutex.lock();

		logs.push_back(log);

		logsMutex.unlock();
	}

	std::vector<std::string> logs;
	std::mutex logsMutex;
	std::unique_ptr<std::thread> printThread;
};


#endif /* INCLUDE_HMLOG_HPP_ */
