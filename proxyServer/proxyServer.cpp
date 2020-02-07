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
    int outPort = 19870;
    int inPort = 21;
    char* outPortStr = nullptr;
    char* inPortStr = nullptr;

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
    tcpServer server;
    return 0;
}
