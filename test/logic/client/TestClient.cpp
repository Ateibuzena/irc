#include "../../../includes/logic/Client.hpp"
#include "../../../includes/logic/Channel.hpp"
#include "../../TestUtils.hpp"

/*
🧩 Test de robustez para Client y su interacción con Channel.
Cubre:
 - Validación de nickname/username
 - Join/leave con casos de error
 - Mensajes vacíos, duplicados y a sí mismo
 - Disconnect y limpieza de canales
 - Buffers de mensajes
*/

int main()
{
    separator("🧩 TEST CLIENT (ROBUST)");

    // Crear cliente base
    Client c(42);
    success("Client created: fd=42");

    //==============================//
    //  VALIDACIÓN DE DATOS BÁSICOS //
    //==============================//
    separator("🧠 Nickname & Username validation");

    std::string invalidNicks[] = {"", "!", "Ana@", "123", "#badNick"};
    for (int i = 0; i < 5; ++i)
    {
        bool valid = isValidNickname(invalidNicks[i]);
        info("Nickname '" + invalidNicks[i] + "' valid? " + (valid ? "YES" : "NO"));
    }

    std::string invalidUsers[] = {"", "root$", " ", "A", "user!"};
    for (int i = 0; i < 5; ++i)
    {
        bool valid = isValidUsername(invalidUsers[i]);
        info("Username '" + invalidUsers[i] + "' valid? " + (valid ? "YES" : "NO"));
    }

    c.setNickname("Ana_42");
    c.setUsername("azubieta");
    c.setRegistered(true);
    success("✅ Valid nickname and username applied");

    //==============================//
    //         JOIN / LEAVE         //
    //==============================//
    separator("🏠 Join / Leave channels");

    Channel channel1("#channel1");
    Channel channel2("#42Malaga");
    Channel channel3("#random");

    ChannelStatus status;

    // join válido
    status = c.joinChannel(&channel1);
    success("Join #channel1: " + to_string_c98(status));

    // duplicado
    status = c.joinChannel(&channel1);
    success("Join #channel1 again (expect DUPLICATE): " + to_string_c98(status));

    // join a canal inválido (nombre inválido)
    Channel* invalid = new Channel("badName"); // sin #
    status = c.joinChannel(invalid);
    success("Join invalid channel: " + to_string_c98(status));

    // join null
    status = c.joinChannel(NULL);
    success("Join NULL channel: " + to_string_c98(status));

    // join normal
    status = c.joinChannel(&channel2);
    success("Join #42Malaga: " + to_string_c98(status));

    c.printInfo();

    // leave normal
    status = c.leaveChannel(&channel1);
    success("Leave #channel1: " + to_string_c98(status));

    // leave doble
    status = c.leaveChannel(&channel1);
    success("Leave #channel1 again (expect NOT_IN_CHANNEL): " + to_string_c98(status));

    // leave null
    status = c.leaveChannel(NULL);
    success("Leave NULL channel: " + to_string_c98(status));

    //==============================//
    //     MENSAJES Y BUFFERS       //
    //==============================//
    separator("✉️ Sending / Receiving messages");

    // Mensaje normal
    c.sendMessage("Hello!", "#42Malaga");
    // Vacío
    c.sendMessage("", "#42Malaga");
    // A sí mismo
    c.sendMessage("This is me!", "Ana_42");

    // Recibir normal
    c.receiveMessage("Welcome Ana!", "Server");
    // Recibir vacío
    c.receiveMessage("", "Server");

    success("Sent messages: " + to_string_c98(c.getSentMessages().size()));
    success("Received messages: " + to_string_c98(c.getReceivedMessages().size()));

    //==============================//
    //           DISCONNECT          //
    //==============================//
    separator("🔌 Disconnect behavior");

    c.disconnect();
    success("Client disconnected");

    info("Registered after disconnect? " + std::string(c.isRegistered() ? "YES" : "NO"));
    info("Channels after disconnect: " + to_string_c98(c.getChannels().size()));

    separator("✅ TEST CLIENT FINISHED");
    return (0);
}
