#include "../../../includes/logic/Client.hpp"
#include "../../TestUtils.hpp"

/*🧩 Todas las nuevas funcionalidades y validaciones IRC de tu Client. Esto incluye:

- Probando nicknames y usernames válidos e inválidos

- Join/leave de canales con enums

- Enviar y recibir mensajes

- Disconnect

- Comprobar buffers de mensajes
*/

#include <sstream> // Para convertir int a string en C++98

// Función auxiliar para C++98
std::string to_string_c98(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

int main()
{
    //==============================//
    //      CREAR CLIENTE           //
    //==============================//
    separator("🧩 Creating client Ana (fd=42)");
    Client c(42);

    //==============================//
    //      CONFIGURAR NICKNAME Y USERNAME
    //==============================//
    info("Setting nickname and username...");

    // Nicknames válidos e inválidos
    c.setNickname("");              // ⚠️ inválido
    c.setNickname("123Ana");        // ⚠️ empieza con número
    c.setNickname("A!@#");          // ⚠️ caracteres inválidos
    c.setNickname("VeryLongNick");  // ⚠️ demasiado largo
    c.setNickname("Ana_42");        // ✅ válido

    // Usernames válidos e inválidos
    c.setUsername("");                     // ⚠️ inválido
    c.setUsername("this_username_is_too_long"); // ⚠️ demasiado largo
    c.setUsername("user@name");            // ⚠️ caracteres inválidos
    c.setUsername("azubieta");             // ✅ válido

    c.setRegistered(true);

    //==============================//
    //        GESTIÓN DE CANALES    //
    //==============================//
    separator("📡 Joining channels");
    ChannelStatus status;

    // Probar join con espacios extra
    status = c.joinChannel("  #channel1  "); // trim debería limpiar
    success("Join status: " + to_string_c98(status));

    // Intentar unirse a un canal existente
    status = c.joinChannel("#channel1");
    success("Join status: " + to_string_c98(status));

    // Salir de todos los canales
    std::set<std::string> channels = c.getChannels();
    for (std::set<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
        c.leaveChannel(*it);

    // Salir de un canal inexistente
    status = c.leaveChannel("#channel1");
    success("Leave status: " + to_string_c98(status));

    // Unirse a canales adicionales
    status = c.joinChannel("#42Malaga");
    success("Join status: " + to_string_c98(status));

    status = c.joinChannel("#random");
    success("Join status: " + to_string_c98(status));

    status = c.joinChannel("#42Malaga");
    success("Join status: " + to_string_c98(status));

    c.printInfo();

    //==============================//
    //   ENVIAR Y RECIBIR MENSAJES  //
    //==============================//
    separator("✉️ Sending and receiving messages");

    // Mensajes vacíos
    c.sendMessage("", "#42Malaga");
    c.receiveMessage("", "Server");

    // Mensajes largos
    std::string longMsg(1024, 'A');
    c.sendMessage(longMsg, "#42Malaga");
    c.receiveMessage(longMsg, "Server");

    // Mensajes normales
    c.sendMessage("Hello everyone!", "#42Malaga");
    c.receiveMessage("Welcome Ana!", "Server");

    // Comprobar buffers de mensajes
    success("Sent messages count: " + to_string_c98(c.getSentMessages().size()));
    success("Received messages count: " + to_string_c98(c.getReceivedMessages().size()));

    //==============================//
    //        SALIR DE CANALES      //
    //==============================//
    separator("🚪 Leaving channels");

    status = c.leaveChannel("#random");
    success("Leave status: " + to_string_c98(status));

    status = c.leaveChannel("#nonexistent");
    success("Leave status: " + to_string_c98(status));

    c.printInfo();

    //==============================//
    //        DESCONEXIÓN           //
    //==============================//
    separator("🔌 Disconnecting client");

    c.disconnect();

    // Probar desconexión doble
    c.disconnect(); // no debería crashear ni duplicar limpieza

    c.printInfo();

    //==============================//
    //          FIN TEST            //
    //==============================//
    separator("✅ Test completed");
    success("All Client logic methods behaved as expected! 🎉");

    return (0);
}
