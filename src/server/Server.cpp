/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clalopez <clalopez@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 14:41:15 by clalopez          #+#    #+#             */
/*   Updated: 2026/02/26 16:42:34 by clalopez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port): _port(port)
{
    
}

Server::~Server()
{
    
}

void Server::start()
{
    (void)_port;
    (void)_portFd;
    //Crea un punto de comunicacion para enviar o recibir datos
    //Primer parametro, es el dominio de las ip, el segundo el tipo de protocolo
    //y el tercero elige el protocolo, 0 por defecto
    int server_fd = socket(AF_INET,SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Error: Can't create socket" << std::endl;
        exit(EXIT_FAILURE);        
    }
    
    //Configura las opciones de un socket
    //Fd del socket, nivel de protocolo, opciones, valor de la opcion, tamaño del valor de la opcion
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "Error: Socket options failure" << std::endl;
        exit(EXIT_FAILURE);   
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; //Que sean de tipo ipv4
    
    //Htons convierte un entero  de 16 bits al orden de bytes del host al de la red, trabajan con un
    //orden de bytes distinto, little-endian y big-endian. Esto se usa para asegurar que los
    //numeros de los puertos y otras cosas si interpreten correctamente en la red
    server_addr.sin_port = htons(6667); 
    server_addr.sin_addr.s_addr = INADDR_ANY; //Que escuche de todas las redes locales
    
    int bind_res = bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (bind_res < 0)
    {
        std::cerr << "Error: Bind" << std::endl;
        exit(EXIT_FAILURE);
    }

    //marca el socket para que espere conexiones entrantes
    int listen_res = listen(server_fd, 10);
    if (listen_res < 0)
    {
        std::cerr << "Error: Can't listen" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    struct sockaddr_in client_addr;
    socklen_t   client_len = sizeof(client_addr);
    //Server fd sigue escuchando nuevas conexiones y se comunica con client_fd
    int client_fd = accept(server_fd,(struct  sockaddr*)&client_addr, &client_len);
    if (client_fd < 0)
    {
        std::cerr << "Error: Can't accept" << std::endl;
        exit(EXIT_FAILURE);
    }

    //Recibir el mensaje, tamaño del mensaje, y al recv se le pasa el descriptor del cliente
    //el buffer, el tamaño maximo del buffer y las flags, de normal es 0
    char buffer[512];
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes < 0)
    {
        std::cerr << "Error: Can't receive message" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (bytes == 0)
    {
        cout << "Cliente desconectado" << endl;
        close(client_fd);   
    }
    else
    {
        buffer[bytes] = '\0';
        cout << "Recibido: " << buffer << endl;
    }
}