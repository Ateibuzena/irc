#include "../../../includes/logic/ServerLogic.hpp"

// Handle PASS command (set password) (Replies ready, Msg ready, Errors ready)
void    ServerLogic::handlePASS(Client* client, const ParsedInput& input)
{
    const std::string& password = input.params[0];

    std::string nickname = client->getNickname();
    if (nickname.empty())
        nickname = "*";

    if (client->isRegistered())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_ALREADYREGISTERED].code + " " + nickname;

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    "",
                                    messagesError[ERR_ALREADYREGISTERED].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    if (password != _serverPassword)
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_PASSWDMISMATCH].code + " " + nickname;

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    "",
                                    messagesError[ERR_PASSWDMISMATCH].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    client->setPassword(password);

    // If it already had username, nickname and password matches or there was none, mark as registered
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        setClientRegistered(client);
}

// Handle NICK command (set nickname) (Replies ready, Msg ready, Errors ready)
void    ServerLogic::handleNICK(Client* client, const ParsedInput& input)
{
    const std::string nickname = input.params[0];
    
    std::string oldNickname = client->getNickname();
    if (oldNickname.empty())
        oldNickname = "*";

    // Check if another client already has that nickname
    if (_serverNicknames.find(nickname) != _serverNicknames.end())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NICKNAMEINUSE].code + " " + nickname;

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    "",
                                    messagesError[ERR_NICKNAMEINUSE].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    // If it had a previous nickname, remove it from the map
    if (!client->getOldNickname().empty())
        _serverNicknames.erase(oldNickname);

    client->setOldNickname(oldNickname);
    
    // Assign the new nickname
    client->setNickname(nickname);

        // Send nickname change message
    if (client->isRegistered())
    {
        // Build NICK message
        _prefix.clear();
        _prefix = ":" + client->getOldNickname() + "!" + client->getUsername() + "@" + _serverHost;
        std::string nickMsg = buildMessage(_prefix, "NICK", "", client->getNickname());

        // Send to all channels the client is in
        std::set<std::string> channelsCopy = client->getChannels();
        std::set<std::string>::iterator it = channelsCopy.begin();
        while (it != channelsCopy.end())
        {
            std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(*it);
            if (chanIt != _serverChannels.end())
            {
                Channel* channel = chanIt->second;
                sendMessageToChannel(channel, nickMsg, client);
            }
            ++it;
        }
        sendMessageToClient(client, nickMsg);
    }

    // Add to the nicknames map
    _serverNicknames[nickname] = client;

    // If it already had username, nickname and password matches or there was none, mark as registered
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        setClientRegistered(client);
}

// Handle USER command (set username) (Replies ready, Msg ready, Errors ready)
void    ServerLogic::handleUSER(Client* client, const ParsedInput& input)
{
    const std::string username = input.params[0];

    std::string nickname = client->getNickname();
    if (nickname.empty())
        nickname = "*";

    if (client->isRegistered())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_ALREADYREGISTERED].code + " " + nickname;

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    "",
                                    messagesError[ERR_ALREADYREGISTERED].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    client->setUsername(username);

    // If it already had username, nickname and password matches or there was none, mark as registered
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        setClientRegistered(client);
}

// Handle QUIT command (disconnect client) (Replies ready, Msg ready, Errors ready)
void    ServerLogic::handleQUIT(Client* client, const ParsedInput& input)
{
    std::string nickname = client->getNickname();
    if (nickname.empty())
        nickname = "*";

    std::string msg = "Client disconnected";
    if (!input.params.empty() && !input.params[0].empty())
    {
        msg.clear();
        msg = input.params[0];
    }

    client->markForDisconnect();

    _prefix = ":" + nickname + "!" + client->getUsername() + "@" + _serverHost;
    std::string quitMsg = buildMessage(_prefix, "QUIT", "", msg);

    
    std::set<std::string> channelsCopy = client->getChannels();
    std::set<std::string>::const_iterator it = channelsCopy.begin();
    while (it != channelsCopy.end())
    {
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(*it);
        if (chanIt != _serverChannels.end())
        {
            Channel* channel = chanIt->second;

            // Send QUIT message to the other clients in the channel
            sendMessageToChannel(channel, quitMsg, client);
        }
        ++it;
    }
}



