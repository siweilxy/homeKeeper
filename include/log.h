/*
 * log.h
 *
 *  Created on: Jan 2, 2020
 *      Author: siwei
 */

#ifndef COMMON_LOG_H_
#define COMMON_LOG_H_

#include<stdio.h>
#include "logging.h"

#define ENTER LOG(INFO)<<"进入函数:"<<__FUNCTION__;
#define EXIT LOG(INFO)<<"离开函数:"<<__FUNCTION__;
class log
{
public:
    log();
    ~log();
    int init(int stderrFlag = 0);
    void setLevel(char* msg);
    static log& getInstance();
private:
    int flag = 0;
};

#endif /* COMMON_LOG_H_ */
