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

extern int sig;

// Maximum users per channel limit
# define MAX_USERS_PER_CHANNEL 100

// Default maximum users per channel
# define DEFAULT_MAX_USERS_PER_CHANNEL 50

// Maximum total connected clients on the server
# define MAX_CLIENTS 500

// Maximum channels on the server
# define MAX_CHANNELS 200

// Maximum channels per client
# define MAX_CHANNELS_PER_CLIENT 100

// Maximum characters in an IRC message (RFC 2812)
# define MAX_MESSAGE_LENGTH 512

// 🎨 Colores IRC
# define RESET      "\x03"      // Reset
# define RED        "\x034"     // Red
# define GREEN      "\x033"     // Green
# define YELLOW     "\x038"     // Yellow (orange in some clients)
# define BLUE       "\x032"     // Blue
# define MAGENTA    "\x036"     // Purple


struct ParsedInput
{
    std::string                 name; // Ex: "NICK", "JOIN", "PRIVMSG"
    std::vector<std::string>    params; // Ex: {"#42Malaga", "hola mundo"}
    std::string                 raw; // Original message (for debug)

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
    QUIT,
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

#include <sstream> // To use std::ostringstream

// Helper function for C++98
std::string to_string_c98(int value);

// Function to split a string into a vector using a delimiter
std::vector<std::string> str_to_vector(const std::string& str, char delimiter);

// Function to convert time to string
std::string time_to_string(std::time_t t);

// Functions to build messages
std::string buildMessage(const std::string& prefix,
                        const std::string& command,
                        const std::string& target = "",
                        const std::string& aux = "");

// Function to build error messages
std::string buildErrorMessage(const std::string& prefix,
                            const std::string& aux = "",
                            const std::string& msg = "");

void    handle_kill(int sig);

#endif