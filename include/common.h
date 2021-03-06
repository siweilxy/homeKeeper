/*
 * common.h
 *
 *  Created on: Jan 2, 2020
 *      Author: siwei
 */

#ifndef COMMON_COMMON_H_
#define COMMON_COMMON_H_


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include "json.hpp"
#include <mysql/mysql.h>

#define FIFO_NAME "entry"

#define SUCCESS 0
#define FAILED 1

typedef struct homeKeeperMsg_s
{
    int msgType;
    int length;
    char sender[512];
    char msg[0];
} homeKeeper_t;

typedef struct emailInfo_s
{
    std::string userName;
    std::string passwd;
    std::string smtpServer;
    std::string recipient;
    std::string mailFrom;
} emailInfo_t;

typedef struct sql_stmt_s
{
    std::string sql;
    MYSQL_STMT *stmt;
    int flag;
} sql_stmt_t;


typedef struct ipInfo_s
{
    int rec_id;
    std::string ip;
    std::string crt_ts;
    std::string upd_ts;
    std::string send_flag;
} ipInfo_t;

typedef struct downloadFile_s
{
    std::string fileName;
    std::string path;
    std::string crt_ts;
    std::string upd_ts;
    int send_flag;
} downloadFile_t;

typedef struct fd_s
{
    int fd;
    std::string file;
} fd_t;

enum msgType
{
    ul_log = 0, ul_end, ul_process_success, ul_other,ul_end_one
};

typedef struct internal_msg_s
{
    char srcName[512];
    char destName[512];
    char msg[512];
    char reslut[512];
    char masterIp[512];
    msgType type;
} internal_msg_t;

typedef struct heartBeatMsg_s
{
    char srcId[512];
    char destId[512];
    char srcIP[512];
    char destIP[512];
    uint64_t srcPort;
    uint64_t destPort;
    msgType type;
} heartBeatMsg_t;

int getfile(const std::string& path, std::string& result);

typedef void (*initFunc)();

typedef void (*doneFunc)();

typedef void (*processFunc)();

namespace U
{
class pipe
{
private:
    int fd = 0;
    std::string fifo;
    int owner = 0;
public:
    pipe(std::string fifoName);
    ~pipe();
    int senMsg(char* buf, int length);
    int recvMsg(char* buff, int len);
    int getFd();
};
}


#endif /* COMMON_COMMON_H_ */
