#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

class ServerSocket {
   private:
    int my_sktfd;
    int my_clientfd;
    std::string my_ip;
    ushort my_port;
    /* data */
   public:
    ServerSocket() : my_sktfd(-1), my_clientfd(-1) {};
    ~ServerSocket() {
        closelisten();
        closeclient();
    };

    bool initServer(const ushort& in_port) {
        if (my_sktfd != -1) {
            return false;
        }
        my_sktfd = socket(AF_INET, SOCK_STREAM, 0);
        if (my_sktfd == -1) {
            return false;
        }

        my_port = in_port;
        struct sockaddr_in skaddr_in;
        memset(&skaddr_in, 0, sizeof(skaddr_in));
        skaddr_in.sin_family = AF_INET;
        skaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
        skaddr_in.sin_port = htons(my_port);

        if (bind(my_sktfd, (struct sockaddr*)&skaddr_in, sizeof(skaddr_in)) != 0) {
            perror("bind");
            my_sktfd = -1;
            close(my_sktfd);
            return false;
        }
        if (listen(my_sktfd, 5) == -1) {
            close(my_sktfd);
            my_sktfd = -1;
            return false;
        }
        return true;
    }

    bool accept() {
        struct sockaddr_in caddr;
        socklen_t addrlen = sizeof(caddr);
        if ((my_clientfd = ::accept(my_sktfd, (struct sockaddr*)&caddr, &addrlen)) == -1) {
            return false;
        }
        my_ip = inet_ntoa(caddr.sin_addr);
        std::cout << "客户端已连接。\n";
        return true;
    }

    const std::string& clientip() const { return my_ip; }

    bool send(const std::string& buffer) {
        if (my_clientfd == -1) {
            return false;
        }
        if ((::send(my_clientfd, buffer.data(), buffer.size(), 0)) <= 0) {
            return false;
        }
        return true;
    }

    bool recv(std::string& buffer, const size_t maxlen) {
        buffer.clear();
        buffer.resize(maxlen);
        int readn = ::recv(my_clientfd, &buffer[0], buffer.size(), 0);
        if (readn <= 0) {
            buffer.clear();
            return false;
        }
        buffer.resize(readn);
        return true;
    }

    bool closelisten() {
        if (my_sktfd == -1) {
            return false;
        }
        ::close(my_sktfd);
        my_sktfd = -1;
        return true;
    }

    bool closeclient() {
        if (my_clientfd == -1) {
            return false;
        }
        ::close(my_clientfd);
        my_clientfd = -1;
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Using:./demo2 通讯端口\nExample:./demo2 5005\n\n";  // 端口大于1024，不与其它的重复。
        std::cout << "注意:运行服务端程序的Linux系统的防火墙必须要开通5005端口。\n";
        std::cout << "      如果是云服务器，还要开通云平台的访问策略。\n\n";
        return -1;
    }

    ServerSocket server_skt;
    if (!server_skt.initServer(atoi(argv[1]))) {
        perror("initServer");
        return -1;
    }

    if (!server_skt.accept()) {
        perror("accept");
        return -1;
    }

    std::string buffer;
    while (true) {
        if (server_skt.recv(buffer, 1024) == false) {
            perror("recv()");
            break;
        }
        std::cout << "接收：" << buffer << std::endl;
        buffer = "ok";
        if (server_skt.send(buffer) == false) {
            perror("send");
            break;
        }
        std::cout << "发送：" << buffer << std::endl;
    }

    /*int my_sktfd = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sktfd == -1) {
        return -1;
    }
    struct sockaddr_in skaddr_in;
    memset(&skaddr_in, 0, sizeof(skaddr_in));
    skaddr_in.sin_family = AF_INET;
    skaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    skaddr_in.sin_port = htons(atoi(argv[1]));

    if (bind(my_sktfd, (struct sockaddr*)&skaddr_in, sizeof(skaddr_in)) != 0) {
        perror("bind");
        close(my_sktfd);
        return -1;
    }

    if (listen(my_sktfd, 5) != 0) {
        perror("listen");
        close(my_sktfd);
        return -1;
    }

    int clientfd = accept(my_sktfd, 0, 0);
    if (clientfd == -1) {
        perror("accept");
        close(my_sktfd);
        return -1;
    }

    std::cout << "客户端已连接。\n";

    // 第5步：与客户端通信，接收客户端发过来的报文后，回复ok。
    char buffer[1024];
    while (true) {
        int iret;
        memset(buffer, 0, sizeof(buffer));
        //
    接收客户端的请求报文，如果客户端没有发送请求报文，recv()函数将阻塞等待。
        // 如果客户端已断开连接，recv()函数将返回0。
        if ((iret = recv(clientfd, buffer, sizeof(buffer), 0)) <= 0) {
            std::cout << "iret=" << iret << std::endl;
            break;
        }
        std::cout << "接收：" << buffer << std::endl;

        strcpy(buffer, "ok");  // 生成回应报文内容。
        // 向客户端发送回应报文。
        if ((iret = send(clientfd, buffer, strlen(buffer), 0)) <= 0) {
            perror("send");
            break;
        }
        std::cout << "发送：" << buffer << std::endl;
    }

    // 第6步：关闭socket，释放资源。
    close(my_sktfd);  // 关闭服务端用于监听的socket。
    close(clientfd);  // 关闭客户端连上来的socket。*/
}