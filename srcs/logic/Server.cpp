#include "Server.hpp"

Server::Server(int port)
    :   _serverSocket(socket(AF_INET, SOCK_STREAM, 0)),
        _fds(),
        _logic(),
        _port(port)
{
    std::cout << "Inicializando servidor en el puerto: " << port << std::endl;
    // Configuración del socket
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(_port);

    /*std::cout << "Configurando socket en el puerto: " << _port << std::endl;
    if (getaddrinfo(NULL, to_string_c98(_port).c_str(), NULL, NULL) != 0)
        throw std::runtime_error("Error getaddrinfo");*/

    std::cout << "Socket configurado correctamente." << std::endl;
    int i = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int)) < 0)
        throw std::runtime_error("Error setsockopt");

    std::cout << "Poniendo socket en modo no bloqueante." << std::endl;
    if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Error fcntl");

    std::cout << "Haciendo bind del socket." << std::endl;
    if (bind(_serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        throw std::runtime_error("Error bind socket");

    std::cout << "Poniendo el socket a escuchar." << std::endl;
    if (listen(_serverSocket, SOMAXCONN) < 0)
        throw std::runtime_error("Error listen");

    struct pollfd pfd;
    pfd.fd = _serverSocket;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _fds.push_back(pfd);
    //_nfds++;
}

Server::~Server()
{
    close(_serverSocket);
}

std::string Server::recvMessage(int fd)
{
    char        buffer[BUFFER_SIZE];

    ssize_t     bytes_received = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received < 0)
    {
        std::cerr << "Error al recibir datos del cliente fd=" << fd << std::endl;
        return (std::string());
    }
    else if (bytes_received == 0)
    {
        std::cout << "Cliente desconectado: fd=" << fd << std::endl;
        return (std::string());
    }

    buffer[bytes_received] = '\0'; // Null-terminate the string
    return (std::string(buffer));
}

void Server::acceptNewClient()
{
    int client_fd = accept(_serverSocket, NULL, NULL);

    if (client_fd < 0)
        throw std::runtime_error("Error al aceptar cliente");

    // 🔹 Añadimos a poll()
    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    _fds.push_back(pfd);
    //nfds++;

    // 🔹 Notificamos a la lógica
    _logic.addClientServer(client_fd);

    std::cout << "🟢 Nuevo cliente conectado: fd = " << client_fd << std::endl;
}

void Server::loop()
{
    std::cout << "Servidor escuchando en el puerto: "<< _port << std::endl;
    while (true)
    {
        std::cout << "Esperando eventos..." << std::endl;
        int event_status = poll(_fds.data(), _fds.size(), -1);

        if (event_status < 0)
            throw std::runtime_error("Error poll"); //limpiar
        else if (event_status == 0)
            continue; // timeout, seguir esperando
        for (unsigned long i = 0; i < _fds.size(); ++i)
        {
            std::cout << "Revisando fd: " << _fds[i].fd << std::endl;
            if (_fds[i].fd == _serverSocket && (_fds[i].revents & POLLIN))
            {
                std::cout << "Nueva conexión entrante..." << std::endl;
                // 🔸 NUEVA CONEXIÓN
                acceptNewClient();
            }
            else if (_fds[i].revents & POLLIN)
            {
                std::cout << "Datos entrantes en fd: " << _fds[i].fd << std::endl;
                // 🔸 MENSAJE DE CLIENTE EXISTENTE
                std::string raw = recvMessage(_fds[i].fd);
                if (raw.empty())
                {
                    std::cout << "Cliente desconectado, limpiando fd: " << _fds[i].fd << std::endl;
                    _logic.removeClient(_fds[i].fd);
                    close(_fds[i].fd);
                    continue ;
                }
                std::cout << "Mensaje recibido: " << raw << std::endl;
                Command cmd;

                // Trim de espacios, \r, \n al final (y si quieres también al principio)
                raw.erase(raw.find_last_not_of(" \r\n") + 1);
                raw.erase(0, raw.find_first_not_of(" \r\n"));
                //"USER azubieta 0 * realname\n" //hostname ignored y servername ignored
                cmd.raw = raw;
                cmd.name = raw.substr(0, raw.find(' '));
                cmd.params.push_back(raw.substr(raw.find(' ') + 1));
                // Aquí parsear más si quieres (split por espacios, etc.)
                for (size_t pos = 0; pos < cmd.params.size(); pos++)
                {
                    size_t space_pos = cmd.params[pos].find(' ');
                    if (space_pos != std::string::npos)
                    {
                        std::string token = cmd.params[pos].substr(0, space_pos);
                        cmd.params[pos] = token;
                        if (space_pos + 1 < cmd.params[pos].size())
                            cmd.params.push_back(cmd.params[pos].substr(space_pos + 1));
                        else
                            break;
                    }
                    else
                        break;
                }
                std::cout << "Comando recibido: " << cmd.name << std::endl;
                _logic.executeCommand(cmd, _fds[i].fd);
            }
        }
    }
}
