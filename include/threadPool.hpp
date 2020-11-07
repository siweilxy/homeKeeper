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
#include <functional>
#include <map>
#include <sys/prctl.h>
#include "hmLog.hpp"
static void* startFunc (void *para);
static std::map<std::string,std::function<void* (void*)>> funcs;

typedef struct para_s
{
    std::string name;
} para_t;

class threadPool
{
public:
    threadPool ()
    {
        threadNo = sysconf (_SC_NPROCESSORS_ONLN);
        LOG(INFO) << "threadNO is " << threadNo;
    }

    static void setFunction (std::string name,std::function<void* (void*)> func)
    {
        funcs[name]=func;
    }

    static std::function<void* (void*)> getFun (std::string i)
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

        for (auto iter:funcs)
        {
            para_t* p = new para_t;;
            p->name =iter.first ;
            pthread_t *t = (pthread_t*)malloc(sizeof(pthread_t));
            pthread_create (t, NULL, startFunc, (void*) p);
            threads.push_back(t);
            LOG(WARNING) << "pthread_create end name is "<<p->name;
        }
        for(auto p:threads)
        {
            pthread_join(*p,nullptr);
        }
    }

    ~threadPool ()
    {
        for(auto p:threads)
        {
            free(p);
        }
    }

private:
    int threadNo = 0;
    std::vector<pthread_t*> threads;
};

static void* startFunc (void *para)
{
    std::string name = ((para_t*)para)->name;
    prctl(PR_SET_NAME, name.c_str());
    LOG(ERROR) << "prctl name is "<<name;
    delete (para_t*)para;
    auto func = threadPool::getFun (name);
    func(nullptr);
    LOG(WARNING) << "index fun ended";
    return nullptr;
}

#endif /* INCLUDE_THREADPOOL_HPP_ */
