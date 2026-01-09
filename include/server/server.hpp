#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include "parser/parser.hpp"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")

    using sock_t = SOCKET;
    using thread_ret_t = DWORD;
    #define THREAD_CALL WINAPI
    #define MUTEX_TYPE CRITICAL_SECTION
    #define INIT_MUTEX(cs) InitializeCriticalSection(&cs)
    #define DESTROY_MUTEX(cs) DeleteCriticalSection(&cs)
    #define LOCK_MUTEX(cs) EnterCriticalSection(&cs)
    #define UNLOCK_MUTEX(cs) LeaveCriticalSection(&cs)
    #define INVALID_SOCK INVALID_SOCKET

#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <pthread.h>
    #include <cstring>

    using sock_t = int;
    using thread_ret_t = void*;
    #define THREAD_CALL
    #define MUTEX_TYPE pthread_mutex_t
    #define INIT_MUTEX(cs) pthread_mutex_init(&cs, nullptr)
    #define DESTROY_MUTEX(cs) pthread_mutex_destroy(&cs)
    #define LOCK_MUTEX(cs) pthread_mutex_lock(&cs)
    #define UNLOCK_MUTEX(cs) pthread_mutex_unlock(&cs)
    #define INVALID_SOCK -1
#endif

class TcpServer {
public:
    TcpServer(int port, Parser& parser);
    ~TcpServer();

    bool start();
    void stop();

private:
    static thread_ret_t THREAD_CALL clientThread(void* param);
    void handleClient(sock_t clientSocket);

private:
    int port;
    sock_t serverSocket;
    bool running;
    MUTEX_TYPE cs;
    Parser& parser;
};

#endif
