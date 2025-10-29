#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unistd.h>



// Límite máximo de usuarios por canal
#define MAX_USERS_PER_CHANNEL 100

// Límite máximo total de clientes conectados al servidor
#define MAX_CLIENTS 500

// Límite máximo de canales por cliente
#define MAX_CHANNELS_PER_CLIENT 100

// Límite máximo de caracteres en un mensaje IRC (RFC 2812)
#define MAX_MESSAGE_LENGTH 512

//
// 🎨 Colores ANSI
//
#define RESET   "\033[0m"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[0;36m"
#define MAGENTA "\033[0;35m"
#define BOLDWHITE "\033[1;37m"

struct Command
{
    std::string name;                 // Ej: "NICK", "JOIN", "PRIVMSG"
    std::vector<std::string> params;  // Ej: {"#42Malaga", "hola mundo"}
    std::string raw;                  // Mensaje original (por debug)
};

enum ErrorCodes
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
    RPL_YOUREOPER,
    QUIT,
    ERR_NOSUCHCHANNEL,
    ERR_TOOMANYCHANNELS,
    ERR_BADCHANNELKEY,
    ERR_BANNEDFROMCHAN,
    ERR_CHANNELISFULL,
    ERR_INVITEONLYCHAN,
    ERR_BADCHANMASK,
    RPL_TOPIC,
    RPL_TOPICWHOTIME,
    RPL_NAMREPLY,
    RPL_ENDOFNAMES,
    ERR_NOTONCHANNEL,
    ERR_CHANOPRIVSNEEDED,
    RPL_NOTOPIC,
    RPL_LISTSTART,
    RPL_LIST,
    RPL_LISTEND,
    RPL_INVITING,
    ERR_USERONCHANNEL,
    ERR_USERNOTINCHANNEL,
    ERR_NOSUCHSERVER,
    ERR_NOMOTD,
    RPL_MOTDSTART,
    RPL_MOTD,
    RPL_ENDOFMOTD,
    RPL_ISUPPORT,
    RPL_VERSION,
    RPL_ADMINME,
    RPL_ADMINLOC1,
    RPL_ADMINLOC2,
    RPL_ADMINEMAIL,
    ERR_NOPRIVILEGES,
    ERR_NOPRIVS,
    RPL_TIME,
    RPL_INFO,
    RPL_ENDOFINFO,
    ERR_NOSUCHNICK,
    ERR_USERSDONTMATCH,
    RPL_UMODEIS,
    ERR_UMODEUNKNOWNFLAG,
    RPL_CHANNELMODEIS,
    RPL_CREATIONTIME,
    ERR_CANNOTSENDTOCHAN,
    ERR_TOOMANYTARGETS,
    ERR_NORECIPIENT,
    ERR_NOTEXTTOSEND,
    ERR_NOTOPLEVEL,
    ERR_WILDTOPLEVEL,
    RPL_AWAY,
    ERR_UNKNOWN,
    ERR_NOTREGISTERED,
    ERR_PASSWDAUTHORIZED
};

extern std::string MenssagesError[];

#include <sstream> // Para convertir int a string en C++98

// Función auxiliar para C++98
std::string to_string_c98(int value);

std::vector<std::string> str_to_vector(const std::string& str, char delimiter);

#endif // COMMAND_HPP

/*común entre Parser y Lógica*/