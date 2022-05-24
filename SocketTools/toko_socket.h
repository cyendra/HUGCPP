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
#include <set>
#include <vector>

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

int get_server_socket(sockaddr_in servaddr) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd) {
        printf("Create socket error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
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


int get_server_socket(int port) {
    auto servaddr = get_server_sockaddr(port);
    return get_server_socket(servaddr);
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

class Select {
private:
    int _server_sock_fd;
    fd_set _fdsr{};
    fd_set _cpy_reads{};
    timeval _tv{};//超时时间
    sockaddr_in _server_sockaddr{};
    socklen_t _sockaddr_length;
    std::set<int> _fd_set;
    std::vector<int> result;

public:
    explicit Select(int server_sock_fd, sockaddr_in server_sockaddr) {
        this->_server_sock_fd = server_sock_fd;
        this->_server_sockaddr = server_sockaddr;
        this->_sockaddr_length = sizeof(_server_sockaddr);
        result.clear();
        _fd_set.clear();
        FD_ZERO(&_fdsr);//描述符集合清零
        FD_SET(_server_sock_fd, &_fdsr);//添加到描述符集合
        _fd_set.insert(_server_sock_fd);
        _tv.tv_sec = 1;//超时时间 秒
        _tv.tv_usec = 0;
    }

    void close_fd(int fd) {
        FD_CLR(fd,&_fdsr);
        close(fd);
        _fd_set.erase(fd);
        std::cout<<"closed client: "<<fd<<std::endl;
    }

    std::vector<int> get_fd_list() {
        return result;
    }

    int do_select() {
        result.clear();
        _cpy_reads = _fdsr;//必须要有这句，否则无法建立新链接
        int ret;
        //select第四个参数如果设置为NULL，则会一直阻塞，直到有新消息
        while(result.empty()) {
            int maxsock = *std::max_element(_fd_set.begin(), _fd_set.end());
            std::cout << "select... maxsock " << maxsock << std::endl;
            ret = ::select(maxsock + 1, &_cpy_reads, nullptr, nullptr, &_tv);
            std::cout << "ret " << ret << std::endl;

            if (ret < 0) {
                std::cout << "select error" << std::endl;
                return ret;
            } else if (ret == 0) {
                std::cout << "timeout..." << std::endl;
                return ret;
            } else {
                //有新的连接请求
                if (FD_ISSET(_server_sock_fd, &_cpy_reads)) {
                    int cli_sock = accept(_server_sock_fd, (struct sockaddr *) &_server_sockaddr, &_sockaddr_length);
                    FD_SET(cli_sock, &_fdsr);
                    _fd_set.insert(cli_sock);
                    std::cout << "connect client: " << cli_sock << std::endl;
                }

                for (int i: _fd_set) {
                    if (i == _server_sock_fd) {
                        continue;
                    }
                    if (FD_ISSET(i, &_cpy_reads)) {
                        std::cout << "i=" << i << std::endl;
                        result.push_back(i);
                    }
                }
            }
        }

        return ret;
    }

};



}

#endif //SOCKETTOOLS_TOKO_SOCKET_H
