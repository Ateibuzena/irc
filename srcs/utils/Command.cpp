#include "../../includes/utils/Command.hpp"

std::map<int, std::string> messagesError =
{
    {200, "Success"},                       // ERR_SUCCESS
    {461, "Not enough parameters"},          // ERR_NEEDMOREPARAMS
    {462, "You may not reregister"},         // ERR_ALREADYREGISTERED
    {464, "Password incorrect"},             // ERR_PASSWDMISMATCH
    {431, "No nickname given"},              // ERR_NONICKNAMEGIVEN
    {432, "Erroneous nickname"},             // ERR_ERRONEUSNICKNAME
    {433, "Nickname is already in use"},     // ERR_NICKNAMEINUSE
    {409, "No origin specified"},            // ERR_NOORIGIN
    {491, "No OPer host"},                   // ERR_NOOPERHOST
    {403, "No such channel"},                // ERR_NOSUCHCHANNEL
    {405, "Too many channels"},              // ERR_TOOMANYCHANNELS
    {475, "Bad channel key"},                // ERR_BADCHANNELKEY
    {474, "Banned from channel"},            // ERR_BANNEDFROMCHAN
    {471, "Channel is full"},                // ERR_CHANNELISFULL
    {473, "Invite only channel"},            // ERR_INVITEONLYCHAN
    {476, "Bad channel mask"},               // ERR_BADCHANMASK
    {442, "Not on channel"},                 // ERR_NOTONCHANNEL
    {482, "You're not channel operator"},    // ERR_CHANOPRIVSNEEDED
    {443, "User is already on channel"},     // ERR_USERONCHANNEL
    {441, "User not in channel"},            // ERR_USERNOTINCHANNEL
    {402, "No such server"},                 // ERR_NOSUCHSERVER
    {422, "No MOTD"},                        // ERR_NOMOTD
    {491, "No privileges"},                  // ERR_NOPRIVILEGES
    {502, "No privs"},                       // ERR_NOPRIVS
    {401, "No such nick"},                   // ERR_NOSUCHNICK
    {502, "Users don't match"},              // ERR_USERSDONTMATCH
    {501, "Unknown MODE flag"},              // ERR_UMODEUNKNOWNFLAG
    {404, "Cannot send to channel"},         // ERR_CANNOTSENDTOCHAN
    {407, "Too many targets"},               // ERR_TOOMANYTARGETS
    {411, "No recipient given (PRIVMSG)"},   // ERR_NORECIPIENT
    {412, "No text to send"},                // ERR_NOTEXTTOSEND
    {413, "No toplevel domain specified"},   // ERR_NOTOPLEVEL
    {414, "Wildcard in toplevel domain"},    // ERR_WILDTOPLEVEL
    {421, "Unknown Error"},                  // ERR_UNKNOWN
    {451, "You have not registered"},        // ERR_NOTREGISTERED
    {465, "Password not authorized"},        // ERR_PASSWDAUTHORIZED
    {-1,  "Quit : Closing Link: SERVER_NAME"}// QUIT
};

std::map<int, std::string> messagesReplay =
{
    {381, "You're OPer"},                    // RPL_YOUREOPER
    {332, "Topic"},                          // RPL_TOPIC
    {333, "Topic who time"},                 // RPL_TOPICWHOTIME
    {353, "Name reply"},                     // RPL_NAMREPLY
    {366, "End of names"},                   // RPL_ENDOFNAMES
    {331, "No topic is set"},                // RPL_NOTOPIC
    {321, "List start"},                     // RPL_LISTSTART
    {322, "List"},                           // RPL_LIST
    {323, "List end"},                       // RPL_LISTEND
    {341, "Inviting"},                       // RPL_INVITING
    {375, "MOTD start"},                     // RPL_MOTDSTART
    {372, "MOTD"},                           // RPL_MOTD
    {376, "End of MOTD"},                    // RPL_ENDOFMOTD
    {5,   "ISUPPORT"},                       // RPL_ISUPPORT (005 → 5)
    {351, "VERSION"},                        // RPL_VERSION
    {256, "Admin Me"},                       // RPL_ADMINME
    {257, "Admin Loc1"},                     // RPL_ADMINLOC1
    {258, "Admin Loc2"},                     // RPL_ADMINLOC2
    {259, "Admin Email"},                    // RPL_ADMINEMAIL
    {391, "Time"},                           // RPL_TIME
    {371, "Info"},                           // RPL_INFO
    {374, "End of Info"},                    // RPL_ENDOFINFO
    {221, "User mode is"},                   // RPL_UMODEIS
    {324, "Channel mode is"},                // RPL_CHANNELMODEIS
    {329, "Creation time"},                  // RPL_CREATIONTIME
    {301, "Away"}                            // RPL_AWAY
};

std::string to_string_c98(int value)
{
    std::ostringstream  oss;

    oss << value;

    return (oss.str());
}

std::vector<std::string> str_to_vector(const std::string& str, char delimiter)
{
    std::vector<std::string>    result;
    size_t                      start = 0;
    size_t                      end = 0;

    end = str.find(delimiter, start);
    while (end != std::string::npos)
    {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    result.push_back(str.substr(start));
    
    return (result);
}