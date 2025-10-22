#include "../../../includes/logic/ServerLogic.hpp"

/*----------------------------------HELPERS-----------------------------------*/

/*User Authentication*/

// Manejar el comando PASS (establecer contraseña)
void    ServerLogic::handlePASS(Client* client, const Command& cmd)
{
    const std::string& password = cmd.params[0];

    if (client->isRegistered())
        throw (ERR_ALREADYREGISTERED);

    if (password != _serverPassword)
        throw (ERR_PASSWDMISMATCH);

    client->setPassword(password);

    // Si ya tenía username, nickname y la contraseña coincide o no había, lo marcamos como registrado
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        client->setRegistered(true);
}

// Manejar el comando QUIT (desconectar cliente)
void    ServerLogic::handleQUIT(Client* client, const Command& cmd)
{
    std::string msg = "Client disconnected";
    if (!cmd.params.empty())
    {
        msg.clear();

        // Construimos el mensaje completo concatenando todos los parámetros a partir del 1
        /*msg = cmd.params[0];*/
        size_t i = 0;
        while (i < cmd.params.size())
        {
            if (i > 1)
                msg += " ";
            msg += cmd.params[i];
            i++;
        }
        if (!msg.empty() && msg[0] == ':')
            msg = msg.substr(1);
    }

    std::string fullMsg = buildMessage(client->getNickname(), "QUIT", "", msg);

    // Enviar mensaje de QUIT a todos los canales donde esté el cliente
    const std::set<std::string>& channelsNames = client->getChannels();
    std::set<std::string>::const_iterator it = channelsNames.begin();
    while (it != channelsNames.end())
    {
        std::map<std::string, Channel*>::iterator chanIt = _channels.find(*it);
        if (chanIt != _channels.end())
        {
            try
            {
                Channel* channel = chanIt->second;
                channel->broadcast(fullMsg, client);
                channel->removeClient(client);
            }
            catch(int error)
            {
                throw (error);
            }
        }
        ++it;
    }

    // Eliminar cliente del servidor
    serverRemoveClient(client->getFd());

    std::cout << RED << "❌ Client quit: " << client->getNickname() << RESET << std::endl;

    delete (client);
}

/*Channel Operations*/

void    ServerLogic::handleJOIN(Client* client, const Command& cmd)
{
    const std::string& channelName = cmd.params[0];

    /*if (channel->isInviteOnly() && !channel->isInvited(client) && !channel->isOperator(client))
        throw (ERR_INVITEONLYCHAN);*/
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);
    try
    {
        Channel* channel = createChannel(channelName, client);

        if (channel->getClients().size() >= channel->getMaxClients())
            throw (ERR_CHANNELISFULL);

        channel->addClient(client);
        client->joinChannel(channel);
    }
    catch (int  error)
    {
        throw (error);
    }
}

void    ServerLogic::handleINVITE(Client* client, const Command& cmd)
{
    const std::string& channelName = cmd.params[0];
    const std::string& nickname = cmd.params[1];

    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);

    // Primero buscamos el canal
    std::map<std::string, Channel*>::iterator chanIt = _channels.find(channelName);
    if (chanIt == _channels.end())
        throw (ERR_NOSUCHCHANNEL);
    Channel* channel = chanIt->second;

    // Si el canal es invite-only, solo los operadores pueden invitar
    if (channel->isInviteOnly() && !channel->isOperator(client))
        throw (ERR_CHANOPRIVSNEEDED);

    if (channel->getClients().size() >= channel->getMaxClients())
        throw (ERR_CHANNELISFULL);

    // Luego buscamos el cliente a invitar
    std::map<std::string, Client*>::iterator nickIt = _nicknames.find(nickname);
    if (nickIt == _nicknames.end())
        throw (ERR_NOSUCHNICK);
    Client* invitedClient = nickIt->second;

    // Invitamos al cliente al canal
    channel->inviteClient(invitedClient);

    // Enviamos el mensaje de invitación
    std::string inviteMsg = buildMessage(client->getNickname(), "INVITE", invitedClient->getNickname(), channelName);
    channel->broadcast(inviteMsg, client);
}

