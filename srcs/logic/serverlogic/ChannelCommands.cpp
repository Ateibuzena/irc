#include "../../../includes/logic/ServerLogic.hpp"

// Handle INVITE command (review multi-channel invites) (Replies ready, Msg ready, Errors ready)
void    ServerLogic::handleINVITE(Client* client, const ParsedInput& input)
{
    const std::string& nickname = input.params[0];
    const std::string& channelName = input.params[1];

    std::string clientNickname = client->getNickname();
    if (clientNickname.empty())
        clientNickname = "*";

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

    // First look for the channel
    std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(channelName);
    if (chanIt == _serverChannels.end())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    channelName,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, _errorMsg));
    }
    Channel* channel = chanIt->second;

    // Check that the executor is in the channel
    if (!channel->hasClient(client))
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOTONCHANNEL].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    channel->getName(),
                                    messagesError[ERR_NOTONCHANNEL].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    // If the channel is invite-only, only operators can invite
    if (channel->isInviteOnly() && !channel->isOperator(client))
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_CHANOPRIVSNEEDED].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    channel->getName(),
                                    messagesError[ERR_CHANOPRIVSNEEDED].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    std::set<Client*> clientsCopy = channel->getClients();

    // If the channel is full, return error
    if (clientsCopy.size() >= channel->getMaxClients())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_CHANNELISFULL].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    channel->getName(),
                                    messagesError[ERR_CHANNELISFULL].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    // Then search for the client to invite
    std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(nickname);
    if (nickIt == _serverNicknames.end())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    nickname,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, _errorMsg));
    }
    Client* invitedClient = nickIt->second;

    // Invite the client to the channel
    channel->inviteClient(invitedClient);

    // Build the message of INVITE to send to the invited client
    _prefix.clear();
    _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
    std::string inviteMsg = buildMessage(_prefix,
                                        "INVITE",
                                        invitedClient->getNickname(),
                                        channel->getName());

    // Send the invitation message to the invited client
    sendMessageToClient(invitedClient, inviteMsg);

    // Build the confirmation message for the inviter
    _replyMsg.clear();
    _replyMsg = buildReplyMessage(messagesReplay[RPL_INVITING].code,
                                            client,
                                            invitedClient->getNickname(),
                                            channel->getName(),
                                            "");

    // Send the confirmation message to the inviter
    sendMessageToClient(client, _replyMsg);
}

// Handle KICK command (kick user from channel) (Replies ready, Msg ready, Errors ready)
void    ServerLogic::handleKICK(Client* client, const ParsedInput& input)
{
    std::string clientNickname = client->getNickname();
    if (clientNickname.empty())
        clientNickname = "*";

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

    const std::string& channelName = input.params[0];
    const std::string& targetNickname = input.params[1];

    // First look for the channel
    std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(channelName);
    if (chanIt == _serverChannels.end())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    channelName,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, _errorMsg));
    }
    Channel* channel = chanIt->second;

    // Check that the executor is in the channel
    if (!channel->hasClient(client))
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOTONCHANNEL].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    channel->getName(),
                                    messagesError[ERR_NOTONCHANNEL].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    // If the client is not an operator, return error
    if (!channel->isOperator(client))
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_CHANOPRIVSNEEDED].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    channel->getName(),
                                    messagesError[ERR_CHANOPRIVSNEEDED].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    // Search for the client to kick
    std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(targetNickname);
    if (nickIt == _serverNicknames.end())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    targetNickname,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, _errorMsg));
    }
    Client* targetClient = nickIt->second;

    // Check that the client to be kicked is in the channel
    if (!channel->hasClient(targetClient))
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_USERNOTINCHANNEL].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    targetClient->getNickname() + " " + channel->getName(),
                                    messagesError[ERR_USERNOTINCHANNEL].message);
        return (sendMessageToClient(client, _errorMsg));
    }
    
    // Build the KICK message to send to everyone
    std::string msg = "Kicked";
    if (input.params.size() > 2)
    {
        msg.clear();
        msg = input.params[2];
    }

    // Build the message of KICK to send to everyone
    _prefix.clear();
    _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;

    std::string kickMsg = buildMessage(_prefix, "KICK", "", "");
    kickMsg += " " + channel->getName() + " " + targetClient->getNickname() + " :" + msg + "\r\n";

    // Notify all channel members
    sendMessageToChannel(channel, kickMsg, NULL);

    // Remove the client from the channel
    channel->removeClient(targetClient);
    handleOperator(channel, targetClient);
    targetClient->leaveChannel(channel);

    // If channel is empty, remove it from the server
    if (channel->getDeleteMe())
    {
        delete (channel);
        _serverChannels.erase(chanIt);
    }
}

