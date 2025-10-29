#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <cstddef>
#include "../parser/Parser.hpp"

class ServerLogic;

class Server
{
    private:
    
        int                             port_;
        int                             listenFd_;
        
        std::vector<pollfd>             pfds_;     // pfds[0] = listenFd
        std::map<int, std::string>      recvBuf_;  // acumulación por cliente
        std::map<int, std::string>      sendBuf_;  // cola de salida por cliente
        ServerLogic*                    logic_;

        // --- helpers internos ---
        static void                     setNonBlocking(int fd);
        static bool                     findLineEnd(const std::string& buf, size_t& pos);
        
        void                            addClient(int cfd);
        void                            removeClientAtIndex(size_t idx); // borra pfds[idx] y limpia mapas
        void                            handleReadable(size_t idx);      // procesa POLLIN para pfds[idx]
        void                            handleWritable(size_t idx);      // procesa POLLOUT para pfds[idx]


    public:
                                        Server(int port, const std::string& password);
                                        ~Server();

        int                             run();  // Bucle principal (bloqueante hasta que ocurra error o se termine desde fuera)

        void                            queueMessage(int fd, const std::string& line); // API para Persona 2: encola una línea (se añadirá "\r\n" automáticamente)
};

#endif // SERVER_HPP
