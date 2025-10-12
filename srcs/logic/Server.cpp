#include "Server.hpp"

/*
void Server::acceptNewClient()
{
    int client_fd = accept(_serverSocket, NULL, NULL);

    if (client_fd < 0)
        throw std::runtime_error("Error al aceptar cliente");

    // 🔹 Añadimos a poll()
    struct pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    fds.push_back(pfd);
    nfds++;

    // 🔹 Notificamos a la lógica
    _logic.addClient(client_fd);

    std::cout << "🟢 Nuevo cliente conectado: fd = " << client_fd << std::endl;
}

void Server::loop()
{
    while (true)
    {
        poll(fds, nfds, -1);

        for (int i = 0; i < nfds; ++i)
        {
            if (fds[i].fd == _serverSocket && (fds[i].revents & POLLIN))
            {
                // 🔸 NUEVA CONEXIÓN
                acceptNewClient();
            }
            else if (fds[i].revents & POLLIN)
            {
                // 🔸 MENSAJE DE CLIENTE EXISTENTE
                std::string raw = recvMessage(fds[i].fd);
                if (raw.empty())
                {
                    // cliente se desconectó
                    _logic.removeClient(fds[i].fd);
                    close(fds[i].fd);
                    continue;
                }

                Command cmd = _parser.parse(raw);
                _logic.executeCommand(cmd, fds[i].fd);
                //_parser.dispatch(cmd, fds[i].fd);
            }
        }
    }
}
*/