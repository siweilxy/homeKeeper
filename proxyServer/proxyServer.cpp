/*
 * proxyServer.cpp
 *
 *  Created on: 7 Feb 2020
 *      Author: siwei
 */

#include <iostream>
#include "tcpServer.hpp"

int main()
{
    int outPort = 21;
    int inPort = 19870;
    char* outPortStr = nullptr;
    char* inPortStr = nullptr;
    char* stderrFlagStr = nullptr;
    int stderrFlag = 0;

    stderrFlagStr =getenv("LOG_FLAG");
    if (stderrFlagStr == nullptr)
    {
        printf("LOG_FLAG 没有设置,使用默认值 0\n");
    }else
    {
        outPort = atoi(stderrFlagStr);
    }

    outPortStr = getenv("OUT_PORT");
    if (outPortStr == nullptr)
    {
        printf("OUT_PORT 没有设置,使用默认值%d\n",outPort);
    }else
    {
        outPort = atoi(outPortStr);
    }

    inPortStr = getenv("IN_PORT");
    if (inPortStr == nullptr)
    {
        printf("IN_PORT 没有设置,使用默认值%d\n",inPort);
    }else
    {
        inPort = atoi(inPortStr);
    }
    printf("inPort is %d,outPort is %d\n",inPort,outPort);
    tcpServer server(inPort,outPort,stderrFlag);
    return 0;
}
