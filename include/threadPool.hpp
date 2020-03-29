/*
 * threadPool.hpp
 *
 *  Created on: 30 Mar 2020
 *      Author: siwei
 */

#ifndef INCLUDE_THREADPOOL_HPP_
#define INCLUDE_THREADPOOL_HPP_

#include <pthread.h>
#include <unistd.h>
#include <vector>
#include "log.h"
#include <functional>

static void* startFunc (void *para);
static std::vector<std::function<void* (void*)>> funcs;

class threadPool
{
public:
    threadPool ()
    {
        threadNo = sysconf (_SC_NPROCESSORS_ONLN);
        LOG(INFO) << "threadNO is " << threadNo;
    }

    static void setFunction (std::function<void* (void*)> func)
    {
        funcs.push_back (func);
    }

    static std::function<void* (void*)> getFun (int i)
    {
        return funcs[i];
    }

    static int getFunNo ()
    {
        return funcs.size ();
    }
    void start ()
    {
        if (threadPool::getFunNo () == 0)
        {
            LOG(WARNING) << "funNo is " << threadPool::getFunNo ();
            return;
        }

        for (int i = 0; i < funcs.size (); i++)
        {
            pthread_t t;
            int index = i;
            pthread_create (&t, NULL, startFunc, (void*) &index);
            LOG(WARNING) << "pthread_create end i is "<<i;
        }
        while(1)
        {
            sleep(1);
        };
    }

    ~threadPool ()
    {

    }

private:
    int threadNo = 0;
    std::vector<pthread_t*> threads;
};

static void* startFunc (void *para)
{
    int index = *(int*)para;
    auto func = threadPool::getFun (index);
    func(nullptr);

    return nullptr;
}

#endif /* INCLUDE_THREADPOOL_HPP_ */
