#ifndef COMMANDDISPATCHER_HPP
# define COMMANDDISPATCHER_HPP

#include <string>
#include <iostream>

using std::cout;
using std::endl;
typedef std::string string;

class Server;
class Client;

class CommandDispatcher
{
    private:


    public:
        CommandDispatcher();
        ~CommandDispatcher();

        void executeCommand(Server &server, int fd, const string &message);

        void cmdPass(std::istringstream &ss, Server &server, int fd);
        void cmdNick(std::istringstream &ss, Server &server, int fd);
        void cmdUser(std::istringstream &ss, Server &server, int fd);
        void checkRegister(int fd, Client *client, Server &Server);
        
        void cmdJoin(std::istringstream &ss, Server &server, int fd);
        void cmdPrivmsg(std::istringstream &ss, Server &server, int fd);
        void cmdQuit(std::istringstream &ss, Server &server, int fd);

        void cmdKick(std::istringstream &ss, Server &server, int fd);
        void cmdInvite(std::istringstream &ss, Server &server, int fd);
        void cmdTopic(std::istringstream &ss, Server &server, int fd);
        void cmdMode(std::istringstream &ss, Server &server, int fd);
};

#endif
