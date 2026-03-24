#include "CommandDispatcher.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include <sstream>
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
    Client *client = server.getClient(fd);
    if (!client)
        return;

    string channelName, key;
    ss >> channelName >> key;

    if (channelName.empty())
    {
        server.sendMsgToClient(fd, "461 " + client->getNickName() + " JOIN :Not enough parameters\r\n");
        return;
    }

    // Los canales en IRC deben empezar por #
    if (channelName[0] != '#' && channelName[0] != '&')
    {
        channelName = "#" + channelName; // Auto forzar '#' si el cliente no lo pone, solo para ayudar (nc)
    }

    std::map<string, Channel> &channels = server.getChannels();
    bool isNewChannel = false;
    
    if (channels.find(channelName) == channels.end())
    {
        channels[channelName] = Channel(channelName);
        isNewChannel = true;
    }

    Channel &channel = channels[channelName];

    if (channel.isClient(fd))
        return;

    channel.addClient(fd);
    client->joinChannel(channelName);

    if (isNewChannel)
        channel.addOperator(fd);

    string prefix = ":" + client->getNickName() + "!" + client->getUserName() + "@localhost";
    string joinMsg = prefix + " JOIN :" + channelName + "\r\n";
    
    std::vector<int> mems = channel.getClients();
    for (size_t i = 0; i < mems.size(); ++i)
        server.sendMsgToClient(mems[i], joinMsg);

    if (!channel.getTopic().empty())
        server.sendMsgToClient(fd, "332 " + client->getNickName() + " " + channelName + " :" + channel.getTopic() + "\r\n");

    string namesList = "";
    for (size_t i = 0; i < mems.size(); ++i)
    {
        Client *c = server.getClient(mems[i]);
        if (c)
        {
            if (channel.isOperator(mems[i]))
                namesList += "@";
            namesList += c->getNickName() + " ";
        }
    }
    
    server.sendMsgToClient(fd, "353 " + client->getNickName() + " = " + channelName + " :" + namesList + "\r\n");
    server.sendMsgToClient(fd, "366 " + client->getNickName() + " " + channelName + " :End of /NAMES list.\r\n");
}

void CommandDispatcher::cmdPrivmsg(std::istringstream &ss, Server &server, int fd)
{
    Client *client = server.getClient(fd);
    if (!client)
        return;

    string target, message;
    ss >> target;

    std::getline(ss, message);
    
    if (target.empty() || message.empty() || message == " " || message == " :")
    {
        server.sendMsgToClient(fd, "411 " + client->getNickName() + " :No recipient given or no text to send\r\n");
        return;
    }

    if (!message.empty() && message[0] == ' ')
        message.erase(0, 1);
    if (!message.empty() && message[0] == ':')
        message.erase(0, 1);

    string prefix = ":" + client->getNickName() + "!" + client->getUserName() + "@localhost";

    if (target[0] == '#' || target[0] == '&')
    {
        Channel *channel = server.getChannel(target);
        if (!channel)
        {
            server.sendMsgToClient(fd, "403 " + client->getNickName() + " " + target + " :No such channel\r\n");
            return;
        }
        if (!channel->isClient(fd))
        {
            server.sendMsgToClient(fd, "404 " + client->getNickName() + " " + target + " :Cannot send to channel\r\n");
            return;
        }
        string fullMsg = prefix + " PRIVMSG " + target + " :" + message + "\r\n";
        std::vector<int> channelClients = channel->getClients();
        for (size_t i = 0; i < channelClients.size(); ++i)
        {
            if (channelClients[i] != fd)
            {
                server.sendMsgToClient(channelClients[i], fullMsg);
            }
        }
    }
    else
    {
        bool found = false;
        std::map<int, Client>& clients = server.getClients();
        std::map<int, Client>::iterator it;
        
        for (it = clients.begin(); it != clients.end(); ++it)
        {
            if (it->second.getNickName() == target)
            {
                found = true;
                string fullMsg = prefix + " PRIVMSG " + target + " :" + message + "\r\n";
                server.sendMsgToClient(it->first, fullMsg);
                break;
            }
        }
        if (!found)
            server.sendMsgToClient(fd, "401 " + client->getNickName() + " " + target + " :No such nick/channel\r\n");
    }
}

void CommandDispatcher::cmdQuit(std::istringstream &ss, Server &server, int fd)
{
    Client *client = server.getClient(fd);
    if (!client)
        return;

    string reason;
    std::getline(ss, reason);

    if (!reason.empty() && reason[0] == ' ')
        reason.erase(0, 1);
    if (!reason.empty() && reason[0] == ':')
        reason.erase(0, 1);

    if (reason.empty())
        reason = "Client Quit";

    string prefix = ":" + client->getNickName() + "!" + client->getUserName() + "@localhost";
    string quitMsg = prefix + " QUIT :Quit: " + reason + "\r\n";

    std::vector<int> notifiedFds;
    std::vector<string> channels = client->getChannels();

    for (size_t i = 0; i < channels.size(); ++i)
    {
        Channel *ch = server.getChannel(channels[i]);
        if (ch)
        {
            std::vector<int> mems = ch->getClients();
            for (size_t j = 0; j < mems.size(); ++j)
            {
                int toFd = mems[j];
                if (toFd != fd)
                {
                    if (std::find(notifiedFds.begin(), notifiedFds.end(), toFd) == notifiedFds.end())
                    {
                        notifiedFds.push_back(toFd);
                        server.sendMsgToClient(toFd, quitMsg);
                    }
                }
            }
            ch->removeClient(fd);
            ch->removeOperator(fd);
            ch->removeInvite(fd);
        }
    }

    server.sendMsgToClient(fd, "ERROR :Closing Link: localhost (" + reason + ")\r\n");
    server.disconnectClient(fd);
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