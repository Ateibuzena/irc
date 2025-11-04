#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "../utils/Command.hpp"

class Parser
{
    private:
        static std::map<std::string, int>   commands;

    public:
        static bool                                ft_isvalidusername(const std::string& name);
        static void                                ft_params(const std::string &rawMessage, Command &toret, unsigned long i, unsigned long size);
        static void                                ft_lastparam(const std::string &rawMessage, Command &toret, unsigned long i, unsigned long size);
        static void                                ft_parsecommand(Command toret);
        static bool                                ft_checkchannel(std::string str);
        static bool                                ft_checkkeys(std::string str);
        static bool                                ft_checkchanneluser(std::string str);
        static bool                                ft_checkskey(std::string str);
        static bool                                ft_checknumber(std::string str);
        static bool                                ft_checksinglechannel(std::string str);
        static bool                                ft_checkflags(Command tocheck);
        static void                                 initCommands();
        static Command                      parse(const std::string &rawMessage);

};

#endif