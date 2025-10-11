#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

class Client; // forward declaration

class Channel
{
    private:

        std::string                 _name;
        std::set<Client *>          _clients;

    public:

                                    Channel(const std::string& name);
                                    
                                    ~Channel();

        const std::string&          getName() const;
        const std::set<Client *>&   getClients() const;
        
        void                        addClient(Client* client);
        void                        removeClient(Client* client);
        void                        broadcast(const std::string& message, int exceptFd = -1);

    
};

#endif // CHANNEL_HPP

/*🏠 Channel.hpp

Representa un canal con usuarios dentro.*/