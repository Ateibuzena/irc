#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../utils/Command.hpp"

class Channel; // Declaración adelantada

class Client
{
    private:

        int                             _fd;
        std::string                     _nickname;
        std::string                     _username;
        std::string                     _password;
        bool                            _registered;

        std::set<std::string>           _channels;

        std::vector<std::string>        _sentMessages; // Buffer para mensajes enviados
        std::vector<std::string>        _receivedMessages; // Buffer para mensajes recibidos

    public:

                                        Client(int fd);

                                        ~Client();

        int                             getFd() const;
        const std::string&              getNickname() const;
        const std::string&              getUsername() const;
        const std::string&              getPassword() const;
        bool                            isRegistered() const;
        const std::set<std::string>&    getChannels() const;

        const std::vector<std::string>& getSentMessages() const; // Para testing
        const std::vector<std::string>& getReceivedMessages() const; // Para testing


        void                            setFd(int fd);
        void                            setNickname(const std::string& nickname);
        void                            setUsername(const std::string& username);
        void                            setPassword(const std::string& password);
        void                            setRegistered(bool state);

        void                            joinChannel(Channel* channel);
        void                            leaveChannel(Channel* channel);

        void                            receiveMessage(const std::string& message, const std::string& sender);
        void                            sendMessage(const std::string& message);

        void                            printInfo() const; // Imprime info del cliente para debugging
}; 

#endif // CLIENT_HPP
