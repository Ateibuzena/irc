#include "logic/Channel.hpp"
#include <cctype>

bool    isValidName(const std::string& name)
{
    if (name.empty() || (name[0] != '#' && name[0] != '&'))
        return (false);
    return (name.find(' ') == std::string::npos);
}
