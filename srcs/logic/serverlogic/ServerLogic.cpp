#include "ServerLogic.hpp"

/*💡 Notas:

- Usamos separator, info y success de tu TestUtils.hpp para que la salida sea clara y bonita.

- Liberamos toda la memoria dinámica (Client* y Channel*) para evitar leaks.

- disconnect() del cliente limpia su estado antes de borrarlo.

- Esto no hace nada de lógica de IRC todavía, solo gestiona la creación y destrucción de objetos del servidor.*/

/*--------------------------------CONSTRUCTORS--------------------------------*/

ServerLogic::ServerLogic(size_t defaultChannelLimitValue)
{
    if (defaultChannelLimitValue <= 0 || defaultChannelLimitValue > MAX_USERS_PER_CHANNEL)
        _defaultChannelLimit = MAX_USERS_PER_CHANNEL;
    else
        _defaultChannelLimit = defaultChannelLimitValue;

    std::cout << "✅ ServerLogic initialized with default limit = "
              << _defaultChannelLimit << std::endl;
}

/*--------------------------------DESTRUCTORS---------------------------------*/

ServerLogic::~ServerLogic()
{
    std::cout << RED << "🔴 ServerLogic Destructor" << RESET << std::endl;

    // Limpiamos todos los clientes
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        int fd = it->first;
        Client* client = it->second;
        if (client)
        {
            client->disconnect(); // limpia datos del cliente
            delete (client);        // liberamos memoria
            std::cout << GREEN << "✅ Client with fd " << fd << " deleted." << RESET << std::endl;
        }
    }
    _clients.clear();
    _nicknames.clear();

    // Limpiamos canales
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        Channel* channel = it->second;
        if (channel)
        {
            delete (channel);
            std::cout << GREEN << "✅ Channel " << it->first << " deleted." << RESET << std::endl;
        }
    }
    _channels.clear();

    std::cout << BLUE << "Server destroyed successfully." << RESET << std::endl;
}

/*----------------------------------HELPERS-----------------------------------*/

/*💡 Notas importantes:

- handleNICK y handleUSER actualizan el cliente y tu mapa _nicknames.

- handleJOIN usa getOrCreateChannel para crear un canal si no existía.

- handlePART revisa primero si el canal existe.

- handlePRIVMSG distingue entre canales y clientes por nickname.

- Todos los métodos usan tus helpers info, success y warn para debug.*/

// Cambiar nickname
void    ServerLogic::handleNICK(Client* client, const std::string& nickname)
{
    if (!client)
        return ;

    std::cout << BLUE << "ℹ️  handleNICK: Client with fd " << client->getFd() << " changes nickname to " << nickname << RESET << std::endl;

    // Comprobamos si ya existe otro cliente con ese nickname
    if (_nicknames.find(nickname) != _nicknames.end())
    {
        std::cout << YELLOW << "⚠️  Nickname already in use: " << nickname << RESET << std::endl;
        return ;
    }

    // Si tenía un nickname anterior, lo eliminamos del map
    if (!client->getNickname().empty())
        _nicknames.erase(client->getNickname());

    client->setNickname(nickname);
    _nicknames[nickname] = client;

    std::cout << GREEN << "✅ Nickname updated: " << nickname << RESET << std::endl;
}

// Establecer username
void    ServerLogic::handleUSER(Client* client, const std::string& username)
{
    if (!client)
        return ;

    std::cout << BLUE << "ℹ️  handleUSER: Client with fd " << client->getFd() << " username: " << username << RESET << std::endl;
    client->setUsername(username);
    std::cout << GREEN << "✅ Username saved: " << username << RESET << std::endl;
}

// Unirse a un canal (o crearlo si no existe)
void    ServerLogic::handleJOIN(Client* client, const std::string& channelName)
{
    if (!client)
        return ;

    std::cout << BLUE << "ℹ️  handleJOIN: Client " << client->getNickname() << " joins channel " << channelName << RESET << std::endl;

    Channel* channel = createChannel(channelName);
    ChannelStatus status = client->joinChannel(channel);

    switch (status)
    {
        case JOIN_SUCCESS:
            std::cout << GREEN << "✅ " << client->getNickname() << " se unió a " << channelName << RESET << std::endl;
            break ;
        case ALREADY_IN_CHANNEL:
            std::cout << YELLOW << "⚠️  " << client->getNickname() << " ya estaba en " << channelName << RESET << std::endl;
            break ;
        case CHANNEL_FULL:
            std::cout << YELLOW << "⚠️  " << channelName << " está lleno" << RESET << std::endl;
            break ;
        default:
            std::cout << RED << "❌ Error to join channel " << channelName << RESET << std::endl;
            break ;
    }
}

// Salir de un canal
void    ServerLogic::handlePART(Client* client, const std::string& channelName)
{
    if (!client)
        return ;

    std::cout << BLUE << "ℹ️  handlePART: Client " << client->getNickname() << " leaves channel " << channelName << RESET << std::endl;

    std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
    if (it == _channels.end())
    {
        std::cout << YELLOW << "⚠️  Channel not found: " << channelName << RESET << std::endl;
        return ;
    }


    Channel* channel = it->second;
    ChannelStatus status = client->leaveChannel(channel);

    switch (status)
    {
        case LEAVE_SUCCESS:
            std::cout << GREEN << "✅ " << client->getNickname() << " salió de " << channelName << RESET << std::endl;
            break ;
        case NOT_IN_CHANNEL:
            std::cout << YELLOW << "⚠️  " << client->getNickname() << " no estaba en " << channelName << RESET << std::endl;
            break ;
        default:
            std::cout << RED << "❌ Error to leave channel " << channelName << RESET << std::endl;
            break ;
    }
}

