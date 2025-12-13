#include "../../includes/server/Server.hpp"
#include "../../includes/logic/ServerLogic.hpp"

#include <iostream>
#include <stdexcept>
#include <cstring>          // std::strerror
#include <cerrno>           // errno
#include <csignal>          // ::signal, SIGPIPE
#include <unistd.h>         // close, recv, send
#include <fcntl.h>          // fcntl
#include <sys/socket.h>
#include <netinet/in.h>     //both for sockets - sockaddr_in y AF_INET
#include <arpa/inet.h>      //convertir IP entre texto y binario

std::string MessagesError[] =
{
    "200 :Success",                      // ERR_SUCCESS
    "461 :Not enough parameters",        // ERR_NEEDMOREPARAMS
    "462 :You may not reregister",       // ERR_ALREADYREGISTERED
    "464 :Password incorrect",           // ERR_PASSWDMISMATCH
    "431 :No nickname given",            // ERR_NONICKNAMEGIVEN
    "432 :Erroneous nickname",           // ERR_ERRONEUSNICKNAME
    "433 :Nickname is already in use",   // ERR_NICKNAMEINUSE
    "409 :No origin specified",          // ERR_NOORIGIN
    "491 :No OPer host",                 // ERR_NOOPERHOST
    "381 :You're OPer",                  // RPL_YOUREOPER
    "Quit : Closing Link: SERVER_NAME",  // QUIT
    "403 :No such channel",              // ERR_NOSUCHCHANNEL
    "405 :Too many channels",            // ERR_TOOMANYCHANNELS
    "475 :Bad channel key",              // ERR_BADCHANNELKEY
    "474 :Banned from channel",          // ERR_BANNEDFROMCHAN
    "471 :Channel is full",              // ERR_CHANNELISFULL
    "473 :Invite only channel",          // ERR_INVITEONLYCHAN
    "476 :Bad channel mask",             // ERR_BADCHANMASK
    "332 :Topic",                        // RPL_TOPIC
    "333 :Topic who time",               // RPL_TOPICWHOTIME
    "353 :Name reply",                   // RPL_NAMREPLY
    "366 :End of names",                 // RPL_ENDOFNAMES
    "442 :Not on channel",               // ERR_NOTONCHANNEL
    "482 :You're not channel operator",  // ERR_CHANOPRIVSNEEDED
    "331 :No topic is set",              // RPL_NOTOPIC
    "321 :List start",                   // RPL_LISTSTART
    "322 :List",                         // RPL_LIST
    "323 :List end",                     // RPL_LISTEND
    "341 :Inviting",                     // RPL_INVITING
    "443 :User is already on channel",   // ERR_USERONCHANNEL
    "441 :User not in channel",          // ERR_USERNOTINCHANNEL
    "402 :No such server",               // ERR_NOSUCHSERVER
    "422 :No MOTD",                      // ERR_NOMOTD
    "375 :MOTD start",                   // RPL_MOTDSTART
    "372 :MOTD",                         // RPL_MOTD
    "376 :End of MOTD",                  // RPL_ENDOFMOTD
    "005 :ISUPPORT",                     // RPL_ISUPPORT
    "351 :VERSION",                      // RPL_VERSION
    "256 :Admin Me",                     // RPL_ADMINME
    "257 :Admin Loc1",                   // RPL_ADMINLOC1
    "258 :Admin Loc2",                   // RPL_ADMINLOC2
    "259 :Admin Email",                  // RPL_ADMINEMAIL
    "491 :No privileges",                // ERR_NOPRIVILEGES
    "502 :No privs",                     // ERR_NOPRIVS
    "391 :Time",                         // RPL_TIME
    "371 :Info",                         // RPL_INFO
    "374 :End of Info",                  // RPL_ENDOFINFO
    "401 :No such nick",                 // ERR_NOSUCHNICK
    "502 :Users don't match",            // ERR_USERSDONTMATCH
    "221 :User mode is",                 // RPL_UMODEIS
    "501 :Unknown MODE flag",            // ERR_UMODEUNKNOWNFLAG
    "324 :Channel mode is",              // RPL_CHANNELMODEIS
    "329 :Creation time",                // RPL_CREATIONTIME
    "404 :Cannot send to channel",       // ERR_CANNOTSENDTOCHAN
    "407 :Too many targets",             // ERR_TOOMANYTARGETS
    "411 :No recipient given (PRIVMSG)",// ERR_NORECIPIENT
    "412 :No text to send",              // ERR_NOTEXTTOSEND
    "413 :No toplevel domain specified",// ERR_NOTOPLEVEL
    "414 :Wildcard in toplevel domain",  // ERR_WILDTOPLEVEL
    "301 :Away",                         // RPL_AWAY
    "421 :Unknown Error",                // ERR_UNKNOWN
    "451 :You have not registered",      // ERR_NOTREGISTERED
    "465 :Password not authorized"       // ERR_PASSWDAUTHORIZED
};

