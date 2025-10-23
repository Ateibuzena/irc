#include "../../../includes/logic/ServerLogic.hpp"

// Manejar el comando PASS (establecer contraseña)
void    ServerLogic::handlePASS(Client* client, const Command& cmd)
{
    const std::string& password = cmd.params[0];

    if (client->isRegistered())
        throw (ERR_ALREADYREGISTERED);

    if (client->getUsername() || client->getNickname())
        throw (ERR_PASSWDAUTHORIZED);

    if (password != _serverPassword)
        throw (ERR_PASSWDMISMATCH);

    client->setPassword(password);

    // Si ya tenía username, nickname y la contraseña coincide o no había, lo marcamos como registrado
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        client->setRegistered(true);
}

// Manejar el comando NICK (establecer nickname)
void    ServerLogic::handleNICK(Client* client, const Command& cmd)
{
    const std::string nickname = cmd.params[0];

    // Comprobamos si ya existe otro cliente con ese nickname
    if (_serverNicknames.find(nickname) != _serverNicknames.end())
        throw (ERR_NICKNAMEINUSE);

    // Si tenía un nickname anterior, lo eliminamos del map
    if (!client->getNickname().empty())
        _serverNicknames.erase(client->getNickname());

    // Asignamos el nuevo nickname
    client->setNickname(nickname);

    // Añadimos al map de nicknames
    _serverNicknames[nickname] = client;

    // Si ya tenía username, nickname y la contraseña coincide o no había, lo marcamos como registrado
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        client->setRegistered(true);
}

// Manejar el comando USER (establecer username)
void    ServerLogic::handleUSER(Client* client, const Command& cmd)
{
    const std::string username = cmd.params[0];

    if (client->isRegistered())
        throw (ERR_ALREADYREGISTERED);

    client->setUsername(username);

    // Si ya tenía username, nickname y la contraseña coincide o no había, lo marcamos como registrado
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        client->setRegistered(true);
}

// Manejar el comando QUIT (desconectar cliente)
void    ServerLogic::handleQUIT(Client* client, const Command& cmd)
{
    // Construir mensaje de QUIT
    std::string msg = "Client disconnected";
    if (!cmd.params.empty())
    {
        msg.clear();
        msg = cmd.params[0];
    }
    std::string fullMsg = buildMessage(client->getNickname(), "QUIT", "", msg);

    // Limpiar canales a los que pertenece
    const std::set<std::string>& channelsNames = client->getChannels();
    std::set<std::string>::const_iterator it = channelsNames.begin();
    while (it != channelsNames.end())
    {
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(*it);
        if (chanIt != _serverChannels.end())
        {
            try
            {
                Channel* channel = chanIt->second;

                // Enviar mensaje de QUIT a los demás clientes del canal
                channel->broadcast(fullMsg, client);

                // Eliminar cliente del canal
                channel->removeClient(client);

                // Si el canal queda vacío, eliminarlo del servidor
                if (channel->getDeleteMe())
                {
                    delete (channel);
                    _serverChannels.erase(chanIt);
                }
            }
            catch(int error)
            {
                throw (error);
            }
        }
        ++it;
    }

    // Eliminar cliente del servidor
    serverRemoveClient(client->getFd());
}
