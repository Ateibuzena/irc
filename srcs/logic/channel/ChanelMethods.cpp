#include "logic/Channel.hpp"
#include "logic/Client.hpp"

/*-------------------------------------METHODS------------------------------------*/

void    Channel::addClient(Client* clientValue)
{
    if (clientValue->getChannels().size() >= MAX_CHANNELS_PER_CLIENT)
        throw (ERR_TOOMANYCHANNELS);
    if (_clients.size() >= _maxClients)
        throw (ERR_CHANNELISFULL);
    _clients.insert(clientValue);
}

ChannelStatus   Channel::removeClient(Client* clientValue)
{
    if (_clients.erase(clientValue) == 0)
        throw (ERR_NOTONCHANNEL);
}

void    Channel::broadcast(const std::string& msg, Client* sender)
{
    // Hacemos una copia para iterar seguro aunque un cliente se elimine
    std::set<Client*> clientsCopy = _clients;

    std::set<Client*>::iterator it = clientsCopy.begin();
    while (it != clientsCopy.end())
    {
        Client* client = *it;
        if (client != sender)
            client->receiveMessage(msg, _name);
        ++it;
    }
}

bool    Channel::hasClient(Client* client) const
{
    return (_clients.find(client) != _clients.end());
}