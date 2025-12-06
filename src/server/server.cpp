#include "server/server.hpp"
#include "parser/parser.hpp"
#include <iostream>

struct ClientParam {
    TcpServer* server;
    SOCKET clientSocket;
};

TcpServer::TcpServer(int port)
    : port(port), serverSocket(INVALID_SOCKET), running(false) {
    InitializeCriticalSection(&cs);
}

TcpServer::~TcpServer() {
    stop();
    DeleteCriticalSection(&cs);
}

bool TcpServer::start() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    running = true;
    std::cout << "Server started on port " << port << "\n";

    while (running) {
        sockaddr_in clientAddr;
        int addrSize = sizeof(clientAddr);
        SOCKET clientSock = accept(serverSocket, (sockaddr*)&clientAddr, &addrSize);
        if (clientSock == INVALID_SOCKET) {
            if (!running) break;
            std::cerr << "Accept failed\n";
            continue;
        }

        // Create thread for client
        ClientParam* param = new ClientParam;
        param->server = this;
        param->clientSocket = clientSock;

        HANDLE hThread = CreateThread(NULL, 0, clientThread, param, 0, NULL);
        if (hThread != NULL) CloseHandle(hThread); // detach
    }

    return true;
}

void TcpServer::stop() {
    if (!running) return;

    running = false;
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    WSACleanup();
    std::cout << "Server stopped.\n";
}

DWORD WINAPI TcpServer::clientThread(LPVOID param) {
    ClientParam* p = (ClientParam*)param;
    if (p) {
        p->server->handleClient(p->clientSocket);
        delete p;
    }
    return 0;
}

void TcpServer::handleClient(SOCKET clientSocket) {
    char buffer[4096];
    int bytesRecv;

    while (running) {
        ZeroMemory(buffer, 4096);
        bytesRecv = recv(clientSocket, buffer, 4096, 0);
        if (bytesRecv <= 0) break;

        std::string received(buffer, bytesRecv);

        // here we call the parser to process the received data
        
        std::string response = parser.route(received);
        send(clientSocket, response.c_str(), (int)response.size(), 0);
        // ---------------------------------------------------------------------
        // 🔵 HOOK: CALL YOUR PARSER HERE
        //
        // Example:
        //     std::string response = parser.process(received);
        //     send(clientSocket, response.c_str(), response.size(), 0);
        //
        // For now, just echo:
        // ---------------------------------------------------------------------

        // std::string echo = "ECHO: " + received;
        // send(clientSocket, echo.c_str(), (int)echo.size(), 0);
    }

    closesocket(clientSocket);
}
