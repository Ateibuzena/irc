#include "../../includes/utils/Utils.hpp"

std::map<int, MessageInfo> messagesError;
std::map<int, MessageInfo> messagesReplay;

void initErrorMessages(void)
{
    messagesError[ERR_NEEDMOREPARAMS]    = MessageInfo("461", "Not enough parameters");
    messagesError[ERR_ALREADYREGISTERED] = MessageInfo("462", "You may not reregister");
    messagesError[ERR_PASSWDMISMATCH]    = MessageInfo("464", "Password incorrect");
    messagesError[ERR_NONICKNAMEGIVEN]   = MessageInfo("431", "No nickname given");
    messagesError[ERR_ERRONEUSNICKNAME]  = MessageInfo("432", "Erroneous nickname");
    messagesError[ERR_NICKNAMEINUSE]     = MessageInfo("433", "Nickname is already in use");
    messagesError[ERR_NOSUCHNICKCHANNEL] = MessageInfo("401", "No such nick/channel");
    messagesError[ERR_CANNOTSENDTOCHAN]  = MessageInfo("404", "Cannot send to channel");
    messagesError[ERR_TOOMANYCHANNELS]   = MessageInfo("405", "Too many channels");
    messagesError[ERR_TOOMANYTARGETS]    = MessageInfo("407", "Too many targets");
    messagesError[ERR_NOTONCHANNEL]      = MessageInfo("442", "You're not on that channel");
    messagesError[ERR_USERONCHANNEL]     = MessageInfo("443", "User is already on channel");
    messagesError[ERR_USERNOTINCHANNEL]  = MessageInfo("441", "User not in channel");
    messagesError[ERR_NOTREGISTERED]     = MessageInfo("451", "You have not registered");
    messagesError[ERR_BANNEDFROMCHAN]    = MessageInfo("474", "Banned from channel");
    messagesError[ERR_INVITEONLYCHAN]    = MessageInfo("473", "Cannot join channel (+i)");
    messagesError[ERR_CHANNELISFULL]     = MessageInfo("471", "Cannot join channel (+l)");
    messagesError[ERR_BADCHANNELKEY]     = MessageInfo("475", "Cannot join channel (+k)");
    messagesError[ERR_CHANOPRIVSNEEDED]  = MessageInfo("482", "You're not channel operator");
    messagesError[ERR_UNKNOWNMODEFLAG]   = MessageInfo("472", "Unknown mode flag");
    messagesError[ERR_NORECIPIENT]       = MessageInfo("411", "No recipient given (PRIVMSG)");
    messagesError[ERR_NOTEXTTOSEND]      = MessageInfo("412", "No text to send");
    messagesError[ERR_UNKNOWN]           = MessageInfo("421", "Unknown command");
}

void initReplayMessages(void)
{
    messagesReplay[RPL_WELCOME]        = MessageInfo("001", "Welcome to the IRC Network ");
    messagesReplay[RPL_YOURHOST]       = MessageInfo("002", "Your host is ");
    messagesReplay[RPL_CREATED]        = MessageInfo("003", "This server was created ");
    messagesReplay[RPL_MYINFO]         = MessageInfo("004", "Server info: ");
    messagesReplay[RPL_UMODEIS]        = MessageInfo("221", "User mode is");
    messagesReplay[RPL_CHANNELMODEIS]  = MessageInfo("324", "Channel mode is");
    messagesReplay[RPL_CREATIONTIME]   = MessageInfo("329", "Channel creation time");
    messagesReplay[RPL_NOTOPIC]        = MessageInfo("331", "No topic is set");
    messagesReplay[RPL_TOPIC]          = MessageInfo("332", "Topic");
    messagesReplay[RPL_TOPICWHOTIME]   = MessageInfo("333", "Topic set by");
    messagesReplay[RPL_NAMREPLY]       = MessageInfo("353", "Name reply");
    messagesReplay[RPL_ENDOFNAMES]     = MessageInfo("366", "End of NAMES list");
    messagesReplay[RPL_INVITING]       = MessageInfo("341", "Inviting");
}

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

std::string time_to_string(std::time_t t)
{
    std::tm*    timeinfo = std::localtime(&t);
    char        buffer[80];

    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

    return (std::string(buffer));
}

std::string buildMessage(const std::string& prefix,
                        const std::string& command,
                        const std::string& target,
                        const std::string& aux)
{
    // Construir el mensaje completo
    std::string fullMsg;

    if (command == "NICK"
        || command == "QUIT"
        || command == "JOIN")
    {
        fullMsg = prefix + " " + command + " :" + aux + "\r\n";
        return (fullMsg);
    }
    else if (command == "INVITE"
        || command == "TOPIC"
        || command == "PART"
        || command == "NOTICE"
        || command == "PRIVMSG")
    {
        fullMsg = prefix + " " + command + " " + target + " :" + aux + "\r\n";
        return (fullMsg);
    }
    else if (command == "KICK")
    {
        fullMsg = prefix + " " + command;
        return (fullMsg);
    }

    // Truncar si excede el máximo permitido
    if (fullMsg.size() > MAX_MESSAGE_LENGTH)
    {
        // Reservamos espacio para CRLF y los demás campos
        size_t maxLen = MAX_MESSAGE_LENGTH - (prefix.size() + command.size() + target.size() + 4);
        std::string truncated = aux.substr(0, maxLen);
        fullMsg = ":" + prefix + " " + command + " " + target + " :" + truncated + "\r\n";
    }

    return (fullMsg);
}

std::string buildErrorMessage(const std::string& prefix,
                            const std::string& aux,
                            const std::string& msg)
{
    std::string fullMsg = prefix + " ";

    if (!aux.empty())
    {
        fullMsg += aux + " ";
    }

    fullMsg += ":" + msg + "\r\n";

    return (fullMsg);
}
