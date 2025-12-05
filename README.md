# 💬 IRC Project - 42 Málaga

## 🏗️ Overview

The goal of this project is to implement a **functional IRC (Internet Relay Chat) server**, following the classic protocol defined in [RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459).

Our server will be able to handle multiple simultaneous connections, interpret IRC commands (`NICK`, `JOIN`, `PRIVMSG`, etc.), and manage users and channels with the corresponding internal logic.

---

## 👥 Team Roles

| Person | Role | Main Files | Description |
|--------|------|------------|------------|
| 👤 **Noelia** | **Networking / Server Core** | `Server.hpp / Server.cpp` | Implements the TCP server: sockets, bind, listen, accept. Handles multiplexing (poll/select) for multiple clients. Provides `receiveMessage()` and `sendMessage()`. |
| 👤 **Juan** | **Parser / IRC Protocol** | `Parser.hpp / Parser.cpp`, `Command.hpp` | Converts text messages into `Command` structures. Implements `parse()` and a `dispatch()` connecting to server logic. |
| 👤 **Ana Zubieta** | **Application Logic (Users & Channels)** | `Client.hpp / Client.cpp`, `Channel.hpp / Channel.cpp`, `ServerLogic.hpp / ServerLogic.cpp` | Implements classes representing users, channels, and command execution logic (`NICK`, `JOIN`, `PRIVMSG`, etc.). Manages server state. |

---

## ⚙️ Project Architecture
```bash
+---------------------------+
| Client |
| (WeeChat / irssi / etc) |
+-------------+-------------+
|
v
+-------------+-------------+
| Server (Noelia) |
| accept(), poll(), send() |
+-------------+-------------+
|
v
+-------------+-------------+
| Parser (Juan) |
| parse() → Command |
+-------------+-------------+
|
v
+-------------+-------------+
| Logic (Ana Zubieta) |
| executeCommand(cmd, fd) |
| Manages users/channels |
+---------------------------+

Each layer is **modular and independent**, allowing parallel development and testing without dependency on others.

---

## 🔗 Module Communication

| From                  | To                 | Function / Example                                           |
|-----------------------|------------------|-------------------------------------------------------------|
| Server (Noelia)       | Parser (Juan)     | `rawMessage = receiveMessage(fd)`                           |
| Parser (Juan)         | Logic (Ana Zubieta) | `cmd = parse(rawMessage)` → `ServerLogic::executeCommand(cmd, clientFd)` |
| Logic (Ana Zubieta)   | Server (Noelia)   | `Server::sendMessage(fd, response)`                         |

---

## 📂 Directory Structure

```bash
includes/
├── parser/      # Juan - Parser / IRC Protocol
│   └── Parser.hpp
│
├── server/      # Noelia - Networking / Core
│   └── Server.hpp
│
└── logic/       # Ana Zubieta - Application Logic
    ├── Client.hpp
    ├── Channel.hpp
    └── ServerLogic.hpp

srcs/
├── parser/
│   └── Parser.cpp
├── server/
│   └── Server.cpp
└── logic/
    ├── Client.cpp
    ├── Channel.cpp
    └── ServerLogic.cpp

Makefile
main.cpp
README.md
CONTRIBUTING.md
```

## 🧩 Code Dependencies

- **Noelia (Server)**: does not depend on Parser or Logic.  
- **Juan (Parser)**: only depends on `Command.hpp`.  
- **Ana Zubieta (Logic)**: depends on `Command` and uses public Server functions to send messages (`sendMessage(fd, msg)`).

---

## 🧱 General IRC Server Flow

### 1️⃣ Startup
- `Server::run()` listens on the configured port (`bind`, `listen`).  
- Waits for events via `poll()` or `select()`.

### 2️⃣ Client Connection
- `Server::_acceptNewClient()` creates a new `Client` in `ServerLogic`.

### 3️⃣ Receiving Data
- `Server::receiveMessage(fd)` retrieves the raw message from the socket.

### 4️⃣ Parsing
- `Parser::parse(rawMessage)` returns a `Command` with name and parameters.

### 5️⃣ Logic
- `ServerLogic::executeCommand(cmd, clientFd)` executes the action.  

#### Example:
```cpp
if (cmd.name == "JOIN")
    _handleJoin(cmd, client);
```
### Response

- Logic sends text back via `Server::sendMessage(fd, "message")`.

### Disconnection

- If `recv()` returns 0, the client is removed and others are notified.

---

## 💡 Example of Real Flow

```bash
Client → "NICK ana"
Parser → Command{name="NICK", params=["ana"]}
Logic → assigns nickname to client
Server → sendMessage(fd, ":server 001 ana :Welcome to IRC!")
```
## 🧪 Module Testing

| Module            | How to Test                                      | Tools                        |
|------------------|-------------------------------------------------|------------------------------|
| Server (Noelia)   | Connect with telnet localhost <port> and send text | telnet, nc                   |
| Parser (Juan)     | Unit tests with strings: `parse("JOIN #channel")` | GoogleTest or asserts        |
| Logic (Ana Zubieta)| Simulate Command and Client directly           | Temporary main or local tests|

---

## 🚀 Team Guidelines

- Standardize includes: shared code in `includes/`.
- No namespaces: C++98 only (42 norm).
- Consistency: public methods must be defined in corresponding `.cpp`.
- Individual testing: each member should run their module independently.
- Progressive integration: first connect Server → Logic, then Parser.

---

## 🧭 Next Steps

| Phase | Task                                      | Responsible       |
|-------|------------------------------------------|-----------------|
| 1️⃣    | Implement Server with poll() and sockets  | Noelia            |
| 2️⃣    | Implement Parser::parse() and dispatch()  | Juan              |
| 3️⃣    | Implement Client, Channel, and ServerLogic| Ana Zubieta       |
| 4️⃣    | Connect Server → Parser → Logic           | All               |
| 5️⃣    | Joint testing and final integration       | All               |

---

## ✨ Authors

- 👤 Noelia — Networking & Server Core  
- 👤 Juan — Parser & Protocol  
- 👤 Ana Zubieta — Application Logic (Users & Channels)
