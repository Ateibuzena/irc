#include "logic/Client.hpp"
#include <cctype>

// Función auxiliar para limpiar espacios al inicio y final
std::string trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");

    if (start == std::string::npos || end == std::string::npos)
        return ("");

    return (str.substr(start, end - start + 1));
}

bool    isValidNickname(const std::string& nickname)
{
    if (nickname.empty() || nickname.size() > 9)
        return (false);
    
    if (!std::isalpha(nickname[0])) // debe empezar con letra
        return (false);

    for (size_t i = 0; i < nickname.size(); ++i)
    {
        char c = nickname[i];
        if (!std::isalnum(c) && c != '-' && c != '_')
            return (false);
    }
    return (true);
}

bool    isValidUsername(const std::string& username)
{
    if (username.empty() || username.size() > 12)
        return (false);

    for (size_t i = 0; i < username.size(); ++i)
    {
        char c = username[i];
        if (!std::isalnum(c) && c != '-' && c != '_')
            return (false);
    }
    return (true);
}
