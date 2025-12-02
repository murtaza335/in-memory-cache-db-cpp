#ifndef SERVER_HPP
#define SERVER_HPP

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

class TcpServer {
public:
    TcpServer(int port);
    ~TcpServer();

    bool start();
    void stop();

private:
    static DWORD WINAPI clientThread(LPVOID param);
    void handleClient(SOCKET clientSocket);

private:
    int port;
    SOCKET serverSocket;
    bool running;
    CRITICAL_SECTION cs;  // for thread-safe shared data
};

#endif