// Manejar el comando KICK (expulsar usuario de canal)
void    ServerLogic::handleKICK(Client* client, const Command& cmd)
{
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);
    
    const std::string& channelName = cmd.params[0];
    const std::string& targetNickname = cmd.params[1];

    // Primero buscamos el canal
    std::map<std::string, Channel*>::iterator chanIt = _channels.find(channelName);
    if (chanIt == _channels.end())
        throw (ERR_NOSUCHCHANNEL);

    Channel* channel = chanIt->second;

    // Comprobamos que el ejecutor esté en el canal
    if (!channel->hasClient(client))
        throw (ERR_NOTONCHANNEL);

    // Si el cliente no es operador, lanzamos error
    if (!channel->isOperator(client))
        throw (ERR_CHANOPRIVSNEEDED);

    // Luego buscamos el cliente a expulsar
    std::map<std::string, Client*>::iterator nickIt = _nicknames.find(targetNickname);
    if (nickIt == _nicknames.end())
        throw (ERR_NOSUCHNICK);

    Client* targetClient = nickIt->second;

    // Construimos el mensaje de KICK para enviar a todos
    std::string msg = "Kicked";
    if (cmd.params.size() > 2)
    {
        // Construimos el mensaje completo concatenando todos los parámetros a partir del 2
        msg.clear();
        size_t i = 2;
        while (i < cmd.params.size())
        {
            if (i > 2)
                msg += " ";
            msg += cmd.params[i];
            i++;
        }
        if (!msg.empty() && msg[0] == ':')
            msg = msg.substr(1);
    }

    std::string fullMsg = buildMessage(client->getNickname(), "KICK", channelName + " " + targetNickname, msg);
    
    // Notificamos a todos los miembros del canal
    channel->broadcast(fullMsg, NULL);

    // Quitamos al cliente del canal y viceversa
    try
    {
        channel->removeClient(targetClient);
        targetClient->leaveChannel(channel);
    }
    catch(int error)
    {
        throw (error);
    }
}

/*Server Queries and Information*/

void    ServerLogic::handleMODE(Client* client, const Command& cmd)
{

}

/*User Registration*/

// Manejar el comando NICK (establecer nickname)
void    ServerLogic::handleNICK(Client* client, const Command& cmd)
{
    const std::string nickname = cmd.params[0];

    // Comprobamos si ya existe otro cliente con ese nickname
    if (_nicknames.find(nickname) != _nicknames.end())
        throw (ERR_NICKNAMEINUSE);

    // Si tenía un nickname anterior, lo eliminamos del map
    if (!client->getNickname().empty())
        _nicknames.erase(client->getNickname());

    // Asignamos el nuevo nickname
    client->setNickname(nickname);

    // Añadimos al map de nicknames
    _nicknames[nickname] = client;

    // Si ya tenía username, nickname y la contraseña coincide o no había, lo marcamos como registrado
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        client->setRegistered(true);
}

// Manejar el comando USER (establecer username)
void    ServerLogic::handleUSER(Client* client, const Command& cmd)
{
    const std::string username = cmd.params[0];

    if (client->isRegistered())
        throw (ERR_ALREADYREGISTERED);

    client->setUsername(username);

    // Si ya tenía username, nickname y la contraseña coincide o no había, lo marcamos como registrado
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        client->setRegistered(true);
}

// Salir de un canal
void    ServerLogic::handlePART(Client* client, const Command& cmd)
{
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);

    // Si el último parámetro empieza por ':', es mensaje
    std::vector<std::string> channels = cmd.params;
    std::string partMessage;

    if (!channels.empty() && channels.back()[0] == ':')
    {
        partMessage = channels.back().substr(1);
        channels.pop_back();
    }

    // Iteramos por todos los canales que vienen en params
    size_t i = 0;
    while (i < channels.size())
    {
        const std::string& channelName = channels[i];

        // Primero buscamos el canal
        std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
        if (it == _channels.end())
            throw (ERR_NOSUCHCHANNEL);

        Channel* channel = it->second;

        // Construimos el mensaje de PART para enviar a todos
        std::string fullMsg = buildMessage(client->getNickname(), "PART", channelName, partMessage);

        // Broadcast antes de eliminar al cliente
        channel->broadcast(fullMsg, client);

        // Quitamos al cliente del canal y viceversa
        try
        {
            channel->removeClient(client);
            client->leaveChannel(channel);
        }
        catch(int error)
        {
            throw (error);
        }
        i++;
    }
}

