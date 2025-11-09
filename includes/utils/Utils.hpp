#ifndef UTILS_HPP
#define UTILS_HPP

# include <string>
# include <map>
# include <set>
# include <vector>
# include <string>
# include <iostream>
# include <algorithm>
# include <unistd.h>
# include <ctime>



// Límite máximo de usuarios por canal
# define MAX_USERS_PER_CHANNEL 100

// Límite máximo total de clientes conectados al servidor
# define MAX_CLIENTS 500

// Límite máximo de canales por cliente
# define MAX_CHANNELS_PER_CLIENT 100

// Límite máximo de caracteres en un mensaje IRC (RFC 2812)
# define MAX_MESSAGE_LENGTH 512

//
// 🎨 Colores ANSI
//
/*# define RESET   "\033[0m"
# define RED     "\033[0;31m"
# define GREEN   "\033[0;32m"
# define YELLOW  "\033[1;33m"
# define BLUE    "\033[1;34m"
# define CYAN    "\033[0;36m"
# define MAGENTA "\033[0;35m"
# define BOLDWHITE "\033[1;37m"*/

// 🎨 Colores IRC — usan el carácter de control \x03
# define RESET      "\x03"      // Resetea el color
# define RED        "\x034"     // Rojo
# define GREEN      "\x033"     // Verde
# define YELLOW     "\x038"     // Amarillo (naranja en algunos clientes)
# define BLUE       "\x032"     // Azul
# define MAGENTA    "\x036"     // Púrpura / magenta


struct ParsedInput
{
    std::string                 name; // Ej: "NICK", "JOIN", "PRIVMSG"
    std::vector<std::string>    params; // Ej: {"#42Malaga", "hola mundo"}
    std::string                 raw; // Mensaje original (por debug)

    ParsedInput()
        :   name(""),
            params(),
            raw("")
    {

    }
};

// ---------------- STRUCT ----------------
struct MessageInfo
{
    //int         code;
    std::string code;
    std::string message;

    MessageInfo()
        :   code(""),
            message("")
    {

    }

    MessageInfo(std::string c, const std::string& msg)
        :   code(c),
            message(msg)
    {

    }
};

// ---------------- ENUMS ----------------
enum errorCodes
{
    ERR_SUCCESS,
    ERR_NEEDMOREPARAMS,
    ERR_ALREADYREGISTERED,
    ERR_PASSWDMISMATCH,
    ERR_NONICKNAMEGIVEN,
    ERR_ERRONEUSNICKNAME,
    ERR_NICKNAMEINUSE,
    ERR_NOORIGIN,
    ERR_NOOPERHOST,
    ERR_TOOMANYCHANNELS,
    ERR_BADCHANNELKEY,
    ERR_BANNEDFROMCHAN,
    ERR_CHANNELISFULL,
    ERR_INVITEONLYCHAN,
    ERR_BADCHANMASK,
    ERR_NOTONCHANNEL,
    ERR_CHANOPRIVSNEEDED,
    ERR_USERONCHANNEL,
    ERR_USERNOTINCHANNEL,
    ERR_NOSUCHSERVER,
    ERR_NOMOTD,
    ERR_NOPRIVILEGES,
    ERR_NOPRIVS,
    ERR_NOSUCHNICKCHANNEL,
    ERR_USERSDONTMATCH,
    ERR_UMODEUNKNOWNFLAG,
    ERR_CANNOTSENDTOCHAN,
    ERR_TOOMANYTARGETS,
    ERR_NORECIPIENT,
    ERR_NOTEXTTOSEND,
    ERR_NOTOPLEVEL,
    ERR_WILDTOPLEVEL,
    ERR_NOTREGISTERED,
    ERR_PASSWDAUTHORIZED,
    QUIT, // mensaje especial
    ERR_INVALIDUSERNAME,
    ERR_UNKNOWNMODEFLAG,
    ERR_UNKNOWN
};

enum replayCodes
{
    RPL_YOUREOPER,
    RPL_TOPIC,
    RPL_TOPICWHOTIME,
    RPL_NAMREPLY,
    RPL_ENDOFNAMES,
    RPL_NOTOPIC,
    RPL_LISTSTART,
    RPL_LIST,
    RPL_LISTEND,
    RPL_INVITING,
    RPL_MOTDSTART,
    RPL_MOTD,
    RPL_ENDOFMOTD,
    RPL_ISUPPORT,
    RPL_VERSION,
    RPL_ADMINME,
    RPL_ADMINLOC1,
    RPL_ADMINLOC2,
    RPL_ADMINLOC3,
    RPL_ADMINEMAIL,
    RPL_TIME,
    RPL_INFO,
    RPL_ENDOFINFO,
    RPL_UMODEIS,
    RPL_CHANNELMODEIS,
    RPL_CREATIONTIME,
    RPL_AWAY,
    RPL_WELCOME,
    RPL_YOURHOST,
    RPL_CREATED,
    RPL_MYINFO
};

extern std::map<int, MessageInfo> messagesError;
extern std::map<int, MessageInfo> messagesReplay;

void    initErrorMessages(void);
void    initReplayMessages(void);

#include <sstream> // Para convertir int a string en C++98

// Función auxiliar para C++98
std::string to_string_c98(int value);

// Función para dividir una cadena en un vector de cadenas usando un delimitador
std::vector<std::string> str_to_vector(const std::string& str, char delimiter);

// Función para convertir tiempo a string
std::string time_to_string(std::time_t t);

// Funciones para construir mensajes
std::string buildMessage(const std::string& prefix,
                        const std::string& command,
                        const std::string& target = "",
                        const std::string& aux = "");

// Función para construir mensajes de error
std::string buildErrorMessage(const std::string& prefix,
                            const std::string& aux = "",
                            const std::string& msg = "");

#endif // COMMAND_HPP

/*común entre Parser y Lógica*/