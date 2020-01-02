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
    static log& getInstance();

};

#endif /* COMMON_LOG_H_ */
