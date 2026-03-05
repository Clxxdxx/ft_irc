#include <iostream>
#include <cstdlib>
#include <cctype>
#include "Server.hpp"

typedef std::string string;
using std::cout;
using std::endl;

bool isNumber(const string &str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!std::isdigit(static_cast<unsigned char>(str[i])))
            return false;
    }
    return true;
}

bool checkArgs(const string &portStr, const string &password)
{
    if (!isNumber(portStr))
    {
        std::cerr << "Error: Port must be numeric" << std::endl;
        return false;
    }

    int portInt = std::atoi(portStr.c_str());

    if (portInt < 1024 || portInt > 49151)
    {
        std::cerr << "Error: Port must be between 1024 and 49151" << std::endl;
        return false;
    }

    if (password.empty())
    {
        std::cerr << "Error: Password cannot be empty" << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    std::signal(SIGQUIT, handleSignal);
    std::signal(SIGINT, handleSignal);
    std::signal(SIGPIPE, SIG_IGN);
    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    string portStr = argv[1];
    string password = argv[2];

    if (!checkArgs(portStr, password))
        return 1;

    int port = std::atoi(portStr.c_str());
    Server server(port);
    server.start();
    std::cout << "Todo bien" << std::endl;
    return 0;
}