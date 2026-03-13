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

void CommandDispatcher::checkRegister(int fd, Client *client, Server &server)
{
    if (client->tryRegister())
    {
        server.sendMsgToClient(fd, "Welcome to the server irc\r\n");
        cout << "Client " << client->getUserName() << " authenticated" << endl;
    }
}

void CommandDispatcher::cmdPass(std::istringstream &ss, Server &server, int fd)
{
    string password;
    ss >> password;

    Client *client = server.getClient(fd);
    if (!client)
        return;

    if (password.empty())
        server.sendMsgToClient(fd, "ERROR :Empty password\r\n");
    else if (password == server.getPassword())
    {
        client->setHasProvidedPass(true);
        checkRegister(fd, client, server);
    }
    else
        server.sendMsgToClient(fd, "ERROR :Password incorrect\r\n"); 
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
    checkRegister(fd, client, server);
}

void CommandDispatcher::cmdUser(std::istringstream &ss, Server &server, int fd)
{
    string username;
    string mode;
    string unused;
    string realname;

    ss >> username >> mode >> unused;

    std::getline(ss, realname);

    Client *client = server.getClient(fd);
    if (!client)
        return;

    if (username.empty())
    {
        server.sendMsgToClient(fd, "ERROR: No username\r\n");
        return;
    }

    client->setUserName(username);
    checkRegister(fd, client, server);
}

void CommandDispatcher::cmdJoin(std::istringstream &ss, Server &server, int fd)
{
    (void)ss;
    (void)server;
    (void)fd;
}

void CommandDispatcher::cmdPrivmsg(std::istringstream &ss, Server &server, int fd)
{
    (void)ss;
    (void)server;
    (void)fd;
}

void CommandDispatcher::cmdQuit(std::istringstream &ss, Server &server, int fd)
{
    (void)ss;
    (void)server;
    (void)fd;
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
    else if (command == "USER")
        cmdUser(ss, server, fd);
    else if (command == "JOIN")
        cmdJoin(ss, server, fd);
    else if (command == "PRIVMSG")
        cmdPrivmsg(ss, server, fd);
    else if (command == "QUIT")
        cmdQuit(ss, server, fd);
    else
        server.sendMsgToClient(fd, "Invalid command\r\n");
}