#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int initServer(uint32_t port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock <= 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    uint32_t len = sizeof(opt);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, len);

    sockaddr_in sock_server;
    sock_server.sin_family = AF_INET;
    sock_server.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_server.sin_port = htons(port);

    if (bind(sock, (sockaddr *)&sock_server, sizeof(sock_server)) != 0) {
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "请输入正确的格式并重新启动." << std::endl;
        return -1;
    }

    int listen_sock = initServer(atoi(argv[1]));
    if (listen_sock <= 0) {
        perror("initServer");
        return -1;
    }

    std::cout << "listen_sock:" << listen_sock << std::endl;

    int epoll_fd = epoll_create(1);

    epoll_event ev;
    ev.data.fd = listen_sock;
    ev.events = EPOLLIN;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &ev);

    epoll_event evs[10];

    while (true) {
        int in_fds = epoll_wait(epoll_fd, evs, 10, -1);

        if (in_fds == -1) {
            perror("epoll failed");
            break;
        }

        if (in_fds == 0) {
            std::cout << "timeout" << std::endl;
            continue;
        }

        for (int i = 0; i < in_fds; i++) {
            if (evs[i].data.fd == listen_sock) {
                sockaddr_in client;
                socklen_t client_len = sizeof(client);
                int client_sock = accept(listen_sock, (sockaddr *)&client, &client_len);
                std::cout << "accept client(socket=" << client_sock << ")" << std::endl;

                ev.data.fd = client_sock;
                ev.events = EPOLLIN;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &ev);
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