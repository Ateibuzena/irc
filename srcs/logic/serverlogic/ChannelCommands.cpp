#include "../../../includes/logic/ServerLogic.hpp"

// revisar si se puede invitar a alguien a varios canales a la vez (Replay listo, Msg listo, Error listo)
void    ServerLogic::handleINVITE(Client* client, const ParsedInput& input)
{
    //Juan :irc.server.com 461 <nick> INVITE :Not enough parameters

    const std::string& channelName = input.params[0];
    const std::string& nickname = input.params[1];

    std::string prefix = ":" + _serverName + " ";
    std::string errorMsg;

    if (!client->isRegistered())
    {
        prefix += messagesError[ERR_NOTREGISTERED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_NOTREGISTERED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Primero buscamos el canal
    std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(channelName);
    if (chanIt == _serverChannels.end())
    {
        prefix += messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channelName,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, errorMsg));
    }
    Channel* channel = chanIt->second;

    // Comprobamos que el ejecutor esté en el canal
    if (!channel->hasClient(client))
    {
        prefix += messagesError[ERR_NOTONCHANNEL].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channel->getName(),
                                    messagesError[ERR_NOTONCHANNEL].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Si el canal es invite-only, solo los operadores pueden invitar
    if (channel->isInviteOnly() && !channel->isOperator(client))
    {
        prefix += messagesError[ERR_CHANOPRIVSNEEDED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channel->getName(),
                                    messagesError[ERR_CHANOPRIVSNEEDED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Si el canal está lleno, lanzamos error
    if (channel->getClients().size() >= channel->getMaxClients())
    {
        prefix += messagesError[ERR_CHANNELISFULL].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channel->getName(),
                                    messagesError[ERR_CHANNELISFULL].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Luego buscamos el cliente a invitar
    std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(nickname);
    if (nickIt == _serverNicknames.end())
    {
        prefix += messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    nickname,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, errorMsg));
    }
    Client* invitedClient = nickIt->second;

    // Invitamos al cliente al canal
    channel->inviteClient(invitedClient);

    // Construimos el mensaje de INVITE para enviar al invitado
    prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
    std::string inviteMsg = buildMessage(prefix,
                                        "INVITE",
                                        invitedClient->getNickname(),
                                        channel->getName());

    // Enviamos el mensaje de invitación al cliente invitado
    sendMessageToClient(invitedClient, inviteMsg);

    // Construimos el mensaje de confirmación para el invitador
    std::string replayMsg = buildReplyMessage(messagesReplay[RPL_INVITING].code,
                                            client,
                                            invitedClient->getNickname(),
                                            channel->getName(),
                                            "");

    // Enviamos el mensaje de confirmación al invitador
    sendMessageToClient(client, replayMsg);
}

// Manejar el comando KICK (expulsar usuario de canal) (Replay listo, Msg listo, Error listo)
void    ServerLogic::handleKICK(Client* client, const ParsedInput& input)
{
    //Juan :irc.server.com 461 <nick> KICK :Not enough parameters

    std::string prefix = ":" + _serverName + " ";
    std::string errorMsg;

    if (!client->isRegistered())
    {
        prefix += messagesError[ERR_NOTREGISTERED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_NOTREGISTERED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    const std::string& channelName = input.params[0];
    const std::string& targetNickname = input.params[1];

    // Primero buscamos el canal
    std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(channelName);
    if (chanIt == _serverChannels.end())
    {
        std::cout << "No such channel: " << channelName << std::endl;
        prefix += messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channelName,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, errorMsg));
    }
    Channel* channel = chanIt->second;

    // Comprobamos que el ejecutor esté en el canal
    if (!channel->hasClient(client))
    {
        prefix += messagesError[ERR_NOTONCHANNEL].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channel->getName(),
                                    messagesError[ERR_NOTONCHANNEL].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Si el cliente no es operador, lanzamos error
    if (!channel->isOperator(client))
    {
        prefix += messagesError[ERR_CHANOPRIVSNEEDED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channel->getName(),
                                    messagesError[ERR_CHANOPRIVSNEEDED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Buscamos el cliente a expulsar
    std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(targetNickname);
    if (nickIt == _serverNicknames.end())
    {
        std::cout << "No such nick: " << targetNickname << std::endl;
        prefix += messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    targetNickname,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, errorMsg));
    }
    Client* targetClient = nickIt->second;

    // Construimos el mensaje de KICK para enviar a todos
    std::string msg = "Kicked";
    if (input.params.size() > 2)
    {
        msg.clear();
        msg = input.params[2];
    }

    // Construir mensaje de KICK para enviar a todos
    prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;

    std::string kickMsg = buildMessage(prefix, "KICK", "", "");
    kickMsg += " " + channel->getName() + " " + targetClient->getNickname() + " :" + msg + "\r\n";

    // Notificamos a todos los miembros del canal
    sendMessageToChannel(channel, kickMsg, NULL);

    // Quitamos al cliente del canal
    channel->removeClient(targetClient);
    targetClient->leaveChannel(channel);

    // Si el canal queda vacío, eliminarlo del servidor
    if (channel->getDeleteMe())
    {
        delete (channel);
        _serverChannels.erase(chanIt);
    }
}

// Manejar el comando TOPIC (ver o establecer tema del canal) (Replay listo, Msg listo, Error listo)
void    ServerLogic::handleTOPIC(Client* client, const ParsedInput& input)
{
    // Juan :irc.server.com 461 <nick> TOPIC :Not enough parameters

    const std::string& channelName = input.params[0];

    std::string prefix = ":" + _serverName + " ";
    std::string errorMsg;

    // Comprobar si el cliente está registrado
    if (!client->isRegistered())
    {
        prefix += messagesError[ERR_NOTREGISTERED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_NOTREGISTERED].message);
        return (sendMessageToClient(client, errorMsg));
    }
    
    // Primero buscamos el canal
    std::map<std::string, Channel*>::iterator it = _serverChannels.find(channelName);
    if (it == _serverChannels.end())
    {
        prefix += messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channelName,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, errorMsg));
    }
    Channel* channel = it->second;

    // Si el cliente no es miembro del canal, lanzamos un error
    if (!channel->hasClient(client))
    {
        prefix += messagesError[ERR_NOTONCHANNEL].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channel->getName(),
                                    messagesError[ERR_NOTONCHANNEL].message);
        return (sendMessageToClient(client, errorMsg));
    }

    std::string replayMsg;
    
    // Si solo hay un parámetro, mostramos el tema actual
    if (input.params.size() == 1)
    {
        std::string topic = channel->getTopic();

        // Si no hay tema establecido
        if (topic.empty())
            replayMsg = buildReplyMessage(messagesReplay[RPL_NOTOPIC].code,
                                        client,
                                        channelName,
                                        "",
                                        messagesReplay[RPL_NOTOPIC].message);
        else // Hay un tema establecido
        {
            std::string setter = channel->getSetter();
            std::string timeSet = channel->getTimeSet();

            replayMsg = buildReplyMessage(messagesReplay[RPL_TOPIC].code,
                                        client,
                                        channelName,
                                        "",
                                        topic);
            replayMsg += buildReplyMessage(messagesReplay[RPL_TOPICWHOTIME].code,
                                        client,
                                        channelName,
                                        setter + " " + timeSet,
                                        "");
        }

        // Enviamos el mensaje al cliente
        sendMessageToClient(client, replayMsg);

        return ;
    }

    // Si client quiere cambiar el tema, debe ser operador y el canal debe permitirlo
    if (channel->isTopicProtected() && !channel->isOperator(client))
    {
        prefix += messagesError[ERR_CHANOPRIVSNEEDED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channel->getName(),
                                    messagesError[ERR_CHANOPRIVSNEEDED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Establecemos el nuevo tema
    std::string newTopic = input.params[1];
    channel->setTopic(newTopic);

    // Actualizamos setter
    channel->setSetter(client->getNickname());

    // Actualizamos timeSet
    std::time_t now = std::time(NULL);
    channel->setTimeSet(time_to_string(now));

    // Construimos el mensaje de TOPIC para enviar a todos
    prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
    std::string topicMsg = buildMessage(prefix, "TOPIC", channel->getName(), newTopic);

    // Notificamos a todos los miembros del canal
    sendMessageToChannel(channel, topicMsg, NULL);
}

// Manejar el comando JOIN (unirse a canal) (Replay listo, Msg listo, Error listo)
void    ServerLogic::handleJOIN(Client* client, const ParsedInput& input) 
{
    // Juan :irc.server.com 461 <nick> MODE :Not enough parameters

    std::string prefix = ":" + _serverName + " ";
    std::string errorMsg;

    if (!client->isRegistered())
    {
        prefix += messagesError[ERR_NOTREGISTERED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_NOTREGISTERED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // El primer parámetro contiene la lista de canales separados por ','
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
    
    // El segundo parámetro (opcional) contiene las claves para los canales
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

    // Asociamos cada canal con su clave (si existe)
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
            
            if (channel == NULL)
                return ;
            
            if (!channel->getPassword().empty()) // Canal ya existente con clave
            {
                if (key.empty() || channel->getPassword() != key)
                {
                    errorMsg = buildErrorMessage(prefix +
                                                messagesError[ERR_BADCHANNELKEY].code + " " + client->getNickname() + " ",
                                                channel->getName(),
                                                messagesError[ERR_BADCHANNELKEY].message);
                    return (sendMessageToClient(client, errorMsg));
                }
            }
            else if (channel->getClients().empty()) // Canal nuevo
            {
                // Si se proporciona una clave, la establecemos
                if (key != "" && !Parser::ft_checkkey(key))
                {
                    errorMsg = buildErrorMessage(prefix +
                                                messagesError[ERR_BADCHANNELKEY].code + " " + client->getNickname() + " ",
                                                channel->getName(),
                                                messagesError[ERR_BADCHANNELKEY].message);                    
                    return (sendMessageToClient(client, errorMsg));
                }
                channel->setPassword(key);
            }

            // Comprobar si el canal es invite-only
            if (channel->isInviteOnly() && !channel->isInvited(client))
            {
                errorMsg = buildErrorMessage(prefix +
                                            messagesError[ERR_INVITEONLYCHAN].code + " " + client->getNickname() + " ",
                                            channel->getName(),
                                            messagesError[ERR_INVITEONLYCHAN].message);
                return (sendMessageToClient(client, errorMsg));
            }
            
            // Si el canal está lleno, lanzamos error
            if (channel->getClients().size() >= channel->getMaxClients())
            {
                errorMsg = buildErrorMessage(prefix +
                                            messagesError[ERR_CHANNELISFULL].code + " " + client->getNickname() + " ",
                                            channel->getName(),
                                            messagesError[ERR_CHANNELISFULL].message);
                return (sendMessageToClient(client, errorMsg));
            }

            channel->addClient(client);
            client->joinChannel(channel);
            /*************************************************************************************** */
            //adeed by noe debug canal no limpia clientes??

            const std::set<Client *> &members = channel->getClients();
            std::cout << "[DEBUG] after JOIN, members of " << channel->getName() << ":";
            for (std::set<Client *>::const_iterator itM = members.begin();
                 itM != members.end(); ++itM)
            {
                Client *c = *itM;
                std::cout << " '" << c->getNickname() << "'(fd=" << c->getFd() << ")";
            }
            std::cout << std::endl;

            /************************************************************************************************* */

            // Construimos el mensaje de JOIN para enviar a todos
            prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
            std::string joinMsg = buildMessage(prefix,
                                            "JOIN",
                                            "",
                                            channel->getName());

            sendMessageToChannel(channel, joinMsg, NULL);

            // Creamos la lista de usuarios del canal
            std::string userList;
            const std::set<Client *>& clientsInChannel = channel->getClients();
            std::set<Client *>::const_iterator it = clientsInChannel.begin();

            while (it != clientsInChannel.end())
            {
                Client* chanClient = *it;

                if (!userList.empty())
                    userList += " ";

                // Si el cliente es operador, añadimos un '@' antes del nickname
                if (channel->isOperator(chanClient))
                    userList += "@" + chanClient->getNickname();
                else
                    userList += chanClient->getNickname();
                it++;
            }
            
            std::string replayMsg;

            // Enviamos topic al cliente
            if (channel->getTopic().empty())
            {
                replayMsg = buildReplyMessage(messagesReplay[RPL_NOTOPIC].code,
                                            client,
                                            channel->getName(),
                                            "",
                                            messagesReplay[RPL_NOTOPIC].message);
            }
            else
            {
                replayMsg = buildReplyMessage(messagesReplay[RPL_TOPIC].code,
                                            client,
                                            channel->getName(),
                                            "",
                                            channel->getTopic());
            }
            sendMessageToClient(client,  replayMsg);

            // Enviamos la lista de usuarios al cliente
            replayMsg = buildReplyMessage(messagesReplay[RPL_NAMREPLY].code,
                                        client,
                                        "= " + channel->getName(),
                                        "",
                                        userList);
            sendMessageToClient(client, replayMsg);

            // Enviamos el fin de la lista de usuarios al cliente
            replayMsg = buildReplyMessage(messagesReplay[RPL_ENDOFNAMES].code,
                                        client,
                                        channel->getName(),
                                        "",
                                        messagesReplay[RPL_ENDOFNAMES].message);
            sendMessageToClient(client, replayMsg);
        }
        catch (const std::string& errorMsg)//esto captura strings?? pero el error es un int
        {
            throw (errorMsg);
        }
        i++;
    }
}

// Manejar el comando MODE (ver o cambiar modos de canal) (NULL, NULL, Error listo)
void    ServerLogic::handleMODE(Client* client, const ParsedInput& input)
{
    // Juan :irc.server.com 461 <nick> MODE :Not enough parameters

    std::string prefix = ":" + _serverName + " ";
    std::string errorMsg;

    if (!client->isRegistered())
    {
        prefix += messagesError[ERR_NOTREGISTERED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_NOTREGISTERED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    const std::string& target = input.params[0];

    // Buscamos si es un canal
    std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(target);
    if (chanIt == _serverChannels.end())
    {
        // No es un canal, lanzamos error
        prefix += messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    target,
                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
        return (sendMessageToClient(client, errorMsg));
    }
    Channel* channel = chanIt->second;

    // Comprobamos que el ejecutor esté en el canal
    if (!channel->hasClient(client))
    {
        prefix += messagesError[ERR_NOTONCHANNEL].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channel->getName(),
                                    messagesError[ERR_NOTONCHANNEL].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Si solo hay un parámetro, mostramos el modo actual
    if (input.params.size() == 1)
    {
        std::string modes = channel->getModes();
        prefix = ":" + _serverName + " ";
        std::string replayMsg = buildReplyMessage(messagesReplay[RPL_CHANNELMODEIS].code,
                                                client,
                                                channel->getName(),
                                                "",
                                                modes);

        return (sendMessageToClient(client, replayMsg));
    }

    // Si el cliente no es operador, lanzamos error
    if (!channel->isOperator(client))
    {
        prefix += messagesError[ERR_CHANOPRIVSNEEDED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    channel->getName(),
                                    messagesError[ERR_CHANOPRIVSNEEDED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Modificar modos del canal
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
                case 'i': // Modo invite-only
                {
                    channel->setInviteOnly(adding);
                    break;
                }
                case 't': // Modo topic-operator-only
                {
                    channel->setTopicProtected(adding);
                    break;
                }
                case 'k': // Modo key (contraseña)
                {
                    if (adding)
                    {
                        if (index >= input.params.size())
                        {
                            prefix += messagesError[ERR_NEEDMOREPARAMS].code + " " + client->getNickname();
                            errorMsg = buildErrorMessage(prefix,
                                                        channel->getName(),
                                                        messagesError[ERR_NEEDMOREPARAMS].message);
                            return (sendMessageToClient(client, errorMsg));
                        }
                        // parsear primero input.params[index++
                        if (!Parser::ft_checkkey(input.params[index]))
                        {
                            prefix += messagesError[ERR_BADCHANNELKEY].code + " " + client->getNickname();
                            errorMsg = buildErrorMessage(prefix,
                                                        channel->getName(),
                                                        messagesError[ERR_BADCHANNELKEY].message);
                            return (sendMessageToClient(client, errorMsg));
                        }
                        channel->setPassword(input.params[index++]);
                    }
                    else
                        channel->setPassword("");
                    break;
                }
                case 'o': // Añadir o quitar operador
                {
                    if (index >= input.params.size())
                    {
                        prefix += messagesError[ERR_NEEDMOREPARAMS].code + " " + client->getNickname();
                        errorMsg = buildErrorMessage(prefix,
                                                    channel->getName(),
                                                    messagesError[ERR_NEEDMOREPARAMS].message);
                        return (sendMessageToClient(client, errorMsg));
                    }
                    
                    const std::string& operatorName = input.params[index++];

                    // Buscamos el cliente por nickname
                    std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(operatorName);
                    if (nickIt == _serverNicknames.end())
                    {
                        prefix += messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();
                        errorMsg = buildErrorMessage(prefix,
                                                    operatorName,
                                                    messagesError[ERR_NOSUCHNICKCHANNEL].message);
                        return (sendMessageToClient(client, errorMsg));
                    }

                    Client* operatorClient = nickIt->second;

                    if (adding)
                        channel->addOperator(operatorClient);
                    else
                    {
                        if (channel->getClients().size() == 1)
                            break ;; // No se puede quitar el único operador

                        channel->removeOperator(operatorClient);
                        modeMsg = buildMessage(":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost,
                                                        "MODE",
                                                        channel->getName(),
                                                        "-o " + operatorClient->getUsername());
                        sendMessageToChannel(channel, modeMsg, NULL);

                        // Asignar nuevo operador si el que se va es el único operador
                        if (channel->isOperator(operatorClient) && channel->getOperators().size() == 1)
                        {
                            std::set<Client *>::const_iterator it = channel->getClients().begin();
                            while (it != channel->getClients().end())
                            {
                                Client* potentialOp = *it;
                                if (potentialOp != operatorClient)
                                {
                                    channel->addOperator(potentialOp);
                                    std::string opMsg = buildMessage(":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost,
                                                                    "MODE",
                                                                    channel->getName(),
                                                                    "+o " + potentialOp->getUsername());
                                    sendMessageToChannel(channel, opMsg, NULL);
                                    break ;
                                }
                                it++;
                            }
                        }
                    }
                    break;
                }
                case 'l': // Modo limit (límite de usuarios)
                {
                    if (adding)
                    {
                        if (index >= input.params.size())
                        {
                            prefix += messagesError[ERR_NEEDMOREPARAMS].code + " " + client->getNickname();
                            errorMsg = buildErrorMessage(prefix,
                                                        channel->getName(),
                                                        messagesError[ERR_NEEDMOREPARAMS].message);
                            return (sendMessageToClient(client, errorMsg));
                        }
                        // parsear primero input.params[index++]
                        size_t limit = static_cast<size_t>(std::atoi(input.params[index++].c_str()));
                        
                        if (limit <= 0 || limit < channel->getClients().size())
                            channel->setMaxClients(channel->getMaxClients());
                        else if (limit > MAX_USERS_PER_CHANNEL)
                            channel->setMaxClients(MAX_USERS_PER_CHANNEL);
                        else
                            channel->setMaxClients(limit);
                    }
                    else
                        channel->setMaxClients(channel->getMaxClients());
                    
                    modeMsg = buildMessage(":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost,
                                        "MODE",
                                        channel->getName(),
                                        "-l " + to_string_c98(channel->getMaxClients()));
                    sendMessageToClient(client, modeMsg);
                    break ;
                }
                default:
                {
                    prefix += messagesError[ERR_UNKNOWNMODEFLAG].code + " " + client->getNickname();
                    errorMsg = buildErrorMessage(prefix,
                                                std::string(1, modeChar),
                                                messagesError[ERR_UNKNOWNMODEFLAG].message);
                    return (sendMessageToClient(client, errorMsg));
                }
            }
        }
        i++;
    }
/************************************************************************************************* */
//added by noe
    std::string fullMsg = buildMessage(
        ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost,
        "MODE",
        channel->getName(),
        modeChanges);

    // Notificamos a todos los miembros del canal sobre el cambio de modos
    //std::string fullMsg = buildMessage(client->getNickname(), "MODE", channel->getName(), modeChanges);
/******************************************************************************************************** */
    sendMessageToChannel(channel, fullMsg, NULL);
}

// Manejar el comando PART (salir de canal) (Replay listo, Msg listo, Error listo)
void    ServerLogic::handlePART(Client* client, const ParsedInput& input)
{
    // Juan :irc.server.com 461 <nick> PART :Not enough parameters

    std::string prefix = ":" + _serverName + " ";
    std::string errorMsg;

    // Comprobar si el cliente está registrado
    if (!client->isRegistered())
    {
        prefix += messagesError[ERR_NOTREGISTERED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_NOTREGISTERED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // El primer parámetro contiene la lista de canales separados por ','
    std::vector<std::string> channels = str_to_vector(input.params[0], ',');
    std::string msg;

    // Mensaje opcional
    if (input.params.size() > 1)
        msg = input.params[1];

    // Iteramos por todos los canales que vienen en params
    size_t i = 0;
    while (i < channels.size())
    {
        const std::string& channelName = channels[i];

        // Primero buscamos el canal
        std::map<std::string, Channel*>::iterator it = _serverChannels.find(channelName);
        if (it == _serverChannels.end())
        {
            prefix += messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname();
            errorMsg = buildErrorMessage(prefix,
                                        channelName,
                                        messagesError[ERR_NOSUCHNICKCHANNEL].message);
            
            // Enviar el mensaje de error al cliente
            return (sendMessageToClient(client, errorMsg));
        }
        Channel* channel = it->second;

        // Si el cliente no es miembro del canal, lanzamos un error
        if (!channel->hasClient(client))
        {
            prefix += messagesError[ERR_NOTONCHANNEL].code + " " + client->getNickname();
            errorMsg = buildErrorMessage(prefix,
                                        channel->getName(),
                                        messagesError[ERR_NOTONCHANNEL].message);
            
            // Enviar el mensaje de error al cliente
            return (sendMessageToClient(client, errorMsg));
        }
        
        // Construimos el mensaje de PART para enviar a todos
        std::string prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
        std::string partMsg = buildMessage(prefix, "PART", channelName, msg);

        // Notificamos a todos los miembros del canal
        sendMessageToChannel(channel, partMsg, NULL);

        channel->removeClient(client);
        client->leaveChannel(channel);

        // Si el canal queda vacío, eliminarlo del servidor
        if (channel->getDeleteMe())
        {
            delete (channel);
            _serverChannels.erase(channelName);
        }
        
        i++;
    }
}
