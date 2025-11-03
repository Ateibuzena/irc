#include "../../../includes/logic/ServerLogic.hpp"

// revisar si se puede invitar a alguien a varios canales a la vez (Replay listo, Msg listo, NULL)
void    ServerLogic::handleINVITE(Client* client, const Command& cmd)
{
    const std::string& channelName = cmd.params[0];
    const std::string& nickname = cmd.params[1];

    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);

    // Primero buscamos el canal
    std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(channelName);
    if (chanIt == _serverChannels.end())
        throw (ERR_NOSUCHCHANNEL);
    Channel* channel = chanIt->second;

    // Si el canal es invite-only, solo los operadores pueden invitar
    if (channel->isInviteOnly() && !channel->isOperator(client))
        throw (ERR_CHANOPRIVSNEEDED);

    if (channel->getClients().size() >= channel->getMaxClients())
        throw (ERR_CHANNELISFULL);

    // Luego buscamos el cliente a invitar
    std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(nickname);
    if (nickIt == _serverNicknames.end())
        throw (ERR_NOSUCHNICK);
    Client* invitedClient = nickIt->second;

    // Invitamos al cliente al canal
    channel->inviteClient(invitedClient);

    // Construimos el mensaje de INVITE para enviar al invitado
    std::string prefix = client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
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
                                            NULL);

    // Enviamos el mensaje de confirmación al invitador
    sendMessageToClient(client, replayMsg);
}

// Manejar el comando KICK (expulsar usuario de canal)
void    ServerLogic::handleKICK(Client* client, const Command& cmd)
{
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);
    
    const std::string& channelName = cmd.params[0];
    const std::string& targetNickname = cmd.params[1];

    // Primero buscamos el canal
    std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(channelName);
    if (chanIt == _serverChannels.end())
        throw (ERR_NOSUCHCHANNEL);

    Channel* channel = chanIt->second;

    // Comprobamos que el ejecutor esté en el canal
    if (!channel->hasClient(client))
        throw (ERR_NOTONCHANNEL);

    // Si el cliente no es operador, lanzamos error
    if (!channel->isOperator(client))
        throw (ERR_CHANOPRIVSNEEDED);

    // Buscamos el cliente a expulsar
    std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(targetNickname);
    if (nickIt == _serverNicknames.end())
        throw (ERR_NOSUCHNICK);

    Client* targetClient = nickIt->second;

    // Comprobamos si el cliente a expulsar está en el canal
    if (!channel->hasClient(targetClient))
        throw (ERR_NOTONCHANNEL);

    // Construimos el mensaje de KICK para enviar a todos
    std::string msg = "Kicked";
    if (cmd.params.size() > 2)
    {
        msg.clear();
        msg = cmd.params[2];
    }

    // Quitamos al cliente del canal y viceversa
    try
    {
        // Construir mensaje de KICK para enviar a todos
        std::string prefix = client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;

        std::string kickMsg = buildMessage(prefix, "KICK", NULL, NULL);
        kickMsg += " " + channel->getName() + " " + targetClient->getNickname() + " :" + msg + "\r\n";

        // Quitamos al cliente del canal
        channel->removeClient(targetClient);
        targetClient->leaveChannel(channel);
        
        // Notificamos a todos los miembros del canal
        sendMessageToChannel(channel, kickMsg, NULL);
    }
    catch(int error)
    {
        throw (error);
    }
}

