#include "Server.hpp"

int main(void)
{
    Server server(8080);

    server.loop();

    return (0);
}
