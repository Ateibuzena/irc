#include "../../../includes/logic/ServerLogic.hpp"

/*--------------------------------CONSTRUCTORS--------------------------------*/

ServerLogic::ServerLogic(Server* server, const std::string& serverName, const std::string& serverPassword)
    :   _server(server),
        _serverName(serverName),
        _serverPassword(serverPassword),
        _serverHost("localhost"),
        _serverVersion("1.0"),
        _serverStartTime(std::time(NULL)),
        _serverClients(),
        _serverNicknames(),
        _serverChannels()
{
    std::cout << "✅ ServerLogic initialized." << std::endl;
}

/*--------------------------------DESTRUCTORS---------------------------------*/

ServerLogic::~ServerLogic()
{
    // Limpiamos todos los clientes
    std::map<int, Client*>::iterator itClient = _serverClients.begin();
    while (itClient != _serverClients.end())
    {
        // Cerramos el socket del cliente
        int fd = itClient->first;
        if (fd >= 0)
            close(fd);

        // Liberamos memoria
        Client* client = itClient->second;
        if (client)
            delete (client);
        ++itClient;
    }
    _serverClients.clear();

    // Limpiamos nicknames
    _serverNicknames.clear();

    // Limpiamos todos los canales
    std::map<std::string, Channel*>::iterator itChannel = _serverChannels.begin();
    while (itChannel != _serverChannels.end())
    {
        Channel* channel = itChannel->second;
        if (channel)
            delete (channel);
        ++itChannel;
    }
    _serverChannels.clear();
}

/*----------------------------------GETTERS------------------------------------*/

// Devuelve el cliente según su fd
Client* ServerLogic::getClient(int fd) const
{
    std::map<int, Client*>::const_iterator it = _serverClients.find(fd);
    if (it != _serverClients.end())
        return (it->second);
    return (NULL);
}

Channel* ServerLogic::getChannel(const std::string& name) const
{
    std::map<std::string, Channel*>::const_iterator it = _serverChannels.find(name);
    if (it != _serverChannels.end())
        return (it->second);
    return (NULL);
}

const std::string& ServerLogic::getServerName() const
{
    return (_serverName);
}

const std::string& ServerLogic::getServerHost() const
{
    return (_serverHost);
}

const std::string& ServerLogic::getServerVersion() const
{
    return (_serverVersion);
}

/*----------------------------------SETTERS------------------------------------*/

void    ServerLogic::setHostname(const std::string& hostname)
{
    _serverHost = hostname;
}

void    ServerLogic::setClientRegistered(Client* client)
{
    client->setRegistered(true);

    std::string replayMsg;

    replayMsg = buildReplyMessage(messagesReplay[RPL_WELCOME].code, client, NULL, NULL, messagesReplay[RPL_WELCOME].message + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost);
    replayMsg += buildReplyMessage(messagesReplay[RPL_YOURHOST].code, client, NULL, NULL, messagesReplay[RPL_YOURHOST].message + _serverName + ", version " + _serverVersion);
    replayMsg += buildReplyMessage(messagesReplay[RPL_CREATED].code, client, NULL, NULL, messagesReplay[RPL_CREATED].message + time_to_string(_serverStartTime));
    replayMsg += buildReplyMessage(messagesReplay[RPL_MYINFO].code, client, NULL, NULL, messagesReplay[RPL_MYINFO].message + _serverName + " " + _serverVersion + " o O"); //preguntar "ao mtov"??

    sendMessageToClient(client, replayMsg);
}

/*----------------------------------METHODS------------------------------------*/

