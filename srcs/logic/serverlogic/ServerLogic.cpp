#include "ServerLogic.hpp"

/*💡 Notas:

- Usamos separator, info y success de tu TestUtils.hpp para que la salida sea clara y bonita.

- Liberamos toda la memoria dinámica (Client* y Channel*) para evitar leaks.

- disconnect() del cliente limpia su estado antes de borrarlo.

- Esto no hace nada de lógica de IRC todavía, solo gestiona la creación y destrucción de objetos del servidor.*/

/*--------------------------------CONSTRUCTORS--------------------------------*/
ServerLogic::ServerLogic(size_t defaultChannelLimit)
    : _defaultChannelLimit(defaultChannelLimit)
{
    std::cout << GREEN << "🟢 ServerLogic Constructor" << RESET << std::endl;
    std::cout << BLUE << "Servidor inicializado. Clientes y canales vacíos." << RESET << std::endl;
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
            std::cout << GREEN << "✅ Cliente con fd " << fd << " eliminado." << RESET << std::endl;
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
            std::cout << GREEN << "✅ Canal " << it->first << " eliminado." << RESET << std::endl;
        }
    }
    _channels.clear();

    std::cout << BLUE << "Servidor destruido correctamente." << RESET << std::endl;
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

    std::cout << BLUE << "ℹ️  handleNICK: Cliente fd " << client->getFd() << " cambia nickname a " << nickname << RESET << std::endl;

    // Comprobamos si ya existe otro cliente con ese nickname
    if (_nicknames.find(nickname) != _nicknames.end())
    {
        std::cout << YELLOW << "⚠️  Nickname ya en uso: " << nickname << RESET << std::endl;
        return ;
    }

    // Si tenía un nickname anterior, lo eliminamos del map
    if (!client->getNickname().empty())
        _nicknames.erase(client->getNickname());

    client->setNickname(nickname);
    _nicknames[nickname] = client;

    std::cout << GREEN << "✅ Nickname actualizado: " << nickname << RESET << std::endl;
}

// Establecer username
void    ServerLogic::handleUSER(Client* client, const std::string& username)
{
    if (!client)
        return ;

    std::cout << BLUE << "ℹ️  handleUSER: Cliente fd " << client->getFd() << " username: " << username << RESET << std::endl;
    client->setUsername(username);
    std::cout << GREEN << "✅ Username establecido: " << username << RESET << std::endl;
}

// Unirse a un canal (o crearlo si no existe)
void    ServerLogic::handleJOIN(Client* client, const std::string& channelName)
{
    if (!client)
        return ;

    std::cout << BLUE << "ℹ️  handleJOIN: Cliente " << client->getNickname() << " se une al canal " << channelName << RESET << std::endl;

    Channel* channel = getOrCreateChannel(channelName);
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
            std::cout << RED << "❌ Error al unirse al canal " << channelName << RESET << std::endl;
            break ;
    }
}

// Salir de un canal
void    ServerLogic::handlePART(Client* client, const std::string& channelName)
{
    if (!client)
        return ;

    std::cout << BLUE << "ℹ️  handlePART: Cliente " << client->getNickname() << " sale del canal " << channelName << RESET << std::endl;

    std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
    if (it == _channels.end())
    {
        std::cout << YELLOW << "⚠️  Canal no existe: " << channelName << RESET << std::endl;
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
            std::cout << RED << "❌ Error al salir del canal " << channelName << RESET << std::endl;
            break ;
    }
}

