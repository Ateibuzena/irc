#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <sstream>
#include <vector>
#include "../Command.hpp"

class Parser {
public:
    Parser();
    ~Parser();

    Command parse(const std::string &rawMessage);

    // más adelante se conecta a la lógica:
    void dispatch(const Command &cmd, int clientFd);
};

#endif // PARSER_HPP

/*👉 El parser convierte std::string → Command y luego llama a ServerLogic::executeCommand().*/