// Handle TOPIC command (view or set channel topic) (Replies ready, Msg ready, Errors ready)
void    ServerLogic::handleTOPIC(Client* client, const ParsedInput& input)
{
    const std::string& channelName = input.params[0];

    // Check if client is a member of the channel
    std::string clientNickname = client->getNickname();
    if (clientNickname.empty())
        clientNickname = "*";

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
    
    // First look for the channel
    std::map<std::string, Channel*>::iterator it = _serverChannels.find(channelName);
    if (it == _serverChannels.end())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    channelName,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, _errorMsg));
    }
    Channel* channel = it->second;

    // Check if client is a member of the channel
    if (!channel->hasClient(client))
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOTONCHANNEL].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    channel->getName(),
                                    messagesError[ERR_NOTONCHANNEL].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    // If only one parameter, show current topic
    if (input.params.size() == 1)
    {
        std::string topic = channel->getTopic();

        // If no topic is set
        if (topic.empty())
        {
            _replyMsg.clear();
            _replyMsg = buildReplyMessage(messagesReplay[RPL_NOTOPIC].code,
                                        client,
                                        channelName,
                                        "",
                                        messagesReplay[RPL_NOTOPIC].message);
        }
        else // There is a set topic
        {
            std::string setter = channel->getSetter();
            std::string timeSet = channel->getTimeSet();

            _replyMsg.clear();
            _replyMsg = buildReplyMessage(messagesReplay[RPL_TOPIC].code,
                                        client,
                                        channelName,
                                        "",
                                        topic);
            _replyMsg += buildReplyMessage(messagesReplay[RPL_TOPICWHOTIME].code,
                                        client,
                                        channelName,
                                        setter + " " + timeSet,
                                        "");
        }

        // Send the message to the client
        sendMessageToClient(client, _replyMsg);

        return ;
    }

    // If client wants to change the topic, must be an operator and the channel must allow it
    if (channel->isTopicProtected() && !channel->isOperator(client))
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_CHANOPRIVSNEEDED].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    channel->getName(),
                                    messagesError[ERR_CHANOPRIVSNEEDED].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    // Set the new topic
    std::string newTopic = input.params[1];
    channel->setTopic(newTopic);

    // Update setter
    channel->setSetter(client->getNickname());

    // Update timeSet
    std::time_t now = std::time(NULL);
    channel->setTimeSet(time_to_string(now));

    // Build the message of TOPIC to send to everyone
    _prefix.clear();
    _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
    std::string topicMsg = buildMessage(_prefix, "TOPIC", channel->getName(), newTopic);

    // Notify all channel members
    sendMessageToChannel(channel, topicMsg, NULL);
}