// Manejar el comando TOPIC (ver o establecer tema del canal)
void    ServerLogic::handleTOPIC(Client* client, const Command& cmd)
{
    const std::string& channelName = cmd.params[0];

    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);
    
    // Primero buscamos el canal
    std::map<std::string, Channel*>::iterator it = _serverChannels.find(channelName);
    if (it == _serverChannels.end())
        throw (ERR_NOSUCHCHANNEL);

    Channel* channel = it->second;

    // Si el cliente no es miembro del canal, lanzamos un error
    if (!channel->hasClient(client))
        throw (ERR_NOTONCHANNEL);

    std::string replayMsg;
    
    // Si solo hay un parámetro, mostramos el tema actual
    if (cmd.params.size() == 1)
    {
        std::string topic = channel->getTopic();

        // Si no hay tema establecido
        if (topic.empty())
            replayMsg = buildReplyMessage(messagesReplay[RPL_NOTOPIC].code,
                                        client,
                                        channelName,
                                        NULL,
                                        messagesReplay[RPL_NOTOPIC].message);
        else // Hay un tema establecido
        {
            std::string setter = channel->getSetter();
            std::string timeSet = channel->getTimeSet();

            replayMsg = buildReplyMessage(messagesReplay[RPL_TOPIC].code,
                                        client,
                                        channelName,
                                        NULL,
                                        topic);
            replayMsg += buildReplyMessage(messagesReplay[RPL_TOPICWHOTIME].code,
                                        client,
                                        channelName,
                                        setter + " " + timeSet,
                                        NULL);
        }

        // Enviamos el mensaje al cliente
        sendMessageToClient(client, replayMsg);

        return ;
    }

    // Si client quiere cambiar el tema, debe ser operador y el canal debe permitirlo
    if (channel->isTopicProtected() && !channel->isOperator(client))
        throw (ERR_CHANOPRIVSNEEDED);

    // Establecemos el nuevo tema
    std::string newTopic = cmd.params[1];
    channel->setTopic(newTopic);

    // Actualizamos setter
    channel->setSetter(client->getNickname());

    // Actualizamos timeSet
    std::time_t now = std::time(NULL);
    channel->setTimeSet(time_to_string(now));

    // Construimos el mensaje de TOPIC para enviar a todos
    std::string prefix = client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
    std::string topicMsg = buildMessage(prefix, "TOPIC", channel->getName(), newTopic);

    // Notificamos a todos los miembros del canal
    sendMessageToChannel(channel, topicMsg, NULL);

    // También enviamos el mensaje al cliente que cambió el tema
    sendMessageToClient(client, topicMsg);
}

// Manejar el comando JOIN (unirse a canal)
void    ServerLogic::handleJOIN(Client* client, const Command& cmd)
{
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);

    // El primer parámetro contiene la lista de canales separados por ','
    std::string channelsParam = cmd.params[0];
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
    if (cmd.params.size() > 1)
    {
        std::string keysParam = cmd.params[1];
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
            Parser parser;
            
            if (!parser.ft_checkchannel(channelName))
                throw (ERR_NOSUCHCHANNEL);

            Channel* channel = createChannel(channelName, client);

            if (!channel->getPassword().empty()) // Canal ya existente con clave
            {
                if (key.empty() || channel->getPassword() != key)
                    throw (ERR_BADCHANNELKEY);
            }
            else if (channel->getClients().empty()) // Canal nuevo
            {
                channel->addOperator(client);
                // Si se proporciona una clave, la establecemos
                if (!key.empty())
                    channel->setPassword(key);
            }

            // Comprobar si el canal es invite-only
            if (channel->isInviteOnly() && !channel->isOperator(client))
                    throw (ERR_CHANOPRIVSNEEDED);

            // Si el canal está lleno, lanzamos error
            if (channel->getClients().size() >= channel->getMaxClients())
                throw (ERR_CHANNELISFULL);

            channel->addClient(client);
            client->joinChannel(channel);

            // Construimos el mensaje de JOIN para enviar a todos
            std::string msg = ":" + client->getNickname() + " JOIN " + channel->getName() + "\r\n";
          
            sendMessageToChannel(channel, msg, client);
            sendMessageToClient(client, msg);
            
        }
        catch (int  error)
        {
            throw (error);
        }
        i++;
    }
}


