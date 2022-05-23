//
// Created by cyendra on 2022/5/23.
//

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <cerrno>
#include <csignal>
#include <unistd.h>

#include "toko_socket.h"

const int BUFFSIZE = 2048;
const int PORT = 114514;

int sockfd;

void stopServerRunning(int p)
{
    close(sockfd);
    printf("Close Server\n");
    exit(0);
}

int main() {
    char buff[BUFFSIZE];
    sockfd = toko::get_server_socket(PORT);
    printf("Listening...\n");
    signal(SIGINT, stopServerRunning);    // 这句用于在输入Ctrl+C的时候关闭服务器
    int connfd = accept(sockfd, nullptr, nullptr);
    if (-1 == connfd)
    {
        printf("Accept error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    while (true)
    {
        bzero(buff, BUFFSIZE);
        recv(connfd, buff, BUFFSIZE - 1, 0);
        printf("Recv: %s\n", buff);
        send(connfd, buff, strlen(buff), 0);
        if (strcmp(buff, "exit;") == 0) {
            break;
        }
    }
    close(connfd);
    return 0;
}