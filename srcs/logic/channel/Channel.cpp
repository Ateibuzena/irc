#include "logic/Channel.hpp"

/*-------------------------------------CONSTRUCTORS------------------------------------*/

Channel::Channel(const std::string& nameValue)
    :   _name(nameValue)
{
     // Constructor simple, inicializa solo el nombre
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

/*-------------------------------------METHODS------------------------------------*/

/*✅ Cosas importantes de esta implementación:

- Usamos std::set<Client*> para evitar duplicados automáticamente en addClient.

- removeClient hace un pequeño mensaje de debug si el cliente no existía.

- broadcast recorre todos los clientes y usa Client::receiveMessage para simular envío de mensajes, excluyendo opcionalmente al remitente.

- No borramos los punteros Client* en el destructor, porque la gestión de memoria la debe hacer quien crea los Client.
*/

void    Channel::addClient(Client* clientValue)
{
    if (!_clients.insert(clientValue).second)
    {
        std::cout << "Client ya está en el canal " << _name << std::endl;
    }
}

void    Channel::removeClient(Client* clientValue)
{
    if (_clients.erase(clientValue) == 0)
    {
        std::cout << "Client no estaba en el canal " << _name << std::endl;
    }
}

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
