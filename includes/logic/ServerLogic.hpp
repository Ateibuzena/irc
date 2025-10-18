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
        const std::string                   _serverPassword;

        //User Authentication
        void                                handlePASS(Client* client, const Command& cmd);
        void                                handleQUIT(Client* client, const Command& cmd);

        //User Registration
        void                                handleNICK(Client* client, const Command& cmd);
        void                                handleUSER(Client* client, const Command& cmd);

        //Channel Operations
        void                                handleJOIN(Client* client, const Command& cmd);
        void                                handlePART(Client* client, const Command& cmd);
        void                                handleTOPIC(Client* client, const Command& cmd);
        void                                handleNAMES(Client* client, const Command& cmd);
        void                                handleLIST(Client* client); //no necesita cmd
        void                                handleINVITE(Client* client, const Command& cmd);
        void                                handleKICK(Client* client, const Command& cmd);

        //Server Queries and Information
        void                                handleMOTD(Client* client); //no necesita cmd
        void                                handleVERSION(Client* client); //no necesita cmd
        void                                handleADMIN(Client* client); //no necesita cmd
        void                                handleTIME(Client* client); //no necesita cmd
        void                                handleINFO(Client* client); //no necesita cmd
        void                                handleMODE(Client* client, const Command& cmd);

        //Sending Messages
        void                                handleNOTICE(Client* client, const Command& cmd);
        void                                handlePRIVMSG(Client* client, const Command& cmd);

    public:

                                            ServerLogic(const std::string& serverPassword);
                                                
                                            ~ServerLogic();

        Client*                             getClient(int fd) const;
        Channel*                            getChannel(const std::string& name) const;

        Channel*                            createChannel(const std::string& name);
        
        void                                serverAddClient(int fd);
        void                                serverRemoveClient(int fd);

        void                                executeCommand(const Command& cmd, int clientFd);

};

#endif // SERVERLOGIC_HPP
