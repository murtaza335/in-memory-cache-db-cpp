#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9090

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serverAddr;
    char buffer[1024] = {0};
    const char *msg = "this msg has been sent by a tcp client";

    // Initialize Winsock
    WSAStartup(MAKEWORD(2,2), &wsaData);

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // use inet_addr instead of inet_pton

    // Connect to server
    if(connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) {
        std::cout << "Connected to server!\n";
        send(sock, msg, strlen(msg), 0);
        int valread = recv(sock, buffer, sizeof(buffer), 0);
        std::cout << "Received from server: " << buffer << "\n";
    } else {
        std::cout << "Failed to connect\n";
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}
