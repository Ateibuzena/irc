#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <set>

#include "logic/Client.hpp" // necesitamos la definición completa de Client

class Channel
{
    private:

        std::string                 _name;
        std::set<Client *>          _clients;

    public:

                                    Channel(const std::string& nameValue);
                                    
                                    ~Channel();

        const std::string&          getName() const;
        const std::set<Client *>&   getClients() const;
        
        void                        addClient(Client* clientValue);
        void                        removeClient(Client* clientValue);
        void                        broadcast(const std::string& messageValue, int exceptFd = -1);

    
};

#endif // CHANNEL_HPP

/*🏠 Channel.hpp

Representa un canal con usuarios dentro.*/