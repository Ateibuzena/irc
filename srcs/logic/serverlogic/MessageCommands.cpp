#include "../../../includes/logic/ServerLogic.hpp"

// Manejar el comando NOTICE (enviar mensaje de aviso)
void    ServerLogic::handleNOTICE(Client* client, const Command& cmd)
{
    // Verificamos que el cliente esté registrado
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);

    std::vector<std::string> receivers = str_to_vector(cmd.params[0], ',');

    // Verificamos si hay mensaje
    std::string msg = cmd.params[1];
    /*if (msg.empty())
        throw (ERR_NOTEXTTOSEND);*/ // puede ser vacío

    size_t i = 0;
    while (i < receivers.size())
    {
        // Obtenemos el destinatario actual
        const std::string& target = receivers[i];
        if (target.empty())
            throw (ERR_NORECIPIENT);

        // Buscamos si es un cliente
        std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(target);
        if (nickIt != _serverNicknames.end())
        {
            Client* recipient = nickIt->second;

            // Construimos el mensaje completo
            std::string noticeMsg = buildMessage(client, NULL, "NOTICE", recipient->getNickname() + " :" + msg);

            sendMessageToClient(recipient, noticeMsg);
            continue ;
        }
        
        // Buscamos si es un canal
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(target);
        if (chanIt != _serverChannels.end())
        {
            Channel* channel = chanIt->second;
            
            // Construimos el mensaje completo
            std::string noticeMsg = buildMessage(client, NULL, "NOTICE", channel->getName() + " :" + msg);

            sendMessageToChannel(channel, noticeMsg, client);
            continue ;
        }
        i++;
    }
}

// Manejar el comando PRIVMSG (enviar mensaje privado)
void    ServerLogic::handlePRIVMSG(Client* client, const Command& cmd)
{
    // Verificamos que el cliente esté registrado
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);

    std::vector<std::string> receivers = str_to_vector(cmd.params[0], ',');

    // Verificamos si hay mensaje
    if (cmd.params.size() < 2)
        throw (ERR_NOTEXTTOSEND);

    std::string msg = cmd.params[1];

    size_t i = 0;
    while (i < receivers.size())
    {
        // Obtenemos el destinatario actual
        const std::string& target = receivers[i];
        if (target.empty())
            throw (ERR_NORECIPIENT);

        // Buscamos si es un cliente
        std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(target);
        if (nickIt != _serverNicknames.end())
        {
            Client* recipient = nickIt->second;

            // Construimos el mensaje completo
            std::string privMsg = buildMessage(client, NULL, "NOTICE", recipient->getNickname() + " :" + msg);

            sendMessageToClient(recipient, privMsg);
            
            i++;
            continue ;
        }

        // Buscamos si es un canal
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(target);
        if (chanIt != _serverChannels.end())
        {
            Channel* channel = chanIt->second;
            
            // Construimos el mensaje completo
            std::string privMsg = buildMessage(client, NULL, "NOTICE", channel->getName() + " :" + msg);

            sendMessageToChannel(channel, privMsg, client);
            i++;
            continue ;
        }

        // Si no es ni cliente ni canal, lanzamos error
        if (target[0] == '#' || target[0] == '&')
            throw (ERR_CANNOTSENDTOCHAN);
        else
            throw (ERR_NOSUCHNICK);
    }
}
