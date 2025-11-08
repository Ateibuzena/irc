#include "../../../includes/logic/ServerLogic.hpp"

// Manejar el comando PASS (establecer contraseña) (Replay listo, Msg listo, Error listo)
void    ServerLogic::handlePASS(Client* client, const ParsedInput& input)
{
    //Juan :irc.server.com 461 <nick> PASS :Not enough parameters

    const std::string& password = input.params[0];

    std::string prefix = ":" + _serverName + " ";
    std::string errorMsg; 
    std::string nickname = client->getNickname();
    if (nickname.empty())
        nickname = "*";

    if (client->isRegistered())
    {
        prefix += messagesError[ERR_ALREADYREGISTERED].code + " " + nickname;
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_ALREADYREGISTERED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    if (password != _serverPassword)
    {
        std::cout << "entro" << std::endl;
        prefix += messagesError[ERR_PASSWDMISMATCH].code + " " + nickname;
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_PASSWDMISMATCH].message);
        return (sendMessageToClient(client, errorMsg));
    }

    client->setPassword(password);

    // Si ya tenía username, nickname y la contraseña coincide o no había, lo marcamos como registrado
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        setClientRegistered(client);
}

// Manejar el comando NICK (establecer nickname) (Replay listo, Msg listo, Error listo)
void    ServerLogic::handleNICK(Client* client, const ParsedInput& input)
{
    //Juan:
    //:irc.server.com 431 * :No nickname given
    //:irc.server.com 432 * <nick> :Erroneous nickname

    //??
    //:irc.server.com 436 * <nick> :Nickname collision KILL
    //:irc.server.com 484 <nick> :Restricted

    const std::string nickname = input.params[0];
    std::string oldNickname = client->getNickname();
    if (oldNickname.empty())
        oldNickname = "*";

    std::string prefix = ":" + _serverName + " ";
    std::string errorMsg;

    // Comprobamos si ya existe otro cliente con ese nickname
    if (_serverNicknames.find(nickname) != _serverNicknames.end())
    {
        prefix += messagesError[ERR_NICKNAMEINUSE].code + " " + nickname;
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_NICKNAMEINUSE].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Si tenía un nickname anterior, lo eliminamos del map
    if (!client->getOldNickname().empty())
        _serverNicknames.erase(oldNickname);

    client->setOldNickname(oldNickname);

    // Asignamos el nuevo nickname
    client->setNickname(nickname);

    // Enviar mensaje de cambio de nickname
    if (client->isRegistered())
    {
        // Construir mensaje de NICK para los canales
        std::string prefix = ":" + client->getOldNickname() + "!" + client->getUsername() + "@" + _serverHost;
        std::string nickMsg = buildMessage(prefix, "NICK", "", client->getNickname());

        // Enviar a todos los canales donde está el cliente
        std::set<std::string> channelsNames = client->getChannels();
        std::set<std::string>::iterator it = channelsNames.begin();
        while (it != channelsNames.end())
        {
            std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(*it);
            if (chanIt != _serverChannels.end())
            {
                Channel* channel = chanIt->second;
                sendMessageToChannel(channel, nickMsg, client);
            }
            ++it;
        }

        // También enviamos el mensaje al propio cliente
        sendMessageToClient(client, nickMsg);

    }

    // Añadimos al map de nicknames
    _serverNicknames[nickname] = client;

    // Si ya tenía username, nickname y la contraseña coincide o no había, lo marcamos como registrado
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        setClientRegistered(client);
}

// Manejar el comando USER (establecer username) (Replay listo, Msg listo, Error listo)
void    ServerLogic::handleUSER(Client* client, const ParsedInput& input)
{
    //Juan :irc.server.com 461 <nick> USER :Not enough parameters

    const std::string username = input.params[0];

    std::string prefix = ":" + _serverName + " ";
    std::string errorMsg;
    std::string nickname = client->getNickname();
    if (nickname.empty())
        nickname = "*";

    if (client->isRegistered())
    {
        prefix += messagesError[ERR_ALREADYREGISTERED].code + " " + nickname;
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_ALREADYREGISTERED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    client->setUsername(username);

    // Si ya tenía username, nickname y la contraseña coincide o no había, lo marcamos como registrado
    if (!client->getUsername().empty()
        && (_serverPassword.empty() || client->getPassword() == _serverPassword)
        && !client->getNickname().empty())
        setClientRegistered(client);
}

// Manejar el comando QUIT (desconectar cliente) (Replay listo, Msg listo, Error listo)
void    ServerLogic::handleQUIT(Client* client, const ParsedInput& input)
{
    // Construir mensaje de QUIT
    std::string msg = "Client disconnected";
    if (!input.params.empty())
    {
        msg.clear();
        msg = input.params[0];
    }
    std::string nickname = client->getNickname();
    if (nickname.empty())
        nickname = "*";
    std::string prefix = ":" + nickname + "!" + client->getUsername() + "@" + _serverHost;
    std::string quitMsg = buildMessage(prefix, "QUIT", "", msg);

    // Limpiar canales a los que pertenece
    const std::set<std::string>& channelsNames = client->getChannels();
    std::set<std::string>::const_iterator it = channelsNames.begin();
    while (it != channelsNames.end())
    {
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(*it);
        if (chanIt != _serverChannels.end())
        {
            Channel* channel = chanIt->second;

            // Enviar mensaje de QUIT a los demás clientes del canal
            sendMessageToChannel(channel, quitMsg, client);

            // Eliminar cliente del canal
            channel->removeClient(client);

            // Si el canal queda vacío, eliminarlo del servidor
            if (channel->getDeleteMe())
            {
                delete (channel);
                _serverChannels.erase(chanIt);
            }
        }
        ++it;
    }

    // Eliminar cliente del servidor
    serverRemoveClient(client->getFd());
}
