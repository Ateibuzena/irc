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

        bool                                ft_isvalidusername(const std::string& name);
        void                                ft_params(const std::string &rawMessage, Command &toret, unsigned long i, unsigned long size);
        void                                ft_lastparam(const std::string &rawMessage, Command &toret, unsigned long i, unsigned long size);
        void                                ft_parsecommand(Command toret);
        bool                                ft_checkchannel(std::string str);
        bool                                ft_checkkeys(std::string str);
        bool                                ft_checkchanneluser(std::string str);
        bool                                ft_checkskey(std::string str);
        bool                                ft_checknumber(std::string str);
        bool                                ft_checksinglechannel(std::string str);
        bool                                ft_checkflags(Command tocheck);

    public:

        void                                initCommands();
        Command                             parse(const std::string &rawMessage);

};

#endif