// Enviar mensaje privado o a canal
void    ServerLogic::handlePRIVMSG(Client* client, const std::string& target, const std::string& msg)
{
    if (!client)
        return ;

    std::cout << BLUE << "ℹ️  handlePRIVMSG: Client " << client->getNickname() << " -> " << target << ": " << msg << RESET << std::endl;

    // Primero buscamos si es un canal
   std::map<std::string, Channel*>::iterator chanIt = _channels.find(target);
    if (chanIt != _channels.end())
    {
        Channel* channel = chanIt->second;
        channel->broadcast(msg, client);
        std::cout << GREEN << "✅ Message sent to channel " << target << RESET << std::endl;
        return ;
    }

    // Luego buscamos si es un cliente por nickname
    std::map<std::string, Client*>::iterator nickIt = _nicknames.find(target);
    if (nickIt != _nicknames.end())
    {
        Client* recipient = nickIt->second;
        recipient->receiveMessage(msg, client->getNickname());
        std::cout << GREEN << "✅ Private message sent to " << target << RESET << std::endl;
        return ;
    }


    std::cout << YELLOW << "⚠️  Destination not found: " << target << RESET << std::endl;
}

/*----------------------------------GETTERS------------------------------------*/

// Devuelve el cliente según su fd
Client* ServerLogic::getClient(int fd) const
{
    std::map<int, Client*>::const_iterator it = _clients.find(fd);
    if (it != _clients.end())
    {
        std::string msg = "📌 getClient: Client found with fd " + to_string_c98(fd);
        std::cout << CYAN << msg << RESET << std::endl;
        return (it->second);
    }

    std::string msg = "⚠️ getClient: Client not found with fd " + to_string_c98(fd);
    std::cout << YELLOW << msg << RESET << std::endl;
    return (NULL);
}

Channel* ServerLogic::getChannel(const std::string& name) const
{
    std::map<std::string, Channel*>::const_iterator it = _channels.find(name);
    if (it != _channels.end())
    {
        std::string msg = "📌 getChannel: Channel found " + name;
        std::cout << CYAN << msg << RESET << std::endl;
        return (it->second);
    }

    std::string msg = "⚠️ getChannel: Channel not found " + name;
    std::cout << YELLOW << msg << RESET << std::endl;
    return (NULL);
}

size_t  ServerLogic::getDefaultChannelLimit() const
{
    return (_defaultChannelLimit);
}

/*----------------------------------METHODS------------------------------------*/

// Devuelve un canal existente o lo crea si no existe
Channel*    ServerLogic::createChannel(const std::string& name)
{
    std::map<std::string, Channel*>::const_iterator it = _channels.find(name);
    if (it != _channels.end())
    {
        std::string msg = "📌 createChannel: Channel already exists " + name;
        std::cout << CYAN << msg << RESET << std::endl;
        return (it->second);
    }

    std::string msg = "🟢 createChannel: Creating channel " + name;
    std::cout << BLUE << msg << RESET << std::endl;

    Channel* newChannel = new Channel(name, _defaultChannelLimit); // Por defecto límite de clientes
    _channels[name] = newChannel;

    std::string successMsg = "✅ Channel created: " + name;
    std::cout << GREEN << successMsg << RESET << std::endl;

    return (newChannel);
}

void    ServerLogic::addClient(int fd)
{
    if (_clients.find(fd) != _clients.end())
    {
        std::cout << YELLOW << "⚠️ addClient: Client with fd " 
                  << to_string_c98(fd) << " already exists." << RESET << std::endl;
        return ;
    }

    Client* newClient = new Client(fd);
    _clients[fd] = newClient;

    std::cout << CYAN << "📌 Client added with fd " 
              << to_string_c98(fd) << RESET << std::endl;
}

void    ServerLogic::removeClient(int fd)
{
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if (it == _clients.end())
    {
        std::cout << YELLOW << "⚠️ removeClient: Client with fd " 
                  << to_string_c98(fd) << " not found." << RESET << std::endl;
        return ;
    }

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
    {
        std::cout << YELLOW << "⚠️ executeCommand: Client with fd " 
                  << to_string_c98(clientFd) << " not found." << RESET << std::endl;
        return ;
    }

    Client* client = it->second;
    const std::string& commandName = cmd.name;        // ej: "NICK", "USER", "JOIN"
    const std::vector<std::string>& params = cmd.params;
    // Comparar comandos y llamar al handler correspondiente
    if (commandName == "NICK" && !params.empty())
    {
        handleNICK(client, params[0]);
    }
    else if (commandName == "USER" && !params.empty())
    {
        handleUSER(client, params[0]);
    }
    else if (commandName == "JOIN" && !params.empty())
    {
        handleJOIN(client, params[0]);
    }
    else if (commandName == "PART" && !params.empty())
    {
        handlePART(client, params[0]);
    }
    else if (commandName == "PRIVMSG" && params.size() >= 2)
    {
        handlePRIVMSG(client, params[0], params[1]);
    }
    else
    {
        std::cout << YELLOW << "⚠️ executeCommand: Command not recognized or missing params: "
                  << commandName << RESET << std::endl;
    }
}
