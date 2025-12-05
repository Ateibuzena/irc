# 🤝 Contribution Guide - IRC Project (42 Málaga)

> This document explains the team workflow, commit format, branch structure, and collaboration process for all team members.

---

## 🧭 Git Workflow

### 🏁 Main Branches

| Branch | Description | Who works on it |
|--------|------------|----------------|
| `main` | **Stable** server version. Only reviewed merges. | 🔒 Nobody without review |
| `develop` | Integration branch. Feature branches are merged here. | All (via PR) |
| `feature/networking` | Noelia (Server - Networking / Core) | 👤 Noelia |
| `feature/parser` | Juan (Parser / Protocol) | 👤 Juan |
| `feature/logic` | Ana Zubieta (Application Logic - Users & Channels) | 👤 Ana Zubieta |

---

## 🌿 Branch Creation

Each developer works **only on their own branch** and does not modify others’ branches.

To create or update a branch:

```bash
git checkout develop
git pull origin develop
git checkout -b feature/<name>
git push -u origin feature/<name>
```

Example (Ana Zubieta):

```bash
git checkout develop
git checkout -b feature/logic
git push -u origin feature/logic
```
## 💬 Commits — Style Guidelines

Commits should be clear, concise, and specific.
Use square bracket prefixes [] to indicate the affected module.

## 📌 Recommended Format
[Module] Short description of the change

## 📋 Examples
[Logic] Implement Client and Channel classes
[Parser] Fix split() error for trailing params
[Server] Add poll() loop and client accept
[Core] Update Makefile and include paths

## ❌ Avoid messages like:
update
fixes
new stuff
final push

## 🔄 Integration Flow
### 1️⃣ Local Development
#### 1. Work on your feature branch
git checkout feature/logic

#### 2. Make small, descriptive commits
git add src/Client.cpp
git commit -m "[Logic] Add Client implementation"

#### 3. Push your changes
git push

### 2️⃣ Sync Your Branch with develop

Before opening a Pull Request (PR), synchronize your branch:
```bash
git checkout develop
git pull origin develop
git checkout feature/logic
git merge develop
```
#### Resolve conflicts if any

### 3️⃣ Pull Request (PR)

When your feature is stable or complete:

    - Open a Pull Request from your branch → develop.

    - Assign a teammate for review.

    - Wait for approval before merging.

### Example:

Merge feature/logic → develop
Description: "Added user and channel management (Client/Channel)"

## 🔍 Code Review

Before approving a PR, check:

    - Compiles without warnings (make re).

    - No memory leaks (valgrind ./ircserv).

    - Clear function and class names.

    - Atomic, clean commits.

    - No commented-out or temporary debug code.

## 🧪 Module Testing

```bash
Module	What to test	How to test
Server (Noelia)	TCP connection and reading	telnet localhost <port>
Parser (Juan)	Raw → Command translation	Unit tests with strings
Logic (Ana Zubieta)	User/channel management	Temporary main simulating commands
```

## 🧹 Branch Cleanup (after merge)

Once a feature/* branch is merged into develop, you can delete it:

    git branch -d feature/logic
    git push origin --delete feature/logic


## ⚠️ Do not delete branches that are not yours.

## 💡 Code Style Recommendations

Use C++98 (42 norm).

Avoid auto, nullptr, and modern initialization.

Follow naming conventions:

    - Classes → PascalCase → ServerLogic, Client
    - Methods → camelCase → getNickname()
    - Private variables → _snake_case → _nickname, _fd

Include #ifndef / #define / #endif in all .hpp files.

## 🧰 Example Full Cycle (Ana Zubieta)
#### Create branch
```bash
git checkout -b feature/logic develop

#### Edit code
```bash
vim src/Client.cpp
```

#### Commit
```bash
git add includes/Client.hpp src/Client.cpp
git commit -m "[Logic] Implement Client and Channel management"
```

#### Push
```bash
git push
```

#### Keep branch updated
```bash
git pull origin develop
git merge develop
```

Create PR on GitHub: feature/logic → develop
## 🧱 Project Structure

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

## 🚀 Tips to Avoid Conflicts

    - Pull daily: always git pull origin develop before coding.

    - Small commits: do not wait until the end of the day.

    - Peer review: another teammate always reviews your PR.

    - Conflicts: resolve them locally, never via GitHub web editor.

## ✨ Team

### 👤 Noelia: Networking / Server Core

### 👤 Juan: Parser / IRC Protocol

### 👤 Ana Zubieta: Application Logic (Users & Channels)

## 📚 Useful References

    - RFC 1459 - IRC Protocol

    - 42 Málaga - C++ Project Guide

    - Git Flow simplified