// Handle JOIN command (join channel) (Replies ready, Msg ready, Errors ready)
void    ServerLogic::handleJOIN(Client* client, const ParsedInput& input) 
{
    std::string clientNickname = client->getNickname();
    if (clientNickname.empty())
        clientNickname = "*";

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

    // The first parameter contains the list of channels separated by ','
    std::string channelsParam = input.params[0];
    std::vector<std::string> channelNames;
    std::string temp;
    size_t i = 0;

    while (i < channelsParam.size())
    {
        if (channelsParam[i] == ',')
        {
            if (!temp.empty())
                channelNames.push_back(temp);
            temp.clear();
        }
        else
            temp += channelsParam[i];
        i++;
    }
    if (!temp.empty())
        channelNames.push_back(temp);
    
    // The second parameter (opcional) contains keys for the channels
    std::vector<std::string> keys;
    if (input.params.size() > 1)
    {
        std::string keysParam = input.params[1];
        temp.clear();
        i = 0;
        while (i < keysParam.size())
        {
            if (keysParam[i] == ',')
            {
                if (!temp.empty())
                    keys.push_back(temp);
                temp.clear();
            }
            else
                temp += keysParam[i];
            i++;
        }
        if (!temp.empty())
            keys.push_back(temp);
    }

    // Associate each channel with its key (if present)
    i = 0;
    while (i < channelNames.size())
    {
        const std::string& channelName = channelNames[i];
        std::string key;
        if (i < keys.size())
            key = keys[i];
        else
            key = "";
        try
        {
            Channel* channel = createChannel(channelName, client);
            
            if (channel == NULL || channel->hasClient(client))
                return ;
            
            if (!channel->getPassword().empty()) // Channel has a key
            {
                if (key.empty() || channel->getPassword() != key)
                {
                    _prefix.clear();
                    _prefix = ":" + _serverName + " " + messagesError[ERR_BADCHANNELKEY].code + " " + client->getNickname() + " ";

                    _errorMsg.clear();
                    _errorMsg = buildErrorMessage(_prefix,
                                                channel->getName(),
                                                messagesError[ERR_BADCHANNELKEY].message);
                    return (sendMessageToClient(client, _errorMsg));
                }
            }
            else if (channel->getClients().empty()) // Channel is new
            {
                // If a key is provided, set it
                if (key != "" && !Parser::ft_checkkey(key))
                {
                    _prefix.clear();
                    _prefix = ":" + _serverName + " " + messagesError[ERR_BADCHANNELKEY].code + " " + client->getNickname() + " ";

                    _errorMsg.clear();
                    _errorMsg = buildErrorMessage(_prefix,
                                                channel->getName(),
                                                messagesError[ERR_BADCHANNELKEY].message);                    
                    return (sendMessageToClient(client, _errorMsg));
                }
                channel->setPassword(key);
            }

            // If the channel is invite-only and the client is not invited, return error
            if (channel->isInviteOnly() && !channel->isInvited(client))
            {
                _prefix.clear();
                _prefix = ":" + _serverName + " " + messagesError[ERR_INVITEONLYCHAN].code + " " + client->getNickname() + " ";

                _errorMsg.clear();
                _errorMsg = buildErrorMessage(_prefix,
                                            channel->getName(),
                                            messagesError[ERR_INVITEONLYCHAN].message);
                return (sendMessageToClient(client, _errorMsg));
            }
            
            // If the channel is full, return error
            if (channel->getClients().size() >= channel->getMaxClients())
            {
                _prefix.clear();
                _prefix = ":" + _serverName + " " + messagesError[ERR_CHANNELISFULL].code + " " + client->getNickname() + " ";

                _errorMsg.clear();
                _errorMsg = buildErrorMessage(_prefix,
                                            channel->getName(),
                                            messagesError[ERR_CHANNELISFULL].message);
                return (sendMessageToClient(client, _errorMsg));
            }

            channel->addClient(client);
            client->joinChannel(channel);
            
            // Build the JOIN message to send to everyone
            _prefix.clear();
            _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
            std::string joinMsg = buildMessage(_prefix,
                                            "JOIN",
                                            "",
                                            channel->getName());

            sendMessageToChannel(channel, joinMsg, NULL);

            // Create the user list for the channel (SEPARATE FUNCTION?)
            std::string userList;
            std::set<Client *> clientsCopy = channel->getClients();
            std::set<Client *>::const_iterator it = clientsCopy.begin();
            while (it != clientsCopy.end())
            {
                Client* chanClient = *it;

                if (!userList.empty())
                    userList += " ";

                // If the client is an operator, prefix with '@'
                if (channel->isOperator(chanClient))
                    userList += "@" + chanClient->getNickname();
                else
                    userList += chanClient->getNickname();
                it++;
            }
            // Send the topic to the client
            if (channel->getTopic().empty())
            {
                _replyMsg.clear();
                _replyMsg = buildReplyMessage(messagesReplay[RPL_NOTOPIC].code,
                                            client,
                                            channel->getName(),
                                            "",
                                            messagesReplay[RPL_NOTOPIC].message);
            }
            else
            {
                _replyMsg.clear();
                _replyMsg = buildReplyMessage(messagesReplay[RPL_TOPIC].code,
                                            client,
                                            channel->getName(),
                                            "",
                                            channel->getTopic());
            }
            sendMessageToClient(client,  _replyMsg);

            // Send the list of users to the client
            _replyMsg.clear();
            _replyMsg = buildReplyMessage(messagesReplay[RPL_NAMREPLY].code,
                                        client,
                                        "= " + channel->getName(),
                                        "",
                                        userList);
            
            sendMessageToClient(client, _replyMsg);

            // Send end of users list to the client
            _replyMsg.clear();
            _replyMsg = buildReplyMessage(messagesReplay[RPL_ENDOFNAMES].code,
                                        client,
                                        channel->getName(),
                                        "",
                                        messagesReplay[RPL_ENDOFNAMES].message);
            sendMessageToClient(client, _replyMsg);
        }
        catch (const std::string& errorMsg)
        {
            throw (errorMsg);
        }
        i++;
    }
}

