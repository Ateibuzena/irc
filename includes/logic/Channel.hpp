#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Command.hpp"

class Client; // Declaración adelantada

class Channel
{
    private:

        const std::string           _name;
        std::string                 _topic;
        std::set<Client *>          _clients;
        const size_t                _maxClients;

    public:

                                    Channel(const std::string& name, size_t maxClients = 50);
                                    
                                    ~Channel();

        const std::string&          getName() const;
        const std::string&          getTopic() const;
        size_t                      getMaxClients() const;
        const std::set<Client *>&   getClients() const;

        void                        setTopic(const std::string& topic);

        void                        addClient(Client* client);
        void                        removeClient(Client* client);
        void                        broadcast(const std::string& message, Client* sender = NULL);

        bool                        hasClient(Client* client) const;
    
};

#endif // CHANNEL_HPP

/*🏠 Channel.hpp

Representa un canal con usuarios dentro.*/