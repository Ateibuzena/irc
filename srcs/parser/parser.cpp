#include "../../includes/parser/Parser.hpp"

std::map<std::string, int> Parser::commands = {
    std::make_pair("PASS", 0),
    std::make_pair("NICK", 1),
    std::make_pair("USER", 2),
    std::make_pair("JOIN", 3),
    std::make_pair("PART", 4),
    std::make_pair("PRIVMSG", 5),
    std::make_pair("NOTICE", 6), // no obligatorio
    std::make_pair("QUIT", 7),
    std::make_pair("MODE", 10),
    std::make_pair("TOPIC", 11),
    std::make_pair("INVITE", 12),
    std::make_pair("KICK", 13),
};


Command Parser::parse(const std::string &rawMessage)
{
    if (rawMessage.size() < 2)
        throw ERR_UNKNOWN;

    if (rawMessage[rawMessage.size() - 1] != '\n' || rawMessage[rawMessage.size() - 2] != '\r')
        throw ERR_UNKNOWN;

    unsigned long i = 0;
    unsigned long size = rawMessage.size() - 2;
    Command toret;

    while (i < size && rawMessage[i] != ' ')
        i ++;
    if (i == 0)
        throw ERR_UNKNOWN;
    toret.name = rawMessage.substr(0, i);

    while (i < size && rawMessage[i] == ' ')
        i ++;

    if (i < size)
        this->ft_params(rawMessage, toret, i, size);

    toret.raw = rawMessage;

    ft_parsecommand(toret);

    return (toret);
}

void Parser::ft_params(const std::string &rawMessage, Command &toret, unsigned long i, unsigned long size)
{
    unsigned long u = i;

    if (rawMessage[i] == ':')
    {
        ft_lastparam(rawMessage, toret, i + 1, size);
        return ;
    }

    while (u < size && rawMessage[u] != ' ')
        u ++;

    toret.params.push_back(rawMessage.substr(i, u - i));

    while (u < size && rawMessage[u] == ' ')
        u ++;

    if (u < size)
        this->ft_params(rawMessage, toret, u, size);
}

void Parser::ft_lastparam(const std::string &rawMessage, Command &toret, unsigned long i, unsigned long size)
{
    if (i + 1 >= size)
    {
        toret.params.push_back("");
        return ;
    }
    toret.params.push_back(rawMessage.substr(i, size - i));
}

void Parser::ft_parsecommand(Command tocheck)
{
    const std::map<std::string, int>::iterator node = this->commands.find(tocheck.name);
    int command;

    if (node == this->commands.end())
        command = -1;
    else
        command = node->second;
    switch (command)
    {
        case 0:
            if (tocheck.params.size() < 1)
                throw ERR_NEEDMOREPARAMS;
            break;
        case 1:
            if (tocheck.params.size() < 1)
                throw ERR_NONICKNAMEGIVEN;
            if (this->ft_isvalidusername(tocheck.params[0]) != true)
                throw ERR_ERRONEUSNICKNAME;
            break;
        case 2:
            if (tocheck.params.size() < 4)
                throw ERR_NEEDMOREPARAMS;
            if (this->ft_isvalidusername(tocheck.params[0]) != true)
                throw ERR_ERRONEUSNICKNAME;
            if (tocheck.params[1].size() != 1)
                throw ERR_UMODEUNKNOWNFLAG;
            if (tocheck.params[1][0] != '0')
                throw ERR_UMODEUNKNOWNFLAG;
            break;
        case 3: // quitar
            if (tocheck.params.size() < 1)
                throw ERR_NEEDMOREPARAMS;
            break;
        case 4: //quitar
            if (tocheck.params.size() < 1)
                throw ERR_NEEDMOREPARAMS;
            break;
        case 5:  //quitar
            if (tocheck.params.size() < 1)
                throw ERR_NEEDMOREPARAMS;
            if (tocheck.params.size() == 1)
                throw ERR_NOTEXTTOSEND;
            break;
        case 6: //quitar
            if (tocheck.params.size() < 1)
                throw ERR_NEEDMOREPARAMS;
            if (tocheck.params.size() == 1)
                throw ERR_NOTEXTTOSEND;
            if (tocheck.params[1].size() < 1)
                throw ERR_NOTEXTTOSEND;
            break;
        case 7:
            if (tocheck.params.size() > 0)
                if (tocheck.params[0].size() < 1)
                 throw ERR_NOTEXTTOSEND;
            break;
        case 10:
            if (tocheck.params.size() < 1)
                throw ERR_NEEDMOREPARAMS;
            if (tocheck.params.size() > 1)
                if (!ft_checkflags(tocheck))
                    throw ERR_UMODEUNKNOWNFLAG;
            if (ft_checksinglechannel(tocheck.params[0]) == false)
                throw ERR_NOSUCHCHANNEL;
            break;
        case 11:
            if (tocheck.params.size() < 1)
                throw ERR_NEEDMOREPARAMS;
            if (ft_checksinglechannel(tocheck.params[0]) == false)
                throw ERR_NOSUCHCHANNEL;
            break;
        case 12:
            if (tocheck.params.size() < 2)
                throw ERR_NEEDMOREPARAMS;
            if (!ft_checksinglechannel(tocheck.params[1]))
                throw ERR_NOSUCHCHANNEL;
            if (!ft_isvalidusername(tocheck.params[0]))
                throw ERR_ERRONEUSNICKNAME;
            break;
        case 13:
            if (tocheck.params.size() < 2)
                throw ERR_NEEDMOREPARAMS;
            if (!ft_checksinglechannel(tocheck.params[1]))
                throw ERR_NOSUCHCHANNEL;
            if (!ft_isvalidusername(tocheck.params[0]))
                throw ERR_ERRONEUSNICKNAME;
            break;
        default:
            break;
    }
}

