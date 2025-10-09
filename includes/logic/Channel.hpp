#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

class Client; // forward declaration

class Channel {
public:
    Channel(const std::string &name);
    ~Channel();

    const std::string &getName() const;
    void addClient(Client *client);
    void removeClient(Client *client);
    const std::set<Client *> &getClients() const;

    void broadcast(const std::string &message, int exceptFd = -1);

private:
    std::string _name;
    std::set<Client *> _clients;
};

#endif // CHANNEL_HPP

/*🏠 Channel.hpp

Representa un canal con usuarios dentro.*/