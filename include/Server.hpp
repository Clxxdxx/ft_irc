/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clalopez <clalopez@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:36:56 by clalopez          #+#    #+#             */
/*   Updated: 2026/03/02 16:29:18 by clalopez         ###   ########.fr       */
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
using std::cout;
using std::endl;

class Server
{
    private:
        int _port;
        int _portFd;
        int _serverFd;
        int _clientFd;
        std::vector<pollfd> _pFds;

    public:
        Server(int port);
        ~Server();

        void start();
        void acceptClient();
        void recvMsg(size_t &i);
        void connectionHandler();
};