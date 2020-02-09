/*
 * tcpClient.hpp
 *
 *  Created on: 9 Feb 2020
 *      Author: siwei
 */

#include <string>
#include "log.h"
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

class tcpClient
{
private:
    int sock;
    std::string ip;
    int port;
    char message[1024] = {0};
    int str_len, recv_len, recv_cnt;
    struct sockaddr_in serv_adr;

public:
    ~tcpClient()
    {
        LOG(INFO)<<"~tcpClient";
        close(sock);
    }

    tcpClient (std::string ip, int port) : ip (ip), port (port)
    {
        ENTER
        int ret = 0;
        int flag = 0;
        std::cout << "flag is " << flag << std::endl;
        ret = log::getInstance ().init (flag);
        if (ret != 0)
        {
            printf ("log init error");
        }

        connectToServer ();
        EXIT
    }

    int connectToServer ()
    {
        ENTER
        sock = socket (PF_INET, SOCK_STREAM, 0);
        if (sock == -1)
        {
            LOG(ERROR) << "socket() error" << strerror (errno);
            return -1;
        }
        memset (&serv_adr, 0, sizeof(serv_adr));
        serv_adr.sin_family = AF_INET;
        serv_adr.sin_addr.s_addr = inet_addr (ip.c_str ());
        serv_adr.sin_port = htons (port);
        if (connect (sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))
                == -1)
        {
            LOG(ERROR) << "connect error " << strerror (errno);
            return -1;
        }
        EXIT
        return 0;
    }

    int sendMsg(std::string messageIn)
    {
        int len = 0;
        len = write(sock,messageIn.c_str(),messageIn.length());
        LOG(INFO)<<"write len is "<<len<<" message len is "<<messageIn.length();
        return 0;
    }

    int getMsg(std::string& messageOut)
    {
        ENTER
        recv_len = 0;
        while (1)
        {
            recv_cnt = read (sock, &message[recv_len], sizeof(message) - 1);
            if (recv_cnt == -1)
            {
                LOG(ERROR) << "read error:" << strerror (errno);
                return -1;
            }

            recv_len += recv_cnt;
            if(message[recv_len - 2] == 13&&message[recv_len - 1] == 10)
            {
                LOG(INFO) << "message is " << message<<"len is "<<recv_len - 1;
                break;
            }
        }

//        for(int i = 0;i<recv_len;i++)
//        {
//            LOG(INFO)<<(int)message[i];
//        }

        messageOut = message;

        //str_len = write(sock,message,strlen(message));
        EXIT
        return 0;
    }

};
