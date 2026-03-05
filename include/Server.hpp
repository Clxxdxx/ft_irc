/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clalopez <clalopez@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:36:56 by clalopez          #+#    #+#             */
/*   Updated: 2026/03/05 12:41:43 by clalopez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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


class Server
{
    private:
        int _port;
        int _portFd;
        int _serverFd;
        int _clientFd;
        struct sockaddr_in _server_addr;
        std::vector<pollfd> _pFds;
        std::map<int, std::string> _clientBuffers;

    public:
        Server(int port);
        ~Server();

        std::vector<int> getAllClients();
        
        void start();
        void acceptClient();
        bool recvMsg(size_t &i);
        void connectionHandler();
        void sendMsgServerClosed();
        void sendMsgToClient(int fd, const std::string &msg);
        void sendMsgToMany(const std::vector<int> &fds, const std::string &msg);
};

void handleSignal(int signum);
