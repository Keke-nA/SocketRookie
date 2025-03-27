#include <functional>
#include <iostream>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <string>

class ChatServerTest {
  public:
    ChatServerTest(muduo::net::EventLoop *loop,
                   const muduo::net::InetAddress &listenaddr,
                   const std::string &servername)
        : server_loop(loop), chat_server(loop, listenaddr, servername) {
        chat_server.setConnectionCallback(std::bind(
            &ChatServerTest::onConnection, this, std::placeholders::_1));
        chat_server.setMessageCallback(
            std::bind(&ChatServerTest::onMessage, this, std::placeholders::_1,
                      std::placeholders::_2, std::placeholders::_3));
        chat_server.setThreadNum(2);
    }

    void onConnection(const muduo::net::TcpConnectionPtr &conn) {
        if (conn->connected()) {
            std::cout << conn->peerAddress().toIpPort() << "->"
                      << conn->localAddress().toIpPort() << "  state:online"
                      << std::endl;
        } else {
            std::cout << conn->peerAddress().toIpPort() << "->"
                      << conn->localAddress().toIpPort() << "  state:offline"
                      << std::endl;
            conn->shutdown();
        }
    }

    void onMessage(const muduo::net::TcpConnectionPtr &conn,
                   muduo::net::Buffer *buffer, muduo::Timestamp time) {
        std::string buf = buffer->retrieveAllAsString();
        std::cout << "recv data:" << buf;
        conn->send(buf);
    }

    void start() { chat_server.start(); }

  private:
    muduo::net::EventLoop *server_loop;
    muduo::net::TcpServer chat_server;
};

int main() {
    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr("127.0.0.1", 6000);
    ChatServerTest chat_server_test(&loop, addr, "ChatServerTest");
    chat_server_test.start();
    loop.loop();
    return 0;
}