namespace
{
    static void log_errno(const char* where)
    {
        std::cerr << where << ": " << std::strerror(errno) << std::endl;
    }
}

Server::Server(int port, const std::string& password)
    :   port_(port),
        listenFd_(-1),
        pfds_(),
        recvBuf_(),
        sendBuf_(),
        logic_(new ServerLogic(this, "irc.server.com", password))
{

}

Server::~Server()
{
    // Close all fds
    for (size_t i = 0; i < pfds_.size(); ++i)
    {
        if (pfds_[i].fd >= 0)
            ::close(pfds_[i].fd);
    }

    // Free logic
    delete (logic_);
    logic_ = NULL;
}

void Server::setNonBlocking(int fd)
{
    int flags = ::fcntl(fd, F_GETFL, 0);

    if (flags < 0)
        flags = 0;

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        throw std::runtime_error("fcntl(O_NONBLOCK) failed");
    }
}

bool    Server::findLineEnd(const std::string &buf, size_t &pos)
{
    // prioritize CRLF
    for (size_t i = 0; i + 1 < buf.size(); ++i)
        if (buf[i] == '\r' && buf[i + 1] == '\n')
        {
            pos = i;
            return (true);
        }

    // tolerate LF alone
    for (size_t i = 0; i < buf.size(); ++i)
        if (buf[i] == '\n')
        {
            pos = i;
            return (true);
        }
    return (false);
}

int Server::run()
{
    Parser::initCommands();

    // Ignore SIGPIPE to avoid crashes when writing to closed sockets
    signal(SIGPIPE, SIG_IGN);

    // 1) socket + REUSEADDR + bind + listen
    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd_ < 0)
    {
        log_errno("socket");
        return (1);
    }

    int yes = 1;
    if (setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        log_errno("setsockopt(SO_REUSEADDR)");
        close(listenFd_);
        return (1);
    }

    sockaddr_in addr; std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port_);
    addr.sin_addr.s_addr = INADDR_ANY; // uses any local IP address

    if (bind(listenFd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        log_errno("bind");
        close(listenFd_);
        return (1);
    }
    if (listen(listenFd_, 128) < 0)
    {
        log_errno("listen");
        close(listenFd_);
        return (1);
    }

    try
    {
        setNonBlocking(listenFd_);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        close(listenFd_);
        return (1);
    }

    // 2) pfds[0] = listenFd
    pfds_.clear();
    recvBuf_.clear();
    sendBuf_.clear();

    pollfd p;
    p.fd = listenFd_;
    p.events = POLLIN;
    p.revents = 0;
    pfds_.push_back(p);

    std::cout << "[server] Listening on port " << port_
              << " (non-blocking + poll, multi-client)\n";

    signal(SIGINT, &handle_kill);
    // 3) Principal loop
    while (true)
    {
        if (sig == 1)
        {  
            std::cout << "\n[server] Caught SIGINT, shutting down...\n";
            break ;
        }
        int rv = poll(&pfds_[0], pfds_.size(), 1000);

        if (rv < 0)
        {
            if (errno == EINTR)
                continue ;
            log_errno("poll");
            break ;
        }
        // A) new connections (listenFd in pfds_[0])
        if (pfds_[0].revents & POLLIN)
        {
            for (;;)
            {
                int cfd = accept(listenFd_, NULL, NULL);
                if (cfd < 0)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        break;
                    log_errno("accept");
                    break;
                }
                try
                {
                    setNonBlocking(cfd);
                }
                catch (...)
                {
                    close(cfd);
                    continue ;
                }
                addClient(cfd);
                std::cout << "[+] Client connected fd=" << cfd << "\n";
            }
        }
        // B) Existing clients
        size_t i = 1;
        while (i < pfds_.size())
        {
            int fd = pfds_[i].fd;
            short re = pfds_[i].revents;

            Client *client = logic_->getClient(fd);

            if (client)
            {
                if (client->shouldDisconnect())
                {
                    removeClientAtIndex(i);
                    continue; // do not increment i, the vector has been compacted
                }
            }

            // 1) errors / close
            if (re & (POLLERR | POLLHUP | POLLNVAL))
            {
                std::cout << "[-] Close/err fd=" << fd << "\n";
                removeClientAtIndex(i);
                continue; // do not increment i, the vector has been compacted
            }
            // 2) read
            if (re & POLLIN)
            {
                handleReadable(i);
                if (i >= pfds_.size())
                    break ; // for safety, if client was removed
                // if client was removed, continue without incrementing i
            }

            // 3) write
            if (i < pfds_.size() && (pfds_[i].revents & POLLOUT))
            {
                handleWritable(i);
            }

            ++i;
        }
    }
    return (0);
}

