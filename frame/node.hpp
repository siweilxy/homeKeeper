/*
 * node.hpp
 *
 *  Created on: Jan 4, 2020
 *      Author: siwei
 */

#ifndef FRAME_NODE_HPP_
#define FRAME_NODE_HPP_

#include "common.h"
#include <vector>
#include <pthread.h>
/*
 * node 表示计算节点，包含对master的通信和交互，以及node和node之间的通信还有通信消息的处理，所以node中要有一个线程池;
 * 一个消息队列，线程池循环的从消息队列中取出消息来进行处理，但是心跳需要单独拿出来一个消息队列存放，否则心跳太多，会有影响
 * 有一个map，对应自我实现的消息处理，任务名称可以配置，支持热更新，使用tcp链接;
 * */

typedef void (*processFunc)();

class node
{
public:
    node(int threads)
    {

    }
private:
    std::vector<heartBeatMsg_t> heartBeatList;
    std::map<std::string, processFunc> submitFuncs;
};

#endif /* FRAME_NODE_HPP_ */
