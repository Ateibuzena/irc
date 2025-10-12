#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <poll.h>
#include <string>
#include <stdexcept>
#include <unistd.h>  // close()

#include "parser/Parser.hpp" // tu parser de mensajes a Command
#include "logic/ServerLogic.hpp"

class Server
{
    private:
        int                                     _serverSocket; // socket principal del servidor
        std::vector<struct pollfd>              _fds; // lista de fds para poll()
        int                                     nfds; // número de fds activos
        ServerLogic                             _logic; // lógica del IRC
        Parser                                  _parser;
        
        void                                    acceptNewClient(); // acepta nueva conexión y la añade a _logic
        std::string                             recvMessage(int fd); // lee mensaje de un cliente (recv)

    public:
                                                Server(int port); // constructor: crea y bind del socket
                                                ~Server(); // destructor: cierra sockets y limpia

        void                                    loop(); // bucle principal del servidor
};

#endif // SERVER_HPP
