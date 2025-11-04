#include "../../../includes/logic/Client.hpp"
#include "../../../includes/logic/Channel.hpp"

/*------------------------------METHODS---------------------------*/

void    Client::joinChannel(Channel* channel)
{
    _channels.insert(channel->getName());
}

void    Client::leaveChannel(Channel* channel)
{
    _channels.erase(channel->getName());
}

/*void    Client::printInfo() const
{
    std::cout << BOLDWHITE << "---- Client Info ----" << RESET << std::endl;
    std::cout << "FD: " << _fd << std::endl;
    std::cout << "Nickname: " << _nickname << std::endl;
    std::cout << "Username: " << _username << std::endl;
    std::cout << "Registered: " << (_registered ? "true" : "false") << std::endl;
    std::cout << "Channels: ";
    for (std::set<std::string>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    std::cout << BOLDWHITE << "---------------------" << RESET << std::endl;
}*/
