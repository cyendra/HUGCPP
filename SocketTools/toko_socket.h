//
// Created by cyendra on 2022/5/23.
//

#ifndef SOCKETTOOLS_TOKO_SOCKET_H
#define SOCKETTOOLS_TOKO_SOCKET_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <cerrno>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>

namespace toko {

    const static int BUFF_SIZE = 2048;
    const static int DEFAULT_PORT = 2048;
    const static int MAX_LINK = 2048;

    sockaddr_in get_server_sockaddr(int port) {
        sockaddr_in servaddr{};
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(port);
        return servaddr;
    }

    sockaddr_in get_client_sockaddr(const std::string &ip, int port) {
        sockaddr_in servaddr{};
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr);
        servaddr.sin_port = htons(port);
        return servaddr;
    }

    int get_server_socket(int port) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == sockfd) {
            printf("Create socket error(%d): %s\n", errno, strerror(errno));
            return -1;
        }
        auto servaddr = get_server_sockaddr(port);
        int bindres = bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        if (-1 == bindres) {
            printf("Bind error(%d): %s\n", errno, strerror(errno));
            return -1;
        }
        int listenres = listen(sockfd, MAX_LINK);
        if (-1 == listenres) {
            printf("Listen error(%d): %s\n", errno, strerror(errno));
            return -1;
        }
        return sockfd;
    }

    int get_client_socket(const std::string& ip, int port) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == sockfd) {
            printf("Create socket error(%d): %s\n", errno, strerror(errno));
            return -1;
        }
        auto servaddr = get_client_sockaddr(ip, port);
        int connectres = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
        if (-1 == connectres)
        {
            printf("Connect error(%d): %s\n", errno, strerror(errno));
            return -1;
        }
        return sockfd;
    }

}

#endif //SOCKETTOOLS_TOKO_SOCKET_H
