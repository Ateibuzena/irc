#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

struct Command
{
    std::string name;                 // Ej: "NICK", "JOIN", "PRIVMSG"
    std::vector<std::string> params;  // Ej: {"#42Malaga", "hola mundo"}
    std::string raw;                  // Mensaje original (por debug)
};

enum ChannelStatus
{
    JOIN_SUCCESS,

    ALREADY_IN_CHANNEL,
    CHANNEL_FULL,          // <--- nuevo
    CHANNEL_EMPTY,

    INVALID_CLIENT,      // <--- nuevo
    
    LEAVE_SUCCESS,

    NOT_IN_CHANNEL
};

#include <sstream> // Para convertir int a string en C++98

// Función auxiliar para C++98
inline std::string to_string_c98(int value)
{
    std::ostringstream oss;
    oss << value;
    return (oss.str());
}

#endif // COMMAND_HPP

/*común entre Parser y Lógica*/