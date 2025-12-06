#include <iostream>
#include <string>
#include "storage/murmurhash/murmurhash3.hpp"
#include "storage/RedisHashMap.hpp"
#include "parser/parser.hpp"
#include "server/server.hpp"
#include <conio.h>

int main() {

    // create a baseMap and then create it a parser and inject the baseMap into it
    // then create a server and inject the parser into it
    
    RedisHashMap baseMap(1024); 
    Parser parser(baseMap);

    TcpServer server(6379, parser);  // inject parser

    // we start the service
     // default Redis port
    
    if (server.start()) {
        // The server is now running and listening for clients
        // It will block here in start() until you stop it

        std::cout<<"check in main"<<std::endl;
    }



    getch();
    return 0;
}