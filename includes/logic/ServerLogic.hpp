#ifndef SERVERLOGIC_HPP
#define SERVERLOGIC_HPP

# include "Client.hpp"
# include "Channel.hpp"
# include "../utils/Utils.hpp"
# include "../server/Server.hpp"

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

        std::string                         _prefix;
        std::string                         _errorMsg;
        std::string                         _replyMsg;

        //User Authentication
        void                                handlePASS(Client* client, const ParsedInput& input);
        void                                handleQUIT(Client* client, const ParsedInput& input);

        //User Registration
        void                                handleNICK(Client* client, const ParsedInput& input);
        void                                handleUSER(Client* client, const ParsedInput& input);

        //Channel Operations
        void                                handleJOIN(Client* client, const ParsedInput& input);
        void                                handlePART(Client* client, const ParsedInput& input);
        void                                handleTOPIC(Client* client, const ParsedInput& input);
        void                                handleINVITE(Client* client, const ParsedInput& input);
        void                                handleKICK(Client* client, const ParsedInput& input);

        //Server Queries and Information
        void                                handleMODE(Client* client, const ParsedInput& input);

        //Sending Messages
        void                                handleNOTICE(Client* client, const ParsedInput& input);
        void                                handlePRIVMSG(Client* client, const ParsedInput& input);

    public:

                                            ~ServerLogic();
                                            
                                            ServerLogic(Server* server,
                                                            const std::string& serverName,
                                                            const std::string& serverPassword);

        Client*                             getClient(int fd) const;
        Channel*                            getChannel(const std::string& name) const;
        const std::string&                  getServerName() const;
        const std::string&                  getServerHost() const;
        const std::string&                  getServerVersion() const;
        std::time_t                         getServerStartTime() const;

        void                                setHostname(const std::string& hostname);
        void                                setClientRegistered(Client* client);

        std::string                         buildReplyMessage(std::string code,
                                                            const Client* client,
                                                            const std::string& target = "",
                                                            const std::string& aux = "",
                                                            const std::string& msg = "") const;

        void                                serverAddClient(int fd);
        void                                serverRemoveClient(int fd);

        Channel*                            createChannel(const std::string& name, Client* creator);

        void                                executeCommand(const ParsedInput& input, int clientFd);

        void                                handleOperator(Channel* channel, Client* client);

        void                                sendMessageToClient(Client* client, const std::string& message);
        void                                sendMessageToChannel(Channel* channel, const std::string& message, Client* sender = NULL);
};

#endif