// Manejar el comando TOPIC (ver o establecer tema del canal)
void    ServerLogic::handleTOPIC(Client* client, const Command& cmd)
{
    const std::string& channelName = cmd.params[0];

    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);
    
    // Primero buscamos el canal
    std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
    if (it == _channels.end())
        throw (ERR_NOSUCHCHANNEL);

    Channel* channel = it->second;

    // Si el cliente no es miembro del canal, lanzamos un error
    if (!channel->hasClient(client))
        throw (ERR_NOTONCHANNEL);

    std::string msg;
    
    // Si solo hay un parámetro, mostramos el tema actual
    if (cmd.params.size() == 1)
    {
        // Solo queremos ver el tema actual
        std::string topic = channel->getTopic();
        if (topic.empty())
            msg = buildMessage(_serverName, to_string_c98(RPL_NOTOPIC), channelName, "No topic is set for this channel.");
        else
            msg = buildMessage(_serverName, to_string_c98(RPL_TOPIC), channelName, topic);
        client->sendMessage(msg);

        // Si hubo error al enviar el mensaje porque el cliente se desconectó, eliminamos el cliente
        if (client->getFd() < 0)
            serverRemoveClient(client->getFd());

        return ;
    }

    // Si client quiere cambiar el tema, debe ser operador
    if (!channel->isOperator(client))
        throw (ERR_CHANOPRIVSNEEDED);

    // Construimos el topic completo concatenando todos los parámetros a partir del 1
    std::string newTopic; /*= cmd.params[1];*/
    size_t i = 1;
    while (i < cmd.params.size())
    {
        if (i > 1)
            newTopic += " ";
        newTopic += cmd.params[i];
        i++;
    }
    if (!newTopic.empty() && newTopic[0] == ':')
        newTopic = newTopic.substr(1);

    // Establecemos el nuevo tema
    channel->setTopic(newTopic);

    // Notificamos a todos los miembros del canal
    msg = buildMessage(client->getNickname(), "TOPIC", channelName, newTopic);
    channel->broadcast(msg, NULL);
}

/*Sending Messages*/

void    ServerLogic::handleNOTICE(Client* client, const Command& cmd)
{
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);

    const std::string& target = cmd.params[0];  

    // Construimos el mensaje completo concatenando todos los parámetros a partir del 1
    std::string msg; /*= cmd.params[1];*/
    size_t i = 1;
    while (i < cmd.params.size())
    {
        if (i > 1)
            msg += " ";
        msg += cmd.params[i];
        i++;
    }
    if (!msg.empty() && msg[0] == ':')
        msg = msg.substr(1);

    std::string fullMsg = buildMessage(client->getNickname(), "NOTICE", target, msg);

    // Buscamos si es un cliente por nickname
    std::map<std::string, Client*>::iterator nickIt = _nicknames.find(target);
    if (nickIt != _nicknames.end())
    {
        Client* recipient = nickIt->second;
        recipient->receiveMessage(fullMsg, client->getNickname());
        return ;
    }
    
    // Buscamos si es un canal
    std::map<std::string, Channel*>::iterator chanIt = _channels.find(target);
    if (chanIt != _channels.end())
    {
        Channel* channel = chanIt->second;
        channel->broadcast(fullMsg, client);
        return ;
    }

    // Si no es ni cliente ni canal, NOTICE no genera error, simplemente se ignora
}

// Manejar el comando PRIVMSG (enviar mensaje privado)
void    ServerLogic::handlePRIVMSG(Client* client, const Command& cmd)
{
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);
    
    const std::string& target = cmd.params[0];

    // Construimos el mensaje completo concatenando todos los parámetros a partir del 1
    std::string msg; /*= cmd.params[1];*/
    size_t i = 1;
    while (i < cmd.params.size())
    {
        if (i > 1)
            msg += " ";
        msg += cmd.params[i];
        i++;
    }
    if (!msg.empty() && msg[0] == ':')
        msg = msg.substr(1);

    std::string fullMsg = buildMessage(client->getNickname(), "PRIVMSG", target, msg);

    // Buscamos si es un cliente por nickname
    std::map<std::string, Client*>::iterator nickIt = _nicknames.find(target);
    if (nickIt != _nicknames.end())
    {
        Client* recipient = nickIt->second;
        recipient->receiveMessage(fullMsg, client->getNickname());
        return ;
    }

    // Buscamos si es un canal
    std::map<std::string, Channel*>::iterator chanIt = _channels.find(target);
    if (chanIt != _channels.end())
    {
        Channel* channel = chanIt->second;
        channel->broadcast(fullMsg, client);
        return ;
    }

    // Si no es ni cliente ni canal, lanzamos error
    if (target[0] == '#' || target[0] == '&')
        throw (ERR_CANNOTSENDTOCHAN);
    else
        throw (ERR_NOSUCHNICK);
}
