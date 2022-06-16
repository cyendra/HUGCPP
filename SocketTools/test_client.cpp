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

int main(int argc, char **argv) {
    int port = PORT;
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i] << std::endl;
    }
    if (argc == 2) {
        port = std::stoi(argv[1]);
    }
    std::cout << "port: " << port << std::endl;
    char buff[BUFFSIZE];
    sockfd = toko::get_client_socket("127.0.0.1", port);
    printf("Please input: ");
//    while (true) {
        scanf("%s", buff);
        send(sockfd, buff, strlen(buff), 0);
        bzero(buff, sizeof(buff));
        recv(sockfd, buff, BUFFSIZE - 1, 0);
        printf("Recv: %s\n", buff);
        if (strcmp(buff, "exit;") == 0) {
            printf("Break!!");
//            break;
        }
//    }
    close(sockfd);
    return 0;
}