void Server::addClient(int cfd)
{
    pollfd c;
    c.fd = cfd;
    c.events = POLLIN;
    c.revents = 0;
    pfds_.push_back(c);
    recvBuf_[cfd] = "";
    sendBuf_[cfd] = "";
    try
    {
        logic_->serverAddClient(cfd); // inform logic
    }
    catch(const std::string& errorMsg)
    {
        // send error message to client
        queueMessage(cfd, errorMsg + "\r\n");

        // remove client immediately after sending the message
        removeClientAtIndex(pfds_.size() - 1);
        
    }
    
}

void Server::removeClientAtIndex(size_t idx)
{
    if (idx >= pfds_.size())
        return;

    int fd = pfds_[idx].fd;
    ::close(fd);
    recvBuf_.erase(fd);
    sendBuf_.erase(fd);
    
    logic_->serverRemoveClient(fd); // inform logic
    pfds_.erase(pfds_.begin() + idx);
}

void Server::handleReadable(size_t idx)
{
    int fd = pfds_[idx].fd;

    char buf[1024];
    for (;;)
    {
        ssize_t n = recv(fd, buf, sizeof(buf), 0);
        if (n > 0)
        {
            recvBuf_[fd].append(buf, n);

            // extract lines
            size_t pos;
            while (findLineEnd(recvBuf_[fd], pos))
            {
                std::string line = recvBuf_[fd].substr(0, pos);
                // remove CRLF or LF
                if (pos + 1 < recvBuf_[fd].size()
                    && recvBuf_[fd][pos] == '\r'
                    && recvBuf_[fd][pos+1] == '\n')
                    recvBuf_[fd].erase(0, pos + 2);
                else // only LF
                    recvBuf_[fd].erase(0, pos + 1);
                    
                ParsedInput input;
                try
                {
                    Client *client = logic_->getClient(fd);
                    if (!client)
                        return;
                    std::string nickname;
                    if (client->getNickname().empty())
                        nickname = "*";
                    else
                        nickname = client->getNickname();

                    input = Parser::parse(line, logic_->getServerName(), nickname);
                }
                catch(const std::string& errorMsg)
                {
                    queueMessage(fd, errorMsg);

                    continue ;
                }
                try
                {
                    logic_->executeCommand(input, fd);
                }
                catch(const std::string& errorMsg)
                {
                    queueMessage(fd, errorMsg + "\r\n");
                }
                
            }
        
        }
        else if (n == 0)
        {
            // closed by peer
            std::cout << "[-] Peer closed fd=" << fd << "\n";
            // find current idx (it may have changed if there were movements)
            // here we continue using the idx we received because we have not altered pfds_
            removeClientAtIndex(idx);
            return ; // no need to touch idx anymore
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break; // no more data
            log_errno("recv");
            removeClientAtIndex(idx);
            return ;
        }
    }
}

void Server::handleWritable(size_t idx)
{
    int fd = pfds_[idx].fd;
    Client* client = logic_->getClient(fd);

    // if client was removed in another thread (very rare), exit
    if (!client)
    {
        pfds_[idx].events &= ~POLLOUT;
        return ;
    }

    std::string& out = sendBuf_[fd];
    while (!out.empty())
    {
        ssize_t m = send(fd, out.c_str(), out.size(), 0);
        if (m > 0)
        {
            out.erase(0, (size_t)m);
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            log_errno("send");
            removeClientAtIndex(idx);
            return ;
        }
    }

    if (out.empty())
    {
        // deactivate POLLOUT
        pfds_[idx].events &= ~POLLOUT;
    }
}

void Server::queueMessage(int fd, const std::string& line)
{
    // append line to send buffer
    sendBuf_[fd].append(line);

    // activate POLLOUT
    for (size_t i = 1; i < pfds_.size(); ++i)
    {
        if (pfds_[i].fd == fd)
        {
            pfds_[i].events |= POLLOUT;
            break;
        }
    }
}

