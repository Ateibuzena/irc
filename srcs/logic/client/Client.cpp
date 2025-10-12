#include "logic/Client.hpp"
#include "logic/Channel.hpp"

/*
1️⃣ Validaciones básicas

Verificar que nickname y username cumplan reglas de IRC (longitud, caracteres permitidos).

Comprobar que al cambiar nickname no se repita con otro cliente (esto será más relevante cuando tengas un gestor de clientes).

Si quisiera endurecer la validación, podría requerir longitud mínima >1 o prohibir solo letras. Pero en términos de flujo, 100% correcto.
*/

/*------------------------------CONSTRUCTORS---------------------------*/
Client::Client(int fdValue)
    :   _fd(fdValue),
        _nickname(""),
        _username(""),
        _registered(false),
        _channels()
        //_realname(""),
        //_hostname(""),
        //_servername(""),
        //_password(""),
        //_is_operator(false),
        //_is_away(false),
        //_away_message(""),
        //_last_command_time(0)
{
    std::cout << "Client created with fd: " << _fd << std::endl;
}

/*------------------------------DESTRUCTORS---------------------------*/

Client::~Client()
{
    std::cout << "Client with fd " << _fd << " is being destroyed." << std::endl;
}

/*------------------------------GETTERS---------------------------*/

int Client::getFd() const
{
    return (_fd);
}

const   std::string& Client::getNickname() const
{
    return (_nickname);
}

const   std::string& Client::getUsername() const
{
    return (_username);
}

bool    Client::isRegistered() const
{
    return (_registered);
}

const   std::set<std::string>& Client::getChannels() const
{
    return (_channels);
}

const   std::vector<std::string>& Client::getSentMessages() const // Para testing
{
    return (_sentMessages);
}

const   std::vector<std::string>& Client::getReceivedMessages() const // Para testing
{
    return (_receivedMessages);
}

/*------------------------------SETTERS---------------------------*/

/*💡 Explicación rápida:

- Evita nombres vacíos, demasiado largos o con caracteres no permitidos

- Evita nicknames que empiecen con número o guion

- Mensajes de debug claros

- Preparado para integrarse con Server, donde se podrá chequear duplicados
*/

void    Client::setNickname(const std::string& nicknameValue)
{
    if (!isValidNickname(nicknameValue)) {
        std::cout << RED << "⚠️  Invalid nickname: " << nicknameValue << RESET << std::endl;
        return;
    }
    _nickname = nicknameValue;
    std::cout << GREEN << "✅ Nickname set to: " << _nickname << RESET << std::endl;
}

void    Client::setUsername(const std::string& usernameValue)
{
    if (!isValidUsername(usernameValue)) {
        std::cout << RED << "⚠️  Invalid username: " << usernameValue << RESET << std::endl;
        return;
    }
    _username = usernameValue;
    std::cout << GREEN << "✅ Username set to: " << _username << RESET << std::endl;
}

void    Client::setRegistered(bool stateValue)
{
    _registered = stateValue;
    std::cout << CYAN << "📜 Registered status: "
              << (_registered ? "true" : "false")
              << RESET << std::endl;
}

/*------------------------------METHODS---------------------------*/

/*✅ Explicación rápida:

- El Client interactúa con el objeto Channel real.

- _channels solo se actualiza si el canal acepta al cliente (JOIN_SUCCESS).

- El canal aplica el límite de clientes y evita duplicados automáticamente.

- Validación de nombre de canal integrada (isValidName).
*/

ChannelStatus   Client::joinChannel(Channel* channelPtr)
{
    if (!channelPtr)
        return (CHANNEL_EMPTY);

    if (!isValidName(channelPtr->getName()))
        return (CHANNEL_EMPTY); // o NOT_A_VALID_CHANNEL si quieres otro enum

    ChannelStatus status = channelPtr->addClient(this);

    if (status == JOIN_SUCCESS)
        _channels.insert(channelPtr->getName());

    return (status);
}

