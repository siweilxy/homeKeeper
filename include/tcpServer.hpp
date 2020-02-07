/*
 * server.hpp
 *
 *  Created on: 7 Feb 2020
 *      Author: siwei
 */

#ifndef INCLUDE_TCPSERVER_HPP_
#define INCLUDE_TCPSERVER_HPP_

#include "log.h"

class tcpServer
{
private:
    int inPort = 19870;
    int outPort = 21;
    void startTcpServer ()
    {
        ENTER

        EXIT
    }
public:
    tcpServer (int inPort = 19870, int outPort = 21) : inPort (inPort), outPort (
            outPort)
    {
        int ret = 0;
        ret = log::getInstance ().init ();
        if (ret != 0)
        {
            printf ("log init error");
        }
        startTcpServer ();
    }
    ~tcpServer ()
    {

    }
};

#endif /* INCLUDE_TCPSERVER_HPP_ */
