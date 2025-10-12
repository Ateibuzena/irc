#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <set>

#include "Command.hpp"

#include "../../test/TestUtils.hpp"

class Client; // Declaración adelantada

class Channel
{
    private:

        std::string                 _name;
        std::set<Client *>          _clients;
        size_t                      _maxClients;

    public:

                                    Channel(const std::string& nameValue, size_t maxClients = 50);
                                    
                                    ~Channel();

        const std::string&          getName() const;
        const std::set<Client *>&   getClients() const;

        std::vector<std::string>    getNicknames() const;
        
        ChannelStatus               addClient(Client* clientValue);
        ChannelStatus               removeClient(Client* clientValue);
        void                        broadcast(const std::string& messageValue, Client* sender = NULL);

        bool                        hasClient(Client* client) const;
    
};

bool                                isValidName(const std::string& name);

#endif // CHANNEL_HPP

/*🏠 Channel.hpp

Representa un canal con usuarios dentro.*/