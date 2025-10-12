#include "logic/Channel.hpp"
#include "logic/Client.hpp"

/*-------------------------------------CONSTRUCTORS------------------------------------*/

Channel::Channel(const std::string& nameValue, size_t maxClients)
    :   _name(nameValue),
        _maxClients(maxClients)
{
    // Constructor simple, inicializa solo el nombre
    if (!isValidName(nameValue))
        std::cerr << "⚠️  Nombre de canal inválido: " << nameValue << std::endl;
    else
        std::cout << "Channel created: " << _name << std::endl;
}

/*-------------------------------------DESTRUCTORS------------------------------------*/

Channel::~Channel()
{
    // Destructor: opcional limpiar punteros, pero no borramos Clients
    _clients.clear();
    std::cout << "Channel destroyed: " << _name << std::endl;
}

/*-------------------------------------GETTERS------------------------------------*/

const std::string& Channel::getName() const
{
    return (_name);
}

const std::set<Client *>& Channel::getClients() const
{
    return (_clients);
}

std::vector<std::string> Channel::getNicknames() const
{
    std::vector<std::string> nicknames;
    for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        Client* client = *it;
        nicknames.push_back(client->getNickname());
    }
    return (nicknames);
}

/*-------------------------------------METHODS------------------------------------*/

/*✅ Cosas importantes de esta implementación:

- Usamos std::set<Client*> para evitar duplicados automáticamente en addClient.

- removeClient hace un pequeño mensaje de debug si el cliente no existía.

- broadcast recorre todos los clientes y usa Client::receiveMessage para simular envío de mensajes, excluyendo opcionalmente al remitente.

- No borramos los punteros Client* en el destructor, porque la gestión de memoria la debe hacer quien crea los Client.

- joinChannel recibe un puntero a un Channel existente en memoria (Channel*).

- Esto permite que el método:

    Interactúe con el canal real: _clients.insert(this)

    Revise el límite de clientes

    Devuelva un ChannelStatus real según lo que pase (JOIN_SUCCESS, ALREADY_IN_CHANNEL, CHANNEL_FULL)

    Solo agregue el nombre del canal a _channels si realmente pudo unirse

En otras palabras: el Channel ya existe como objeto en memoria, con su propio _clients, y Client puede interactuar con él directamente.

✅ Antes: solo manejabas strings, no objetos reales.
✅ Ahora: manejas objetos reales, como haría un IRC server de verdad.
*/

ChannelStatus Channel::addClient(Client* clientValue)
{
    if (!clientValue)
        return (INVALID_CLIENT);

    if (_clients.find(clientValue) != _clients.end())
        return (ALREADY_IN_CHANNEL);

    if (_clients.size() >= _maxClients)
        return (CHANNEL_FULL);

    _clients.insert(clientValue);
    return (JOIN_SUCCESS);
}

ChannelStatus   Channel::removeClient(Client* clientValue)
{
    if (!clientValue)
        return (INVALID_CLIENT);
    if (_clients.erase(clientValue) == 0)
        return (NOT_IN_CHANNEL);
    return (LEAVE_SUCCESS);
}

/*Ahora broadcast siempre excluye a exceptFd solo si se pasa. Podrías tener un booleano includeSender para mayor claridad*/

void    Channel::broadcast(const std::string& messageValue, int exceptFd)
{
    for (std::set<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        Client* client = *it;
        if (client->getFd() != exceptFd)
        {
            client->receiveMessage(messageValue, "Channel:" + _name);
        }
    }
}

bool    Channel::hasClient(Client* client) const
{
    return (_clients.find(client) != _clients.end());
}