/*
 * testClient.cpp
 *
 *  Created on: 9 Feb 2020
 *      Author: siwei
 */




#include <iostream>
#include "tcpClient.hpp"

int main(int aggc,char** agrv)
{
    int port = 21;
    tcpClient client("0.0.0.0",port);
    std::string message;
    std::cout<<message<<std::endl;
    return 0;
}
