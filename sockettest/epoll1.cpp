#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

int initserver(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock <= 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    uint32_t len = sizeof(opt);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, len);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        perror("bind");
        return -1;
    }

    if (listen(sock, 5) != 0) {
        perror("listen");
        close(sock);
        return -1;
    }

    return sock;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "输入格式错误,请重试并重新输入。" << std::endl;
        return -1;
    }

    int listen_sock = initserver(atoi(argv[1]));
    if (listen_sock < 0) {
        std::cout << "initserver failed" << std::endl;
        return -1;
    }
    std::cout << "listen_sock:" << listen_sock << std::endl;

    int epollfd = epoll_create(1);
    struct epoll_event ev;
    ev.data.fd = listen_sock;
    ev.events = EPOLLIN;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev);
    epoll_event evs[10];

    while (true) {
        int in_fds = epoll_wait(epollfd, evs, 10, -1);

        if (in_fds < 0) {
            perror("epoll failed");
            break;
        }

        if (in_fds == 0) {
            printf("epoll() timeout.\n");
            continue;
        }

        for (int i = 0; i < in_fds; i++) {
            if (evs[i].data.fd == listen_sock) {
                struct sockaddr_in client;
                socklen_t client_len = sizeof(client);
                int client_sock = accept(listen_sock, (struct sockaddr*)&client, &client_len);
                std::cout << "accept client(socket=" << client_sock << ")" << std::endl;

                ev.data.fd = client_sock;
                ev.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sock, &ev);
            } else {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                if (recv(evs[i].data.fd, buffer, sizeof(buffer), 0) <= 0) {
                    std::cout << "client(eventfd=" << evs[i].data.fd << ") disconnected." << std::endl;
                    close(evs[i].data.fd);
                } else {
                    std::cout << "recv:" << buffer << std::endl;
                    send(evs[i].data.fd, buffer, strlen(buffer), 0);
                }
            }
        }
    }

    return 0;
}