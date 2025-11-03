#ifndef SERVERLOGIC_HPP
#define SERVERLOGIC_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include "../utils/Command.hpp"
#include "../server/Server.hpp"

class Server;

class ServerLogic
{
    private:

        Server*                             _server;
        const std::string                   _serverName;
        const std::string                   _serverPassword;
        std::string                         _serverHost;
        const std::string                   _serverVersion;
        const std::time_t                   _serverStartTime;

        std::map<int, Client *>             _serverClients;
        std::map<std::string, Client*>      _serverNicknames;
        std::map<std::string, Channel *>    _serverChannels;

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
        void                                handleINVITE(Client* client, const Command& cmd);
        void                                handleKICK(Client* client, const Command& cmd);

        //Server Queries and Information
        void                                handleMODE(Client* client, const Command& cmd);

        //Sending Messages
        void                                handleNOTICE(Client* client, const Command& cmd);
        void                                handlePRIVMSG(Client* client, const Command& cmd);

    public:

                                            ServerLogic(Server* server,
                                                            const std::string& serverName,
                                                            const std::string& serverPassword);

                                            ~ServerLogic();

        Client*                             getClient(int fd) const;
        Channel*                            getChannel(const std::string& name) const;
        const std::string&                  getServerName() const;
        const std::string&                  getServerHost() const;
        const std::string&                  getServerVersion() const;
        const std::time_t                   getServerStartTime() const;

        void                                setHostname(const std::string& hostname);
        void                                setClientRegistered(Client* client);

        std::string                         buildMessage(const std::string& prefix,
                                                        const std::string& command,
                                                        const std::string& target,
                                                        const std::string& aux) const;

        std::string                         buildErrorMessage(const std::string& prefix,
                                                            const std::string& aux,
                                                            const std::string& msg) const;

        std::string                         buildReplyMessage(std::string code,
                                                            const Client* client,
                                                            const std::string& target = "",
                                                            const std::string& aux = "",
                                                            const std::string& msg = "") const;

        Channel*                            createChannel(const std::string& name, Client* creator);
        
        void                                serverAddClient(int fd);
        void                                serverRemoveClient(int fd);

        void                                executeCommand(const Command& cmd, int clientFd);

        void                                sendMessageToClient(Client* client, const std::string& message);
        void                                sendMessageToChannel(Channel* channel, const std::string& message, Client* sender = NULL);
};

#endif // SERVERLOGIC_HPP
