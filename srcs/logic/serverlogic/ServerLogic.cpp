#include "ServerLogic.hpp"

/*--------------------------------CONSTRUCTORS--------------------------------*/

ServerLogic::ServerLogic()
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

/*----------------------------------HELPERS-----------------------------------*/

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
    if (!client->getUsername().empty())
        client->setRegistered(true);

    // Añadimos al map de nicknames
    _nicknames[nickname] = client;
}

// Establecer username
void    ServerLogic::handleUSER(Client* client, const std::string& username)
{
    client->setUsername(username);
    if (!client->getNickname().empty())
        client->setRegistered(true);
}

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
void    ServerLogic::handlePART(Client* client, const std::string& channelName)
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

void    ServerLogic::addClientServer(int fd)
{
    // Si ya existe, no hacemos nada
    if (_clients.find(fd) != _clients.end())
        return ;

    Client* newClient = new Client(fd);
    if (!newClient)
        throw (ERR_UNKNOWN);

    _clients[fd] = newClient;
}

void    ServerLogic::removeClient(int fd)
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
    const std::string& commandName = cmd.name;        // ej: "NICK", "USER", "JOIN"
    const std::vector<std::string>& params = cmd.params; // ej: {"#channel", "key", "verga"}

    // Comparar comandos y llamar al handler correspondiente
    try
    {
        if (commandName == "NICK" && !params.empty())
            handleNICK(client, params[0]);
        else if (commandName == "USER" && !params.empty())
            handleUSER(client, params[0]);
        else if (commandName == "JOIN" && !params.empty())
            handleJOIN(client, params[0]);
        else if (commandName == "PART" && !params.empty())
            handlePART(client, params[0]);
        else if (commandName == "PRIVMSG" && params.size() >= 2)
            handlePRIVMSG(client, params[0], params[1]);
    }
    catch(int error)
    {
        throw (error);
    }
}
