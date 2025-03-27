#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

class ClientSocket {
   private:
    int my_sktfd;
    std::string my_ip;
    ushort my_port;

   public:
    ClientSocket() : my_sktfd(-1) {}
    ~ClientSocket() { close(); }

    bool connect(const std::string& in_ip, const ushort& in_port) {
        if (my_sktfd != -1) {
            return false;
        }

        my_ip = in_ip;
        my_port = in_port;
        my_sktfd = socket(AF_INET, SOCK_STREAM, 0);
        if (my_sktfd == -1) {
            perror("socket");
            return false;
        }
        struct hostent* host_ent;
        if ((host_ent = gethostbyname(in_ip.c_str())) == nullptr) {
            ::close(my_sktfd);
            my_sktfd = -1;
            return false;
        }
        struct sockaddr_in skaddr_in;
        memset(&skaddr_in, 0, sizeof(skaddr_in));
        skaddr_in.sin_family = AF_INET;
        memcpy(&skaddr_in.sin_addr, host_ent->h_addr_list[0],
               host_ent->h_length);
        skaddr_in.sin_port = htons(in_port);

        if (::connect(my_sktfd, (struct sockaddr*)&skaddr_in,
                      sizeof(skaddr_in)) == -1) {
            perror("::connect");
            ::close(my_sktfd);
            my_sktfd = -1;
            return false;
        }
        return true;
    }

    bool send(const std::string& buffer) {
        if (my_sktfd == -1) {
            return false;
        }
        if (::send(my_sktfd, buffer.data(), buffer.size(), 0) <= 0) {
            perror("::send");
            return false;
        }
        return true;
    }

    bool recv(std::string& buffer, size_t max_size) {
        if (my_sktfd == -1) {
            return false;
        }
        buffer.clear();
        buffer.resize(max_size);
        int read_size = ::recv(my_sktfd, &buffer[0], max_size, 0);
        if (read_size <= 0) {
            perror("::recv");
            buffer.clear();
            return false;
        }
        buffer.resize(read_size);
        return true;
    }

    bool close() {
        if (my_sktfd == -1) {
            return false;
        }
        ::close(my_sktfd);
        my_sktfd = -1;
        return true;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "输入错误" << std::endl;
        return -1;
    }
    ClientSocket clt_skt;
    if (clt_skt.connect(argv[1], atoi(argv[2]))) {
        std::string buffer;
        for (int i = 0; i < 3; i++) {
            buffer = "这是第" + std::to_string(i) +
                     "个超级女生,编号为:" + std::to_string(i);
            if (!clt_skt.send(buffer)) {
                perror("send");
                break;
            }
            std::cout << "发送：" << buffer << std::endl;
            if (!clt_skt.recv(buffer, 1024)) {
                perror("recv");
                break;
            }
            std::cout << "接收：" << buffer << std::endl;

            sleep(1);
        }
    }

    // int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    // if (socket_fd == -1) {
    //     perror("socket");
    //     return -1;
    // }
    // struct hostent* host_ent;
    // if ((host_ent = gethostbyname(argv[1])) == 0) {
    //     std::cout << "gethostbyname() failed" << std::endl;
    //     close(socket_fd);
    //     return -1;
    // }
    // struct sockaddr_in skaddr_in;
    // memset(&skaddr_in, 0, sizeof(skaddr_in));
    // skaddr_in.sin_family = AF_INET;
    // memcpy(&skaddr_in.sin_addr, host_ent->h_addr_list[0],
    // host_ent->h_length); skaddr_in.sin_port = htons(atoi(argv[2])); if
    // (connect(socket_fd, (struct sockaddr*)&skaddr_in, sizeof(skaddr_in)) ==
    //     -1) {
    //     perror("connect");
    //     close(socket_fd);
    //     return -1;
    // }

    // char buffer[1024];
    // for (int i = 0; i < 3; i++) {
    //     int iret;
    //     memset(buffer, 0, sizeof(buffer));
    //     sprintf(buffer, "这是第%d个超级女生，编号为%d。", i, i);
    //     if ((iret = send(socket_fd, buffer, sizeof(buffer), 0)) <= 0) {
    //         perror("send");
    //         break;
    //     }
    //     std::cout << "发送：" << buffer << std::endl;
    //     memset(buffer, 0, sizeof(buffer));
    //     if ((iret = recv(socket_fd, buffer, sizeof(buffer), 0)) <= 0) {
    //         std::cout << "iret=" << iret << std::endl;
    //         break;
    //     }
    //     std::cout << "接收：" << buffer << std::endl;

    //     sleep(1);
    // }
    // close(socket_fd);
}