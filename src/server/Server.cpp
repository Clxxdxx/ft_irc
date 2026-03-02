/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clalopez <clalopez@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:41:15 by clalopez          #+#    #+#             */
/*   Updated: 2026/03/02 16:29:53 by clalopez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port): _port(port)
{
    
}

Server::~Server()
{
    
}

void Server::acceptClient()
{
    //Aceptar conexiones y devuelve el fd del cliente
    _clientFd = accept(_serverFd, NULL, NULL);
    if (_clientFd < 0)
    {
        std::cerr << "Error: accept failed" << endl;
        return;
    }
    
    
    pollfd client_poll;
    client_poll.fd = _clientFd; // Que socket vigilar
    client_poll.events = POLLIN; //Saber si hay datos a leer
    client_poll.revents = 0; //Como no hay eventos, no hacer nada
    
    //Añdir el cliente al al vector de los polls
    _pFds.push_back(client_poll);
    cout << "Cliente conectado: fd " << _clientFd << endl; 
}

void Server::recvMsg(size_t &i)
{
    char buffer[512];
    //Recibir el mensaje del fd del poll, -3 para que termine n \r\n\0
    int bytes = recv(_pFds[i].fd, buffer, sizeof(buffer) - 3, 0);

    if (bytes <= 0)
    {
        //Cerrar el fd y eliminar el poll del vector
        cout << "Cliente desconectado: fd " << _pFds[i].fd << endl;
        close(_pFds[i].fd);
        _pFds.erase(_pFds.begin() + i);
    }
    else
    {
        //\r mueve el cursor al principio de la linea indicando que se ha terminado el mensaje
        buffer[bytes] = '\r';
        buffer[bytes + 1] = '\n';
        buffer[bytes + 2] = '\0';
        cout << "Mensaje: " << buffer;
        
    }
}

void Server::connectionHandler()
{
    //Añadir el servidor al poll
    pollfd server_poll;
    server_poll.fd = _serverFd;
    server_poll.events = POLLIN; // esto sirve para saber cuando algo se conecta, si hay datos para leer
    server_poll.revents = 0; //Porque aun no se ha llamado a poll, aun no ha ocurrido ningun evento

    //Añadir el server de poll al vector
    _pFds.push_back(server_poll);

    while (1)
    {
        //Revisa los fd de los polls y espera a que ocurra algo
        int active = poll(_pFds.data(), _pFds.size(), -1);
        if (active < 0)
            break;
        
        //Recorre los fd y comprueba su ha pasado algo
        for (size_t i = 0; i < _pFds.size(); i++)
        {
            if (_pFds[i].revents & POLLIN)
            {
                //Si el fd es del servidor es que se ha creado una nueva conexion, sino, es el
                //cliente el que le envia el mensaje al servidor
                if (_pFds[i].fd == _serverFd)
                    acceptClient();
                else
                    recvMsg(i);
            }
        }
    }
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
    
    //Configura las opciones de un socket
    //Fd del socket, nivel de protocolo, opciones, valor de la opcion, tamaño del valor de la opcion
    int opt = 1;
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "Error: Socket options failure" << endl;
        exit(EXIT_FAILURE);   
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; //Que sean de tipo ipv4
    
    //Big-endian y little-endian son formas de ordenar bytes para almacenar datos numericos
    //Htons convierte un entero  de 16 bits al orden de bytes del host al de la red, trabajan con un
    //orden de bytes distinto, little-endian y big-endian. Esto se usa para asegurar que los
    //numeros de los puertos y otras cosas si interpreten correctamente en la red
    server_addr.sin_port = htons(_port); 
    server_addr.sin_addr.s_addr = INADDR_ANY; //Que escuche de todas las redes locales
    
    int bind_res = bind(_serverFd, (struct sockaddr*)&server_addr, sizeof(server_addr));
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