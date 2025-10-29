#include "../../../includes/logic/Client.hpp"
#include "../../../includes/logic/Channel.hpp"

/*------------------------------METHODS---------------------------*/

void    Client::joinChannel(Channel* channel)
{
    _channels.insert(channel->getName());
}

void    Client::leaveChannel(Channel* channel)
{
    if (_channels.erase(channel->getName()) == 0)
        throw (ERR_NOTONCHANNEL);
}

/*void    Client::sendMessage(const std::string& message)
{
    _sentMessages.push_back(message);
    
    if (_fd < 0)
        return ;

    ssize_t totalSent = 0;
    ssize_t len = static_cast<ssize_t>(message.size());
    const char* buffer = message.c_str();

    while (totalSent < len)
    {
        ssize_t bytesSent = send(_fd, buffer + totalSent, len - totalSent, 0);
        if (bytesSent < 0)
        {
            if (errno == EINTR)
                continue; // La llamada fue interrumpida por señal, reintentamos
            
            // Error real al enviar: cerramos el cliente
            std::cerr << RED << "❌ Error sending message to fd "
                      << to_string_c98(_fd) << RESET << std::endl;
            _fd = -1;
            return ;
        }
        totalSent += bytesSent;
    }
    return ;
}*/

/*void    Client::receiveMessage(const std::string& message, const std::string& sender)
{
    _receivedMessages.push_back(message);
    std::cout << sender << std::endl;

   std::cout << CYAN << "📥 Received message " << RESET << message << std::endl;
}*/

void    Client::printInfo() const
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
}
