#include "../includes/Command.hpp"

std::string MenssagesError[] =
{
    /*SUCCESS*/
    "200 :Success",

    /*PASS*/
    "461 :Not enough parameters",
    "462 :You may not reregister",
    "464 :Password incorrect",
    
    /*NICK*/
    "431 :No nickname given",
    "432 :Erroneous nickname",
    "433 :Nickname is already in use",

    /*USER*/
    /*"461 :Not enough parameters",*/
    /*"462 :You may not reregister",*/
    /*"464 :Password incorrect",,*/

    /*PING*/
    "409 :No origin specified",

    /*PONG*/
    "491 :No OPer host",
    "381 :You're OPer",

    /*QUIT*/
    "Quit : Closing Link: SERVER_NAME",

    /*JOIN*/
    /*"461 :Not enough parameters",*/
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
    /*"461 :Not enough parameters",*/
    /*"403 :No such channel",*/
    "442 :Not on channel",

    /*TOPIC*/
    /*"461 :Not enough parameters",*/
    /*"403 :No such channel",*/
    /*"442 :Not on channel",*/
    "482 :You're not channel operator",
    "331 :No topic is set",
    /*"332 :Topic",*/
    /*"333 :Topic who time",*/

    /*NAMES*/
    /*"353 :Name reply",*/
    /*"366 :End of names",*/

    /*LIST*/
    "321 :List start",
    "322 :List",
    "323 :List end",

    /*INVITE*/
    "341 :Inviting",
    /*"461 :Not enough parameters",*/
    /*"403 :No such channel",*/
    /*"442 :Not on channel",*/
    /*"482 :You're not channel operator",*/
    "443 :User is already on channel",

    /*KICK*/
    /*"461 :Not enough parameters",*/
    /*"403 :No such channel",*/
    /*"482 :You're not channel operator",*/
    "441 :User not in channel",
    /*"442 :Not on channel",*/

    /*MOTD*/
    "402 :No such server",
    "422 :No MOTD",
    "375 :MOTD start",
    "372 :MOTD",
    "376 :End of MOTD",

    /*VERSION*/
    /*"402 :No such server",*/
    "005 :ISUPPORT",
    "351 :VERSION",

    /*ADMIN*/
    /*"402 :No such server",*/
    "256 :Admin Me",
    "257 :Admin Loc1",
    "258 :Admin Loc2",
    "259 :Admin Email",

    /*TIME*/
    /*"402 :No such server",*/
    "391 :Time",

    /*INFO*/
    "371 :Info",
    "374 :End of Info",

    /*MODE*/
    "401 :No such nick",
    "502 :Users don't match",
    "221 :User mode is",
    "501 :Unknown MODE flag",
    /*"403 :No such channel",*/
    "324 :Channel mode is",
    "329 :Creation time",
    /*"482 :You're not channel operator",*/

    /*NOTICE*/
    /*The NOTICE message is used similarly to PRIVMSG. The difference between NOTICE and PRIVMSG is that automatic replies must never be sent in response to a NOTICE message. This rule also applies to servers – they must not send any error back to the client on receipt of a NOTICE command. The intention of this is to avoid loops between a client automatically sending something in response to something it received. This is typically used by ‘bots’ (a client with a program, and not a user, controlling their actions) and also for server messages to clients.*/

    /*PRIVMSG*/
    /*"401 :No such nick",*/
    /*"402 :No such server",*/
    "404 :Cannot send to channel",
    "407 :Too many targets",
    "411 :No recipient given (PRIVMSG)",
    "412 :No text to send",
    "413 :No toplevel domain specified",
    "414 :Wildcard in toplevel domain",
    "301 :Away",

    /*UNKNOWN ERROR*/
    "421 :Unknown Error",

    "451 :You have not registered"

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