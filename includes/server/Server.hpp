#ifndef SERVER_HPP
#define SERVER_HPP

# include <string>
# include <vector>
# include <map>
# include <poll.h>
# include <cstddef>
# include "../parser/Parser.hpp"

class ServerLogic;

class Server
{
    private:
    
        int                             port_;
        int                             listenFd_;
        
        std::vector<pollfd>             pfds_;     // pfds[0] = listenFd
        std::map<int, std::string>      recvBuf_;  // acumulate data per client
        std::map<int, std::string>      sendBuf_;  // outgoing data per client
        ServerLogic*                    logic_;

        // --- internal helpers ---
        static void                     setNonBlocking(int fd);
        static bool                     findLineEnd(const std::string& buf, size_t& pos);
        
        void                            addClient(int cfd);
        void                            removeClientAtIndex(size_t idx); // removes pfds[idx] and clears maps
        void                            handleReadable(size_t idx);      // processes POLLIN for pfds[idx]
        void                            handleWritable(size_t idx);      // processes POLLOUT for pfds[idx]
    

    public:
                                        Server(int port, const std::string& password);
                                        ~Server();

        int                             run();  // Main loop (blocking until error or external shutdown)

        void                            queueMessage(int fd, const std::string& line); // API for Parser: queue a line (CRLF will be added automatically)
   
};

#endif
