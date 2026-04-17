#include "Server.hpp"
#include "Utils.hpp"

typedef std::string string;
using std::cout;
using std::endl;

int main(int argc, char **argv)
{
    std::signal(SIGQUIT, handleSignal);
    std::signal(SIGINT, handleSignal);
    std::signal(SIGPIPE, SIG_IGN);
    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << endl;
        return 1;
    }

    string portStr = argv[1];
    string password = argv[2];

    if (!checkArgs(portStr, password))
        return 1;

    int port = std::atoi(portStr.c_str());
    Server server(port, password);
    server.start();

    return 0;
}