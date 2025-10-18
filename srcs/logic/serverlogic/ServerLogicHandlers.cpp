#include "ServerLogic.hpp"


/*----------------------------------HELPERS-----------------------------------*/

/*User Authentication*/

void    ServerLogic::handlePASS(Client* client, const Command& cmd)
{
    const std::string& password = cmd.params[0];

    if (client->getRegistered() == true)
        throw (ERR_ALREADYREGISTERED);

    if (password != SERVER_PASSWORD)
        throw (ERR_PASSWDMISMATCH);

    client->setPassword(password);
}

void    ServerLogic::handleQUIT(Client* client, const Command& cmd)
{
    std::string quitMessage = "Client disconnected";
    if (!cmd.params.empty())
        quitMessage = cmd.params[0];

    // Enviar mensaje de QUIT a todos los canales donde esté el cliente
    const std::set<Channel*>& channels = client->getChannels();
    std::set<Channel*>::const_iterator it = channels.begin();
    while (it != channels.end())
    {
        (*it)->broadcast(quitMessage, client);
        (*it)->removeClient(client);
        ++it;
    }

    // Eliminar cliente del servidor
    serverRemoveClient(client->getFd());

    delete (client);
}

/*User Registration*/

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

    // Si ya tenía username, lo marcamos como registrado
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword))
        client->setRegistered(true);

    // Añadimos al map de nicknames
    _nicknames[nickname] = client;
}

void    ServerLogic::handleUSER(Client* client, const Command& cmd)
{
    const std::string username = cmd.params[0];

    client->setUsername(username);
    if (!client->getNickname().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword))
        client->setRegistered(true);
}

/*Channel Operations*/

// Unirse a un canal (o crearlo si no existe)
void    ServerLogic::handleJOIN(Client* client, const Command& cmd)
{
    const std::string& channelName = cmd.params[0];

    if (!client->getRegistered())
        throw (ERR_NOTREGISTERED);
    try
    {
        Channel* channel = createChannel(channelName, client);
        channel->addClient(client);
        client->joinChannel(channel);
    }
    catch (int  error)
    {
        throw (error);
    }
}

// Salir de un canal
void    ServerLogic::handlePART(Client* client, const Command& cmd)
{
    const std::string& channelName = cmd.params[0];

    if (!client->getRegistered())
        throw (ERR_NOTREGISTERED);

    // Primero buscamos el canal
    std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
    if (it == _channels.end())
        throw (ERR_NOSUCHCHANNEL);

    Channel* channel = it->second;

    try
    {
        channel->removeClient(client);
        client->leaveChannel(channel);
    }
    catch(int error)
    {
        throw (error);
    }
}

void    ServerLogic::handleTOPIC(Client* client, const Command& cmd)
{
    const std::string& channelName = cmd.params[0];
    const std::string topic = cmd.params[1]

    if (!client->getRegistered())
        throw (ERR_NOTREGISTERED);
    
    // Primero buscamos el canal
    std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
    if (it == _channels.end())
        throw (ERR_NOSUCHCHANNEL);

    Channel* channel = it->second;

    // Si el cliente no es miembro del canal, lanzamos un error
    if (!channel->hasClient(client))
        throw (ERR_NOTONCHANNEL);

    // Si hay un nuevo tema, lo actualizamos
    channel->setTopic(topic);
}

void    ServerLogic::handleINVITE(Client* client, const Command& cmd)
{
    const std::string& channelName = cmd.params[0];
    const std::string& nickname = cmd.params[1];

}

void    ServerLogic::handleKICK(Client* client, const std::string& channelName, const std::string& nickname, const std::string& reason)
{

}


/*Server Queries and Information*/

void    ServerLogic::handleMODE(Client* client, const Command& cmd)
{

}

/*Sending Messages*/

void    ServerLogic::handleNOTICE(Client* client, const Command& cmd)
{
    const std::string& target = cmd.params[0];  
    const std::string& msg = cmd.params[1];

    std::string fullMsg = buildMessage(client->getNickname(), "PRIVMSG", target, msg);

    if (client->getRegistered() == false)
        throw (ERR_NOTREGISTERED);

    // Buscamos si es un canal
    std::map<std::string, Channel*>::iterator chanIt = _channels.find(target);
    if (chanIt != _channels.end())
    {
        Channel* channel = chanIt->second;
        channel->broadcast(fullMsg, client);
        return ;
    }

    // Buscamos si es un cliente por nickname
    std::map<std::string, Client*>::iterator nickIt = _nicknames.find(target);
    if (nickIt != _nicknames.end())
    {
        Client* recipient = nickIt->second;
        recipient->receiveMessage(fullMsg, client->getNickname());
        return ;
    }
}

// Enviar mensaje privado o a canal
void    ServerLogic::handlePRIVMSG(Client* client, const Command& cmd)
{
    const std::string& target = cmd.params[0];  
    const std::string& msg = cmd.params[1];

    std::string fullMsg = buildMessage(client->getNickname(), "PRIVMSG", target, msg);

    if (client->getRegistered() == false)
        throw (ERR_NOTREGISTERED);

    // Buscamos si es un canal
    std::map<std::string, Channel*>::iterator chanIt = _channels.find(target);
    if (chanIt != _channels.end())
    {
        Channel* channel = chanIt->second;
        channel->broadcast(fullMsg, client);
        return ;
    }

    // Buscamos si es un cliente por nickname
    std::map<std::string, Client*>::iterator nickIt = _nicknames.find(target);
    if (nickIt != _nicknames.end())
    {
        Client* recipient = nickIt->second;
        recipient->receiveMessage(fullMsg, client->getNickname());
        return ;
    }
    throw (ERR_CANNOTSENDTOCHAN);
}
