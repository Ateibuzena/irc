#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <map>

// Límite máximo de usuarios por canal
#define MAX_USERS_PER_CHANNEL 100

// Número máximo total de clientes conectados al servidor
#define MAX_CLIENTS 500

// Otros límites o parámetros futuros
#define SERVER_NAME "irc.42malaga.net"
#define SERVER_VERSION "1.0"

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
    /*SUCCESS*/
    ERR_SUCCESS,

    /*NICK*/
    ERR_NONICKNAMEGIVEN,
    ERR_ERRONEUSNICKNAME,
    ERR_NICKNAMEINUSE,

    /*JOIN*/
    ERR_NEEDMOREPARAMS,
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

    /*PART*/
    ERR_NEEDMOREPARAMS,
    ERR_NOSUCHCHANNEL,
    ERR_NOTONCHANNEL,

    /*TOPIC*/
    ERR_NEEDMOREPARAMS,
    ERR_NOSUCHCHANNEL,
    ERR_NOTONCHANNEL,
    ERR_CHANOPRIVSNEEDED,
    RPL_NOTOPIC,
    RPL_TOPIC,
    RPL_TOPICWHOTIME,

    /*NAMES*/
    RPL_NAMREPLY,
    RPL_ENDOFNAMES,

    /*LIST*/
    RPL_LISTSTART,
    RPL_LIST,
    RPL_LISTEND,

    /*INVITE*/
    RPL_INVITING,
    ERR_NEEDMOREPARAMS,
    ERR_NOSUCHCHANNEL,
    ERR_NOTONCHANNEL,
    ERR_CHANOPRIVSNEEDED,
    ERR_USERONCHANNEL,

    /*KICK*/
    ERR_NEEDMOREPARAMS,
    ERR_NOSUCHCHANNEL,
    ERR_CHANOPRIVSNEEDED,
    ERR_USERNOTINCHANNEL,
    ERR_NOTONCHANNEL,

    /*MOTD*/
    ERR_NOSUCHSERVER,
    ERR_NOMOTD,
    RPL_MOTDSTART,
    RPL_MOTD,
    RPL_ENDOFMOTD,

    /*VERSION*/
    ERR_NOSUCHSERVER,
    RPL_ISUPPORT,
    RPL_VERSION,

    /*ADMIN*/
    ERR_NOSUCHSERVER,
    RPL_ADMINME,
    RPL_ADMINLOC1,
    RPL_ADMINLOC2,
    RPL_ADMINEMAIL,

    /*CONNECT*/
    ERR_NOSUCHSERVER,
    ERR_NEEDMOREPARAMS,
    ERR_NOPRIVILEGES,
    ERR_NOPRIVS,

    /*TIME*/
    ERR_NOSUCHSERVER,
    RPL_TIME,

    /*INFO*/
    RPL_INFO,
    RPL_ENDOFINFO,

    /*MODE*/
    ERR_NOSUCHNICK,
    ERR_USERSDONTMATCH,
    RPL_UMODEIS,
    ERR_UMODEUNKNOWNFLAG,
    ERR_NOSUCHCHANNEL,
    RPL_CHANNELMODEIS,
    RPL_CREATIONTIME,
    ERR_CHANOPRIVSNEEDED,


};

std::string MenssagesError[] =
{
    /*SUCCESS*/
    "200 :Success",

    /*NICK*/
    "431 :No nickname given",
    "432 :Erroneous nickname",
    "433 :Nickname is already in use",

    /*JOIN*/
    "461 :Not enough parameters",
    "403 :No such channel",
    "405 :Too many channels",
    "475 :Bad channel key",
    "474 :Banned from channel",
    "471 :Channel is full",
    "473 :Invite only channel",
    "476 :Bad channel mask",
    "332 :Topic",
    "333 :Topic who time",
    "353 :Name reply",
    "366 :End of names",

    /*PART*/
    "461 :Not enough parameters",
    "403 :No such channel",
    "442 :Not on channel",

    /*TOPIC*/
    "461 :Not enough parameters",
    "403 :No such channel",
    "442 :Not on channel",
    "482 :You're not channel operator",
    "331 :No topic is set",
    "332 :Topic",
    "333 :Topic who time",

    /*NAMES*/
    "353 :Name reply",
    "366 :End of names",

    /*LIST*/
    "321 :List start",
    "322 :List",
    "323 :List end",

    /*INVITE*/
    "341 :Inviting",
    "461 :Not enough parameters",
    "403 :No such channel",
    "442 :Not on channel",
    "482 :You're not channel operator",
    "443 :User is already on channel",

    /*KICK*/
    "461 :Not enough parameters",
    "403 :No such channel",
    "482 :You're not channel operator",
    "441 :User not in channel",
    "442 :Not on channel",

    /*MOTD*/
    "402 :No such server",
    "422 :No MOTD",
    "375 :MOTD start",
    "372 :MOTD",
    "376 :End of MOTD",

    /*VERSION*/
    "402 :No such server",
    "005 :ISUPPORT",
    "351 :VERSION",

    /*ADMIN*/
    "402 :No such server",
    "256 :Admin Me",
    "257 :Admin Loc1",
    "258 :Admin Loc2",
    "259 :Admin Email",

    /*TIME*/
    "402 :No such server",
    "391 :Time",

    /*INFO*/
    "371 :Info",
    "374 :End of Info",

    /*MODE*/
    "401 :No such nick",
    "502 :Users don't match",
    "221 :User mode is",
    "501 :Unknown MODE flag",
    "403 :No such channel",
    "324 :Channel mode is",
    "329 :Creation time",
    "482 :You're not channel operator",

    /*NOTICE*/
    /*The NOTICE message is used similarly to PRIVMSG. The difference between NOTICE and PRIVMSG is that automatic replies must never be sent in response to a NOTICE message. This rule also applies to servers – they must not send any error back to the client on receipt of a NOTICE command. The intention of this is to avoid loops between a client automatically sending something in response to something it received. This is typically used by ‘bots’ (a client with a program, and not a user, controlling their actions) and also for server messages to clients.*/
};

enum ChannelStatus
{
    JOIN_SUCCESS,

    ALREADY_IN_CHANNEL,
    CHANNEL_FULL,          // <--- nuevo
    CHANNEL_EMPTY,

    INVALID_CLIENT,      // <--- nuevo
    
    LEAVE_SUCCESS,

    NOT_IN_CHANNEL
};

#include <sstream> // Para convertir int a string en C++98

// Función auxiliar para C++98
inline std::string to_string_c98(int value)
{
    std::ostringstream oss;
    oss << value;
    return (oss.str());
}

#endif // COMMAND_HPP

/*común entre Parser y Lógica*/