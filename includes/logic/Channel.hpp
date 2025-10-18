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
        std::set<Client *>          _operators;
        std::set<Client *>          _invited;
        const size_t                _maxClients;

    public:

                                    Channel(const std::string& name, size_t maxClients = 50);
                                    
                                    ~Channel();

                                    // Getters
        const std::string&          getName() const;
        const std::string&          getTopic() const;
        size_t                      getMaxClients() const;
        const std::set<Client *>&   getClients() const;
        const std::set<Client *>&   getOperators() const;
        const std::set<Client *>&   getInvited() const;

                                    // Setters
        void                        setTopic(const std::string& topic);

                                    // Gestión de invitados
        void                        inviteClient(Client* client);

                                    // Gestión de operadores
        bool                        isOperator(Client* client) const;
        void                        addOperator(Client* client);
        void                        removeOperator(Client* client);

        void                        addClient(Client* client);
        void                        removeClient(Client* client);

        void                        broadcast(const std::string& message, Client* sender = NULL);

        bool                        hasClient(Client* client) const;
    
};

#endif // CHANNEL_HPP

/*🏠 Channel.hpp

Representa un canal con usuarios dentro.*/