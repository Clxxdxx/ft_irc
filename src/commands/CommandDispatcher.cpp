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

    std::map<int, Client> &clients = server.getClients();
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.getNickName() == nick) {
            if (nick.empty())
            {
                server.sendMsgToClient(fd, "ERROR: empty nickname\r\n");
                return;
            }
            
            server.sendMsgToClient(fd, "ERROR: Nickname " + nick + " is already in use\r\n");
            return;
        }
    }

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
    if (!client || !client->isRegistered())
    {
        server.sendMsgToClient(fd, "451 :You have not registered\r\n");
        return;
    }

    string channelName, key;
    ss >> channelName >> key;

    if (channelName.empty())
    {
        server.sendMsgToClient(fd, "461 " + client->getNickName() + " JOIN :Not enough parameters\r\n");
        return;
    }

    if (channelName[0] != '#' && channelName[0] != '&')
        channelName = "#" + channelName;

    std::map<string, Channel> &channels = server.getChannels();
    bool isNewChannel = (channels.find(channelName) == channels.end());
    
    if (isNewChannel)
        channels[channelName] = Channel(channelName);

    Channel &channel = channels[channelName];

    if (channel.isClient(fd))
        return;

    if (!isNewChannel)
    {
        if (channel.isInviteOnly() && !channel.isInvited(fd))
        {
            server.sendMsgToClient(fd, "473 " + client->getNickName() + " " + channelName + " :Cannot join channel (+i)\r\n");
            return;
        }
        if (channel.hasPassword() && key != channel.getPassword())
        {
            server.sendMsgToClient(fd, "475 " + client->getNickName() + " " + channelName + " :Cannot join channel (+k)\r\n");
            return;
        }
        if (channel.hasUserLimit() && channel.getClientCount() >= channel.getUserLimit())
        {
            server.sendMsgToClient(fd, "471 " + client->getNickName() + " " + channelName + " :Cannot join channel (+l)\r\n");
            return;
        }
    }

    channel.addClient(fd);
    client->joinChannel(channelName);

    if (isNewChannel)
        channel.addOperator(fd);
    
    if (channel.isInvited(fd))
        channel.removeInvite(fd);

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
            if (channel.isOperator(mems[i])) namesList += "@";
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

    if (!client->isRegistered())
    {
        server.sendMsgToClient(client->getFd(), "User must be authenticated\r\n");
        return;
    }
    
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

void CommandDispatcher::cmdPart(std::istringstream &ss, Server &server, int fd) {
    string channelName;
    string reason;
    ss >> channelName;
    std::getline(ss, reason); 

    Client *client = server.getClient(fd);
    Channel *chan = server.getChannel(channelName);

    if (!chan) 
        return server.sendMsgToClient(fd, "403 " + channelName + " :No such channel\r\n");
    if (!chan->isClient(fd))
        return server.sendMsgToClient(fd, "442 " + channelName + " :You're not on that channel\r\n");

    if (!reason.empty() && reason[0] == ' ') reason.erase(0, 1);
    if (!reason.empty() && reason[0] == ':') reason.erase(0, 1);

    string prefix = ":" + client->getNickName() + "!" + client->getUserName() + "@localhost";
    string msg = prefix + " PART " + channelName;
    
    if (!reason.empty()) {
        msg += " :" + reason;
    }
    msg += "\r\n";
    
    std::vector<int> members = chan->getClients();
    for (size_t i = 0; i < members.size(); i++)
        server.sendMsgToClient(members[i], msg);

    
    cout << "Saliendo de " << channelName << endl;
    chan->removeClient(fd);
    client->leaveChannel(channelName);
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

    //server.sendMsgToClient(fd, "ERROR :Closing Link: localhost (" + reason + ")\r\n");
    server.disconnectClient(fd);
}

void CommandDispatcher::cmdKick(std::istringstream &ss, Server &server, int fd) {
    string channelName, targetNick, reason;
    ss >> channelName >> targetNick;
    std::getline(ss, reason);

    Client *ref = server.getClient(fd);
    Channel *chan = server.getChannel(channelName);

    if (!chan) 
        return server.sendMsgToClient(fd, channelName + " :No such channel\r\n");
    if (!chan->isClient(fd)) 
        return server.sendMsgToClient(fd, channelName + " :You're not on that channel\r\n");
    if (!chan->isOperator(fd)) 
        return server.sendMsgToClient(fd, channelName + " :You're not channel operator\r\n");

    int targetFd = -1;
    std::map<int, Client> &clients = server.getClients();
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.getNickName() == targetNick) {
            targetFd = it->first;
            break;
        }
    }

    if (targetFd == -1 || !chan->isClient(targetFd))
        return server.sendMsgToClient(fd, targetNick + " " + channelName + " :They aren't on that channel\r\n");

    if (!reason.empty() && reason[0] == ' ')
        reason.erase(0, 1);
    if (!reason.empty() && reason[0] == ':')
        reason.erase(0, 1);

    string finalReason;
    if (reason.empty()) {
        finalReason = "Kicked by operator";
    } else {
        finalReason = reason;
    }

    string prefix = ":" + ref->getNickName() + "!" + ref->getUserName() + "@localhost";
    string msg = prefix + " KICK " + channelName + " " + targetNick + " :" + finalReason + "\r\n";

    std::vector<int> members = chan->getClients();
    for (size_t i = 0; i < members.size(); i++)
        server.sendMsgToClient(members[i], msg);

    chan->removeClient(targetFd);
    server.getClient(targetFd)->leaveChannel(channelName);
}

