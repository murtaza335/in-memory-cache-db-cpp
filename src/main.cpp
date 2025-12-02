#include <iostream>
#include <string>
#include "storage/murmurhash/murmurhash3.hpp"
#include "server/server.hpp"
#include <conio.h>

int main() {
    // std::string s = "name";

    // uint32_t h = MurmurHash3_x86_32(s, 123); 

    // std::cout << "Hash = " << h << "\n";

    // std::cout<<"woohooooo lets goo";

    // we start the service
    TcpServer server(6379); // default Redis port
    
    if (server.start()) {
        // The server is now running and listening for clients
        // It will block here in start() until you stop it
    }

    getch();
    return 0;
}