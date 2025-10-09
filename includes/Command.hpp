#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

struct Command {
    std::string name;                 // Ej: "NICK", "JOIN", "PRIVMSG"
    std::vector<std::string> params;  // Ej: {"#42Malaga", "hola mundo"}
    std::string raw;                  // Mensaje original (por debug)
};

#endif // COMMAND_HPP

/*común entre Parser y Lógica*/