void CommandDispatcher::cmdTopic(std::istringstream &ss, Server &server, int fd) {
    string channelName, newTopic;
    ss >> channelName;
    std::getline(ss, newTopic);

    Channel *chan = server.getChannel(channelName);
    if (!chan) 
        return server.sendMsgToClient(fd, "No such channel\r\n");
    if (!chan->isClient(fd)) 
        return server.sendMsgToClient(fd, "You're not on that channel\r\n");

    if (newTopic.empty()) 
    {
        if (chan->getTopic().empty()) 
            server.sendMsgToClient(fd, channelName + " :No topic is set\r\n");
        else 
            server.sendMsgToClient(fd, channelName + " :" + chan->getTopic() + "\r\n");
    } 
    else 
    {
        if (chan->isTopicRestricted() && !chan->isOperator(fd))
            return server.sendMsgToClient(fd, channelName + " :You're not channel operator\r\n");
        
        if (newTopic[0] == ' ') 
            newTopic.erase(0, 1);
        if (newTopic[0] == ':') 
            newTopic.erase(0, 1);
        
        chan->setTopic(newTopic);
        string msg = ":" + server.getClient(fd)->getNickName() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
        std::vector<int> mems = chan->getClients();
        for (size_t i = 0; i < mems.size(); i++) 
            server.sendMsgToClient(mems[i], msg);
    }
}

void CommandDispatcher::cmdInvite(std::istringstream &ss, Server &server, int fd) {
    string targetNick, channelName;
    ss >> targetNick >> channelName;

    Client *inviter = server.getClient(fd);
    Channel *chan = server.getChannel(channelName);

    if (!chan) 
        return server.sendMsgToClient(fd, channelName + " :No such channel\r\n");
    if (!chan->isClient(fd)) 
        return server.sendMsgToClient(fd, channelName + " :You're not on that channel\r\n");
    if (chan->isInviteOnly() && !chan->isOperator(fd))
        return server.sendMsgToClient(fd, channelName + " :You're not channel operator\r\n");

    int targetFd = -1;
    std::map<int, Client> &clients = server.getClients();
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) 
    {
        if (it->second.getNickName() == targetNick) 
        { 
            targetFd = it->first;
            break; 
        }
    }

    if (targetFd == -1) 
        return server.sendMsgToClient(fd, targetNick + " :No such nick\r\n");
    if (chan->isClient(targetFd)) 
        return server.sendMsgToClient(fd, targetNick + " " + channelName + " :is already on channel\r\n");

    chan->addInvite(targetFd);
    server.sendMsgToClient(fd, inviter->getNickName() + " " + targetNick + " " + channelName + "\r\n");
    server.sendMsgToClient(targetFd, ":" + inviter->getNickName() + " INVITE " + targetNick + " :" + channelName + "\r\n");
}

void CommandDispatcher::cmdMode(std::istringstream &ss, Server &server, int fd) {
    string target, modes, param;
    ss >> target >> modes;
    
    Channel *chan = server.getChannel(target);
    if (!chan) 
        return;
    if (!chan->isOperator(fd)) 
        return server.sendMsgToClient(fd, target + " :You're not channel operator\r\n");

    bool adding = true;
    for (size_t i = 0; i < modes.length(); i++) {
        char c = modes[i];
        if (c == '+') 
        { 
            adding = true; 
            continue; 
        }
        if (c == '-') 
        { 
            adding = false; 
            continue; 
        }

        if (c == 'i') 
            chan->setInviteOnly(adding);
        else if (c == 't') 
            chan->setTopicRestricted(adding);
        else if (c == 'k') {
            if (adding) {
                if (ss >> param)
                    chan->setPassword(param);
            } else {
                chan->setPassword("");
            }
        }
        else if (c == 'l')
        {
            if (adding) 
            { 
                ss >> param; 
                chan->setUserLimit(std::atoi(param.c_str())); 
            }
            else 
                chan->setUserLimit(0);
        }
        else if (c == 'o') {
            ss >> param;
            int targetFd = -1;
            std::map<int, Client> &clients = server.getClients();
            for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
                { 
                    if (it->second.getNickName() == param)
                        targetFd = it->first; 
                }
            if (targetFd != -1) {
                if (adding) 
                    chan->addOperator(targetFd);
                else 
                    chan->removeOperator(targetFd);
            }
        }
    }
    string msg = ":" + server.getClient(fd)->getNickName() + " MODE " + target + " " + modes + "\r\n";
    std::vector<int> mems = chan->getClients();
    for (size_t i = 0; i < mems.size(); i++) 
        server.sendMsgToClient(mems[i], msg);
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
    else if (command == "PART")
        cmdPart(ss, server, fd);
    else if (command == "PRIVMSG")
        cmdPrivmsg(ss, server, fd);
    else if (command == "QUIT")
        cmdQuit(ss, server, fd);
    else if (command == "KICK") 
        cmdKick(ss, server, fd);
    else if (command == "INVITE") 
        cmdInvite(ss, server, fd);
    else if (command == "TOPIC") 
        cmdTopic(ss, server, fd);
    else if (command == "MODE") 
        cmdMode(ss, server, fd);
    else
        server.sendMsgToClient(fd, "Invalid command\r\n");
}