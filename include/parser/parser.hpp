#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

class Parser {
public:
    Parser() = default;
    ~Parser() = default;

    // Parses raw input and returns response
    std::string process(const std::string& input);

private:
    // Helper: split string by space
    std::vector<std::string> split(const std::string& str);
};

#endif
