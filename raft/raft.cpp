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
#include <unistd.h>
#include <stdio.h>
int main()
{
    raft_server_t* raft = raft_new();
    if(raft == nullptr)
    {
        printf("raft is nullptt\n");
    }
    raft_node_t* node = raft_add_node(raft, NULL, 0, 1);//1表示是node
    if(node == nullptr)
    {
        printf("node is nullptr\n");
    }
    while(1)
    {
        sleep(1);
        raft_periodic(raft, 1000);
    }
    return 0;
}

