#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <ostream>

#include <string>
#include <set>
#include <vector>

#include <algorithm>

#include "../../test/TestUtils.hpp"

enum ChannelStatus
{
    JOIN_SUCCESS,
    ALREADY_IN_CHANNEL,
    CHANNEL_EMPTY,
    
    LEAVE_SUCCESS,
    NOT_IN_CHANNEL
};

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


        void                            setNickname(const std::string& nicknameValue);
        void                            setUsername(const std::string& usernameValue);
        void                            setRegistered(bool stateValue);

        ChannelStatus                   joinChannel(const std::string& channelName);
        ChannelStatus                   leaveChannel(const std::string& channelName);

        void                            receiveMessage(const std::string& messageValue, const std::string& sender);
        void                            sendMessage(const std::string& messageValue, const std::string& recipient);

        void                            disconnect(); // Limpia canales y marca como no registrado

        void                            printInfo() const; // Imprime info del cliente para debugging
};

std::string                             trim(const std::string& str);
bool                                    isValidNickname(const std::string& nickname);
bool                                    isValidUsername(const std::string& username);   

#endif // CLIENT_HPP
