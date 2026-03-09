/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clalopez <clalopez@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:39:30 by clalopez          #+#    #+#             */
/*   Updated: 2026/03/09 14:53:13 by clalopez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd): _fd(fd)
{
    
}

Client::~Client()
{
    
}

int Client::getFd() const
{
    return _fd;
}

string Client::getUserName() const
{
    return _userName;
}

string Client::getNickName() const
{
    return _nickName;
}

std::vector<string> Client::getChannels() const
{
    return _channels;
}

bool Client::isRegistered() const
{
    if (_registered == true)
        return true;
    return false;
}

void Client::setUserName(const string &username)
{
    _userName = username;
}

void Client::setNickName(const string &nickname)
{
    _nickName = nickname;
}

void Client::setRegistered(bool value)
{
    _registered = value;
}