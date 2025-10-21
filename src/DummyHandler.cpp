#include "DummyHandler.hpp"
#include <string>

// util simple: case-insensitive prefix
static bool startsWithI(const std::string& s, const char* kw) {
    std::string::size_type i = 0;
    while (kw[i] && i < s.size()) {
        char a = s[i], b = kw[i];
        if ('a' <= a && a <= 'z') a -= 32; // to upper
        if ('a' <= b && b <= 'z') b -= 32;
        if (a != b) return false;
        ++i;
    }
    return kw[i] == '\0';
}

// recorta espacios iniciales (muy básico)
static void ltrim(std::string& x) {
    while (!x.empty() && (x[0] == ' ' || x[0] == '\t')) x.erase(0, 1);
}

void DummyHandler::onLine(int fd, const std::string& line, Server& server) {
    // Línea sin CRLF (la red ya los quitó); no tocar 'line' original por si lo quieres loguear
    std::string s = line;

    if (startsWithI(s, "PING")) {
        // PING <token opcional>
        std::string token;
        if (s.size() > 4) {
            token = s.substr(4);
            ltrim(token);
        }
        server.queueSend(fd, token.empty() ? "PONG" : ("PONG " + token));
        return;
    }

    if (startsWithI(s, "QUIT")) {
        server.queueSend(fd, ":server NOTICE * :Bye!");
        // Si quieres cerrar aquí:
        // server.closeClient(fd);
        return;
    }

    // Por defecto: eco
    server.queueSend(fd, "echo: " + s);
}
