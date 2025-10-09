#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>

class Server {
public:
    Server(int port);
    ~Server();

    void run(); // loop principal

    std::string receiveMessage(int fd);
    void sendMessage(int fd, const std::string &message);

private:
    int _port;
    int _serverFd;
    std::map<int, std::string> _clientsBuffer;

    void _initSocket();
    void _acceptNewClient();
    void _removeClient(int fd);
};

#endif // SERVER_HPP

/*👉 Expone receiveMessage() y sendMessage() para la lógica.
La implementación se hace con poll() y accept() en el .cpp.*/