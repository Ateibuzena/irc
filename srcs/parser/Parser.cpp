#include "../../includes/parser/Parser.hpp"

/*----------------------------------INIT COMMANDS----------------------------------*/

std::map<std::string, int> Parser::commands;

void Parser::initCommands()
{
    commands.insert(std::make_pair("PASS", 0));
    commands.insert(std::make_pair("NICK", 1));
    commands.insert(std::make_pair("USER", 2));
    commands.insert(std::make_pair("JOIN", 3));
    commands.insert(std::make_pair("PART", 4));
    commands.insert(std::make_pair("PRIVMSG", 5));
    commands.insert(std::make_pair("NOTICE", 6));
    commands.insert(std::make_pair("QUIT", 7));
    commands.insert(std::make_pair("MODE", 8));
    commands.insert(std::make_pair("TOPIC", 9));
    commands.insert(std::make_pair("INVITE", 10));
    commands.insert(std::make_pair("KICK", 11));
}


/*----------------------------------PARAMS----------------------------------*/

void Parser::ft_params(const std::string &rawMessage, ParsedInput &toret, unsigned long i, unsigned long size)
{
    unsigned long u = i;

    if (rawMessage[i] == ':')
    {
        ft_lastparam(rawMessage, toret, i + 1, size);
        return ;
    }

    while (u < size && rawMessage[u] != ' ')
        u++;

    toret.params.push_back(rawMessage.substr(i, u - i));

    while (u < size && rawMessage[u] == ' ')
        u++;

    if (u < size)
        ft_params(rawMessage, toret, u, size);
}

void Parser::ft_lastparam(const std::string &rawMessage, ParsedInput &toret, unsigned long i, unsigned long size)
{
    toret.params.push_back(rawMessage.substr(i, size - i));
}

/*----------------------------------USER VALIDATION----------------------------------*/

bool Parser::ft_isvalidusername(const std::string& name)
{
    if (name.empty() || name.size() > 9)
        return (false);
    
    if (name[0] == '&' || name[0] == '#')
        return (false);
    
    for (unsigned int i = 0; i < name.length(); i++)
    {
        if (name[i] == ' ' || name[i] == '@' || name[i] == '\r' || name[i] == '\n' || name[i] == '\0')
            return (false);
    }
    return (true);
}

/*bool Parser::ft_checkchanneluser(std::string str)
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
                throw ERR_NOSUCHNICKCHANNEL;
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
                throw ERR_NOSUCHNICKCHANNEL;
            n = i + 1;
        }
    }
    return (false);
}*/

/*----------------------------------CHANNEL VALIDATION----------------------------------*/

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
        n++;
        i = n;
        while (i < len && ((str[i] > 64 && str[i] < 91) || (str[i] > 96 && str[i] < 123) || (str[i] > 47 && str[i] < 58) || str[i] == '-' || str[i] == '_'))
            i++;
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
            n++;
        else
            return (false);
    }
    return (true);
}

/*----------------------------------KEYS VALIDATION----------------------------------*/

/*bool Parser::ft_checkkeys(std::string str)
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
}*/

bool Parser::ft_checkkey(std::string str)
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
        n++;
    }
    return (true);
}

/*----------------------------------FLAGS VALIDATION----------------------------------*/

