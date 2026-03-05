/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clalopez <clalopez@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:41:15 by clalopez          #+#    #+#             */
/*   Updated: 2026/03/05 15:30:30 by clalopez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

volatile sig_atomic_t g_serv_running = 1; 

void handleSignal(int signum)
{
    (void)signum;
    g_serv_running = 0;
}

Server::Server(int port): _port(port)
{
            
}

Server::~Server()
{
    for (size_t i = 0; i < _pFds.size(); i++)
        close(_pFds[i].fd);
}

std::vector<int> Server::getAllClients()
{
    std::vector<int> clients;
    
    for (size_t i = 0; i < _pFds.size(); i++)
    {
        int fd = _pFds[i].fd;
        if (fd != STDIN_FILENO && fd != _serverFd)
            clients.push_back(fd);
    }
    return clients;
}


void Server::acceptClient()
{
    while (true)
    {
        //Aceptar conexiones y devuelve el fd del cliente
        _clientFd = accept(_serverFd, NULL, NULL);
        if (_clientFd < 0)
            break;
        fcntl(_clientFd, F_SETFL, O_NONBLOCK);
        
        pollfd client_poll;
        client_poll.fd = _clientFd; // Que socket vigilar
        client_poll.events = POLLIN; //Saber si hay datos a leer
        client_poll.revents = 0; //Como no hay eventos, no hacer nada
        
        //Añdir el cliente al al vector de los polls
        _pFds.push_back(client_poll);
        cout << "Cliente conectado: fd " << _clientFd << endl; 
    }
}

bool Server::recvMsg(size_t &i)
{
    char buffer[512];
    int fd = _pFds[i].fd;

    int bytes = recv(fd, buffer, sizeof(buffer), 0);

    if (bytes <= 0)
    {
        if (bytes == 0)
            cout << "Cliente desconectado: fd " << fd << endl;
        else
           return true;

        close(fd);
        _clientBuffers.erase(fd);
        _pFds.erase(_pFds.begin() + i);
        return false;
    }

    _clientBuffers[fd] += std::string(buffer, bytes);

    size_t pos;
    //Añadir que busque tb el \r cuando se haga el parseo
    //EL mensaje tiene que acabar con \r\n para cambiar de linea y mover el cursor
    while ((pos = _clientBuffers[fd].find("\n")) != std::string::npos)
    {
        std::string message = _clientBuffers[fd].substr(0, pos);
        _clientBuffers[fd].erase(0, pos + 1);

        //Aqui ira el usuario que ha hablado y su mensaje
        cout << "Mensaje del [fd " << fd << "]: " << message << endl;
    }

    return true;
}


void Server::sendMsgToClient(int fd, const std::string &msg)
{
    size_t totalSent = 0;
    //Obligar que se mande todo el mensaje porque send no garantiza que se mande todo
    while (totalSent < msg.size())
    {
        ssize_t bytes = send(fd, msg.c_str() + totalSent, msg.size() - totalSent, 0);
        
        if (bytes <= 0)
        {
            std::cerr << "Can't send message to " << fd << endl;
            return ;
        }
        totalSent += bytes;
    }
    
}

void Server::sendMsgToMany(const std::vector<int> &fds, const std::string &msg)
{
    for (size_t i = 0; i < fds.size(); i++)
        sendMsgToClient(fds[i], msg);
    
}

void Server::sendMsgServerClosed()
{
    std::string msg = "Server closed\r\n";
    std::vector<int> clientsFds;
    for (size_t i = 0; i < _pFds.size(); i++)
    {
        int fd = _pFds[i].fd;
        //Si el fd es de un cliente mandarle el mensaje y cerrarlo
        if (fd != _serverFd && fd != STDIN_FILENO)
            clientsFds.push_back(fd);
    }

    sendMsgToMany(clientsFds, msg);
    
    for (size_t i = 0; i < _pFds.size(); i++)
        close(_pFds[i].fd);
    
    close(_serverFd);
    _clientBuffers.clear();
    
}
void Server::connectionHandler()
{
    //Añadir el servidor al poll
    pollfd server_poll;
    server_poll.fd = _serverFd;
    server_poll.events = POLLIN;
    server_poll.revents = 0;
    _pFds.push_back(server_poll);

    //Añadir stdin al poll para poder manejar el ctrl+d
    pollfd stdin_poll;
    stdin_poll.fd = STDIN_FILENO;
    stdin_poll.events = POLLIN;
    stdin_poll.revents = 0;
    _pFds.push_back(stdin_poll);

    //Servidor corriendo
    while (g_serv_running)
    {
        //Monitorear los fds a ver si hay alguno activo
        int active = poll(_pFds.data(), _pFds.size(), -1);
        if (active < 0)
        {
            if (errno == EINTR)
                continue;
            break;
        }

        //Recorrer los fd a ver si hay algo para leer sino, nada    
        for (size_t i = 0; i < _pFds.size();)
        {
            if (_pFds[i].revents & POLLIN)
            {
                //Manejar el ctrl+d del server
                if (_pFds[i].fd == STDIN_FILENO)
                {
                    char buffer[10];
                    int bytes = read(STDIN_FILENO, buffer, sizeof(buffer));

                    if (bytes == 0)
                    {
                        g_serv_running = 0;
                        break;
                    }
                    i++;
                }
                //si el que esta activo es el fd del server, aceptar al cliente
                else if (_pFds[i].fd == _serverFd)
                {
                    acceptClient();
                    i++;
                }
                //si es el del cliente, recibir el mensaje
                else
                {
                    if (recvMsg(i))
                        i++;
                }
            }
            else
                i++;
        }
    }
    sendMsgServerClosed();
}

void Server::start()
{
    (void)_portFd;
    //Crea un punto de comunicacion para enviar o recibir datos
    //Primer parametro, es el dominio de las ip, el segundo el tipo de protocolo
    //y el tercero elige el protocolo, 0 por defecto
    _serverFd = socket(AF_INET,SOCK_STREAM, 0);
    if (_serverFd < 0)
    {
        std::cerr << "Error: Can't create socket" << endl;
        exit(EXIT_FAILURE);        
    }
    fcntl(_serverFd, F_SETFL, O_NONBLOCK);
    
    //Configura las opciones de un socket
    //Fd del socket, nivel de protocolo, opciones, valor de la opcion, tamaño del valor de la opcion
    int opt = 1;
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "Error: Socket options failure" << endl;
        exit(EXIT_FAILURE);   
    }

    _server_addr.sin_family = AF_INET; //Que sean de tipo ipv4
    
    //Big-endian y little-endian son formas de ordenar bytes para almacenar datos numericos
    //Htons convierte un entero  de 16 bits al orden de bytes del host al de la red, trabajan con un
    //orden de bytes distinto, little-endian y big-endian. Esto se usa para asegurar que los
    //numeros de los puertos y otras cosas si interpreten correctamente en la red
    _server_addr.sin_port = htons(_port); 
    _server_addr.sin_addr.s_addr = INADDR_ANY; //Que escuche de todas las redes locales
    
    int bind_res = bind(_serverFd, (struct sockaddr*)&_server_addr, sizeof(_server_addr));
    if (bind_res < 0)
    {
        std::cerr << "Error: Bind" << endl;
        exit(EXIT_FAILURE);
    }

    //marca el socket para que espere conexiones entrantes
    int listen_res = listen(_serverFd, SOMAXCONN);
    if (listen_res < 0)
    {
        std::cerr << "Error: Can't listen" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Servidor iniciado en el puerto " << _port << endl;
    connectionHandler();
}