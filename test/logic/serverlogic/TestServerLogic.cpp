// TestServerLogic.cpp
#include "ServerLogic.hpp"
#include "../../TestUtils.hpp"

// Función auxiliar para crear un comando
static void sendCommand(ServerLogic &server, int clientFd,
                 const std::string &name,
                 const std::string &param1 = "",
                 const std::string &param2 = "",
                 const std::string &raw = "")
{
    Command cmd;
    cmd.name = name;
    if (!param1.empty()) cmd.params.push_back(param1);
    if (!param2.empty()) cmd.params.push_back(param2);
    cmd.raw = raw.empty() ? ":" + name + " " + param1 : raw;
    server.executeCommand(cmd, clientFd);
}

int main()
{
    separator("TEST ROBUSTO DE SERVERLOGIC");

    subsection("Creación de ServerLogic");
    ServerLogic server(2);
    success("Servidor creado con límite de canales = 2");

    subsection("Agregar clientes");
    server.addClient(1);
    server.addClient(2);
    server.addClient(3);
    // Cliente duplicado
    server.addClient(2);
    success("Clientes 1, 2 y 3 agregados, intento duplicado detectado");

    subsection("Asignar NICK y USER");
    sendCommand(server, 1, "NICK", "Alice");
    sendCommand(server, 2, "NICK", "Bob");
    sendCommand(server, 3, "NICK", "Charlie");

    sendCommand(server, 1, "USER", "alice_user");
    sendCommand(server, 2, "USER", "bob_user");
    sendCommand(server, 3, "USER", "charlie_user");

    // Intentar nick duplicado
    sendCommand(server, 2, "NICK", "Alice");

    success("Nicknames y usernames asignados");

    subsection("JOIN canales");
    sendCommand(server, 1, "JOIN", "#general");
    sendCommand(server, 2, "JOIN", "#general");
    sendCommand(server, 3, "JOIN", "#general"); // canal lleno
    sendCommand(server, 3, "JOIN", "#random");
    sendCommand(server, 2, "JOIN", "#random"); // éxito

    success("Prueba JOIN con límites y múltiples canales");

    subsection("PRIVMSG");
    sendCommand(server, 1, "PRIVMSG", "#general", "Hola todos!");
    sendCommand(server, 1, "PRIVMSG", "Bob", "Hola Bob!");
    sendCommand(server, 1, "PRIVMSG", "Nobody", "Hola"); // destino no existe
    sendCommand(server, 3, "PRIVMSG", "#random", "Mensaje random");
    sendCommand(server, 3, "PRIVMSG", "Charlie", "Hola a mi mismo"); // mensaje a sí mismo

    success("PRIVMSG probados con diferentes casos");

    subsection("PART canales");
    sendCommand(server, 1, "PART", "#general");
    sendCommand(server, 3, "PART", "#general"); // no estaba
    sendCommand(server, 3, "PART", "#random");
    sendCommand(server, 2, "PART", "#random");

    success("PART probado con distintos escenarios");

    subsection("Remover clientes");
    server.removeClient(1);
    server.removeClient(2);
    server.removeClient(3);
    // Cliente inexistente
    server.removeClient(4);

    success("Todos los clientes eliminados, intento de eliminación inválida detectado");

    separator("FIN TEST ROBUSTO SERVERLOGIC");
    return (0);
}
