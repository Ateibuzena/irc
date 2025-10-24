#include <iostream>
#include <vector>
#include <string>
#include "../includes/parser/Parser.hpp"

void printCommand(const Command &cmd)
{
    std::cout << "Raw message: \"" << cmd.raw << "\"\n";
    std::cout << "Command name: " << cmd.name << "\n";
    std::cout << "Params (" << cmd.params.size() << "): ";
    for (size_t i = 0; i < cmd.params.size(); i++)
        std::cout << "[" << cmd.params[i] << "] ";
    std::cout << "\n\n";
}

int main()
{
    Parser parser;

    std::vector<std::string> testMessages = {
        // --- Original tests ---
        "NICK John\r\n",
        "USER john 0 * :John Doe\r\n",
        "PRIVMSG #chan :Hello everyone!\r\n",
        "PRIVMSG  #chan   :Extra spaces\r\n",
        "PRIVMSG #chan :\r\n",                // empty trailing param
        "JOIN #channel\r\n",
        "PART #channel\r\n",
        "PING :server1\r\n",
        "PONG server1\r\n",
        "QUIT :Goodbye!\r\n",
        "NOTICE #chan :Notice message\r\n",
        "MODE #chan +o John\r\n",
        "PRIVMSG #chan adsadasdas:aaaa\r\n",  // colon in middle of param
        "USER john 0 * :\r\n",               // empty real name
        "PRIVMSG #chan :Hello :world\r\n",   // colon inside trailing param

        // --- Additional tests for commas, multiple channels/nicks, keys, limits ---
        "JOIN #chan1,#chan2,#chan3\r\n",                 // valid multi-channel
        "PRIVMSG #chan1,#chan2 :Hello multiple channels\r\n",

        // Malformed channel list
        "JOIN #chan1,,#chan3\r\n",                       // empty element
        "JOIN ,#chan1,#chan2\r\n",                       // leading comma
        "JOIN #chan1,#chan2,\r\n",                       // trailing comma
        "JOIN #chan1,#chan$2\r\n",                       // invalid char
        "PRIVMSG #chan1,#chan2,#bad, :Hi\r\n",          // comma before empty element

        // Multiple nicks
        "MODE #chan +o Alice,Bob\r\n",                   // valid

        // Malformed nick list
        "MODE #chan +o Alice,,Bob\r\n",                 // double comma
        "MODE #chan +o ,Alice,Bob\r\n",                 // leading comma
        "MODE #chan +o Alice,Bob,\r\n",                 // trailing comma
        "MODE #chan +o Alice,Bo b\r\n",                 // space in nick
        "MODE #chan +o Alice,@Bob\r\n",                 // invalid char

        // Keys with bad commas
        "MODE #chan +k key1,key2\r\n",                  // only one key allowed
        "MODE #chan +k ,secret\r\n",                    // leading comma
        "MODE #chan +k secret,\r\n",                    // trailing comma

        // Limits with bad input
        "MODE #chan +l 0\r\n",                          // too low
        "MODE #chan +l 1001\r\n",                       // too high
        "MODE #chan +l abc\r\n",                        // non-numeric

        // Empty trailing params
        "PRIVMSG #chan :\r\n",
        "NOTICE #chan :\r\n",

        // Mixed colons
        "PRIVMSG #chan :Hello :world :again\r\n"
    };

    for (size_t i = 0; i < testMessages.size(); i++)
    {
        try
        {
            std::cout << "Testing message: \"" << testMessages[i] << "\"\n";
            Command cmd = parser.parse(testMessages[i]);
            printCommand(cmd);
        }
    catch (ErrorCodes errCode)
    {
        std::cout << RED << "Parsing error (" << errCode << "): "
                      << MenssagesError[errCode] << RESET << "\n\n";
    }
    }

    return 0;
}
