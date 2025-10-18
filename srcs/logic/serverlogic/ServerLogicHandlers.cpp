#include "ServerLogic.hpp"


/*----------------------------------HELPERS-----------------------------------*/

/*User Authentication*/

void    ServerLogic::handlePASS(Client* client, const Command& cmd)
{
    if (client->getRegistered() == true)
        throw (ERR_ALREADYREGISTERED);

    if (cmd.params[0] != SERVER_PASSWORD)
        throw (ERR_PASSWDMISMATCH);

    client->setPassword(cmd.params[0]);
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

void    ServerLogic::handleNICK(Client* client, const std::string& nickname)
{
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

void    ServerLogic::handleUSER(Client* client, const std::string& username)
{
    client->setUsername(username);
    if (!client->getNickname().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword))
        client->setRegistered(true);
}

/*Channel Operations*/

// Unirse a un canal (o crearlo si no existe)
void    ServerLogic::handleJOIN(Client* client, const std::string& channelName)
{
    if (client->getRegistered() == false)
        throw (ERR_NOTREGISTERED);
    try
    {
        Channel* channel = createChannel(channelName);
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
    if (client->getRegistered() == false)
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

void    ServerLogic::handleTOPIC(Client* client, const std::string& channelName, const std::string& topic)
{

}

void    ServerLogic::handleNAMES(Client* client, const std::string& channelName)
{

}

void    ServerLogic::handleLIST(Client* client)
{

}

void    ServerLogic::handleINVITE(Client* client, const std::string& channelName, const std::string& nickname)
{

}

void    ServerLogic::handleKICK(Client* client, const std::string& channelName, const std::string& nickname, const std::string& reason)
{

}


/*Server Queries and Information*/
void    ServerLogic::handleMOTD(Client* client)
{

}

void    ServerLogic::handleVERSION(Client* client)
{

}

void    ServerLogic::handleADMIN(Client* client)
{

}

void    ServerLogic::handleTIME(Client* client)
{

}

void    ServerLogic::handleINFO(Client* client)
{

}

void    ServerLogic::handleMODE(Client* client, const std::string& target, const std::string& modeChanges)
{

}


/*Sending Messages*/
void    ServerLogic::handleNOTICE(Client* client, const std::string& target, const std::string& message)
{

}

// Enviar mensaje privado o a canal
void    ServerLogic::handlePRIVMSG(Client* client, const std::string& target, const std::string& msg)
{
    if (client->getRegistered() == false)
        throw (ERR_NOTREGISTERED);

    // Primero buscamos si es un canal
    std::map<std::string, Channel*>::iterator chanIt = _channels.find(target);
    if (chanIt != _channels.end())
    {
        Channel* channel = chanIt->second;
        channel->broadcast(msg, client);
        return ;
    }

    // Luego buscamos si es un cliente por nickname
    std::map<std::string, Client*>::iterator nickIt = _nicknames.find(target);
    if (nickIt != _nicknames.end())
    {
        Client* recipient = nickIt->second;
        recipient->receiveMessage(msg, client->getNickname());
        return ;
    }
    throw (ERR_CANNOTSENDTOCHAN);
}
