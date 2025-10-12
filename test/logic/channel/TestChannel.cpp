#include "../../../includes/logic/Channel.hpp"
#include "../../../includes/logic/Client.hpp"
#include "../../TestUtils.hpp"

/*
🧠 Nivel 2: Test de robustez del sistema de canales
- Añadir/eliminar clientes duplicados o nulos
- Límite de capacidad del canal
- Broadcast en canal vacío o con clientes desconectados
- Consistencia de nicknames tras operaciones
- Limpieza de memoria y estabilidad
*/

int main()
{
    separator("🧩 TEST CHANNEL (ROBUST V2)");

    //==============================//
    //        SETUP BÁSICO          //
    //==============================//
    subsection("🧱 Creación de clientes y canal");

    Client alice(1);
    Client bob(2);
    Client charlie(3);

    alice.setNickname("Alice");
    bob.setNickname("Bob");
    charlie.setNickname("Charlie");
    success("Clientes creados y nicknames configurados");

    Channel general("#general", 2); // límite de 2
    success("Canal creado: " + general.getName());

    //==============================//
    //          ADD CLIENTS         //
    //==============================//
    subsection("➕ Añadir clientes al canal");
    ChannelStatus status;

    status = general.addClient(&alice);
    success("Add Alice: " + to_string_c98(status));

    status = general.addClient(&bob);
    success("Add Bob: " + to_string_c98(status));

    status = general.addClient(&charlie); // límite alcanzado
    if (status == CHANNEL_FULL)
        success("Add Charlie (esperado CHANNEL_FULL): " + to_string_c98(status));
    else
        error("Add Charlie debería devolver CHANNEL_FULL, devolvió " + to_string_c98(status));

    //==============================//
    //     DUPLICATE / NULL TESTS   //
    //==============================//
    subsection("🧩 Casos duplicados y nulos");

    status = general.addClient(&alice);
    if (status == ALREADY_IN_CHANNEL)
        success("Add Alice otra vez (esperado ALREADY_IN_CHANNEL)");
    else
        error("Add Alice otra vez: resultado inesperado " + to_string_c98(status));

    status = general.addClient(NULL);
    if (status == INVALID_CLIENT)
        success("Add NULL client rechazado correctamente");
    else
        error("Add NULL client debería devolver INVALID_CLIENT, devolvió " + to_string_c98(status));

    //==============================//
    //     VERIFY CLIENT LIST       //
    //==============================//
    subsection("📋 Verificar lista de clientes");

    std::vector<std::string> nicks = general.getNicknames();
    for (size_t i = 0; i < nicks.size(); ++i)
        info(" - " + nicks[i]);

    if (nicks.size() == 2)
        success("Número correcto de clientes en canal: 2");
    else
        error("Esperado 2 clientes, hay " + to_string_c98(nicks.size()));

    //==============================//
    //       BROADCAST TESTS        //
    //==============================//
    subsection("📢 Broadcast de mensajes");

    general.broadcast("Hello everyone!");
    if (alice.getReceivedMessages()[0] == "Hello everyone!" &&
        bob.getReceivedMessages()[0] == "Hello everyone!") {
        success("Mensaje recibido correctamente");
    } else {
        error("Mensaje recibido incorrecto");
    }
    
    //==============================//
    //   REMOVE + REBROADCAST TEST  //
    //==============================//
    subsection("➖ Eliminar clientes y volver a enviar broadcast");

    status = general.removeClient(&alice);
    success("Remove Alice: " + to_string_c98(status));

    general.broadcast("Second broadcast");
    if (bob.getReceivedMessages().size() == 2 && alice.getReceivedMessages().size() == 1)
        success("Broadcast tras eliminar OK (Alice no recibió más)");
    else
        error("Broadcast tras eliminar falló");

    //==============================//
    //     REMOVE INVALID CASES     //
    //==============================//
    subsection("⚠️ Casos inválidos al eliminar");

    status = general.removeClient(&alice);
    if (status == NOT_IN_CHANNEL)
        success("Remove Alice otra vez (esperado NOT_IN_CHANNEL)");
    else
        error("Remove Alice otra vez: resultado inesperado " + to_string_c98(status));

    status = general.removeClient(NULL);
    if (status == INVALID_CLIENT)
        success("Remove NULL client rechazado correctamente");
    else
        error("Remove NULL client: resultado inesperado " + to_string_c98(status));

    //==============================//
    //     EMPTY CHANNEL BROADCAST  //
    //==============================//
    subsection("🕳️ Broadcast en canal vacío");

    general.removeClient(&bob);
    general.broadcast("Should reach nobody");
    success("Broadcast en canal vacío no crasheó");

    if (bob.getReceivedMessages().size() == 2)
        success("Bob no recibió más mensajes tras salir");
    else
        error("Bob recibió mensajes tras salir (error)");

    //==============================//
    //       DESTRUCTION CHECK      //
    //==============================//
    subsection("🧹 Destructor / Limpieza de memoria");
    {
        Channel temp("#temp", 1);
        temp.addClient(&charlie);
        success("Canal temporal creado y cliente añadido");
    }
    success("Destructor del canal temporal ejecutado correctamente");

    separator("✅ TEST CHANNEL FINALIZADO");
    return (0);
}
