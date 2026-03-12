#include "CommandDispatcher.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "sstream"
#include <algorithm>

CommandDispatcher::CommandDispatcher()
{

}

CommandDispatcher::~CommandDispatcher()
{

}

void CommandDispatcher::cmdPass(std::istringstream &ss, Server &server, int fd)
{
        string password;
        ss >> password;

        Client *client = server.getClient(fd);
        if (!client)
            return;

        if (password == server.getPassword())
        {
            client->setHasProvidedPass(true);
            cout << "Client " << fd << " authenticated" << endl;
        }
        else
        {
            server.sendMsgToClient(fd, "ERROR :Password incorrect\r\n");
        }   
}

void CommandDispatcher::cmdNick(std::istringstream &ss, Server &server, int fd)
{
        string nick;
        ss >> nick;

        Client *client = server.getClient(fd);
        if (!client)
            return;

        if (nick.empty())
        {
            server.sendMsgToClient(fd, "ERROR :No nickname\r\n");
            return;
        }

        client->setNickName(nick);

        cout << "Client " << fd << " nick is " << nick << endl;
}


void CommandDispatcher::executeCommand(Server &server, int fd, const string &message)
{
    std::istringstream ss(message);
    string command;
    ss >> command;

    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    if (command == "PASS")
        cmdPass(ss, server, fd);
    else if (command == "NICK")
        cmdNick(ss, server, fd);
    

}