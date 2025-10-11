#include "../../../includes/logic/Client.hpp"
#include "../../TestUtils.hpp"

void printClientInfo(const Client& c)
{
    std::cout
        << YELLOW
        << "🧍 Client Info:"
        << RESET << std::endl;
    std::cout
        << "  "
        << CYAN
        << "FD:          "
        << RESET
        << c.getFd()
        << std::endl;
    std::cout
        << "  "
        << CYAN
        << "Nickname:    "
        << RESET
        << c.getNickname()
        << std::endl;
    std::cout
        << "  "
        << CYAN
        << "Username:    "
        << RESET
        << c.getUsername()
        << std::endl;
    std::cout
        << "  "
        << CYAN
        << "Registered:  "
        << RESET
        << (c.isRegistered() ? (std::string(GREEN) + "✅ yes" + RESET)
                             : (std::string(RED) + "❌ no" + RESET))
        << std::endl;

    const std::set<std::string>& ch = c.getChannels();

    std::cout
        << "  "
        << CYAN
        << "Channels:    "
        << RESET;

    if (ch.empty())
        std::cout << RED << "none" << RESET;
    else
        for (std::set<std::string>::const_iterator it = ch.begin(); it != ch.end(); ++it)
            std::cout << GREEN << *it << " " << RESET;

    std::cout << std::endl;
}

int main()
{
    separator("🧩 Creating client Ana (fd=42)");
    Client c(42);

    info("Setting nickname and username...");
    c.setNickname("Ana");
    c.setUsername("azubieta");
    c.setRegistered(true);

    separator("📡 Joining channels");
    // Estos métodos ya imprimen mensajes de error/confirmación
    c.joinChannel("#42Malaga");    // ✅ Joined channel
    c.joinChannel("#random");       // ✅ Joined channel
    c.joinChannel("#42Malaga");    // ⚠️ Already in channel

    printClientInfo(c);

    separator("🚪 Leaving channels");
    c.leaveChannel("#random");       // ✅ Left channel
    c.leaveChannel("#nonexistent");  // ⚠️ Not in channel

    printClientInfo(c);

    separator("✅ Test completed");
    success("All logic methods behaved as expected! 🎉");

    return (0);
}
