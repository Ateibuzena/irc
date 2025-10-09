# 💬 Proyecto IRC - 42 Málaga

## 🏗️ Descripción general

El objetivo de este proyecto es implementar un **servidor IRC (Internet Relay Chat)** funcional, siguiendo el protocolo clásico definido en el [RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459).

Nuestro servidor será capaz de aceptar múltiples conexiones simultáneas, interpretar comandos IRC (`NICK`, `JOIN`, `PRIVMSG`, etc.), y gestionar usuarios y canales con su correspondiente lógica interna.

---

## 👥 División del equipo

| Persona | Rol | Archivos principales | Descripción |
|----------|-----|----------------------|--------------|
| 👤 **Persona 1** | **Networking / Core del servidor** | `Server.hpp / Server.cpp` | Implementa el servidor TCP: sockets, bind, listen, accept. Maneja la multiplexación (poll/select) para múltiples clientes. Provee funciones `receiveMessage()` y `sendMessage()`. |
| 👤 **Persona 2** | **Parser / Protocolo IRC** | `Parser.hpp / Parser.cpp`, `Command.hpp` | Convierte los mensajes de texto en estructuras `Command`. Implementa `parse()` y un `dispatch()` que conecta con la lógica del servidor. |
| 👤 **Persona 3 (Ana)** | **Lógica de aplicación (Users & Channels)** | `Client.hpp / Client.cpp`, `Channel.hpp / Channel.cpp`, `ServerLogic.hpp / ServerLogic.cpp` | Implementa las clases que representan usuarios, canales y la lógica de ejecución de comandos (`NICK`, `JOIN`, `PRIVMSG`, etc.). Gestiona el estado del servidor. |

---

## ⚙️ Arquitectura del proyecto
```bash
+---------------------------+
| Client |
| (WeeChat / irssi / etc) |
+-------------+-------------+
|
v
+-------------+-------------+
| Server (P1) |
| accept(), poll(), send() |
+-------------+-------------+
|
v
+-------------+-------------+
| Parser (P2) |
| parse() → Command |
+-------------+-------------+
|
v
+-------------+-------------+
| Logic (P3 - Ana) |
| executeCommand(cmd, fd) |
| gestiona usuarios/canales |
+---------------------------+
```

Cada capa es **modular e independiente**, lo que permite trabajar en paralelo y probar sin depender de las demás.

---

## 🔗 Comunicación entre módulos

| De | A | Función / Ejemplo |
|----|---|--------------------|
| Server (P1) | Parser (P2) | `rawMessage = receiveMessage(fd)` |
| Parser (P2) | Logic (P3) | `cmd = parse(rawMessage)` → `ServerLogic::executeCommand(cmd, clientFd)` |
| Logic (P3) | Server (P1) | `Server::sendMessage(fd, response)` |

---

## 📂 Estructura de directorios
```bash
includes/
├── Command.hpp # Estructura común (Parser ↔ Logic)
│
├── server/ # Persona 1 - Networking / Core
│ └── Server.hpp
│
├── parser/ # Persona 2 - Parser / Protocolo
│ └── Parser.hpp
│
└── logic/ # Persona 3 - Lógica de aplicación (Ana)
├── Client.hpp
├── Channel.hpp
└── ServerLogic.hpp

src/
├── server/
│ └── Server.cpp
│
├── parser/
│ └── Parser.cpp
│
└── logic/
├── Client.cpp
├── Channel.cpp
└── ServerLogic.cpp

Makefile
main.cpp
README.md
CONTRIBUTING.md
```

---

## 🧩 Dependencias de código

- **Persona 1 (Server)**: no depende de Parser ni de Logic.
- **Persona 2 (Parser)**: solo depende de `Command.hpp`.
- **Persona 3 (Logic)**: depende de `Command`, y usa funciones públicas de `Server` para enviar mensajes (`sendMessage(fd, msg)`).

---

## 🧱 Flujo general del servidor IRC

1. **Arranque**
   - `Server::run()` escucha en el puerto configurado (`bind`, `listen`).
   - Espera eventos mediante `poll()` o `select()`.

2. **Conexión de cliente**
   - `Server::_acceptNewClient()` crea un nuevo `Client` en `ServerLogic`.

3. **Recepción de datos**
   - `Server::receiveMessage(fd)` obtiene el mensaje raw del socket.

4. **Parsing**
   - `Parser::parse(rawMessage)` devuelve un `Command` con nombre y parámetros.

5. **Lógica**
   - `ServerLogic::executeCommand(cmd, clientFd)` ejecuta la acción.
   - Ejemplo:
     ```cpp
     if (cmd.name == "JOIN")
         _handleJoin(cmd, client);
     ```

6. **Respuesta**
   - La lógica envía texto de vuelta con `Server::sendMessage(fd, "mensaje")`.

7. **Desconexión**
   - Si `recv()` devuelve 0, el cliente se elimina y se notifica al resto.

---

## 💡 Ejemplo de flujo real
```bash
Cliente → "NICK ana"
Parser → Command{name="NICK", params=["ana"]}
Logic → asigna nickname al cliente
Server → sendMessage(fd, ":server 001 ana :Bienvenida a IRC!")
```

---

```bash
🧪 Testing por módulos
Módulo	Cómo testear	Herramientas
Server (P1)	Conectarse con telnet localhost <port> y enviar texto.	telnet, nc
Parser (P2)	Unit tests con strings: parse("JOIN #canal")	GoogleTest o asserts
Logic (P3)	Simular Command y Client directamente.	main temporal o tests locales
```

## 🚀 Normas de trabajo en equipo

- Estandarización de includes: todo lo compartido en includes/.

- Namespace prohibido: código C++98 (norma 42).

- Consistencia: los métodos públicos deben estar definidos en el .cpp correspondiente.

- Pruebas individuales: cada persona debe poder ejecutar su parte sin depender del resto.

- Integración progresiva: se hará primero conexión P1–P3, luego se une el Parser.


## 🧭 Próximos pasos del equipo

```bash
| Fase | Tarea                                           | Responsable         |
| ---- | ----------------------------------------------- | ------------------- |
| 1️⃣  | Implementar `Server` con poll() y sockets       | Persona 1           |
| 2️⃣  | Implementar `Parser::parse()` y `dispatch()`    | Persona 2           |
| 3️⃣  | Implementar `Client`, `Channel` y `ServerLogic` | **Ana (Persona 3)** |
| 4️⃣  | Conectar `Server` → `Parser` → `Logic`          | Todos               |
| 5️⃣  | Testing conjunto e integración final            | Todos               |
```

## ✨ Autores

- Persona 1: Noelia — Networking & Core

- Persona 2: Juan — Parser & Protocol

- Persona 3: Ana — Lógica de aplicación (Users & Channels)
