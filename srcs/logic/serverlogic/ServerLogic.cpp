#include "../../../includes/logic/ServerLogic.hpp"

/*--------------------------------CONSTRUCTORS--------------------------------*/

ServerLogic::ServerLogic(Server* server,
                            const std::string& serverName,
                            const std::string& serverPassword)
    :   _server(server),
        _serverName(serverName),
        _serverPassword(serverPassword),
        _serverHost("localhost"),
        _serverVersion("1.0"),
        _serverStartTime(std::time(NULL)),
        _serverClients(),
        _serverNicknames(),
        _serverChannels(),
        _prefix(""),
        _errorMsg(""),
        _replyMsg("")
{
    std::cout << "[INFO] ServerLogic initialized. Server name: " << _serverName << ", Password: " << (serverPassword.empty() ? "<none>" : "<set>") << std::endl;
}

/*--------------------------------DESTRUCTORS---------------------------------*/

ServerLogic::~ServerLogic()
{
    // Limpiamos todos los clientes
    std::map<int, Client*>::iterator itClient = _serverClients.begin();
    while (itClient != _serverClients.end())
    {
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

    _server = NULL;
    //_serverName.clear();
    //_serverPassword.clear();
    _serverHost.clear();
    //_serverVersion.clear();
    //_serverStartTime.clear();
    _prefix.clear();
    _errorMsg.clear();
    _replyMsg.clear();

    std::cout << "[INFO] ServerLogic destroyed." << std::endl;
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

std::time_t ServerLogic::getServerStartTime() const
{
    return (_serverStartTime);
}

/*----------------------------------SETTERS------------------------------------*/

void    ServerLogic::setHostname(const std::string& hostname)
{
    _serverHost = hostname;
}

void    ServerLogic::setClientRegistered(Client* client)
{
    if (client->isRegistered())
        return ;

    client->setRegistered(true);

    _replyMsg.clear();

    _replyMsg = buildReplyMessage(messagesReplay[RPL_WELCOME].code,
                                    client,
                                    "",
                                    "",
                                    messagesReplay[RPL_WELCOME].message + _serverName);
    _replyMsg += buildReplyMessage(messagesReplay[RPL_YOURHOST].code,
                                    client,
                                    "",
                                    "",
                                    messagesReplay[RPL_YOURHOST].message + _serverHost + ", version " + _serverVersion);
    _replyMsg += buildReplyMessage(messagesReplay[RPL_CREATED].code,
                                    client,
                                    "",
                                    "",
                                    messagesReplay[RPL_CREATED].message + time_to_string(_serverStartTime));
    _replyMsg += buildReplyMessage(messagesReplay[RPL_MYINFO].code,
                                    client,
                                    "",
                                    "",
                                    messagesReplay[RPL_MYINFO].message + _serverName + " " + _serverVersion + " o O"); //preguntar "ao mtov"??

    sendMessageToClient(client, _replyMsg);  
}

/*----------------------------------METHODS------------------------------------*/

std::string ServerLogic::buildReplyMessage(std::string code,
                                        const Client* client,
                                        const std::string& target,
                                        const std::string& aux,
                                        const std::string& msg) const
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
    return (fullMsg);
}

void    ServerLogic::serverAddClient(int fd)
{
    // Si ya existe, no hacemos nada
    if (_serverClients.find(fd) != _serverClients.end())
        return ;

    Client* newClient = NULL;
    try
    {
        newClient = new Client(fd);
    }
    catch(const std::bad_alloc& e)
    {
        throw (std::string(e.what()));
    }

    if (_serverClients.size() + 1 > MAX_CLIENTS)
    {
        delete (newClient);
        _errorMsg.clear();
        _errorMsg = ":" + _serverName + " *" + " :Server is full\r\n";
        throw (_errorMsg);
    }
    _serverClients[fd] = newClient;
}

void ServerLogic::serverRemoveClient(int fd)
{
    // 1) Eliminar nickname registrado
    std::map<int, Client*>::iterator it = _serverClients.find(fd);
    if (it == _serverClients.end())
        return ;

    Client* client = it->second;

    if (!client->getNickname().empty())
        _serverNicknames.erase(client->getNickname());

    // 2) Eliminar de todos los canales en los que esté
    std::set<std::string> channelsCopy = client->getChannels();

    std::set<std::string>::const_iterator itCh = channelsCopy.begin();
    while (itCh != channelsCopy.end())
    {
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(*itCh);
        if (chanIt == _serverChannels.end())
            continue ;

        Channel* channel = chanIt->second;
        channel->removeClient(client);

        // Eliminar del lado del cliente
        client->leaveChannel(channel);

        if (channel->getDeleteMe())
        {
            delete channel;
            _serverChannels.erase(chanIt);
        }
        itCh++;
    }

    // 3) Borrar del mapa de fds y liberar memoria
    _serverClients.erase(it);
    delete client;
}

// Devuelve un canal existente o lo crea si no existe
Channel*    ServerLogic::createChannel(const std::string& name, Client* creator)
{
    // Validar nombre de canal
    if (!Parser::ft_checksinglechannel(name))
    {
        _prefix.clear();
        _prefix = ":" + _serverName + " ";

        _errorMsg.clear();
        _errorMsg = buildErrorMessage(_prefix +
                                        messagesError[ERR_BADCHANMASK].code + " " + creator->getNickname() + " ",
                                        name,
                                        messagesError[ERR_BADCHANMASK].message);
        sendMessageToClient(creator, _errorMsg);
        return (NULL);
    }

    // Buscar canal existente
    std::map<std::string, Channel*>::const_iterator it = _serverChannels.find(name);
    if (it != _serverChannels.end())
        return (it->second);

    // Crear nuevo canal
    Channel* newChannel = NULL;
    try
    {
        newChannel = new Channel(name); // Por defecto límite de clientes
    }
    catch(const std::bad_alloc& e)
    {
        throw(std::string(e.what()));
    }

    if (_serverChannels.size() + 1 > MAX_CHANNELS)
    {
        delete (newChannel);
        _errorMsg.clear();
        _errorMsg = ":" + _serverName + " *" + " :Server is full\r\n";
        throw (_errorMsg);
    }
    // Añadir al mapa de canales
    _serverChannels[name] = newChannel;

    // El creador es operador por defecto
    newChannel->addOperator(creator);

    return (newChannel);
}

void    ServerLogic::executeCommand(const ParsedInput& input, int clientFd)
{
    if (input.name.empty() )
        return ;
    
    // Primero, buscamos el cliente
    std::map<int, Client*>::iterator it = _serverClients.find(clientFd);
    if (it == _serverClients.end())
        return ;

    Client* client = it->second;
    const std::string& command = input.name;

    // Comparar comandos y llamar al handler correspondiente
    try
    {
        /*User Authentication*/
        if (command == "PASS")
            handlePASS(client, input);
        else if (command == "QUIT")
            handleQUIT(client, input);
        /*User Registration*/
        else if (command == "NICK")
            handleNICK(client, input);
        else if (command == "USER")
            handleUSER(client, input);
        /*Channel Operations*/
        else if (command == "JOIN")
            handleJOIN(client, input);
        else if (command == "PART")
            handlePART(client, input);
        else if (command == "TOPIC")
            handleTOPIC(client, input);
        else if (command == "INVITE")
            handleINVITE(client, input);
        else if (command == "KICK")
            handleKICK(client, input);
        else if (command == "MODE")
            handleMODE(client, input);
        /*Sending Messages*/
        else if (command == "NOTICE")
            handleNOTICE(client, input);
        else if (command == "PRIVMSG")
            handlePRIVMSG(client, input);
    }
    catch(const std::string& errorMsg)
    {
        throw (errorMsg);
    }
}

void ServerLogic::handleOperator(Channel* channel, Client* client)
{
    std::set<Client*> clientsCopy = channel->getClients();
    std::string modeMsg;
    
    // Asignar nuevo operador si hay mas clientes y no hay operadores
    if (channel->getOperators().size() == 0)
    {
        std::set<Client *>::const_iterator it = clientsCopy.begin();
        while (it != clientsCopy.end())
        {
            Client* potentialOp = *it;
            if (potentialOp != client)
            {
                channel->addOperator(potentialOp);
                _prefix.clear();
                _prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;

                modeMsg.clear();
                modeMsg = buildMessage(_prefix,
                                        "MODE",
                                        channel->getName(),
                                        "+o " + potentialOp->getNickname());
                sendMessageToChannel(channel, modeMsg, NULL);
                break ;
            }
            it++;
        }
    }
}

void    ServerLogic::sendMessageToClient(Client* client, const std::string& aux)
{
    if (client)
    {
        _server->queueMessage(client->getFd(), aux);
    }
}

void    ServerLogic::sendMessageToChannel(Channel* channel, const std::string& aux, Client* sender)
{
    if (!channel)
        return ;
    
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