std::string ServerLogic::buildMessage(const std::string& prefix,
                                       const std::string& command,
                                       const std::string& target,
                                       const std::string& aux) const
{
    // Construir el mensaje completo
    std::string fullMsg;

    if (command == "NICK"
        || command == "QUIT")
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

std::string ServerLogic::buildReplyMessage(std::string code,
                                        const Client* client,
                                        const std::string& target,
                                        const std::string& aux = "",
                                        const std::string& msg = "") const
{
    std::string fullMsg;

    if (!target.empty() && !aux.empty() && msg.empty())
        fullMsg = ":" + _serverName + " " + code + " " + client->getNickname() + " " + target + " " + aux + "\r\n";
    else if (!target.empty() && aux.empty() && !msg.empty())
        fullMsg = ":" + _serverName + " " + code + " " + client->getNickname() + " " + target + " :" + msg + "\r\n";
    else if (target.empty() && aux.empty() && !msg.empty())
        fullMsg = ":" + _serverName + " " + code + " " + client->getNickname() + " :" + msg + "\r\n";
    else
        fullMsg = "You're not supposed to go in here\r\n"; // POR AHORA MAMAHUEVA
}

// Devuelve un canal existente o lo crea si no existe
Channel*    ServerLogic::createChannel(const std::string& name, Client* creator)
{
    std::map<std::string, Channel*>::const_iterator it = _serverChannels.find(name);
    if (it != _serverChannels.end())
        return (it->second);

    Channel* newChannel = new Channel(name); // Por defecto límite de clientes
    if (!newChannel)
        throw (ERR_UNKNOWN);

    _serverChannels[name] = newChannel;
    
    newChannel->addOperator(creator); // El creador es operador por defecto

    return (newChannel);
}

void    ServerLogic::serverAddClient(int fd)
{
    // Si ya existe, no hacemos nada
    if (_serverClients.find(fd) != _serverClients.end())
        return ;

    Client* newClient = new Client(fd);
    if (!newClient)
        throw (ERR_UNKNOWN);

    _serverClients[fd] = newClient;
}

void    ServerLogic::serverRemoveClient(int fd)
{
    // Buscar cliente
    std::map<int, Client*>::iterator it = _serverClients.find(fd);
    if (it == _serverClients.end())
        return ;

    Client* client = it->second;

    // Cerrar socket si es válido
    if (fd >= 0)
        close(fd);

    /*// Limpiar canales a los que pertenece
    std::set<std::string> channelsCopy = client->getChannels();
    std::set<std::string>::iterator chIt;
    for (chIt = channelsCopy.begin(); chIt != channelsCopy.end(); ++chIt)
    {
        Channel* channel = getChannel(*chIt); // siempre devuelve canal existente
        channel->removeClient(client);
        if (channel->getDeleteMe())
        {
            delete (channel);
            _serverChannels.erase(chIt);
        }
    }*/

    // Limpiar nicknames
    if (!client->getNickname().empty())
        _serverNicknames.erase(client->getNickname());

    // Borrar de mapa y liberar memoria
    _serverClients.erase(it);
    delete (client);

    /*también enviar un mensaje tipo PART a los demás clientes si quieres avisar que se fue.*/
    std::cout << CYAN << "📌 Client removed with fd " 
              << to_string_c98(fd) << RESET << std::endl;
}

void    ServerLogic::executeCommand(const Command& cmd, int clientFd)
{
    // Primero, buscamos el cliente
    std::map<int, Client*>::iterator it = _serverClients.find(clientFd);
    if (it == _serverClients.end())
        throw (ERR_UNKNOWN);

    Client* client = it->second;
    const std::string& command = cmd.name;        // ej: "NICK", "USER", "JOIN"

    // Comparar comandos y llamar al handler correspondiente
    try
    {
        /*User Authentication*/
        if (command == "PASS")
            handlePASS(client, cmd);
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
        else if (command == "INVITE")
            handleINVITE(client, cmd);
        else if (command == "KICK")
            handleKICK(client, cmd);
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

void    ServerLogic::sendMessageToClient(Client* client, const std::string& aux)
{
    if (client)
        _server->queueMessage(client->getFd(), aux);
}

void    ServerLogic::sendMessageToChannel(Channel* channel, const std::string& aux, Client* sender)
{
    if (!channel)
        return ;
    // Hacemos una copia para iterar seguro aunque un cliente se elimine
    std::set<Client*> clientsCopy = channel->getClients();

    std::set<Client*>::iterator it = clientsCopy.begin();
    while (it != clientsCopy.end())
    {
        Client* client = *it;
        if (client != sender)
            sendMessageToClient(client, aux);
        ++it;
    }
}