bool Parser::ft_isvalidusername(const std::string& name) {
    if (name.empty() || name.size() > 9)
        return (false);
    
    if (name[0] == '&' || name[0] == '#')
        return (false);
    
    for (unsigned int i = 0; i < name.length(); i ++) {
        if (name[i] == ' ' || name[i] == '@' || name[i] == '\r' || name[i] == '\n' || name[i] == '\0')
            return (false);
    }
    return (true);
}

bool Parser::ft_checkchannel(std::string str)
{
    unsigned long i = 0;
    unsigned long n = 0;
    unsigned long len = str.length();

    if (len < 2)
        return (false);

    while (n < len)
    {
        if (str[n] != '&' && str[n] != '#')
            return (false);
        n ++;
        i = n;
        while (i < len && ((str[i] > 64 && str[i] < 91) || (str[i] > 96 && str[i] < 123) || (str[i] > 47 && str[i] < 58) || str[i] == '-' || str[i] == '_'))
            i ++;
        if (i == n)
            return (false);
        if (i >= len)
            return (true);
        if (str[i] != ',')
            return (false);
        n = i + 1;
    }
    return (false);
}

bool Parser::ft_checksinglechannel(std::string str)
{
    unsigned long n = 0;
    unsigned long len = str.length();

    if (len < 2)
        return (false);
    if (str[n] != '&' && str[n] != '#')
        return (false);
    n ++;
    while (n < len)
    {
        if ((str[n] > 64 && str[n] < 91) || (str[n] > 96 && str[n] < 123) || (str[n] > 47 && str[n] < 58) || str[n] == '-' || str[n] == '_')
            n ++;
        else
            return (false);
    }
    return (true);
}

bool Parser::ft_checkkeys(std::string str)
{
    unsigned long i = 0;
    unsigned long n = 0;
    unsigned long len = str.length();

    if (len < 1)
        return (false);
    
    if (str[0] == ':')
        return (false);

    while (n < len)
    {
        i = n;
        while (i < len && ((str[i] > 32 && str[i] < 127 && str[i] != ',')))
            i ++;
        if (i == n)
            return (false);
        if (i >= len)
            return (true);
        if (str[i] != ',')
            return (false);
        n = i + 1;
    }
    return (false);
}

bool Parser::ft_checkskey(std::string str)
{
    unsigned long n = 0;
    unsigned long len = str.length();

    if (len < 1)
        return (false);
    
    if (str[0] == ':')
        return (false);

    while (n < len)
    {
        if (str[n] < 33 || str[n] > 126 || str[n] == ',')
            return (false);
        n ++;
    }
    return (true);
}

bool Parser::ft_checkchanneluser(std::string str)
{
    unsigned long i = 0;
    unsigned long n = 0;
    unsigned long len = str.length();

    if (len < 1)
        return (false);

    
    while (n < len)
    {
        if (str[n] == ',')
            return (false);
        if (str[n] == '&' || str[n] == '#')
        {
            n ++;
            i = n;
            while (i < len && ((str[i] > 64 && str[i] < 91) || (str[i] > 96 && str[i] < 123) || (str[i] > 47 && str[i] < 58) || str[i] == '-' || str[i] == '_'))
                i ++;
            if (i == n)
                return (false);
            if (i >= len)
                return (true);
            if (str[i] != ',')
                throw ERR_NOSUCHCHANNEL;
            n = i + 1;
        }
        else
        {
            i = n;
            if (str[i] == ':')
                return (false);
            while (i < len && str[i] != ' ' && str[i] != '@' && str[i] != '\r' && str[i] != '\n' && str[i] != '\0')
                i ++;
            if (i == n)
                return (false);
            if (i >= len)
                return (true);
            if (str[i] != ',')
                throw ERR_NOSUCHNICK;
            n = i + 1;
        }
    }
    return (false);
}

bool Parser::ft_checknumber(std::string str)
{
    unsigned long i = 0;

    for (; i < str.length(); i ++)
        if (!isdigit(str[i]))
            return (false);

    long double n = std::strtod(str.c_str(), NULL);
    if (n > 1000 || n < 1)
        return (false);
    return (true);
}

bool Parser::ft_checkflags(Command tocheck)
{
    unsigned long size = tocheck.params.size();

    if (tocheck.params[1][0] != '+' && tocheck.params[1][0] != '-')
        return (false);

    while (2 < tocheck.params.size())
    {
        if (tocheck.params[2][0] == '-' || tocheck.params[2][0] == '+')
        {
            tocheck.params[1] += tocheck.params[2];
            tocheck.params.erase(tocheck.params.begin() + 2);
        }
        else
            break;
    }
    
    return (true);
}