// Handle MODE command (view or set channel modes) (Replies ready, Msg ready, Errors ready)
void    ServerLogic::handleMODE(Client* client, const ParsedInput& input)
{
    std::string clientNickname = client->getNickname();
    if (clientNickname.empty())
        clientNickname = "*";

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

    const std::string& target = input.params[0];

    // Look for the channel
    std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(target);
    if (chanIt == _serverChannels.end())
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    target,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, _errorMsg));
    }
    Channel* channel = chanIt->second;

    std::set<Client*> clientsCopy = channel->getClients();

    // Check that the client is a member of the channel
    if (!channel->hasClient(client))
    {

        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_NOTONCHANNEL].code + " " + client->getNickname();
        _errorMsg = buildErrorMessage(_prefix,
                                    channel->getName(),
                                    messagesError[ERR_NOTONCHANNEL].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    // If only one parameter, show current modes
    if (input.params.size() == 1)
    {
        std::string modes = channel->getModes();
        _replyMsg.clear();
        _replyMsg = buildReplyMessage(messagesReplay[RPL_CHANNELMODEIS].code,
                                                client,
                                                channel->getName(),
                                                "",
                                                modes);

        return (sendMessageToClient(client, _replyMsg));
    }

    // If client wants to change modes, must be an operator
    if (!channel->isOperator(client))
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " " + messagesError[ERR_CHANOPRIVSNEEDED].code + " " + client->getNickname();

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix,
                                    channel->getName(),
                                    messagesError[ERR_CHANOPRIVSNEEDED].message);
        return (sendMessageToClient(client, _errorMsg));
    }

    // Modify modes
    std::string modeChanges = input.params[1];
    bool adding = true;
    size_t index = 2;
    std::string modeMsg;
    
    size_t i = 0;
    while (i < modeChanges.size())
    {
        char modeChar = modeChanges[i];
        if (modeChar == '+')
            adding = true;
        else if (modeChar == '-')
            adding = false;
        else
        {
            switch (modeChar)
            {
                case 'i': // Mode invite-only
                {
                    channel->setInviteOnly(adding);
                    break;
                }
                case 't': // Mode topic-operator-only
                {
                    channel->setTopicProtected(adding);
                    break;
                }
                case 'k': // Mode key (password)
                {
                    if (adding)
                    {
                        if (index >= input.params.size())
                        {
                            _prefix.clear();
                            _prefix = ":" + _serverName + " " + messagesError[ERR_NEEDMOREPARAMS].code + " " + client->getNickname();

                            _errorMsg.clear();
                            _errorMsg = buildErrorMessage(_prefix,
                                                        channel->getName(),
                                                        messagesError[ERR_NEEDMOREPARAMS].message);
                            return (sendMessageToClient(client, _errorMsg));
                        }
                        
                        if (!Parser::ft_checkkey(input.params[index]))
                        {
                            _prefix.clear();
                            _prefix = ":" + _serverName + " " + messagesError[ERR_BADCHANNELKEY].code + " " + client->getNickname();

                            _errorMsg.clear();
                            _errorMsg = buildErrorMessage(_prefix,
                                                        channel->getName(),
                                                        messagesError[ERR_BADCHANNELKEY].message);
                            return (sendMessageToClient(client, _errorMsg));
                        }
                        channel->setPassword(input.params[index++]);
                    }
                    else
                        channel->setPassword("");
                    break;
                }
                case 'o': // Mode operator
                {
                    if (index >= input.params.size())
                    {
                        _prefix.clear();
                        _prefix = ":" + _serverName + " " + messagesError[ERR_NEEDMOREPARAMS].code + " " + client->getNickname();

                        _errorMsg.clear();
                        _errorMsg = buildErrorMessage(_prefix,
                                                    channel->getName(),
                                                    messagesError[ERR_NEEDMOREPARAMS].message);
                        return (sendMessageToClient(client, _errorMsg));
                    }
                    
                    const std::string& operatorName = input.params[index++];

                    // Look for the client to be made operator
                    std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(operatorName);

                    if (nickIt == _serverNicknames.end())
                    {
                        _prefix.clear();
                        _prefix = ":" + _serverName + " " + messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();
                        
                        _errorMsg.clear();
                        _errorMsg = buildErrorMessage(_prefix,
                                                    operatorName,
                                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
                        return (sendMessageToClient(client, _errorMsg));
                    }

                    if (!channel->hasClient(nickIt->second))
                    {
                        _prefix.clear();
                        _prefix = ":" + _serverName + " " + messagesError[ERR_USERNOTINCHANNEL].code + " " + client->getNickname();

                        _errorMsg.clear();
                        _errorMsg = buildErrorMessage(_prefix,
                                                    operatorName + " " + channel->getName(),
                                                    messagesError[ERR_USERNOTINCHANNEL].message);
                        return (sendMessageToClient(client, _errorMsg));
                    }

                    Client* operatorClient = nickIt->second;

                    std::string mode;
                    if (adding)
                    {
                        mode = "+o";
                        channel->addOperator(operatorClient);
                    }
                    else
                    {
                        mode = "-o";
                        if (clientsCopy.size() == 1)
                            break ; // Do not allow the last operator to remove themselves
                        channel->removeOperator(operatorClient);

                        handleOperator(channel, operatorClient);
                    }
                
                    _prefix.clear();
                    _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;

                    modeMsg.clear();
                    modeMsg = buildMessage(_prefix,
                                            "MODE",
                                            channel->getName(),
                                            mode + " " + operatorClient->getNickname());
                    sendMessageToChannel(channel, modeMsg, NULL);

                    break;
                }
                case 'l': // Mode user limit
                {
                    if (adding)
                    {
                        if (index >= input.params.size())
                        {
                            _prefix = ":" + _serverName + " " + messagesError[ERR_NEEDMOREPARAMS].code + " " + client->getNickname();
                            _errorMsg = buildErrorMessage(_prefix,
                                                        channel->getName(),
                                                        messagesError[ERR_NEEDMOREPARAMS].message);
                            return (sendMessageToClient(client, _errorMsg));
                        }
                        
                        size_t limit = static_cast<size_t>(std::atoi(input.params[index++].c_str()));
                        
                        if (limit <= 0 || limit < clientsCopy.size())
                            channel->setMaxClients(channel->getMaxClients());
                        else if (limit > MAX_USERS_PER_CHANNEL)
                            channel->setMaxClients(MAX_USERS_PER_CHANNEL);
                        else
                            channel->setMaxClients(limit);
                    }
                    else
                        channel->setMaxClients(channel->getMaxClients());

                    _prefix.clear();
                    _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;

                    std::string sign;
                    if (!adding)
                        sign = "-";
                    else
                        sign = "+";
                    
                    modeMsg.clear();
                    modeMsg = buildMessage(_prefix,
                                        "MODE",
                                        channel->getName(),
                                        sign + "l " + to_string_c98(channel->getMaxClients()));
                    sendMessageToChannel(channel, modeMsg, NULL);
                    break ;
                }
                default:
                {
                    _prefix.clear();
                    _prefix = ":" + _serverName + " " + messagesError[ERR_UNKNOWNMODEFLAG].code + " " + client->getNickname();

                    _errorMsg.clear();
                    _errorMsg = buildErrorMessage(_prefix,
                                                std::string(1, modeChar),
                                                messagesError[ERR_UNKNOWNMODEFLAG].message);
                    return (sendMessageToClient(client, _errorMsg));
                }
            }
        }
        i++;
    }

}

