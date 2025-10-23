#include "../../../includes/logic/ServerLogic.hpp"

// Manejar el comando NOTICE (enviar mensaje de aviso)
void    ServerLogic::handleNOTICE(Client* client, const Command& cmd)
{
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);

    std::vector<std::string> receivers = str_to_vector(cmd.params[0], ',');
    std::string msg = cmd.params[1];
    size_t i = 0;
    while (i < receivers.size())
    {
        const std::string& target = receivers[i];
        std::string fullMsg = buildMessage(client->getNickname(), "NOTICE", target, msg);

        // Buscamos si es un cliente por nickname
        std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(target);
        if (nickIt != _serverNicknames.end())
        {
            Client* recipient = nickIt->second;
            recipient->receiveMessage(fullMsg, client->getNickname());
            continue ;
        }
        
        // Buscamos si es un canal
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(target);
        if (chanIt != _serverChannels.end())
        {
            Channel* channel = chanIt->second;
            channel->broadcast(fullMsg, client);
            continue ;
        }
        i++;
    }
}

// Manejar el comando PRIVMSG (enviar mensaje privado)
void    ServerLogic::handlePRIVMSG(Client* client, const Command& cmd)
{
    if (!client->isRegistered())
        throw (ERR_NOTREGISTERED);

    std::vector<std::string> receivers = str_to_vector(cmd.params[0], ',');
    std::string msg = cmd.params[1];
    size_t i = 0;
    while (i < receivers.size())
    {
        const std::string& target = receivers[i];
        std::string fullMsg = buildMessage(client->getNickname(), "PRIVMSG", target, msg);

        // Buscamos si es un cliente por nickname
        std::map<std::string, Client*>::iterator nickIt = _serverNicknames.find(target);
        if (nickIt != _serverNicknames.end())
        {
            Client* recipient = nickIt->second;
            recipient->receiveMessage(fullMsg, client->getNickname());
            return ;
        }

        // Buscamos si es un canal
        std::map<std::string, Channel*>::iterator chanIt = _serverChannels.find(target);
        if (chanIt != _serverChannels.end())
        {
            Channel* channel = chanIt->second;
            channel->broadcast(fullMsg, client);
            return ;
        }

        // Si no es ni cliente ni canal, lanzamos error
        if (target[0] == '#' || target[0] == '&')
            throw (ERR_CANNOTSENDTOCHAN);
        else
            throw (ERR_NOSUCHNICK);
        i++;
    }
}
