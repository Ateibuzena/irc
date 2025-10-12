#include "ServerLogic.hpp"
#include "../../test/TestUtils.hpp"

int main()
{
    separator("TEST DE SERVERLOGIC");

    {
        subsection("Inicialización del servidor lógico");
        ServerLogic logic(2222222222222222);
        success("ServerLogic inicializado con límite de " + to_string_c98(logic.getDefaultChannelLimit()) + " por canal.");
    }

    {
        subsection("Inicialización del servidor lógico");
        ServerLogic logic(-5);
        success("ServerLogic inicializado con límite de " + to_string_c98(logic.getDefaultChannelLimit()) + " por canal.");
    }
    {
        subsection("Inicialización del servidor lógico");
        ServerLogic logic(0);
        success("ServerLogic inicializado con límite de " + to_string_c98(logic.getDefaultChannelLimit()) + " por canal.");
    }
    {
        subsection("Inicialización del servidor lógico");
        ServerLogic logic;
        success("ServerLogic inicializado con límite de " + to_string_c98(logic.getDefaultChannelLimit()) + " por canal.");
    }
    subsection("Inicialización del servidor lógico");
    ServerLogic logic(2); // máximo 2 usuarios por canal
    success("ServerLogic inicializado con límite de " + to_string_c98(logic.getDefaultChannelLimit()) + " por canal.");

    // -------------------------------------------------------------------------
    // 🧱 Simulamos el "network layer": fd asignados a clientes
    // -------------------------------------------------------------------------

    subsection("Llegada de clientes (accept)");

    int                             next_fd = 1;
    std::map<int, std::string>      fakeClients; // fd -> nombre temporal

    // Cliente 1 se conecta
    int fd1 = next_fd++;
    logic.addClient(fd1);
    fakeClients[fd1] = "Cliente_1";
    info("🟢 Conectado nuevo cliente con fd = " + to_string_c98(fd1));

    // Cliente 2 se conecta
    int fd2 = next_fd++;
    logic.addClient(fd2);
    fakeClients[fd2] = "Cliente_2";
    info("🟢 Conectado nuevo cliente con fd = " + to_string_c98(fd2));

    // Cliente 3 se conecta
    int fd3 = next_fd++;
    logic.addClient(fd3);
    fakeClients[fd3] = "Cliente_3";
    info("🟢 Conectado nuevo cliente con fd = " + to_string_c98(fd3));

    success("Clientes conectados correctamente.");

    // -------------------------------------------------------------------------
    // 💬 Simulamos envío de comandos desde los clientes
    // -------------------------------------------------------------------------

    subsection("Procesamiento de comandos");

    Command cmd;

    // Cliente 1: NICK
    cmd.name = "NICK";
    cmd.params.clear(); cmd.params.push_back("Alice");
    cmd.raw = ":NICK Alice";
    logic.executeCommand(cmd, fd1);

    // Cliente 1: USER
    cmd.name = "USER";
    cmd.params.clear(); cmd.params.push_back("alice_user");
    cmd.raw = ":USER alice_user";
    logic.executeCommand(cmd, fd1);

    // Cliente 2: NICK + USER
    cmd.name = "NICK";
    cmd.params.clear(); cmd.params.push_back("Bob");
    cmd.raw = ":NICK Bob";
    logic.executeCommand(cmd, fd2);

    cmd.name = "USER";
    cmd.params.clear(); cmd.params.push_back("bob_user");
    cmd.raw = ":USER bob_user";
    logic.executeCommand(cmd, fd2);

    // Cliente 3: NICK + USER
    cmd.name = "NICK";
    cmd.params.clear(); cmd.params.push_back("Charlie");
    cmd.raw = ":NICK Charlie";
    logic.executeCommand(cmd, fd3);

    cmd.name = "USER";
    cmd.params.clear(); cmd.params.push_back("charlie_user");
    cmd.raw = ":USER charlie_user";
    logic.executeCommand(cmd, fd3);

    success("Asignación de nick y user simulada correctamente.");

    // -------------------------------------------------------------------------
    // 🔗 Simulamos comandos JOIN / PRIVMSG / PART
    // -------------------------------------------------------------------------

    subsection("JOIN / PRIVMSG / PART simulados");

    // Alice y Bob entran a #general
    cmd.name = "JOIN";
    cmd.params.clear(); cmd.params.push_back("#general");
    cmd.raw = ":JOIN #general";
    logic.executeCommand(cmd, fd1);
    logic.executeCommand(cmd, fd2);

    // Charlie intenta unirse (debería fallar por límite)
    logic.executeCommand(cmd, fd3);

    // Alice manda mensaje al canal
    cmd.name = "PRIVMSG";
    cmd.params.clear();
    cmd.params.push_back("#general");
    cmd.params.push_back("Hola a todos desde Alice!");
    cmd.raw = ":PRIVMSG #general :Hola a todos desde Alice!";
    logic.executeCommand(cmd, fd1);

    // Bob responde a Alice
    cmd.name = "PRIVMSG";
    cmd.params.clear();
    cmd.params.push_back("Alice");
    cmd.params.push_back("Hey Alice!");
    cmd.raw = ":PRIVMSG Alice :Hey Alice!";
    logic.executeCommand(cmd, fd2);

    // Alice sale del canal
    cmd.name = "PART";
    cmd.params.clear(); cmd.params.push_back("#general");
    cmd.raw = ":PART #general";
    logic.executeCommand(cmd, fd1);

    success("JOIN / PRIVMSG / PART simulados correctamente.");

     // -------------------------------------------------------------------------
    // 🔚 Simulamos desconexión de clientes
    // -------------------------------------------------------------------------

    subsection("Desconexión de clientes");

    logic.removeClient(fd1);
    info("Cliente " + to_string_c98(fd1) + " desconectado.");
    logic.removeClient(fd2);
    info("Cliente " + to_string_c98(fd2) + " desconectado.");
    logic.removeClient(fd3);
    info("Cliente " + to_string_c98(fd3) + " desconectado.");

    success("Todos los clientes eliminados correctamente.");

    separator("FIN DE SIMULACIÓN DEL SERVIDOR IRC");
   
    return (0);
}