#include "../../../includes/logic/Client.hpp"
#include "../../../includes/logic/Channel.hpp"

/*------------------------------CONSTRUCTORS---------------------------*/
Client::Client(int fd)
    :   _fd(fd),
        _nickname(""),
        _oldNickname(""),
        _username(""),
        _password(""),
        _registered(false),
        _shouldDisconnect(false),
        _channels()
{
    std::cout << "[INFO] Client with fd " << _fd << " created." << std::endl;
}

/*------------------------------DESTRUCTORS---------------------------*/

Client::~Client()
{
    _nickname.clear();
    _oldNickname.clear();
    _username.clear();
    _password.clear();
    _registered = false;
    _channels.clear();
    std::cout << "[INFO] Client with fd " << _fd << " destroyed." << std::endl;
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

const   std::string& Client::getOldNickname() const
{
    return (_oldNickname);
}

const   std::string& Client::getUsername() const
{
    return (_username);
}

const   std::string& Client::getPassword() const
{
    return (_password);
}

bool    Client::isRegistered() const
{
    return (_registered);
}

const std::set<std::string>&    Client::getChannels() const
{
    return (_channels);
}

/*------------------------------SETTERS---------------------------*/

void    Client::setFd(int fd)
{
    _fd = fd;
}

void    Client::setNickname(const std::string& nickname)
{
    _nickname = nickname;
}

void    Client::setOldNickname(const std::string& oldNickname)
{
    _oldNickname = oldNickname;
}

void    Client::setUsername(const std::string& username)
{
    _username = username;
}

void    Client::setPassword(const std::string& password)
{
    _password = password;
}

void    Client::setRegistered(bool state)
{
    _registered = state;

}
