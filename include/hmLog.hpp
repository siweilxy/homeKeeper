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
#include <stdarg.h>
#include <cstring>
#include <map>
#include "env.hpp"
#include "file.hpp"

#include "common.h"

enum log_level {
	debug_level = 0,
	info_level,
	warn_level,
	error_level,
	fatal_level
} ;

std::map<int,const char*> log_str {{debug_level,"debug"},{info_level,"info"},{warn_level,"warn"},{error_level,"error"},{fatal_level,"fatal"}};

#define ENTER INFO("进入函数:%s",__FUNCTION__);
#define EXIT INFO("离开函数:%s",__FUNCTION__);

#define DEBUG(...) hmLog::getInstance().insertLog(debug_level,getpid(),pthread_self(),__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define INFO(...) hmLog::getInstance().insertLog(info_level,getpid(),pthread_self(),__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define WARN(...) hmLog::getInstance().insertLog(warn_level,getpid(),pthread_self(),__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define ERROR(...) hmLog::getInstance().insertLog(error_level,getpid(),pthread_self(),__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define FATAL(...) hmLog::getInstance().insertLog(fatal_level,getpid(),pthread_self(),__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)

void* printLog(void *para);
typedef struct log_def {
	int level;
	char time[255];
	char msg[10240];
} log_t;

class hmLog {
public:
	std::vector<log_t> logs;
	int flag = 0;
	std::shared_ptr<file> logfile;
	std::string processName;

	static hmLog& getInstance() {
		static hmLog instance;
		return instance;
	}

	void setLogPath()
	{
		time_t now;
		struct tm *tm_now;

		time(&now);
		tm_now = localtime(&now);

		char time[100]={0};

		snprintf(time, sizeof(time), "%d-%d-%d-%s",
				tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,processName.c_str());
		std::string logPathAndName = logPath+"/"+time+".log";
		logfile->setPath(logPathAndName);
	}

	int init(int level)
	{
		char strProcessPath[1024] = {0};
		if(readlink("/proc/self/exe", strProcessPath,1024) <=0)
		{
				return -1;
		}
		char *strProcessName = strrchr(strProcessPath, '/');
		processName = strProcessName + 1;

		if(initFlag == 0)
		{
			env env;
			cfgPath = env.getValue("cfg_path");
			std::string level = env.getValue("LOG_LEVEL");
			current_log_level = atoi(level.c_str());
			printf("cfgPah:[%s]\n",cfgPath.c_str());

			file cfgFile(cfgPath);
			logPath = cfgFile.getJsonString("log_path");
			time_t now;
			struct tm *tm_now;

			time(&now);
			tm_now = localtime(&now);

			char time[100]={0};

			snprintf(time, sizeof(time), "%d-%d-%d-%s",
					tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,processName.c_str());

			std::string logPathAndName = logPath+"/"+time+".log";

			printf("logPathAndName is %s\n",logPathAndName.c_str());
			logfile= std::make_shared<file>(logPathAndName);
			initFlag = 1;
		}

		return SUCCESS;
	}

	void setLevel(int level)
	{
		current_log_level = level;
	}

	int getLevel()
	{
		return current_log_level;
	}

	~hmLog() {
		printf("hmLog 结束\n");
		while (1) {
			lock();
			if (logs.empty() == false) {
				printf("logs:%ld\n", logs.size());
				log_signal();
				unlock();
				sleep(1);
			} else {
				printf("等待线程结束\n");
				pthread_cancel(printThread);
				unlock();
				pthread_join(printThread, nullptr);


				break;
			}
		}
//		pthread_cancel(printThread);
//		pthread_join(printThread, nullptr);
	}

	void lock() {
		pthread_mutex_lock(&logsMutex);
	}

	void unlock() {
		pthread_mutex_unlock(&logsMutex);
	}

	void log_wait() {
		pthread_cond_wait(&logCond, &logsMutex);
	}

	void insertLog(int level,long pid,long tid,const char *fileName, const char *funcName, int line,
			const char *pstr, ...) {
		log_t logIn;

		time_t now;
		struct tm *tm_now;

		time(&now);
		tm_now = localtime(&now);

		va_list ap;
		va_start(ap, pstr);

		int count_write = vsnprintf(NULL, 0, pstr, ap);
		va_end(ap);

		// 长度为空
		if (0 >= count_write)
			return;

		count_write++;

		va_start(ap, pstr);
		char *pbuf_out = NULL;
		pbuf_out = (char*) malloc(count_write);
		if (NULL == pbuf_out) {
			va_end(ap);
			return;
		}

		vsnprintf(pbuf_out, count_write, pstr, ap);

		logIn.level = level;
		snprintf(logIn.msg, sizeof(logIn.msg), "%ld:%ld:%s:%s:%d:%s",pid,tid, fileName,
				funcName, line, pbuf_out);
		snprintf(logIn.time, sizeof(logIn.time), "%d-%d-%d %d:%d:%d",
				tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
				tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
		va_end(ap);
		free(pbuf_out);
		pbuf_out = NULL;
		lock();

		logs.push_back(logIn);

		log_signal();

		unlock();

	}

private:
	int current_log_level = info_level;
	std::string cfgPath;
	std::string logPath;
	int initFlag = 0;

	hmLog() {
		pthread_create(&printThread, nullptr, printLog, nullptr);
		printf("hmLog 启动完成\n");
	}

	void log_signal() {
		pthread_cond_signal(&logCond);
	}

	pthread_mutex_t logsMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t logCond = PTHREAD_COND_INITIALIZER;
	pthread_t printThread;
};

void cleanup(void *arg)
{
	hmLog::getInstance().unlock();
}

void* printLog(void *para) {
	 printf("print thread start\n");

	 //忽略信号
	 signal(SIGHUP, SIG_IGN);

	 pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	 pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
	 pthread_cleanup_push(cleanup,NULL);

	while (1) {
		pthread_testcancel();
		hmLog::getInstance().lock();
		hmLog::getInstance().log_wait();
		auto logsTemp = std::move(hmLog::getInstance().logs);

		for (auto log : logsTemp) {
			if(log.level >= hmLog::getInstance().getLevel())
			{
				char temBuf[11240]={0};
				snprintf(temBuf,sizeof(temBuf),"%s:%s:%s\n", log_str[log.level],log.time, log.msg);
				if(hmLog::getInstance().getLevel() == 0)
				{
					printf("%s",temBuf);
				}

				hmLog::getInstance().setLogPath();
				hmLog::getInstance().logfile->write(temBuf,sizeof(temBuf));
			}
		}

		hmLog::getInstance().unlock();
	}
	pthread_cleanup_pop(0);
}

#endif /* INCLUDE_HMLOG_HPP_ */
