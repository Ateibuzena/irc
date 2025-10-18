#include "logic/Client.hpp"
#include "logic/Channel.hpp"

/*------------------------------CONSTRUCTORS---------------------------*/
Client::Client(int fd)
    :   _fd(fd),
        _nickname(""),
        _username(""),
        _registered(false),
        _channels(),
        _sentMessages(),
        _receivedMessages()
{
    std::cout << "Client with fd: " << _fd << " created." << std::endl;
}

/*------------------------------DESTRUCTORS---------------------------*/

Client::~Client()
{
    _nickname.clear();
    _username.clear();
    _registered = false;
    _channels.clear();
    _sentMessages.clear();
    _receivedMessages.clear();
    std::cout << "Client with fd " << _fd << " destroyed." << std::endl;
}

/*------------------------------GETTERS---------------------------*/

int Client::getFd() const
{
    return (_fd);
}

const   std::string& Client::getNickname() const
{
    return (_nickname);
}

const   std::string& Client::getUsername() const
{
    return (_username);
}

bool    Client::isRegistered() const
{
    return (_registered);
}

const   std::set<std::string>& Client::getChannels() const
{
    return (_channels);
}

const   std::vector<std::string>& Client::getSentMessages() const // Para testing
{
    return (_sentMessages);
}

const   std::vector<std::string>& Client::getReceivedMessages() const // Para testing
{
    return (_receivedMessages);
}

/*------------------------------SETTERS---------------------------*/

void    Client::setNickname(const std::string& nickname)
{
    _nickname = nickname;
    std::cout << GREEN << "✅ Nickname set to: " << _nickname << RESET << std::endl;
}

void    Client::setUsername(const std::string& username)
{
    _username = username;
    std::cout << GREEN << "✅ Username set to: " << _username << RESET << std::endl;
}

void    Client::setRegistered(bool state)
{
    _registered = state;
    std::cout << CYAN << "📜 Registered status: "
              << (_registered ? "true" : "false")
              << RESET << std::endl;
}