// Manejar el comando MODE (ver o cambiar modos de canal)
void    ServerLogic::handleMODE(Client* client, const Command& cmd)
{
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);

    const std::string& target = cmd.params[0];

    // Buscamos si es un canal
    std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(target);
    if (chanIt == _serverChannels.end())
        throw (ERR_NOSUCHCHANNEL);
    
    Channel* channel = chanIt->second;

    // Si solo hay un parámetro, mostramos el modo actual
    if (cmd.params.size() == 1)
    {
        std::string modes = channel->getModes();
        std::string msg = buildMessage(_serverName, to_string_c98(RPL_CHANNELMODEIS), channel->getName(), modes);
        //client->sendMessage(msg);
        sendMessageToClient(client, msg);
        return ;
    }

    // Si el cliente no es operador, lanzamos error
    if (!channel->isOperator(client))
        throw (ERR_CHANOPRIVSNEEDED);

    // Modificar modos del canal
    std::string modeChanges = cmd.params[1];
    bool adding = true;
    size_t index = 2;
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
                        if (index >= cmd.params.size())
                            throw (ERR_NEEDMOREPARAMS);
                        // parsear primero cmd.params[index++]
                        channel->setPassword(cmd.params[index++]);
                    }
                    else
                        channel->setPassword("");
                    break;
                }
                case 'o': // Añadir o quitar operador
                {
                    if (index >= cmd.params.size())
                        throw (ERR_NEEDMOREPARAMS);
                    // parsear primero cmd.params[index++]
                    const std::string& operatorName = cmd.params[index++];

                    // Buscamos el cliente por nickname
                    std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(operatorName);
                    if (nickIt == _serverNicknames.end())
                        throw (ERR_NOSUCHNICK);

                    Client* operatorClient = nickIt->second;

                    if (adding)
                        channel->addOperator(operatorClient);
                    else
                        channel->removeOperator(operatorClient);
                    break;
                }
                case 'l': // Modo limit (límite de usuarios)
                {
                    if (adding)
                    {
                        if (index >= cmd.params.size())
                            throw (ERR_NEEDMOREPARAMS);
                        // parsear primero cmd.params[index++]
                        size_t limit = static_cast<size_t>(std::atoi(cmd.params[index++].c_str()));

                        channel->setMaxClients(limit);
                    }
                    else
                        channel->setMaxClients(channel->getMaxClients()); // Valor por defecto?
                    break;
                }
                default:
                {
                    throw (ERR_UNKNOWN);
                }
            }
        }
        i++;
    }

    // Notificamos a todos los miembros del canal sobre el cambio de modos
    std::string fullMsg = buildMessage(client->getNickname(), "MODE", channel->getName(), modeChanges);
    //channel->broadcast(fullMsg, client);
    sendMessageToChannel(channel, fullMsg, client);
}

// Manejar el comando PART (salir de canal) (Replay listo, Msg listo, NULL)
void    ServerLogic::handlePART(Client* client, const Command& cmd)
{
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);

    // El primer parámetro contiene la lista de canales separados por ','
    std::vector<std::string> channels = str_to_vector(cmd.params[0], ',');
    std::string msg;

    // Mensaje opcional
    if (cmd.params.size() > 1)
        msg = cmd.params[1];

    // Iteramos por todos los canales que vienen en params
    size_t i = 0;
    while (i < channels.size())
    {
        const std::string& channelName = channels[i];

        // Primero buscamos el canal
        std::map<std::string, Channel*>::iterator it = _serverChannels.find(channelName);
        if (it == _serverChannels.end())
            throw (ERR_NOSUCHCHANNEL);

        Channel* channel = it->second;

        // Si el cliente no es miembro del canal, lanzamos un error
        if (!channel->hasClient(client))
            throw (ERR_NOTONCHANNEL);

        // Quitamos al cliente del canal y viceversa
        try
        {
            // Construimos el mensaje de PART para enviar a todos
            std::string prefix = client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
            std::string partMsg = buildMessage(prefix, "PART", channelName, msg);

            channel->removeClient(client);
            client->leaveChannel(channel);


            // Notificamos a todos los miembros del canal
            sendMessageToChannel(channel, partMsg, client);
        }
        catch(int error)
        {
            throw (error);
        }
        i++;
    }
}
