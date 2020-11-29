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

    env e;
    std::string path =e.getValue("cfg_path");
    INFO("path [%s]",path.c_str());

    file f(path.c_str());
    auto workerNo =  f.getJsonInt("workerNo");
    auto jsonStr = f.getRes();
    auto js = json::parse (jsonStr);
    auto worker = js["worker"][0];
    INFO("workerNo[%d] worker[%s]",workerNo,worker.c_str());

}
