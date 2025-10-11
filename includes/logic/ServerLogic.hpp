#ifndef SERVERLOGIC_HPP
#define SERVERLOGIC_HPP

#include <map>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"
#include "../Command.hpp"

class ServerLogic
{
    private:

        std::map<int, Client *>             _clients;
        std::map<std::string, Channel *>    _channels;

        // helpers
        void                                _handleNick(const Command& cmd, Client* client);
        void                                _handleJoin(const Command& cmd, Client* client);
        void                                _handlePrivmsg(const Command& cmd, Client* client);

    public:

                                            ServerLogic();
                                                
                                            ~ServerLogic();

        Client*                             getClient(int fd);
        Channel*                            getOrCreateChannel(const std::string& name);

        void                                executeCommand(const Command& cmd, int clientFd);

        void                                addClient(int fd);
        void                                removeClient(int fd);
};

#endif // SERVERLOGIC_HPP

/*🧠 ServerLogic.hpp

Aquí está el centro de la lógica, donde manejarás los comandos.
Este fichero conecta todo lo tuyo con el Server y el Command.*/