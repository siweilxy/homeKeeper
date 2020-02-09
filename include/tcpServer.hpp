/*
 * server.hpp
 *
 *  Created on: 7 Feb 2020
 *      Author: siwei
 */

#ifndef INCLUDE_TCPSERVER_HPP_
#define INCLUDE_TCPSERVER_HPP_

#include "log.h"
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define EPOLL_SIZE 50
#define BUF_SIZE 1
class tcpServer
{
private:
    int inPort = 19870;
    int outPort = 21;
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_adr,clnt_adr;
    socklen_t adr_sz;
    int str_len,i;
    char buf[BUF_SIZE];
    struct epoll_event *ep_events;
    struct epoll_event event;
    int epfd,event_cnt;
    void startTcpServer ()
    {
        ENTER
        serv_sock = socket(PF_INET,SOCK_STREAM,0);
        memset(&serv_adr,0,sizeof(serv_adr));
        serv_adr.sin_family = AF_INET;
        serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
        LOG(INFO)<<"listen port is "<<inPort;
        serv_adr.sin_port = htons(inPort);
        if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr)) == -1)
        {
            LOG(ERROR)<<"bind error:"<<strerror(errno);
        }

        if(listen(serv_sock,5) == -1)
        {
            LOG(ERROR)<<"listen error:"<<strerror(errno);
        }

        epfd = epoll_create(EPOLL_SIZE);
        ep_events = (epoll_event *)malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
        setNonBlockingMode(serv_sock);
        event.events = EPOLLIN;
        event.data.fd = serv_sock;
        epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event);
        while(1)
        {
            event_cnt = epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);
            if(event_cnt == -1)
            {
                LOG(ERROR)<<"epoll_wait error:"<<strerror(errno);
                break;
            }
            for(i = 0;i<event_cnt;i++)
            {
                if(ep_events[i].data.fd == serv_sock)
                {
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&adr_sz);
                    setNonBlockingMode(clnt_sock);
                    event.events = EPOLLIN|EPOLLET;
                    event.data.fd=clnt_sock;
                    epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sock,&event);
                    LOG(INFO)<<"connected client is "<<clnt_sock;
                }else
                {
                    while(1)
                    {
                        str_len = read(ep_events[i].data.fd,buf,BUF_SIZE);
                        if(str_len==0)
                        {
                            epoll_ctl(epfd,EPOLL_CTL_DEL,ep_events[i].data.fd,NULL);
                            close(ep_events[i].data.fd);
                            LOG(INFO)<<"closed client "<<ep_events[i].data.fd;
                            break;
                        }
                        else if(str_len < 0)
                        {
                            if(errno == EAGAIN)
                            {
                                break;
                            }
                        }else
                        {
                            LOG(INFO)<<"read is :"<<buf;
                        }
                    }
                }
            }
        }
        close(serv_sock);
        close(epfd);
        EXIT
    }

    void setNonBlockingMode(int fd)
    {
        int flag = fcntl(fd,F_GETFL,0);
        fcntl(fd,F_SETFL,flag|O_NONBLOCK);
    }

public:
    tcpServer (int inPort = 19870, int outPort = 21,int stderrFlag = 0) : inPort (inPort), outPort (
            outPort)
    {
        int ret = 0;

        ret = log::getInstance ().init (stderrFlag);
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
