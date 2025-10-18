#include "ServerLogic.hpp"

/*--------------------------------CONSTRUCTORS--------------------------------*/

ServerLogic::ServerLogic(const std::string& serverPassword)
    :   _clients(),
        _nicknames(),
        _channels(),
        _serverPassword(serverPassword)
{
    std::cout << "✅ ServerLogic initialized." << std::endl;
}

/*--------------------------------DESTRUCTORS---------------------------------*/

ServerLogic::~ServerLogic()
{
    // Limpiamos todos los clientes
    std::map<int, Client*>::iterator it = _clients.begin();
    while (it != _clients.end())
    {
        int fd = it->first;

        Client* client = it->second;
        if (client)
            delete (client);
        ++it;
    }
    _clients.clear();

    // Limpiamos nicknames
    _nicknames.clear();

    // Limpiamos todos los canales
    std::map<std::string, Channel*>::iterator it = _channels.begin();
    while (it != _channels.end())
    {
        Channel* channel = it->second;
        if (channel)
            delete (channel);
        ++it;
    }
    _channels.clear();
}

/*----------------------------------GETTERS------------------------------------*/

// Devuelve el cliente según su fd
Client* ServerLogic::getClient(int fd) const
{
    std::map<int, Client*>::const_iterator it = _clients.find(fd);
    if (it != _clients.end())
        return (it->second);
    return (NULL);
}

Channel* ServerLogic::getChannel(const std::string& name) const
{
    std::map<std::string, Channel*>::const_iterator it = _channels.find(name);
    if (it != _channels.end())
        return (it->second);
    return (NULL);
}

/*----------------------------------METHODS------------------------------------*/

std::string ServerLogic::buildMessage(const std::string& prefix,
                                      const std::string& command,
                                      const std::string& target,
                                      const std::string& message) const
{
    std::string fullMsg = ":" + prefix + " " + command + " " + target + " :" + message + "\r\n";

    if (fullMsg.size() > MAX_MESSAGE_LENGTH)
    {
        // Reservamos espacio para CRLF y los demás campos
        size_t maxLen = MAX_MESSAGE_LENGTH - (prefix.size() + command.size() + target.size() + 4);
        std::string truncated = message.substr(0, maxLen);
        fullMsg = ":" + prefix + " " + command + " " + target + " :" + truncated + "\r\n";
    }

    return (fullMsg);
}

// Devuelve un canal existente o lo crea si no existe
Channel*    ServerLogic::createChannel(const std::string& name)
{
    std::map<std::string, Channel*>::const_iterator it = _channels.find(name);
    if (it != _channels.end())
        return (it->second);

    Channel* newChannel = new Channel(name, _defaultChannelLimit); // Por defecto límite de clientes
    if (!newChannel)
        throw (ERR_UNKNOWN);

    _channels[name] = newChannel;

    return (newChannel);
}

void    ServerLogic::serverAddClient(int fd)
{
    // Si ya existe, no hacemos nada
    if (_clients.find(fd) != _clients.end())
        return ;

    Client* newClient = new Client(fd);
    if (!newClient)
        throw (ERR_UNKNOWN);

    _clients[fd] = newClient;
}

void    ServerLogic::serverRemoveClient(int fd)
{
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return ;

    Client* client = it->second;

    // Limpiar canales a los que pertenece
    std::set<std::string> channelsCopy = client->getChannels();
    std::set<std::string>::iterator chIt;
    for (chIt = channelsCopy.begin(); chIt != channelsCopy.end(); ++chIt)
    {
        Channel* channel = getChannel(*chIt); // siempre devuelve canal existente
        channel->removeClient(client);
    }

    // Limpiar nicknames
    if (!client->getNickname().empty())
        _nicknames.erase(client->getNickname());

    // Borrar de mapa y liberar memoria
    _clients.erase(it);
    delete (client);

    std::cout << CYAN << "📌 Client removed with fd " 
              << to_string_c98(fd) << RESET << std::endl;
}

void    ServerLogic::executeCommand(const Command& cmd, int clientFd)
{
    // Primero, buscamos el cliente
    std::map<int, Client*>::iterator it = _clients.find(clientFd);
    if (it == _clients.end())
        throw (ERR_UNKNOWN);

    Client* client = it->second;
    const std::string& command = cmd.name;        // ej: "NICK", "USER", "JOIN"

    // Comparar comandos y llamar al handler correspondiente
    try
    {
        /*User Authentication*/
        if (command == "PASS")
            handlePASS(client, cmd);
        else if (command == "PING")
            handlePING(client, cmd);
        else if (command == "PONG")
            handlePONG(client, cmd);
        else if (command == "QUIT")
            handleQUIT(client, cmd);

        /*User Registration*/
        else if (command == "NICK")
            handleNICK(client, cmd);
        else if (command == "USER")
            handleUSER(client, cmd);

        /*Channel Operations*/
        else if (command == "JOIN")
            handleJOIN(client, cmd);
        else if (command == "PART")
            handlePART(client, cmd);
        else if (command == "TOPIC")
            handleTOPIC(client, cmd);
        else if (command == "NAMES")
            handleNAMES(client, cmd);
        else if (command == "LIST")
            handleLIST(client); //no necesita cmd
        else if (command == "INVITE")
            handleINVITE(client, cmd);
        else if (command == "KICK")
            handleKICK(client, cmd);

        /*Server Queries and Information*/
        else if (command == "MOTD")
            handleMOTD(client); //no necesita cmd
        else if (command == "VERSION")
            handleVERSION(client); //no necesita cmd
        else if (command == "ADMIN")
            handleADMIN(client); //no necesita cmd
        else if (command == "TIME")
            handleTIME(client); //no necesita cmd
        else if (command == "INFO")
            handleINFO(client); //no necesita cmd
        else if (command == "MODE")
            handleMODE(client, cmd);

        /*Sending Messages*/
        else if (command == "NOTICE")
            handleNOTICE(client, cmd);
        else if (command == "PRIVMSG")
            handlePRIVMSG(client, cmd);
    }
    catch(int error)
    {
        throw (error);
    }
}
