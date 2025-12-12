#include "server/server.hpp"
#include "parser/parser.hpp"
#include <iostream>

// structure to pass parameters to the client thread
struct ClientParam {
    TcpServer* server;   // pointer to the tcp server instance
    SOCKET clientSocket; // the client socket that this thread will handle
};

// constructor for tcp server
// takes port number and a reference to the parser
TcpServer::TcpServer(int port, Parser& p)
    : port(port), serverSocket(INVALID_SOCKET), running(false), parser(p) 
{
    InitializeCriticalSection(&cs); // initialize critical section for thread safety
}

// destructor for tcp server
TcpServer::~TcpServer() {
    stop(); // stop the server gracefully
    DeleteCriticalSection(&cs); // delete the critical section
}

// start the tcp server
bool TcpServer::start() {
    WSADATA wsa;
    // initialize winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false; // if fails, return false
    }

    // create a tcp socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return false;
    }

    // set up the server address structure
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET; // ipv4
    serverAddr.sin_port = htons(port); // convert port to network byte order
    serverAddr.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces

    // bind the socket to the address and port
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    // start listening for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return false;
    }

    running = true; // mark the server as running
    std::cout << "Server started on port " << port << "\n";

    // main server loop to accept incoming clients
    while (running) {
        sockaddr_in clientAddr;
        int addrSize = sizeof(clientAddr);
        // accept a new client connection
        SOCKET clientSock = accept(serverSocket, (sockaddr*)&clientAddr, &addrSize);
        if (clientSock == INVALID_SOCKET) {
            if (!running) break; // if server stopped exit loop
            std::cerr << "Accept failed\n";
            continue; // continue to next iteration
        }

        // create a parameter structure for the client thread
        ClientParam* param = new ClientParam;
        param->server = this; // assign server pointer
        param->clientSocket = clientSock; // assign client socket

        // create a new thread to handle this client
        HANDLE hThread = CreateThread(NULL, 0, clientThread, param, 0, NULL);
        if (hThread != NULL) CloseHandle(hThread); // close handle to detach thread
    }

    return true; // server started successfully
}

// stop the tcp server
void TcpServer::stop() {
    if (!running) return; // if server not running, do nothing

    running = false; // mark server as stopped
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket); // close the listening socket
        serverSocket = INVALID_SOCKET;
    }

    WSACleanup(); // cleanup winsock resources
    std::cout << "Server stopped.\n";
}

// static function for client thread
// each thread handles a single client
DWORD WINAPI TcpServer::clientThread(LPVOID param) {
    ClientParam* p = (ClientParam*)param; // cast parameter
    if (p) {
        p->server->handleClient(p->clientSocket); // call servers handleClient
        delete p; // delete parameter to free memory
    }
    return 0;
}

// handle communication with a single client
void TcpServer::handleClient(SOCKET clientSocket) {
    char buffer[4096]; // buffer to store received data
    int bytesRecv;

    while (running) {
        ZeroMemory(buffer, 4096); // clear buffer
        bytesRecv = recv(clientSocket, buffer, 4096, 0); // receive data from client
        if (bytesRecv <= 0) break; // if client disconnects or error break loop

        std::string received(buffer, bytesRecv); // convert buffer to string

        // call parser to process received data
        std::string response = parser.route(received);

        // send the response back to the client
        send(clientSocket, response.c_str(), (int)response.size(), 0);
    }

    closesocket(clientSocket); // close the client socket when done
}
