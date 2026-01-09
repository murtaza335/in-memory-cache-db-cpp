#include "server/server.hpp"
#include "parser/parser.hpp"
#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <pthread.h>
    #include <cstring>
#endif

// structure to pass parameters to client thread
struct ClientParam {
    TcpServer* server;
    sock_t clientSocket;
};

// constructor
TcpServer::TcpServer(int port, Parser& p)
    : port(port), serverSocket(INVALID_SOCK), running(false), parser(p)
{
    INIT_MUTEX(cs);
}

// destructor
TcpServer::~TcpServer() {
    stop();
    DESTROY_MUTEX(cs);
}

// start the server
bool TcpServer::start() {

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }
#endif

    // create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
    if (serverSocket == INVALID_SOCKET)
#else
    if (serverSocket < 0)
#endif
    {
        std::cerr << "Socket creation failed\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }

#ifdef _WIN32
    // allow quick reuse of port
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#endif

    // server address
    sockaddr_in serverAddr;
#ifdef _WIN32
    ZeroMemory(&serverAddr, sizeof(serverAddr));
#else
    memset(&serverAddr, 0, sizeof(serverAddr));
#endif

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

#ifdef _WIN32
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
#else
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
#endif
    {
        std::cerr << "Bind failed\n";
#ifdef _WIN32
        closesocket(serverSocket);
        WSACleanup();
#else
        close(serverSocket);
#endif
        return false;
    }

#ifdef _WIN32
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
#else
    if (listen(serverSocket, SOMAXCONN) < 0)
#endif
    {
        std::cerr << "Listen failed\n";
#ifdef _WIN32
        closesocket(serverSocket);
        WSACleanup();
#else
        close(serverSocket);
#endif
        return false;
    }

    running = true;
    std::cout << "Server started on port " << port << "\n";

    // accept loop
    while (running) {
        sockaddr_in clientAddr;
#ifdef _WIN32
        int addrSize = sizeof(clientAddr);
#else
        socklen_t addrSize = sizeof(clientAddr);
#endif

        sock_t clientSock = accept(serverSocket, (sockaddr*)&clientAddr, &addrSize);

#ifdef _WIN32
        if (clientSock == INVALID_SOCKET)
#else
        if (clientSock < 0)
#endif
        {
            if (!running) break;
            std::cerr << "Accept failed\n";
            continue;
        }

        ClientParam* param = new ClientParam{this, clientSock};

#ifdef _WIN32
        HANDLE hThread = CreateThread(nullptr, 0, clientThread, param, 0, nullptr);
        if (hThread != nullptr) CloseHandle(hThread); // detach
#else
        pthread_t tid;
        pthread_create(&tid, nullptr, clientThread, param);
        pthread_detach(tid);
#endif
    }

    return true;
}

// stop the server
void TcpServer::stop() {
    if (!running) return;

    running = false;

#ifdef _WIN32
    if (serverSocket != INVALID_SOCKET) closesocket(serverSocket);
    WSACleanup();
#else
    if (serverSocket >= 0) close(serverSocket);
#endif
    serverSocket = INVALID_SOCK;

    std::cout << "Server stopped.\n";
}

// static client thread function
thread_ret_t THREAD_CALL TcpServer::clientThread(void* param) {
    ClientParam* p = static_cast<ClientParam*>(param);
    if (p) {
        p->server->handleClient(p->clientSocket);
        delete p;
    }

#ifdef _WIN32
    return 0;
#else
    return nullptr;
#endif
}

// handle client communication
void TcpServer::handleClient(sock_t clientSocket) {
    char buffer[4096];

    while (running) {
#ifdef _WIN32
        ZeroMemory(buffer, sizeof(buffer));
        int bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
#else
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
#endif
        if (bytesRecv <= 0) break;

        std::string received(buffer, bytesRecv);
        std::string response = parser.route(received);

#ifdef _WIN32
        send(clientSocket, response.c_str(), (int)response.size(), 0);
#else
        send(clientSocket, response.c_str(), response.size(), 0);
#endif
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}
