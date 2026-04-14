#include <sys/socket.h>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <netinet/in.h>
#include <cstring>
#include <cstdio> 
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <map>
#include <fcntl.h>
#include <csignal>
#include <errno.h>

using std::cout;
using std::endl;
typedef std::string string;

class Client;
class Channel;

class Server
{
    private:
        int _port;
        string _password;
        int _serverFd;
        int _clientFd;
        struct sockaddr_in _server_addr;
        std::vector<pollfd> _pFds;
        std::map<int, string> _clientBuffers;
        std::map<int, Client> _clients;
        std::map<std::string, Channel> _channels;

    public:
        Server(int port, const string &password);
        ~Server();

        Client* getClient(int fd);
        std::map<int, Client> &getClients();
        
        Channel* getChannel(const string& name);
        std::map<string, Channel> &getChannels();

        string getPassword() const;
        
        void start();
        void acceptClient();
        bool recvMsg(size_t &i);
        void connectionHandler();
        void disconnectClient(int fd);
        
        void handleMessage(int fd, const string &msg);
        void sendMsgServerClosed();
        void sendMsgToClient(int fd, const string &msg);
        void sendMsgToMany(const std::vector<int> &fds, const string &msg);
};

void handleSignal(int signum);
