#include "../../../includes/logic/Client.hpp"
#include "../../../includes/logic/Channel.hpp"

/*------------------------------METHODS---------------------------*/

void    Client::markForDisconnect()
{
    _shouldDisconnect = true;
}

bool    Client::shouldDisconnect() const
{
    return _shouldDisconnect;
}

void    Client::joinChannel(Channel* channel)
{
    _channels.insert(channel->getName());
}

void    Client::leaveChannel(Channel* channel)
{
    _channels.erase(channel->getName());
}
