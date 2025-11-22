#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

// Simple in-memory key-value store
std::unordered_map<std::string, std::string> store;

// Function to split a line into tokens
std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to handle commands
void handleCommand(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return;

    std::string command = tokens[0];

    if (command == "SET") {
        if (tokens.size() != 3) {
            std::cout << "ERROR: SET requires key and value\n";
            return;
        }
        store[tokens[1]] = tokens[2];
        std::cout << "OK\n";
    } else if (command == "GET") {
        if (tokens.size() != 2) {
            std::cout << "ERROR: GET requires a key\n";
            return;
        }
        auto it = store.find(tokens[1]);
        if (it != store.end())
            std::cout << it->second << "\n";
        else
            std::cout << "NULL\n";
    } else if (command == "DEL") {
        if (tokens.size() != 2) {
            std::cout << "ERROR: DEL requires a key\n";
            return;
        }
        if (store.erase(tokens[1]))
            std::cout << "OK\n";
        else
            std::cout << "NULL\n";
    } 
    else if(command == "EXPIRE"){
        if(tokens.size() != 3){
            std::cout << "ERROR: EXPIRE requires a key and a timeout value\n";
            return;
        }
        // here we will implement the expiring the key after the specified time
    }
    else {
        std::cout << "ERROR: Unknown command\n";
    }
}

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        auto tokens = tokenize(line);
        std::cout << "hey";
        handleCommand(tokens);
    }
    return 0;
}
