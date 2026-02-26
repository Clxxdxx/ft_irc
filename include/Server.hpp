/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clalopez <clalopez@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:36:56 by clalopez          #+#    #+#             */
/*   Updated: 2026/02/26 16:54:56 by clalopez         ###   ########.fr       */
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

using std::cout;
using std::endl;

class Server
{
    private:
        int _port;
        int _portFd;

    public:
        Server(int port);
        ~Server();

        void start();
};