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