#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>

class Client {
public:
    Client(int fd);
    ~Client();

    int getFd() const;
    const std::string &getNickname() const;
    const std::string &getUsername() const;
    bool isRegistered() const;

    void setNickname(const std::string &nick);
    void setUsername(const std::string &user);
    void setRegistered(bool state);

    void joinChannel(const std::string &channelName);
    void leaveChannel(const std::string &channelName);
    const std::set<std::string> &getChannels() const;

private:
    int _fd;
    std::string _nickname;
    std::string _username;
    bool _registered;
    std::set<std::string> _channels;
};

#endif // CLIENT_HPP

/*🧍 Client.hpp

Representa un usuario conectado.*/