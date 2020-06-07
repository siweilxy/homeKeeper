/*
 * raft.cpp
 *
 *  Created on: 7 Jun 2020
 *      Author: siwei
 */
extern "C"{
#include "raft.h"
#include "raft_log.h"
#include "raft_private.h"
#include "raft_types.h"
}
int main()
{
    void* raft = raft_new();
    return 0;
}

