#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int setSocketNonblocking(int sockfd) {
    // 1. 获取当前文件描述符标志
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL) failed");
        return -1; // 返回错误
    }

    // 2. 设置非阻塞标志
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL) failed");
        return -1; // 返回错误
    }

    return 0; // 成功
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "请输入正确的格式,再重新运行重试." << std::endl;
        return -1;
    }

    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    setSocketNonblocking(client_sock);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(client_sock, (sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        if (errno != EINPROGRESS) {
            std::cout << "connect(" << argv[1] << ":" << argv[2] << ") failed." << std::endl;
            close(client_sock);
            return -1;
        }
    }

    int epoll_fd = epoll_create(1);
    epoll_event ev, evs[1];
    ev.data.fd = client_sock;
    ev.events = EPOLLOUT | EPOLLERR;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &ev);
    int num_events = epoll_wait(epoll_fd, evs, 1, -1);
    if (num_events == -1) {
        perror("epoll_wait failed");
        close(epoll_fd);
        return -1;
    }
    if (evs[0].events & EPOLLERR) {
        std::cout << "connect failed (revents:" << evs[0].events << ")." << std::endl;
    } else {
        std::cout << "connect ok" << std::endl;
    }
    return 0;

    char buffer[1024];
    for (int i = 0; i < 200000; i++) {
        memset(buffer, 0, sizeof(buffer));
        std::cout << "please input:";
        std::cin >> buffer;
        if (send(client_sock, buffer, sizeof(buffer), 0) <= 0) {
            std::cout << "write() failed." << std::endl;
            close(client_sock);
            return -1;
        }

        memset(buffer, 0, sizeof(buffer));
        if (recv(client_sock, buffer, sizeof(buffer), 0) <= 0) {
            std::cout << "read() failed." << std::endl;
            close(client_sock);
            return -1;
        }
        std::cout << "recv:" << buffer << std::endl;
    }
    return 0;
}