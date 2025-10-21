#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <cstddef> 
//AQUI

// Forward-declare para la interfaz
class Server;

/**
 * Interfaz que implementará Persona 2 (Parser/Protocolo).
 * onLine se llama cada vez que la capa de red arma una línea completa (CRLF o LF).
 * El handler puede enviar respuestas usando server.queueSend(fd, texto).
 */
//En la implementación del handler de Persona 2 (p. ej., ParserAdapter.hpp). y se debe incluir el archivo en AQUI
class ILineHandler {
public:
    virtual ~ILineHandler() {}
    virtual void onLine(int fd, const std::string& line, Server& server) = 0;
};

/**
 * Persona 1: core de red (sockets + poll).
 * - Acepta conexiones, lee por líneas y notifica a ILineHandler.
 * - Gestiona cola de salida y escritura no bloqueante.
 * - No interpreta el protocolo IRC.
 */
class Server {
public:
    Server(int port, ILineHandler* handler);
    ~Server();
    
    int run();  // Bucle principal (bloqueante hasta que ocurra error o se termine desde fuera)

    void queueSend(int fd, const std::string& line); // API para Persona 2: encola una línea (se añadirá "\r\n" automáticamente)
    void closeClient(int fd);  // (Opcional) Cerrar un cliente desde la capa superior(ana)

private:
    // --- helpers internos ---
    static void setNonBlocking(int fd);
    static bool findLineEnd(const std::string& buf, size_t& pos);

    void addClient(int cfd);
    void removeClientAtIndex(size_t idx); // borra pfds[idx] y limpia mapas
    void handleReadable(size_t idx);      // procesa POLLIN para pfds[idx]
    void handleWritable(size_t idx);      // procesa POLLOUT para pfds[idx]

private:
    int                 port_;
    int                 listenFd_;
    ILineHandler*       handler_;  // 👈 no-owing; vive fuera (en main)

    std::vector<pollfd>              pfds_;     // pfds[0] = listenFd
    std::map<int, std::string>       recvBuf_;  // acumulación por cliente
    std::map<int, std::string>       sendBuf_;  // cola de salida por cliente
};

#endif // SERVER_HPP
