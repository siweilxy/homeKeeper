/*
 * master.cpp
 *
 *  Created on: 2020年11月29日
 *      Author: siwei
 */

#include "hmLog.hpp"
#include "env.hpp"
#include "file.hpp"
#include "json.hpp"
#include <vector>
#include <string>

std::vector<std::string> processes;

int main()
{
    int ret = 0;
	ret = hmLog::getInstance().init(0);
    if(ret != 0)
    {
        printf("log init error");
        return -1;
    }

    ERROR("master started");

	 //忽略信号
	 signal(SIGHUP, SIG_IGN);

    env e;
    std::string path =e.getValue("cfg_path");
    ERROR("path [%s]",path.c_str());

    std::string bin_path =e.getValue("BIN_PATH");
    ERROR("BIN_PATH [%s]",bin_path.c_str());

    file f(path.c_str());
    auto workerNo =  f.getJsonInt("workerNo");
    auto jsStr = f.getRes();
    INFO("json[%s]",jsStr.c_str());
    auto jsonStr = f.getRes();
    auto js = json::parse (jsonStr);

    auto worker = js["worker"];
    for(int i = 0;i<workerNo;i++)
    {
    	std::string process = worker[i]["file"];
    	std::string execPath = bin_path + "/" + process;
    	ERROR("process [%s]",execPath.c_str());
    	system(execPath.c_str());
    }

    while(1)
    {
        int pid = 0;
    	sleep(1);
    	char* res= nullptr;
    	for(int i = 0;i<workerNo;i++)
    	{
    		char cmd[200]="ps -aux |grep -v grep|grep %s";
        	char result[200]={0};
    		std::string process = worker[i]["file"];
    		std::string processNoStr = worker[i]["number"];
    		int processNo = atoi(processNoStr.c_str());
        	snprintf(cmd,sizeof(cmd),process.c_str());
        	FILE* pidreader = popen(cmd,"r");
        	res = fgets(result,sizeof(result) - 1,pidreader);
        	pclose(pidreader);

        	char * saveptr1;
        	if(strlen(result)>0)
        	{
        		INFO("ps result[%s]",result);
        		res = strtok_r(NULL," ",&saveptr1);
        		if(res != nullptr)
        		{
        			res = strtok_r(NULL," ",&saveptr1);
        			if(res != nullptr)
        			{
        				pid = atoi(res);
        				INFO("pid[%d]",pid);
        			}
        		}
        	}

        	if(pid == 0)
        	{
        		for(int i = 0;i < processNo;i++)
        		{
                	std::string execPath = bin_path + "/" + process + " &";
                	ERROR("重启进程[%d][%s]",i,execPath.c_str());
                	system(execPath.c_str());
        		}
        	}
    	}
    }

}
