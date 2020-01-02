/*
 * log.h
 *
 *  Created on: Jan 2, 2020
 *      Author: siwei
 */

#ifndef COMMON_LOG_H_
#define COMMON_LOG_H_

class log
{
public:
    log();
    ~log();
    int init();
    void setLevel(char* msg);
    static log& getInstance();
private:
    int flag = 0;
};

#endif /* COMMON_LOG_H_ */
