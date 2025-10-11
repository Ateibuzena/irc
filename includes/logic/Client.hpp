#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <ostream>

#include <string>
#include <set>

#include <algorithm>

#include "../../test/TestUtils.hpp"

class Client
{
    private:

        int                         _fd;
        std::string                 _nickname;
        std::string                 _username;
        bool                        _registered;
        std::set<std::string>       _channels;

    public:

                                        Client(int fdValue);

                                        ~Client();

        int                             getFd() const;
        const std::string&              getNickname() const;
        const std::string&              getUsername() const;
        bool                            isRegistered() const;
        const std::set<std::string>&    getChannels() const;


        void                            setNickname(const std::string& nicknameValue);
        void                            setUsername(const std::string& usernameValue);
        void                            setRegistered(bool stateValue);

        bool                            joinChannel(const std::string& channelName);
        bool                            leaveChannel(const std::string& channelName);
};

#endif // CLIENT_HPP

/*🧍 Client.hpp

Representa un usuario conectado.*/