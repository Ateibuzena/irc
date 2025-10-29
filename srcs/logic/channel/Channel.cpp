#include "../../../includes/logic/Channel.hpp"
#include "../../../includes/logic/Client.hpp"

/*-------------------------------------CONSTRUCTORS------------------------------------*/

Channel::Channel(const std::string& name, size_t maxClients)
    :   _name(name),
        _topic(""),
        _clients(),
        _isInviteOnly(false),
        _isTopicProtected(false),
        _password(""),
        _maxClients(maxClients),
        _operators(),
        _invited(),
        _deleteMe(false)
{
    std::cout << "Channel created: " << _name << " with max clients: " << _maxClients << std::endl;
}

/*-------------------------------------DESTRUCTORS------------------------------------*/

Channel::~Channel()
{
    _topic.clear();
    _clients.clear();
    _isInviteOnly = false;
    _isTopicProtected = false;
    _password.clear();
    _operators.clear();
    _invited.clear();
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

bool Channel::isInviteOnly() const
{
    return (_isInviteOnly);
}

bool Channel::isTopicProtected() const
{
    return (_isTopicProtected);
}

const std::string& Channel::getPassword() const
{
    return (_password);
}

size_t Channel::getMaxClients() const
{
    return (_maxClients);
}

const std::set<Client *>& Channel::getOperators() const
{
    return (_operators);
}

const std::set<Client *>& Channel::getInvited() const
{
    return (_invited);
}

bool Channel::getDeleteMe() const
{
    return (_deleteMe);
}

const std::string Channel::getModes() const
{
    std::string modes = "+";
    if (_isInviteOnly)
        modes += "i";
    if (_isTopicProtected)
        modes += "t";
    if (!_password.empty())
        modes += "k";
    if (!_operators.empty())
        modes += "o";
    if (_maxClients != 0 && _maxClients != 50)
        modes += "l"; // 50 es tu valor por defecto
    return (modes);
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

void Channel::setTopicProtected(bool topicProtected)
{
    _isTopicProtected = topicProtected;
}

void Channel::setPassword(const std::string& password)
{
    _password = password;
}

void Channel::setMaxClients(size_t maxClients)
{
    // No permitir reducir el tamaño máximo por debajo del número actual de clientes
    if (maxClients >= _clients.size())
        _maxClients = maxClients;
}

void Channel::setDeleteMe(bool deleteMe)
{
    _deleteMe = deleteMe;
}
