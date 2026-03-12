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
};






#endif
