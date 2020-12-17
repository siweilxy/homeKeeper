/*
 * instance.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: siwei
 */

#ifndef INCLUDE_INSTANCE_HPP_
#define INCLUDE_INSTANCE_HPP_

#include <mutex>
#include "hmLog.hpp"

class instance
{
private:
    instance()
    {
        ERROR("instance");
    }
    std::mutex mu;
public:
    ~instance()
    {

    }

    instance(const instance&) = delete;
    instance& operator=(const instance&)=delete;
    static instance& getInstance()
    {
        static instance ins;
        return ins;
    }
    void lock()
    {
        mu.lock();
    }

    void unlock()
    {
        mu.unlock();
    }
};



#endif /* INCLUDE_INSTANCE_HPP_ */