bool Parser::ft_checkflags(ParsedInput tocheck)
{
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

/*----------------------------------NUMBER VALIDATION----------------------------------*/

/*bool Parser::ft_checknumber(std::string str)
{
    unsigned long i = 0;

    for (; i < str.length(); i ++)
        if (!isdigit(str[i]))
            return (false);

    long double n = std::strtod(str.c_str(), NULL);
    if (n > 1000 || n < 1)
        return (false);
    return (true);
}*/

/*----------------------------------PARSE----------------------------------*/

void Parser::ft_parsecommand(ParsedInput tocheck, const std::string& servername, const std::string& nickname)
{
    const std::map<std::string, int>::iterator node = commands.find(tocheck.name);
    int command;

    if (node == commands.end())
        command = -1;
    else
        command = node->second;

    std::string prefix = ":" + servername;
    std::string errorMsg;
    switch (command)
    {
        case 0: //pass
            if (tocheck.params.size() < 1)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NEEDMOREPARAMS].code + " " + nickname + " PASS",
                                            messagesError[ERR_NEEDMOREPARAMS].message);
                throw (errorMsg);
            }
            break ;
        case 1: //nick
            if (tocheck.params.size() < 1)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NEEDMOREPARAMS].code + " " + nickname + " NICK",
                                            messagesError[ERR_NEEDMOREPARAMS].message);
                throw (errorMsg);
            }
            if (!ft_isvalidusername(tocheck.params[0]))
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_ERRONEUSNICKNAME].code + " " + nickname + " " + tocheck.params[0],
                                            messagesError[ERR_ERRONEUSNICKNAME].message);
                throw (errorMsg);
            }
            break;
        case 2: //user
            if (tocheck.params.size() < 4)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NEEDMOREPARAMS].code + " " + nickname + " USER",
                                            messagesError[ERR_NEEDMOREPARAMS].message);
                throw (errorMsg);
            }
            if (!ft_isvalidusername(tocheck.params[0]))
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_ERRONEUSNICKNAME].code + " " + nickname + " " + tocheck.params[0],
                                            messagesError[ERR_ERRONEUSNICKNAME].message);
                throw (errorMsg);
            }
            if (tocheck.params[1].size() != 1 || (tocheck.params[1][0] < '0' || tocheck.params[1][0] > '9'))
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_UNKNOWNMODEFLAG].code + " " + nickname + " " + tocheck.params[1],
                                            messagesError[ERR_UNKNOWNMODEFLAG].message);
                throw (errorMsg);
            }
            if (tocheck.params[2][0] != '*')
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_UNKNOWNMODEFLAG].code + " " + nickname + " " + tocheck.params[2],
                                            messagesError[ERR_UNKNOWNMODEFLAG].message);
                throw (errorMsg);
            }
            break;
        case 3: //join
            if (tocheck.params.size() < 1)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NEEDMOREPARAMS].code + " " + nickname + " JOIN",
                                            messagesError[ERR_NEEDMOREPARAMS].message);
                throw (errorMsg);
            }
            break;
        case 4: //part
            if (tocheck.params.size() < 1)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NEEDMOREPARAMS].code + " " + nickname + " PART",
                                            messagesError[ERR_NEEDMOREPARAMS].message);
                throw (errorMsg);
            }
            break;
        case 5:  //privmsg
            if (tocheck.params.size() < 1)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NORECIPIENT].code + " " + nickname + " PRIVMSG",
                                            messagesError[ERR_NORECIPIENT].message);
                throw (errorMsg);
            }
            if (tocheck.params.size() == 1)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NOTEXTTOSEND].code + " " + nickname + " PRIVMSG",
                                            messagesError[ERR_NOTEXTTOSEND].message);
                throw (errorMsg);
            }
            break;
        case 6: //notice
            if (tocheck.params.size() < 1)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NORECIPIENT].code + " " + nickname + " NOTICE",
                                            messagesError[ERR_NORECIPIENT].message);
                throw (errorMsg);
            }
            if (tocheck.params.size() == 1 || tocheck.params[1].empty())
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NOTEXTTOSEND].code + " " + nickname + " NOTICE",
                                            messagesError[ERR_NOTEXTTOSEND].message);
                throw (errorMsg);
            }
            break;
        case 7: //quit
            if (tocheck.params.size() > 0)
            {
                if (tocheck.params[0].size() < 1)
                {
                    errorMsg = buildErrorMessage(prefix,
                                                messagesError[ERR_NOTEXTTOSEND].code + " " + nickname + " QUIT",
                                                messagesError[ERR_NOTEXTTOSEND].message);
                    throw (errorMsg);
                }
            }
            break;
        case 8: //mode
            if (tocheck.params.size() < 1)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NEEDMOREPARAMS].code + " " + nickname + " MODE",
                                            messagesError[ERR_NEEDMOREPARAMS].message);
                throw (errorMsg);
            }
            if (tocheck.params.size() > 1)
            {
                if (!ft_checkflags(tocheck))
                {
                    errorMsg = buildErrorMessage(prefix,
                                                messagesError[ERR_UNKNOWNMODEFLAG].code + " " + nickname + " " + tocheck.params[1],
                                                messagesError[ERR_UNKNOWNMODEFLAG].message);
                    throw (errorMsg);
                }
            }
            if (!ft_checksinglechannel(tocheck.params[0]))
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + nickname + " " + tocheck.params[0],
                                            messagesError[ERR_NOSUCHNICKCHANNEL].message);
                throw (errorMsg);
            }
            break;
        case 9: //topic
            if (tocheck.params.size() < 1)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NEEDMOREPARAMS].code + " " + nickname + " TOPIC",
                                            messagesError[ERR_NEEDMOREPARAMS].message);
                throw (errorMsg);
            }
            if (!ft_checksinglechannel(tocheck.params[0]))
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + nickname + " " + tocheck.params[0],
                                            messagesError[ERR_NOSUCHNICKCHANNEL].message);
                throw (errorMsg);
            }
            break;
        case 10: //invite
            if (tocheck.params.size() < 2)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NEEDMOREPARAMS].code + " " + nickname + " INVITE",
                                            messagesError[ERR_NEEDMOREPARAMS].message);
                throw (errorMsg);
            }
            if (!ft_isvalidusername(tocheck.params[0]))
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + nickname + " " + tocheck.params[1],
                                            messagesError[ERR_NOSUCHNICKCHANNEL].message);
                throw (errorMsg);
            }
            if (!ft_checksinglechannel(tocheck.params[1]))
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + nickname + " " + tocheck.params[0],
                                            messagesError[ERR_NOSUCHNICKCHANNEL].message);
                throw (errorMsg);
            }
            
            break;
        case 11: //kick
            if (tocheck.params.size() < 2)
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NEEDMOREPARAMS].code + " " + nickname + " KICK",
                                            messagesError[ERR_NEEDMOREPARAMS].message);
                throw (errorMsg);
            }
            if (!ft_checksinglechannel(tocheck.params[0]))
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + nickname + " " + tocheck.params[0],
                                            messagesError[ERR_NOSUCHNICKCHANNEL].message);
                throw (errorMsg);
            }
            if (!ft_isvalidusername(tocheck.params[1]))
            {
                errorMsg = buildErrorMessage(prefix,
                                            messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + nickname + " " + tocheck.params[1],
                                            messagesError[ERR_NOSUCHNICKCHANNEL].message);
                throw (errorMsg);
            }
            break;
        default:
        {
            errorMsg = buildErrorMessage(prefix,
                                        messagesError[ERR_UNKNOWN].code + " " + nickname + " " + tocheck.name,
                                        messagesError[ERR_UNKNOWN].message);
            throw (errorMsg);
        }
    }
}

ParsedInput Parser::parse(const std::string &rawMessage, const std::string& servername, const std::string& nickname)
{
    unsigned long   i = 0;
    unsigned long   size = rawMessage.size();  //quitado /r/n -2
    ParsedInput         toret;

    // Extract command name
    while (i < size && rawMessage[i] != ' ')
        i++;
    if (i == 0)
        return (toret);
    toret.name = rawMessage.substr(0, i);

    // Extract parameters
    while (i < size && rawMessage[i] == ' ')
        i++;
    if (i < size)
        ft_params(rawMessage, toret, i, size);

    // Store raw message
    toret.raw = rawMessage;

    // Validate command and parameters
    try
    {
        ft_parsecommand(toret, servername, nickname);
    }
    catch(const std::string& msg)
    {
        throw (msg);
    }
    
    return (toret);
}
