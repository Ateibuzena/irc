#include "logic/Channel.hpp"
#include "logic/Client.hpp"

/*-------------------------------------CONSTRUCTORS------------------------------------*/

Channel::Channel(const std::string& nameValue, size_t maxClientsValue)
    :   _name(nameValue),
        _maxClients(maxClientsValue)
{
    std::cout << "Channel created: " << _name << " with max clients: " << _maxClients << std::endl;
}

/*-------------------------------------DESTRUCTORS------------------------------------*/

Channel::~Channel()
{
    _name.clear();
    _clients.clear();
    _maxClients = 0;
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

    std::set<Client*>::iterator it = _clients.begin();
    while (it != _clients.end())
    {
        Client* client = *it;
        nicknames.push_back(client->getNickname());
        ++it;
    }
    return (nicknames);
}

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