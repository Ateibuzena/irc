#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "../utils/Utils.hpp"

class Client; // Forward declaration

class Channel
{
    private:

        const std::string           _name;
        std::string                 _topic;
        std::string                 _setter;  // Who set the topic
        std::string                 _timeSet; // Time when the topic was set
        std::set<Client *>          _clients;
        
        bool                        _isInviteOnly;
        bool                        _isTopicProtected;
        std::string                 _password;
        size_t                      _maxClients;

        std::set<Client *>          _operators;
        std::set<Client *>          _invited;

        bool                        _deleteMe;

    public:

                                    Channel(const std::string& name, size_t maxClients = DEFAULT_MAX_USERS_PER_CHANNEL);
                                    
                                    ~Channel();

                                    // Getters
        const std::string&          getName() const;
        const std::string&          getSetter() const;
        const std::string&          getTimeSet() const;
        const std::string&          getTopic() const;
        const std::set<Client *>&   getClients() const;

        bool                        isInviteOnly() const;
        bool                        isTopicProtected() const;
        const std::string&          getPassword() const;
        size_t                      getMaxClients() const;

        const std::set<Client *>&   getOperators() const;
        const std::set<Client *>&   getInvited() const;

        bool                        getDeleteMe() const;

        const std::string           getModes() const;

                                    // Setters
        void                        setTopic(const std::string& topic);
        void                        setSetter(const std::string& setter);
        void                        setTimeSet(const std::string& timeSet);
        void                        setInviteOnly(bool inviteOnly);
        void                        setTopicProtected(bool topicProtected);
        void                        setPassword(const std::string& password);
        void                        setMaxClients(size_t maxClients);
        void                        setDeleteMe(bool deleteMe);

                                    // Management of invited clients
        void                        inviteClient(Client* client);
        bool                        isInvited(Client* client) const;

                                    // Management of operators and clients
        void                        addOperator(Client* client);
        void                        removeOperator(Client* client);
        bool                        isOperator(Client* client) const;

        void                        addClient(Client* client);
        void                        removeClient(Client* client);
        bool                        hasClient(Client* client) const;

        void                        broadcast(const std::string& message, Client* sender = NULL);

    
};

#endif