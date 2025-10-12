#ifndef SERVERLOGIC_HPP
#define SERVERLOGIC_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "Client.hpp"
#include "Channel.hpp"
#include "../Command.hpp"

class ServerLogic
{
    private:

        std::map<int, Client *>             _clients;    // _fd -> Client
        std::map<std::string, Client*>      _nicknames;    // _nickname -> Client
        std::map<std::string, Channel *>    _channels; // _name -> Channel
        size_t                              _defaultChannelLimit;     // límite personas por defecto para nuevos canales

        // helpers
        void                                handleNICK(Client* client, const std::string& nickname);
        void                                handleUSER(Client* client, const std::string& username);
        void                                handleJOIN(Client* client, const std::string& channelName);
        void                                handlePART(Client* client, const std::string& channelName);
        void                                handlePRIVMSG(Client* client, const std::string& target, const std::string& msg);

    public:

                                            ServerLogic(size_t defaultChannelLimitValue = 10);
                                                
                                            ~ServerLogic();

        Client*                             getClient(int fd) const;
        Channel*                            getChannel(const std::string& name) const;
        size_t                              getDefaultChannelLimit() const;

        Channel*                            createChannel(const std::string& name);
        
        void                                executeCommand(const Command& cmd, int clientFd);

        void                                addClient(int fd);
        void                                removeClient(int fd);
};

#endif // SERVERLOGIC_HPP

/*🧠 ServerLogic.hpp

- executeCommand(const Command&, int) es el punto central:

- Aquí decides qué helper llamar según cmd.getName().

- _clients y _nicknames te permiten validar duplicados de nicknames y buscar clientes rápido.

- _channels crea o retorna canales existentes (getOrCreateChannel).

- Los helpers (handleNICK, etc.) encapsulan la lógica de cada comando.
Así no mezclas parsing con la lógica real de IRC.*/