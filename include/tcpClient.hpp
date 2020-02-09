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
    tcpClient (std::string ip, int port) : ip (ip), port (port)
    {
        int ret = 0;
        int flag = 0;
        std::cout << "flag is " << flag << std::endl;
        ret = log::getInstance ().init (flag);
        if (ret != 0)
        {
            printf ("log init error");
        }
    }

    int connectToServer (std::string& messageOut)
    {
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
        }

        recv_len = 0;
        while (1)
        {
            recv_cnt = read (sock, &message[recv_len], sizeof(message) - 1);
            if (recv_cnt == -1)
            {
                LOG(ERROR) << "read error:" << strerror (errno);
            }

            LOG(INFO) << "message is " << message;
            recv_len += recv_cnt;
            LOG(INFO)<<(int)message[recv_len - 2]<<" "<<(int)message[recv_len - 1];
            if(message[recv_len - 2] == 13&&message[recv_len - 1] == 10)
            {
                LOG(INFO) << "message is " << message;
                break;
            }
        }
        messageOut = message;

        //str_len = write(sock,message,strlen(message));

        close (sock);
        return 0;
    }

};
