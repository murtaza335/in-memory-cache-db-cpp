#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <string>
#include <thread>
#include <atomic>

class TCPServer {
public:
    TCPServer(int port);
    ~TCPServer();

    void start();
    void stop();

private:
    int port;
    int server_fd;
    std::atomic<bool> running;
    std::thread serverThread;

    void run();
    void handleClient(int clientSocket);
};

#endif
