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
    // Comprobar si el canal es invite-only
    if (_isInviteOnly && !isOperator(client) && !isInvited(client))
        throw (ERR_CHANOPRIVSNEEDED);
    
    // Suponiendo que hay una constante MAX_CHANNELS_PER_CLIENT definida en algún lugar
    if (client->getChannels().size() >= MAX_CHANNELS_PER_CLIENT)
        throw (ERR_TOOMANYCHANNELS);

    // Comprobar si el canal está lleno
    if (_clients.size() >= _maxClients)
        throw (ERR_CHANNELISFULL);

    // Agregar el cliente al canal
    _clients.insert(client);
}

void   Channel::removeClient(Client* client)
{
    // Eliminar el cliente del canal
    _clients.erase(client);

    // Eliminar al cliente de operadores
    _operators.erase(client);

    // Asignar nuevo operador si es necesario
    if (_operators.empty() && !_clients.empty())
    {
        Client* newOp = *(_clients.begin());
        _operators.insert(newOp);
    }

    // Eliminar al cliente de invitados
    _invited.erase(client);

    // Si el canal queda vacío, limpiar
    if (_clients.empty())
        _deleteMe = true;
}

bool    Channel::hasClient(Client* client) const
{
    if (_clients.find(client) != _clients.end())
        return (true);
    return (false);
}
