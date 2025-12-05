#ifndef PARSER_HPP
#define PARSER_HPP

# include <string>
# include <sstream>
# include <vector>
# include <map>
# include "../utils/Utils.hpp"

class Parser
{
    private:

        static std::map<std::string, int>   commands;

    public:

        static void                         initCommands();

        static void                         ft_params(const std::string &rawMessage, ParsedInput &toret, unsigned long i, unsigned long size);
        static void                         ft_lastparam(const std::string &rawMessage, ParsedInput &toret, unsigned long i, unsigned long size);
        
        static bool                         ft_isvalidusername(const std::string& name);
        //static bool                         ft_checkchanneluser(std::string str);

        static bool                         ft_checkchannel(std::string str);
        static bool                         ft_checksinglechannel(std::string str);

        //static bool                         ft_checkkeys(std::string str);
        static bool                         ft_checkkey(std::string str);

        static bool                         ft_checkflags(ParsedInput tocheck);
        
        //static bool                         ft_checknumber(std::string str);

        static void                         ft_parsecommand(ParsedInput tocheck, const std::string& servername, const std::string& nickname);
        static ParsedInput                  parse(const std::string &rawMessage, const std::string& servername, const std::string& nickname);
};

#endif