# 🤝 Guía de Contribución - Proyecto IRC (42 Málaga)

> Este documento explica las normas de trabajo, formato de commits, estructura de ramas y proceso de colaboración para todos los miembros del equipo.

---

## 🧭 Flujo de trabajo con Git

### 🏁 Ramas principales

| Rama | Descripción | Quién la toca |
|------|--------------|---------------|
| `main` | Versión **estable** del servidor. Solo merges revisados. | 🔒 Nadie sin revisión |
| `develop` | Rama de integración. Aquí se fusionan las features de cada persona. | Todos (con PR) |
| `feature/networking` | Persona 1 (Server - Networking) | 👤 Persona 1 |
| `feature/parser` | Persona 2 (Parser / Protocol) | 👤 Persona 2 |
| `feature/logic` | Ana (Lógica - Users & Channels) | 👤 Ana |

---

## 🌿 Creación de ramas

Cada desarrollador trabaja **solo en su rama** y no modifica las de otros.

Para crear o actualizar una rama:

```bash
git checkout develop
git pull origin develop
git checkout -b feature/<nombre>
git push -u origin feature/<nombre>
```

Ejemplo (Ana):

```bash
git checkout develop
git checkout -b feature/logic
git push -u origin feature/logic
```

## 💬 Commits — Normas de estilo

Cada commit debe ser claro, corto y específico.
Usa prefijos entre corchetes [] para identificar el módulo afectado.

### 📌 Formato recomendado

```bash
[Tema] Descripción corta del cambio
```

### 📋 Ejemplos

```bash
[Logic] Implement Channel class and member tracking
[Parser] Fix split() error for trailing params
[Server] Add poll() loop and client accept
[Core] Update Makefile and include paths
```

### ❌ Evita mensajes como:

```bash
update
arreglos
cosas nuevas
push final
```

## 🔄 Flujo de integración

### 1️⃣ Desarrollo local

```bash
# 1. Trabaja en tu rama
git checkout feature/logic

# 2. Haz commits pequeños y descriptivos
git add src/Client.cpp
git commit -m "[Logic] Add Client implementation"

# 3. Sube tus cambios
git push
```

### 2️⃣ Actualiza tu rama con develop

Antes de abrir un Pull Request (PR), sincroniza tu rama con develop:

```bash
git checkout develop
git pull origin develop
git checkout feature/logic
git merge develop
# Si hay conflictos, los resuelves aquí
```

### 3️⃣ Pull Request (PR)

Cuando tengas algo estable o completo:

1. Abre un Pull Request desde tu rama → develop.

2. Asigna a otro compañero para revisión.

3. Espera aprobación antes del merge.

Ejemplo:

```bash
Merge feature/logic → develop
Descripción: “Añadida gestión de usuarios y canales (Client/Channel)”
```

## 🔍 Revisión de código

Antes de aprobar un PR, revisa:

- Que compile sin warnings (make re).

- Que no haya leaks (valgrind ./ircserv).

- Que los nombres de funciones y clases sean claros.

- Que los commits sean atómicos y limpios.

- Que no haya código comentado o debugging temporal.

## 🧪 Testing por módulos

```bash
Módulo	Qué probar	Cómo testear
Server (P1)	Conexión y lectura TCP	telnet localhost <port>
Parser (P2)	Traducción raw → Command	Unit tests con strings
Logic (P3)	Gestión de usuarios/canales	main temporal simulando comandos
```

## 🧹 Limpieza de ramas (cuando se mergea)

Cuando una rama feature/* se fusiona con develop, se puede eliminar.

```bash
git branch -d feature/logic
git push origin --delete feature/logic
```

⚠️ No elimines ramas que no sean tuyas.

## 💡 Recomendaciones de estilo de código

- C++98 obligatorio (norma 42).

- No usar auto, nullptr, ni inicialización moderna.

- Respetar nombres coherentes:

    Clases → PascalCase → ServerLogic, Client

    Métodos → camelCase → getNickname()

    Variables → _snake_case privado → _nickname, _fd

- Incluye #ifndef / #define / #endif en todos los .hpp.

## 🧰 Ejemplo de ciclo completo (Ana)

```bash
# Crear rama
git checkout -b feature/logic develop

# Editar código
vim src/Client.cpp

# Commit
git add includes/Client.hpp src/Client.cpp
git commit -m "[Logic] Implement Client and Channel management"

# Push
git push

# Mantener actualizada la rama

git pull origin develop
git merge develop
```

# Crear PR en GitHub: feature/logic → develop

## 🧱 Estructura del proyecto

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

## 🚀 Consejos para evitar conflictos

- Pull diario: antes de empezar a programar, siempre git pull origin develop.

- Commits pequeños: no esperes al final del día.

- Revisión cruzada: siempre otro compañero revisa tu PR.

- Conflictos: resuélvelos localmente, nunca desde la web.

## ✨ Equipo

- 👤 Persona 1: Networking / Core del servidor

- 👤 Persona 2: Parser / Protocolo IRC

- 👤 Ana Zubieta: Lógica de aplicación (Users & Channels)

## 📚 Referencias útiles

- RFC 1459 - IRC Protocol

- Beej’s Guide to Network Programming

- 42 Málaga - Guía de Proyectos en C++

- Git Flow explicado sencillo


---

👉 **Recomendación práctica:**  
Después de añadir este archivo, haz:

```bash
git add CONTRIBUTING.md
git commit -m "[Docs] Add contributing guide for team workflow"
git push
```