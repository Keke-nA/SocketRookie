#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "请输入正确的格式,再重新运行重试." << std::endl;
        return -1;
    }

    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(client_sock, (sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        std::cout << "connect(" << argv[1] << ":" << argv[2] << ") failed." << std::endl;
        close(client_sock);
        return -1;
    }

    std::cout << "connect ok" << std::endl;

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