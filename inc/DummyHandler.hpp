#ifndef DUMMY_HANDLER_HPP
#define DUMMY_HANDLER_HPP

#include "Server.hpp"

// Handler de prueba (Persona 2 “fake”):
// - PING <x>  -> PONG <x>
// - QUIT      -> NOTICE y (opcional) cerrar
// - resto     -> echo: <línea>
class DummyHandler : public ILineHandler {
public:
    virtual ~DummyHandler() {}
    void onLine(int fd, const std::string& line, Server& server);
};

#endif
