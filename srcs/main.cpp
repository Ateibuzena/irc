#include "../includes/server/Server.hpp"
#include <climits>
#include <cerrno>
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return (1);
    }

    char *end = NULL;
    errno = 0;
    long port = std::strtol(argv[1], &end, 10);

    if (*end != '\0')
    {
        std::cerr << "Invalid port (not a number)" << std::endl;
        return 1;
    }

    if ((errno == ERANGE && (port == LONG_MAX || port == LONG_MIN)))
    {
        std::cerr << "Invalid port (out of range for long)" << std::endl;
        return 1;
    }

    if (port < 1024 || port > 65535)
    {
        std::cerr << "Invalid port (must be between 1024 and 65535)" << std::endl;
        return 1;
    }

    std::cout << "Using port = " << port << "\n";
    
    
    initErrorMessages();
    initReplayMessages();

    Server srv(atoi(argv[1]), argv[2]);
    int status = srv.run();
    return (status);
}

//nc -C 127.0.0.1 6667
//nc -C -q 0 127.0.0.1 6667 so Ctrl+D works
//hexaChat
//valgrind --leak-check=full --show-leak-kinds=all ./ircserv 6667 1
//valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./ircserv 6667 1 2>val.txt
