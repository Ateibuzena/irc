#include "logic/Client.hpp"
#include "logic/Channel.hpp"

/*------------------------------CONSTRUCTORS---------------------------*/
Client::Client(int fdValue)
    :   _fd(fdValue),
        _nickname(""),
        _username(""),
        _registered(false),
        _channels(),
        _sentMessages(),
        _receivedMessages()
        //_realname(""),
        //_hostname(""),
        //_servername(""),
        //_password(""),
        //_is_operator(false),
        //_is_away(false),
        //_away_message(""),
        //_last_command_time(0)
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

void    Client::setNickname(const std::string& nicknameValue)
{
    _nickname = nicknameValue;
    std::cout << GREEN << "✅ Nickname set to: " << _nickname << RESET << std::endl;
}

void    Client::setUsername(const std::string& usernameValue)
{
    _username = usernameValue;
    std::cout << GREEN << "✅ Username set to: " << _username << RESET << std::endl;
}

void    Client::setRegistered(bool stateValue)
{
    _registered = stateValue;
    std::cout << CYAN << "📜 Registered status: "
              << (_registered ? "true" : "false")
              << RESET << std::endl;
}

/*------------------------------METHODS---------------------------*/

void    Client::joinChannel(Channel* channelPtr)
{
    _channels.insert(channelPtr->getName());
}

void    Client::leaveChannel(Channel* channelPtr)
{
    _channels.erase(channelPtr->getName());
}

void    Client::sendMessage(const std::string& messageValue, const std::string& recipient)
{
    std::string formattedMessage = "To [" + recipient + "]: " + messageValue;

    _sentMessages.push_back(formattedMessage);

    std::cout << GREEN << "📤 Sending message " << RESET << formattedMessage << std::endl;
}

void    Client::receiveMessage(const std::string& messageValue, const std::string& sender)
{
    std::string formattedMessage = "From [" + sender + "]: " + messageValue;
    
    _receivedMessages.push_back(formattedMessage);
    
   std::cout << CYAN << "📥 Received message " << RESET << formattedMessage << std::endl;
}

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
