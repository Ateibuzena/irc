#include "logic/Client.hpp"

/*------------------------------CONSTRUCTORS---------------------------*/
Client::Client(int fdValue)
    :   _fd(fdValue),
        _nickname(""),
        _username(""),
        _registered(false),
        _channels()
        //_realname(""),
        //_hostname(""),
        //_servername(""),
        //_password(""),
        //_is_operator(false),
        //_is_away(false),
        //_away_message(""),
        //_last_command_time(0)
{
    std::cout << "Client created with fd: " << _fd << std::endl;
}

/*------------------------------DESTRUCTORS---------------------------*/

Client::~Client()
{
    std::cout << "Client with fd " << _fd << " is being destroyed." << std::endl;
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

/*------------------------------SETTERS---------------------------*/

void    Client::setNickname(const std::string& nicknameValue)
{
    _nickname = nicknameValue;
}

void    Client::setUsername(const std::string& usernameValue)
{
    _username = usernameValue;
}

void    Client::setRegistered(bool stateValue)
{
    _registered = stateValue;
}

/*------------------------------METHODS---------------------------*/

bool Client::joinChannel(const std::string& channelName)
{
    if (channelName.empty())
    {
        std::cout << RED << "⚠️  Cannot join empty channel" << RESET << std::endl;
        return (false);
    }

    std::pair<std::set<std::string>::iterator,bool> result = _channels.insert(channelName);

    if (result.second)
        std::cout << GREEN << "✅ Joined channel: " << channelName << RESET << std::endl;
    else
        std::cout << YELLOW << "⚠️  Already in channel: " << channelName << RESET << std::endl;

    return (result.second);
}

bool Client::leaveChannel(const std::string& channelName)
{
    std::set<std::string>::iterator it = _channels.find(channelName);

    if (it != _channels.end())
    {
        _channels.erase(it);
        std::cout << GREEN << "✅ Left channel: " << channelName << RESET << std::endl;
        return (true);
    }

    std::cout << YELLOW << "⚠️  Not in channel: " << channelName << RESET << std::endl;
    return (false);
}
