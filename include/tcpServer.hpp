/*
 * server.hpp
 *
 *  Created on: 7 Feb 2020
 *      Author: siwei
 */

#ifndef INCLUDE_TCPSERVER_HPP_
#define INCLUDE_TCPSERVER_HPP_

#include "log.h"
#include "tcpClient.hpp"
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <map>
#define EPOLL_SIZE 50
#define BUF_SIZE 1
class tcpServer
{
private:
    int inPort = 19870;
    int ret = 0;
    int outPort = 21;
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t adr_sz;
    int str_len, i;
    char buf[BUF_SIZE];
    struct epoll_event *ep_events;
    struct epoll_event event;
    int epfd, event_cnt;
    std::map<int, tcpClient*> clients;
    void startTcpServer ()
    {
        ENTER
        serv_sock = socket (PF_INET, SOCK_STREAM, 0);

        int option;
        int optlen = sizeof(option);
        option = 1;
        setsockopt (serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*) &option,
                    optlen);

        memset (&serv_adr, 0, sizeof(serv_adr));
        serv_adr.sin_family = AF_INET;
        serv_adr.sin_addr.s_addr = htonl (INADDR_ANY);
        LOG(INFO) << "listen port is " << inPort;
        serv_adr.sin_port = htons (inPort);
        if (bind (serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))
                == -1)
        {
            LOG(ERROR) << "bind error:" << strerror (errno);
            return;
        }

        if (listen (serv_sock, 5) == -1)
        {
            LOG(ERROR) << "listen error:" << strerror (errno);
            return;
        }

        epfd = epoll_create (EPOLL_SIZE);
        ep_events = (epoll_event*) malloc (
                sizeof(struct epoll_event) * EPOLL_SIZE);
        memset (ep_events, 0, sizeof(struct epoll_event) * EPOLL_SIZE);
        setNonBlockingMode (serv_sock);
        event.events = EPOLLIN;
        event.data.fd = serv_sock;
        ret = epoll_ctl (epfd, EPOLL_CTL_ADD, serv_sock, &event);
        if (ret != 0)
        {
            LOG(ERROR) << "epoll_ctl ERROR " << strerror (errno);
            return;
        }
        while (1)
        {
            LOG(INFO) << "before epoll_wait";
            event_cnt = epoll_wait (epfd, ep_events, EPOLL_SIZE, -1);
            LOG(INFO) << "after epoll_wait";
            if (event_cnt == -1)
            {
                LOG(ERROR) << "epoll_wait error:" << strerror (errno);
                break;
            }
            for (i = 0; i < event_cnt; i++)
            {
                LOG(INFO) << "i is " << i << " fd is "
                        << ep_events[i].data.fd;
                if (ep_events[i].data.fd == serv_sock)
                {
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock = accept (serv_sock, (struct sockaddr*) &clnt_adr,
                                        &adr_sz);

                    socklen_t len = sizeof(clnt_adr);
                    if (getpeername (clnt_sock, (struct sockaddr*) &clnt_adr,
                                      &len) == 0)
                    {
                        LOG(INFO) << "对方IP："
                                << inet_ntoa (clnt_adr.sin_addr);
                        LOG(INFO) << "对方PORT："
                                << ntohs (clnt_adr.sin_port);
                    }

                    setNonBlockingMode (clnt_sock);
                    event.events = EPOLLIN | EPOLLET;
                    event.data.fd = clnt_sock;

                    LOG(INFO) << "get mgs from ftp";
                    std::string message;
                    auto iter = clients.find (clnt_sock);
                    if (iter == clients.end ())
                    {
                        tcpClient *client = new tcpClient ("0.0.0.0", 21);
                        clients[clnt_sock] = client;
                    }
                    iter = clients.find (clnt_sock);
                    iter->second->getMsg (message);
//                    for(int i = 0;i<message.length();i++)
//                    {
//                        LOG(INFO)<<(int)message[i];
//                    }

                    write (clnt_sock, message.c_str (), message.length () - 1);
                    LOG(INFO) << "message write success:" << message
                            << "len is " << message.length () - 1;

                    ret = epoll_ctl (epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                    if (ret != 0)
                    {
                        LOG(ERROR) << "epoll_ctl ERROR "
                                << strerror (errno);
                        break;
                    }
                    LOG(INFO) << "connected client is " << clnt_sock;
                }
                else
                {
                    LOG(INFO)
                            << "==========================start============================";
//                    LOG(INFO)<<"get mgs from ftp";
//                    std::string message;
//                    auto iter = clients.find(ep_events[i].data.fd);
//                    if(iter == clients.end())
//                    {
//                        tcpClient * client = new tcpClient("0.0.0.0",21);
//                        clients[ep_events[i].data.fd] = client;
//                    }
//                    iter = clients.find(ep_events[i].data.fd);
//                    iter->second->getMsg(message);
////                    for(int i = 0;i<message.length();i++)
////                    {
////                        LOG(INFO)<<(int)message[i];
////                    }
//
//                    write(ep_events[i].data.fd,message.c_str(),message.length() - 1);
//                    LOG(INFO)<<"message write success:"<<message<<"len is "<<message.length() - 1;

                    while (1)
                    {
                        //LOG(INFO)<<"before read";
                        str_len = read (ep_events[i].data.fd, buf, BUF_SIZE);
                        if (str_len == 0)
                        {
                            epoll_ctl (epfd, EPOLL_CTL_DEL,
                                       ep_events[i].data.fd, NULL);
                            auto iter = clients.find (ep_events[i].data.fd);
                            delete iter->second;
                            clients.erase (iter);
                            close (ep_events[i].data.fd);
                            LOG(INFO) << "closed client "
                                    << ep_events[i].data.fd;
                            break;
                        }
                        else if (str_len < 0)
                        {
                            if (errno == EAGAIN)
                            {
                                break;
                            }
                            LOG(INFO) << "str_len < 0  is " << str_len
                                    << " errno is " << errno << " "
                                    << strerror (errno);
                            break;
                        }
                        else
                        {
                            LOG(INFO) << "read is :" << buf;
                        }

                        LOG(INFO) << "send mgs to ftp";
                        std::string message;
                        auto iter = clients.find (ep_events[i].data.fd);
                        if (iter == clients.end ())
                        {
                            LOG(INFO) << "iter == clients.end ()";
//                            tcpClient *client = new tcpClient ("0.0.0.0", 21);
//                            clients[clnt_sock] = client;
                        }
                        iter = clients.find (clnt_sock);
                        iter->second->sendMsg(buf);
                    }
                    LOG(INFO)
                            << "==========================end============================";
                }
            }
        }
        close (serv_sock);
        close (epfd);
        EXIT
    }

    void setNonBlockingMode (int fd)
    {
        int flag = fcntl (fd, F_GETFL, 0);
        fcntl (fd, F_SETFL, flag | O_NONBLOCK);
    }

public:
    tcpServer (int inPort = 19870, int outPort = 21, int stderrFlag = 0) : inPort (
            inPort), outPort (outPort)
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
