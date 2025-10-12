#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

//
// 🎨 Colores ANSI
//
#define RESET   "\033[0m"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[0;36m"
#define MAGENTA "\033[0;35m"
#define BOLDWHITE "\033[1;37m"

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