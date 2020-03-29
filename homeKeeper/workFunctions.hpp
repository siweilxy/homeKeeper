/*
 * workFunctions.hpp
 *
 *  Created on: 30 Mar 2020
 *      Author: siwei
 */

#ifndef HOMEKEEPER_WORKFUNCTIONS_HPP_
#define HOMEKEEPER_WORKFUNCTIONS_HPP_

#include "log.h"
#include <iostream>

void* test(void* para)
{
    while(1)
    {
        sleep(1);
        LOG(INFO)<<"test";
    }
    return nullptr;
}


#endif /* HOMEKEEPER_WORKFUNCTIONS_HPP_ */
