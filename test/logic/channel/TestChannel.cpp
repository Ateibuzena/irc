#include "../../../includes/logic/Channel.hpp"
#include "../../../includes/logic/Client.hpp"
#include "../../TestUtils.hpp"

int main()
{
    separator("TEST DE CHANNEL");

    // Creamos clientes
    Client alice(1);
    Client bob(2);
    Client charlie(3);

    success("Clientes creados: Alice(1), Bob(2), Charlie(3)");

    // Creamos canal
    Channel general("#general");
    success("Canal creado: " + general.getName());

    // Añadimos clientes
    general.addClient(&alice);
    general.addClient(&bob);
    general.addClient(&charlie);
    info("Clientes añadidos al canal");

    // Broadcast inicial
    general.broadcast("¡Hola a todos!");
    success("Mensaje enviado a todos los clientes");

    // Verificar buffers
    const std::vector<std::string>& aliceMsgs = alice.getReceivedMessages();
    const std::vector<std::string>& bobMsgs   = bob.getReceivedMessages();
    const std::vector<std::string>& charlieMsgs = charlie.getReceivedMessages();

    if (!aliceMsgs.empty() && !bobMsgs.empty() && !charlieMsgs.empty())
        success("Todos los clientes recibieron el mensaje correctamente");
    else
        error("Algunos clientes no recibieron el mensaje");

    // Probamos remover un cliente
    general.removeClient(&bob);
    success("Bob eliminado del canal");

    // Broadcast tras eliminar a Bob
    general.broadcast("Bob se ha ido");
    info("Mensaje enviado tras eliminar a Bob");

    // Revisamos los buffers tras el broadcast
    if (alice.getReceivedMessages().size() == 2 && charlie.getReceivedMessages().size() == 2 && bob.getReceivedMessages().size() == 1)
        success("Buffers correctos tras eliminar a Bob");
    else
        error("Error en buffers tras eliminar a Bob");

    // Edge case: añadir cliente duplicado
    general.addClient(&alice);
    warn("Intento de añadir cliente duplicado (Alice)");

    // Edge case: remover cliente no existente
    general.removeClient(&bob);
    warn("Intento de eliminar cliente que no está en el canal (Bob)");

    separator("FIN DE TEST CHANNEL");

    return (0);
}
