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

    auto servaddr = toko::get_server_sockaddr(PORT);
    sockfd = toko::get_server_socket(servaddr);
    signal(SIGINT, stopServerRunning);    // 这句用于在输入Ctrl+C的时候关闭服务器

    printf("Select...\n");

    toko::Select tokoSelect = toko::Select(sockfd, servaddr);

    while (true) {
        int res = tokoSelect.do_select();
        if (res <= 0) {
            break;
        }
        for (const auto &fd: tokoSelect.get_fd_list()) {
            bzero(buff, BUFFSIZE);
            recv(fd, buff, BUFFSIZE - 1, 0);
            printf("Recv: %s\n", buff);
            send(fd, buff, strlen(buff), 0);
            if (strcmp(buff, "exit;") == 0) {
                tokoSelect.close_fd(fd);
            }
        }
    }

    return 0;
}