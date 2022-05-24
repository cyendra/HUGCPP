//
// Created by cyendra on 2022/5/23.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#define PORT 114514
#define QUEUE 20//连接请求队列
int conn;
void thread_task()
{
}

int main()
{
    int cli_sock,sock_fd;
    fd_set fdsr,cpy_reads;
    int i,ret;
    int maxsock=0;
    struct timeval tv;//超时时间
    char recvBuf[1024];//接收
    struct sockaddr_in client_addr;
    socklen_t length=sizeof(client_addr);

    sock_fd = socket(AF_INET,SOCK_STREAM,0);//若成功则返回一个sockfd（套接字描述符）

    struct sockaddr_in server_addr;//一般是储存地址和端口的。用于信息的显示及存储使用
    /*设置 sockaddr_in 结构体中相关参数*/
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);//将一个无符号短整型数值转换为网络字节序，即大端模式(big-endian)
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//将主机的无符号长整形数转换成网络字节顺序。

    if(bind(sock_fd,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    if(listen(sock_fd,QUEUE)==-1)
    {
        perror("listen");
        exit(1);
    }

    maxsock = sock_fd;
    std::cout<<"sock_fd="<<sock_fd<<std::endl;

    FD_ZERO(&fdsr);//描述符集合清零
    FD_SET(sock_fd,&fdsr);//添加到描述符集合
    tv.tv_sec=1;//超时时间 秒
    tv.tv_usec=0;

    while(1)
    {
        std::cout<<"--------------------while-----------------------"<<std::endl;
        cpy_reads = fdsr;//必须要有这句，否则无法建立新链接
        //select第四个参数如果设置为NULL，则会一直阻塞，直到有新消息
        ret=select(maxsock+1,&cpy_reads,NULL,NULL,&tv);
        if (ret < 0)
        {
            std::cout<<"select error"<<std::endl;
            break;
        }
        else if (ret == 0)
        {
            std::cout<<"timeout..."<<std::endl;
            continue;
        }
        else
        {
            for(i=0;i<maxsock+1;i++)
            {

                if(FD_ISSET(i,&cpy_reads))
                {
                    std::cout<<"i="<<i<<std::endl;
                    //有新的连接请求
                    if(i==sock_fd)
                    {
                        cli_sock=accept(sock_fd,(struct sockaddr*)&client_addr,&length);
                        FD_SET(cli_sock,&fdsr);
                        maxsock=maxsock<cli_sock?cli_sock:maxsock;
                        std::cout<<"connect client: "<<cli_sock<<std::endl;
                    }
                    else
                    {
                        memset(recvBuf,0,sizeof(recvBuf));
                        int str_len=recv(i, recvBuf, sizeof(recvBuf), 0);
                        std::cout<<"str_len="<<str_len<<std::endl;
                        std::cout<<"recvBuf="<<recvBuf<<std::endl;
                        if(str_len==0)
                        {
                            FD_CLR(i,&fdsr);
                            close(i);
                            std::cout<<"closed client: "<<cli_sock<<std::endl;
                        }
                        else
                        {
                            std::cout<<"server send: "<<recvBuf <<std::endl;
                            send(i,recvBuf,str_len,0);
                        }
                    }
                }

            }
        }
    }

    close(sock_fd);
    return 0;
}