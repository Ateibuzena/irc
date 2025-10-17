#ifndef SERVERLOGIC_HPP
#define SERVERLOGIC_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include "parser/Parser.hpp"

class ServerLogic
{
    private:

        std::map<int, Client *>             _clients;
        std::map<std::string, Client*>      _nicknames;
        std::map<std::string, Channel *>    _channels;

        // helpers
        //conection messages
        /*PASS message //obligatorio
        PING message
        PONG message
        QUIT message
        ERROR message*/ //solo lo envia server cuando hay un fallo grave
        void                                handleNICK(Client* client, const std::string& nickname);
        void                                handleUSER(Client* client, const std::string& username);

        //Channel Operations
        void                                handleJOIN(Client* client, const std::string& channelName);
        void                                handlePART(Client* client, const std::string& channelName);
        /*TOPIC message
        NAMES message //obligatorio
        LIST message
        INVITE message //obligatorio
        Invite list
        KICK message*/ //obligatorio

        //Server Queries and Commands
        /*MOTD message
        VERSION Message
        ADMIN message
        TIME message
        INFO message
        MODE message //obligatorio
        User mode
        Channel mode*/

        //Sending Messages
        /*NOTICE message*/
        void                                handlePRIVMSG(Client* client, const std::string& target, const std::string& msg);

    public:

                                            ServerLogic();
                                                
                                            ~ServerLogic();

        Client*                             getClient(int fd) const;
        Channel*                            getChannel(const std::string& name) const;

        Channel*                            createChannel(const std::string& name);
        
        void                                executeCommand(const Command& cmd, int clientFd);

        void                                addClientServer(int fd);
        void                                removeClient(int fd);
};

#endif // SERVERLOGIC_HPP
