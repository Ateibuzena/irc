#include "logic/Channel.hpp"
#include "logic/Client.hpp"

/*-------------------------------------CONSTRUCTORS------------------------------------*/

Channel::Channel(const std::string& name, size_t maxClients)
    :   _name(name),
        _maxClients(maxClients)
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
