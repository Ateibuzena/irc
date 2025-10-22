#include "Server.hpp"
#include "DummyHandler.hpp"

int main() {
    DummyHandler handler;
    Server srv(6667, &handler);
    return srv.run();
}
//nc -C 127.0.0.1 6667
//nc -C -q 0 127.0.0.1 6667 para que funcione ctrl+D
//hexaChat