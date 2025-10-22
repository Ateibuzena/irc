#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../utils/Command.hpp"

class Client; // Declaración adelantada

class Channel
{
    private:

        const std::string           _name;
        std::string                 _topic;
        std::set<Client *>          _clients;
        
        bool                        _isInviteOnly;
        bool                        _isTopicProtected;
        std::string                 _password;
        size_t                      _maxClients;

        std::set<Client *>          _operators;
        std::set<Client *>          _invited;

    public:

                                    Channel(const std::string& name, size_t maxClients = 50);
                                    
                                    ~Channel();

                                    // Getters
        const std::string&          getName() const;
        const std::string&          getTopic() const;
        const std::set<Client *>&   getClients() const;

        bool                        isInviteOnly() const;
        bool                        isTopicProtected() const;
        const std::string&          getPassword() const;
        size_t                      getMaxClients() const;

        const std::set<Client *>&   getOperators() const;
        const std::set<Client *>&   getInvited() const;

                                    // Setters
        void                        setTopic(const std::string& topic);
        void                        setInviteOnly(bool inviteOnly);
        void                        setTopicProtected(bool topicProtected);
        void                        setPassword(const std::string& password);
        void                        setMaxClients(size_t maxClients);

                                    // Gestión de invitados
        void                        inviteClient(Client* client);
        bool                        isInvited(Client* client) const;

                                    // Gestión de operadores
        void                        addOperator(Client* client);
        void                        removeOperator(Client* client);
        bool                        isOperator(Client* client) const;

        void                        addClient(Client* client);
        void                        removeClient(Client* client);
        bool                        hasClient(Client* client) const;

        void                        broadcast(const std::string& message, Client* sender = NULL);

    
};

#endif // CHANNEL_HPP

/*🏠 Channel.hpp

Representa un canal con usuarios dentro.*/