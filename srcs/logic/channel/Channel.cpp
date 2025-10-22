#include "../../../includes/logic/Channel.hpp"
#include "../../../includes/logic/Client.hpp"

/*-------------------------------------CONSTRUCTORS------------------------------------*/

Channel::Channel(const std::string& name, size_t maxClients)
    :   _name(name),
        _topic(""),
        _clients(),
        _operators(),
        _invited(),
        _maxClients(maxClients),
        _isInviteOnly(false)
{
    std::cout << "Channel created: " << _name << " with max clients: " << _maxClients << std::endl;
}

/*-------------------------------------DESTRUCTORS------------------------------------*/

Channel::~Channel()
{
    _topic.clear();
    _clients.clear();
    _operators.clear();
    _invited.clear();
    _isInviteOnly = false;
    std::cout << "Channel destroyed: " << _name << std::endl;
}

/*-------------------------------------GETTERS------------------------------------*/

const std::string& Channel::getName() const
{
    return (_name);
}

const std::string& Channel::getTopic() const
{
    return (_topic);
}

const std::set<Client *>& Channel::getClients() const
{
    return (_clients);
}

const std::set<Client *>& Channel::getOperators() const
{
    return (_operators);
}

const std::set<Client *>& Channel::getInvited() const
{
    return (_invited);
}

bool Channel::isInviteOnly() const
{
    return (_isInviteOnly);
}

size_t Channel::getMaxClients() const
{
    return (_maxClients);
}

/*-------------------------------------SETTERS------------------------------------*/

void Channel::setTopic(const std::string& topic)
{
    _topic = topic;
}

void Channel::setInviteOnly(bool inviteOnly)
{
    _isInviteOnly = inviteOnly;
}
