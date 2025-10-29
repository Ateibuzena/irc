#include "../../includes/utils/Command.hpp"

std::string to_string_c98(int value)
{
    std::ostringstream  oss;

    oss << value;

    return (oss.str());
}

std::vector<std::string> str_to_vector(const std::string& str, char delimiter)
{
    std::vector<std::string>    result;
    size_t                      start = 0;
    size_t                      end = 0;

    end = str.find(delimiter, start);
    while (end != std::string::npos)
    {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    result.push_back(str.substr(start));
    
    return (result);
}