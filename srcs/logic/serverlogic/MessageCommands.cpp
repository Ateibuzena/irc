#include "../../../includes/logic/ServerLogic.hpp"

// Manejar el comando NOTICE (enviar mensaje de aviso) (Replay listo, Msg listo, Error listo))
void    ServerLogic::handleNOTICE(Client* client, const ParsedInput& input)
{
    std::string prefix = ":" + _serverName + " ";
    std::string errorMsg; 

    // Verificamos que el cliente esté registrado
    if (!client->isRegistered())
    {
        prefix += messagesError[ERR_NOTREGISTERED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_NOTREGISTERED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Obtenemos la lista de destinatarios
    std::vector<std::string> receivers = str_to_vector(input.params[0], ',');

    // Obtenemos el mensaje
    std::string msg = input.params[1];

    size_t i = 0;
    while (i < receivers.size())
    {
        // Obtenemos el destinatario actual
        const std::string& target = receivers[i];
        if (target.empty())
        {
            i++;
            continue ;
        }

        // Buscamos si es un cliente
        std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(target);
        if (nickIt != _serverNicknames.end())
        {
            Client* recipient = nickIt->second;

            // Construimos el mensaje de NOTICE para notificar a otro cliente
            prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
            std::string noticeMsg = buildMessage(prefix,
                                                "NOTICE",
                                                recipient->getNickname(),
                                                msg);

            // Enviamos el mensaje al destinatario
            sendMessageToClient(recipient, noticeMsg);
        }
        
        // Buscamos si es un canal
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(target);
        if (chanIt != _serverChannels.end())
        {
            Channel* channel = chanIt->second;

            // Construimos el mensaje de NOTICE para notificar a un canal
            prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
            std::string noticeMsg = buildMessage(prefix, "NOTICE", channel->getName(), msg);

            // Enviamos el mensaje a todos los clientes del canal
            sendMessageToChannel(channel, noticeMsg, client);
        }
        i++;
    }
}

// Manejar el comando PRIVMSG (enviar mensaje privado) (Replay listo, Msg listo, Error listo)
void    ServerLogic::handlePRIVMSG(Client* client, const ParsedInput& input)
{
/*************************************************************************************** */
    //added by noe debug
    /*std::cout << "[DEBUG] PRIVMSG target = '" << input.params[0] << "'\n";
    std::cout << "[DEBUG] known nicknames: ";
    for (std::map<std::string, Client *>::const_iterator it = _serverNicknames.begin();
         it != _serverNicknames.end(); ++it)
    {
        std::cout << "'" << it->first << "' ";
    }
    std::cout << std::endl;*/
/*********************************************************************** ******************/
    std::string prefix = ":" + _serverName + " ";
    std::string errorMsg;

    // Verificamos que el cliente esté registrado
    if (!client->isRegistered())
    {
        prefix += messagesError[ERR_NOTREGISTERED].code + " " + client->getNickname();
        errorMsg = buildErrorMessage(prefix,
                                    "",
                                    messagesError[ERR_NOTREGISTERED].message);
        return (sendMessageToClient(client, errorMsg));
    }

    // Obtenemos la lista de destinatarios
    std::vector<std::string> receivers = str_to_vector(input.params[0], ',');

    // Obtenemos el mensaje
    std::string msg = input.params[1];

    size_t i = 0;
    while (i < receivers.size())
    {
        // Obtenemos el destinatario actual
        const std::string& target = receivers[i];
        if (target.empty())
        {
            i++;
            continue ;
        }

        // Buscamos si es un cliente
        std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(target);
        if (nickIt != _serverNicknames.end())
        {
            Client* recipient = nickIt->second;

            // Construimos el mensaje de PRIVMSG para notificar a otro cliente
            std::string prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
            std::string privMsg = buildMessage(prefix, "PRIVMSG", recipient->getNickname(), msg);

            // Enviamos el mensaje al destinatario
            sendMessageToClient(recipient, privMsg);
            
            i++;
            continue ;
        }

        // Buscamos si es un canal
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(target);
        if (chanIt != _serverChannels.end())
        {
            Channel* channel = chanIt->second;
            if (!channel->hasClient(client))
            {
                i++;
                continue ;
            }

            // Construimos el mensaje de PRIVMSG para notificar a un canal
            std::string prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + _serverHost;
            std::string privMsg = buildMessage(prefix, "PRIVMSG", channel->getName(), msg);

            sendMessageToChannel(channel, privMsg, client);
            
            i++;
            continue ;
        }

        // Si no es ni cliente ni canal, lanzamos error
        if (target[0] == '#' || target[0] == '&')
        {
            prefix += messagesError[ERR_CANNOTSENDTOCHAN].code + " " + client->getNickname() + " " + target;
            errorMsg = buildErrorMessage(prefix,
                                        "",
                                        messagesError[ERR_CANNOTSENDTOCHAN].message);
        }
        else
        {
            prefix += messagesError[ERR_NOSUCHNICKCHANNEL].code + " " + client->getNickname() + " " + target;
            errorMsg = buildErrorMessage(prefix,
                                        "",
                                        messagesError[ERR_NOSUCHNICKCHANNEL].message);
        }
        sendMessageToClient(client, errorMsg);
        i++;
    }
}
