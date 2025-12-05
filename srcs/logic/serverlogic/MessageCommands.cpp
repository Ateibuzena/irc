#include "../../../includes/logic/ServerLogic.hpp"

// Handle the NOTICE command (send a notice) (Replies ready, Msg ready, Errors ready)
void    ServerLogic::handleNOTICE(Client* client, const ParsedInput& input)
{
    std::string clientNickname = client->getNickname();
    if (clientNickname.empty())
        clientNickname = "*";

    // Check that the client is registered
    if (!client->isRegistered())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOTREGISTERED].code + " " + clientNickname;

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    "",
                                    messagesError[ERR_NOTREGISTERED].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    // Get the list of receivers
    std::vector<std::string> receivers = str_to_vector(input.params[0], ',');

    // Get the message
    std::string msg = input.params[1];

    size_t i = 0;
    while (i < receivers.size())
    {
        // Get the current receiver
        const std::string& target = receivers[i];
        if (target.empty())
        {
            i++;
            continue ;
        }

        // Look for a client
        std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(target);
        if (nickIt != _serverNicknames.end())
        {
            Client* recipient = nickIt->second;

            // Build the NOTICE message to notify another client
            _prefix.clear();
            _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;

            std::string noticeMsg = buildMessage(_prefix,
                                                "NOTICE",
                                                recipient->getNickname(),
                                                msg);

            // Send the message to the recipient
            sendMessageToClient(recipient, noticeMsg);
        }
        
        // Look for a channel
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(target);
        if (chanIt != _serverChannels.end())
        {
            Channel* channel = chanIt->second;

            // Build the NOTICE message to notify a channel
            _prefix.clear();
            _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;

            std::string noticeMsg = buildMessage(_prefix, "NOTICE", channel->getName(), msg);

            // Send the message to all clients in the channel
            sendMessageToChannel(channel, noticeMsg, client);
        }
        i++;
    }
}

// Handle the PRIVMSG command (send private message) (Replies ready, Msg ready, Errors ready)
void    ServerLogic::handlePRIVMSG(Client* client, const ParsedInput& input)
{
    std::string clientNickname = client->getNickname();
    if (clientNickname.empty())
        clientNickname = "*";

    // Check that the client is registered
    if (!client->isRegistered())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOTREGISTERED].code + " " + clientNickname;
        
        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    "",
                                    messagesError[ERR_NOTREGISTERED].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    // Get the list of receivers
    std::vector<std::string> receivers = str_to_vector(input.params[0], ',');

    // Get the message
    std::string msg = input.params[1];

    size_t i = 0;
    while (i < receivers.size())
    {
        // Get the current receiver
        const std::string& target = receivers[i];
        if (target.empty())
        {
            i++;
            continue ;
        }

        // Look for a client
        std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(target);
        if (nickIt != _serverNicknames.end())
        {
            Client* recipient = nickIt->second;

            // Build the PRIVMSG message to notify another client
            _prefix.clear();
            _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;

            std::string privMsg = buildMessage(_prefix, "PRIVMSG", recipient->getNickname(), msg);

            // Send the message to the recipient
            sendMessageToClient(recipient, privMsg);
            
            i++;
            continue ;
        }

        // Look for a channel
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(target);
        if (chanIt != _serverChannels.end())
        {
            Channel* channel = chanIt->second;
            if (!channel->hasClient(client))
            {
                i++;
                continue ;
            }

            // Build the PRIVMSG message to notify a channel
            _prefix.clear();
            _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;

            std::string privMsg = buildMessage(_prefix, "PRIVMSG", channel->getName(), msg);

            sendMessageToChannel(channel, privMsg, client);
            
            i++;
            continue ;
        }

        // If not a client or channel, send an error
        if (target[0] == '#' || target[0] == '&')
        {
            _prefix.clear();
            _prefix = ":" + _serverName + " " + messagesError[ERR_CANNOTSENDTOCHAN].code + " " + client->getNickname() + " " + target;

            _errorMsg.clear();
            _errorMsg = buildErrorMessage(_prefix,
                                        "",
                                        messagesError[ERR_CANNOTSENDTOCHAN].message);
        }
        else
        {
            _prefix.clear();
            _prefix = ":" + _serverName + " " + messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname() + " " + target;

            _errorMsg.clear();
            _errorMsg = buildErrorMessage(_prefix,
                                        "",
                                        messagesError[ERR_NOSUCHNICKCHANNEL].message);
        }
        sendMessageToClient(client, _errorMsg);
        i++;
    }
}
