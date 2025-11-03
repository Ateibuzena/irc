#include "../includes/server/Server.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return (1);
    }

    initErrorMessages();
    initReplayMessages();

    Server srv(atoi(argv[1]), argv[2]);

    int status = srv.run();
    return (status);
}
//nc -C 127.0.0.1 6667
//nc -C -q 0 127.0.0.1 6667 para que funcione ctrl+D
//hexaChat