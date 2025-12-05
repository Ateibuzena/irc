#include "../../../includes/logic/Channel.hpp"
#include "../../../includes/logic/Client.hpp"

/*-------------------------------------METHODS------------------------------------*/

void    Channel::inviteClient(Client* client)
{
    _invited.insert(client);
}

bool    Channel::isInvited(Client* client) const
{
    if (_invited.find(client) != _invited.end())
        return (true);
    return (false);
}

void    Channel::addOperator(Client* client)
{
    _operators.insert(client);
}

void    Channel::removeOperator(Client* client)
{
    _operators.erase(client);
}

bool    Channel::isOperator(Client* client) const
{
    if (_operators.find(client) != _operators.end())
        return (true);
    return (false);
}

void    Channel::addClient(Client* client)
{
    _clients.insert(client);
}

void   Channel::removeClient(Client* client)
{
    // Remove the client from the channel
    _clients.erase(client);

    // If the channel becomes empty, mark for deletion
    if (_clients.empty())
        _deleteMe = true;

    // Remove the client from operators
    _operators.erase(client);

    // Remove the client from invited list
    _invited.erase(client);
}

bool    Channel::hasClient(Client* client) const
{
    if (_clients.find(client) != _clients.end())
        return (true);
    return (false);
}
