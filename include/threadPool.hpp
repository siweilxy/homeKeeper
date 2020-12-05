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
#include <signal.h>
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
        INFO("threadNo is %d",threadNo);
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
            WARN("funNo is %d",threadPool::getFunNo ());
            return;
        }

        for (auto iter:funcs)
        {
            para_t* p = new para_t;;
            p->name =iter.first ;
            pthread_t *t = (pthread_t*)malloc(sizeof(pthread_t));
            pthread_create (t, NULL, startFunc, (void*) p);
            threads.push_back(t);
            WARN("pthread_create end name is %s",p->name.c_str());
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

    //忽略信号
    signal(SIGHUP, SIG_IGN);

    ERROR("prctl name is %s",name.c_str());
    delete (para_t*)para;
    auto func = threadPool::getFun (name);
    func(nullptr);
    WARN("index fun ended");
    return nullptr;
}

#endif /* INCLUDE_THREADPOOL_HPP_ */