// Manejar el comando PART (salir de canal) (Replay listo, Msg listo, Error listo)
void    ServerLogic::handlePART(Client* client, const ParsedInput& input)
{
    // Check if the client is registered
    std::string clientNickname = client->getNickname();
    if (clientNickname.empty())
        clientNickname = "*";

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

    // The first parameter contains the list of channels separated by ','
    std::vector<std::string> channels = str_to_vector(input.params[0], ',');
    std::string msg;

    // Optional parting message
    if (input.params.size() > 1)
        msg = input.params[1];

    // Iterate through all channels provided in params
    size_t i = 0;
    while (i < channels.size())
    {
        const std::string& channelName = channels[i];

        // Find the channel
        std::map<std::string, Channel*>::iterator it = _serverChannels.find(channelName);
        if (it == _serverChannels.end())
        {
            _prefix.clear();
            _prefix = ":" + _serverName + " " + messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();

            _errorMsg.clear();
            _errorMsg = buildErrorMessage(_prefix,
                                        channelName,
                                        messagesError[ERR_NOSUCHNICKCHANNEL].message);
            
            // Enviar el mensaje de error al cliente
            return (sendMessageToClient(client, _errorMsg));
        }
        Channel* channel = it->second;

        // If the client is not a member of the channel, return error
        if (!channel->hasClient(client))
        {
            _prefix.clear();
            _prefix = ":" + _serverName + " " + messagesError[ERR_NOTONCHANNEL].code + " " + client->getNickname();

            _errorMsg.clear();
            _errorMsg = buildErrorMessage(_prefix,
                                        channel->getName(),
                                        messagesError[ERR_NOTONCHANNEL].message);
            
            // Send the error message to the client
            return (sendMessageToClient(client, _errorMsg));
        }
        
        // Build the PART message to send to everyone
        _prefix.clear();
        _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
        std::string partMsg = buildMessage(_prefix, "PART", channelName, msg);

        // Notify all channel members
        sendMessageToChannel(channel, partMsg, NULL);

        channel->removeClient(client);
        handleOperator(channel, client);
        client->leaveChannel(channel);

        // If channel is empty, remove it from the server
        if (channel->getDeleteMe())
        {
            delete (channel);
            _serverChannels.erase(channelName);
        }
        
        i++;
    }
}