ChannelStatus Client::leaveChannel(Channel* channelPtr)
{
    if (!channelPtr)
        return (NOT_IN_CHANNEL);

    ChannelStatus status = channelPtr->removeClient(this);

    if (status == LEAVE_SUCCESS)
        _channels.erase(channelPtr->getName());

    return (status);
}

/*💡 Explicación rápida:

- Cada vez que se envía o recibe un mensaje, se guarda en el vector correspondiente.

- Se imprime con color (opcional, para que siga siendo útil en debug).

- Los métodos ahora reciben remitente o destinatario — aunque sean simples std::string, en el futuro podrían ser punteros a otros objetos Client.

Podrías, si quieres, hacer que los mensajes vacíos no se agreguen al buffer (depende de tu diseño), pero no es un error.

*/

void    Client::sendMessage(const std::string& messageValue, const std::string& recipient)
{
    std::string formattedMessage = "To [" + recipient + "]: " + messageValue;

    _sentMessages.push_back(formattedMessage); // Almacena el mensaje en el buffer de enviados

    std::cout << GREEN << "📤 Sending message " << RESET << formattedMessage << std::endl;
}

void    Client::receiveMessage(const std::string& messageValue, const std::string& sender)
{
    std::string formattedMessage = "From [" + sender + "]: " + messageValue;
    
    _receivedMessages.push_back(formattedMessage); // Almacena el mensaje en el buffer de recibidos
    
   std::cout << CYAN << "📥 Received message " << RESET << formattedMessage << std::endl;
}

void Client::disconnect()
{
    if (!_registered && _channels.empty())
    {
        std::cout << YELLOW << "⚠️  Client with fd " << _fd << " is already disconnected." << RESET << std::endl;
        return ;
    }
    // Enviar mensaje final de desconexión (solo si estaba registrado)
    if (_registered)
    {
        std::string disconnectMsg = "Client " + _nickname + " has disconnected.";
        _sentMessages.push_back(disconnectMsg);

        std::cout << RED << "🔌 " << disconnectMsg << RESET << std::endl;
    }

    // Limpiar todos los datos del cliente
    _channels.clear();
    _nickname.clear();
    _username.clear();
    _registered = false;

    std::cout << YELLOW << "🧹 Client data reset for fd: " << _fd << RESET << std::endl;
}

void Client::printInfo() const
{
    std::cout << std::endl
              << BOLDWHITE << "╭───────────────────────────────────────────────╮" << RESET << std::endl;
    std::cout << BOLDWHITE << "│                 CLIENT INFO                   │" << RESET << std::endl;
    std::cout << BOLDWHITE << "╰───────────────────────────────────────────────╯" << RESET << std::endl;

    std::cout << CYAN << "  File Descriptor:  " << RESET << _fd << std::endl;
    std::cout << CYAN << "  Nickname:         " << RESET << (_nickname.empty() ? "(none)" : _nickname) << std::endl;
    std::cout << CYAN << "  Username:         " << RESET << (_username.empty() ? "(none)" : _username) << std::endl;
    std::cout << CYAN << "  Registered:       " << RESET << (_registered ? "Yes" : "No") << std::endl;

    std::cout << CYAN << "  Channels:         " << RESET;
    if (_channels.empty())
        std::cout << "(none)";
    else
    {
        std::set<std::string>::const_iterator it = _channels.begin();
        for (; it != _channels.end(); ++it)
        {
            if (it != _channels.begin()) std::cout << ", ";
            std::cout << *it;
        }
    }
    std::cout << std::endl;

    // Mostrar mensajes enviados y recibidos (solo si hay)
    if (!_sentMessages.empty())
    {
        std::cout << CYAN << "  Sent Messages:    " << RESET << std::endl;
        for (size_t i = 0; i < _sentMessages.size(); ++i)
            std::cout << "    [" << i + 1 << "] " << _sentMessages[i] << std::endl;
    }

    if (!_receivedMessages.empty())
    {
        std::cout << CYAN << "  Received Messages:" << RESET << std::endl;
        for (size_t i = 0; i < _receivedMessages.size(); ++i)
            std::cout << "    [" << i + 1 << "] " << _receivedMessages[i] << std::endl;
    }

    std::cout << std::endl;
}