// Enviar mensaje privado o a canal
void    ServerLogic::handlePRIVMSG(Client* client, const std::string& target, const std::string& msg)
{
    if (!client)
        return ;

    std::cout << BLUE << "ℹ️  handlePRIVMSG: Cliente " << client->getNickname() << " -> " << target << ": " << msg << RESET << std::endl;

    // Primero buscamos si es un canal
   std::map<std::string, Channel*>::iterator chanIt = _channels.find(target);
    if (chanIt != _channels.end())
    {
        Channel* channel = chanIt->second;
        channel->broadcast(msg, client);
        std::cout << GREEN << "✅ Mensaje enviado a canal " << target << RESET << std::endl;
        return ;
    }

    // Luego buscamos si es un cliente por nickname
    std::map<std::string, Client*>::iterator nickIt = _nicknames.find(target);
    if (nickIt != _nicknames.end())
    {
        Client* recipient = nickIt->second;
        recipient->receiveMessage(msg, client->getNickname());
        std::cout << GREEN << "✅ Mensaje privado enviado a " << target << RESET << std::endl;
        return ;
    }


    std::cout << YELLOW << "⚠️  Destino no encontrado: " << target << RESET << std::endl;
}

/*----------------------------------GETTERS------------------------------------*/

// Devuelve el cliente según su fd
Client* ServerLogic::getClient(int fd)
{
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if (it != _clients.end())
    {
        std::string msg = "📌 getClient: Encontrado cliente fd " + to_string_c98(fd);
        std::cout << CYAN << msg << RESET << std::endl;
        return (it->second);
    }

    std::string msg = "⚠️ getClient: Cliente no encontrado fd " + to_string_c98(fd);
    std::cout << YELLOW << msg << RESET << std::endl;
    return (NULL);
}

// Devuelve un canal existente o lo crea si no existe
Channel*    ServerLogic::getOrCreateChannel(const std::string& name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end())
    {
        std::string msg = "📌 getOrCreateChannel: Canal existente " + name;
        std::cout << CYAN << msg << RESET << std::endl;
        return (it->second);
    }

    std::string msg = "🟢 getOrCreateChannel: Creando canal " + name;
    std::cout << BLUE << msg << RESET << std::endl;

    Channel* newChannel = new Channel(name, _defaultChannelLimit); // Por defecto límite de clientes
    _channels[name] = newChannel;

    std::string successMsg = "✅ Canal creado: " + name;
    std::cout << GREEN << successMsg << RESET << std::endl;

    return (newChannel);
}

/*----------------------------------METHODS------------------------------------*/

void    ServerLogic::addClient(int fd)
{
    if (_clients.find(fd) != _clients.end())
    {
        std::cout << YELLOW << "⚠️ addClient: Cliente con fd " 
                  << to_string_c98(fd) << " ya existe." << RESET << std::endl;
        return ;
    }

    Client* newClient = new Client(fd);
    _clients[fd] = newClient;

    std::cout << CYAN << "📌 Cliente agregado con fd " 
              << to_string_c98(fd) << RESET << std::endl;
}

void    ServerLogic::removeClient(int fd)
{
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if (it == _clients.end())
    {
        std::cout << YELLOW << "⚠️ removeClient: Cliente con fd " 
                  << to_string_c98(fd) << " no existe." << RESET << std::endl;
        return ;
    }

    Client* client = it->second;

    // Limpiar canales a los que pertenece
    std::set<std::string> channelsCopy = client->getChannels();
    std::set<std::string>::iterator chIt;
    for (chIt = channelsCopy.begin(); chIt != channelsCopy.end(); ++chIt)
    {
        Channel* channel = getOrCreateChannel(*chIt); // siempre devuelve canal existente
        channel->removeClient(client);
    }

    // Limpiar nicknames
    if (!client->getNickname().empty())
        _nicknames.erase(client->getNickname());

    // Borrar de mapa y liberar memoria
    _clients.erase(it);
    delete (client);

    std::cout << CYAN << "📌 Cliente eliminado con fd " 
              << to_string_c98(fd) << RESET << std::endl;
}

void    ServerLogic::executeCommand(const Command& cmd, int clientFd)
{
    // Primero, buscamos el cliente
    std::map<int, Client*>::iterator it = _clients.find(clientFd);
    if (it == _clients.end())
    {
        std::cout << YELLOW << "⚠️ executeCommand: Cliente con fd " 
                  << to_string_c98(clientFd) << " no encontrado." << RESET << std::endl;
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
        std::cout << YELLOW << "⚠️ executeCommand: Comando desconocido o parámetros faltantes: "
                  << commandName << RESET << std::endl;
    }
}
