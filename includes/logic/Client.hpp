#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../utils/Utils.hpp"

class Channel; // Forward declaration

class Client
{
    private:

        int                             _fd;
        std::string                     _nickname;
        std::string                     _oldNickname;
        std::string                     _username;
        std::string                     _password;
        bool                            _registered;
        bool                            _shouldDisconnect;

        std::set<std::string>           _channels;

    public:

                                        Client(int fd);

                                        ~Client();

        int                             getFd() const;
        const std::string&              getNickname() const;
        const std::string&              getOldNickname() const;
        const std::string&              getUsername() const;
        const std::string&              getPassword() const;
        bool                            isRegistered() const;
        const std::set<std::string>&    getChannels() const;

        void                            setFd(int fd);
        void                            setNickname(const std::string& nickname);
        void                            setOldNickname(const std::string& oldNickname);
        void                            setUsername(const std::string& username);
        void                            setPassword(const std::string& password);
        void                            setRegistered(bool state);
        
        void                            markForDisconnect();
        bool                            shouldDisconnect() const;

        void                            joinChannel(Channel* channel);
        void                            leaveChannel(Channel* channel);
        
}; 

#endif
