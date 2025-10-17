#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Command.hpp"

class Channel; // Declaración adelantada

class Client
{
    private:

        int                             _fd;
        std::string                     _nickname;
        std::string                     _username;
        bool                            _registered;
        std::set<std::string>           _channels;

        std::vector<std::string>        _sentMessages; // Buffer para mensajes enviados
        std::vector<std::string>        _receivedMessages; // Buffer para mensajes recibidos

    public:

                                        Client(int fdValue);

                                        ~Client();

        int                             getFd() const;
        const std::string&              getNickname() const;
        const std::string&              getUsername() const;
        bool                            isRegistered() const;
        const std::set<std::string>&    getChannels() const;

        const std::vector<std::string>& getSentMessages() const; // Para testing
        const std::vector<std::string>& getReceivedMessages() const; // Para testing


        bool                            setNickname(const std::string& nicknameValue);
        void                            setUsername(const std::string& usernameValue);
        void                            setRegistered(bool stateValue);

        void                            joinChannel(Channel* channelPtr);
        void                            leaveChannel(Channel* channelPtr);

        void                            receiveMessage(const std::string& messageValue, const std::string& sender);
        void                            sendMessage(const std::string& messageValue, const std::string& recipient);

        void                            disconnect(); // Limpia canales y marca como no registrado

        void                            printInfo() const; // Imprime info del cliente para debugging
}; 

#endif // CLIENT_HPP
