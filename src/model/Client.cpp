/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clalopez <clalopez@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:39:30 by clalopez          #+#    #+#             */
/*   Updated: 2026/03/13 14:58:28 by clalopez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
{
    
}

Client::Client(int fd): _fd(fd), _registered(false), _hasProvidedPass(false)
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

bool Client::hasProvidedPass() const
{
    return _hasProvidedPass;
}

void Client::setHasProvidedPass(bool value)
{
    _hasProvidedPass = value;
}

void Client::joinChannel(const string &channel)
{
    _channels.push_back(channel);
}

void Client::leaveChannel(const string &channel)
{
    for (size_t i = 0; i < _channels.size(); i++)
    {
        if (_channels[i] == channel)
        {
            _channels.erase(_channels.begin() + i);
            return;
        }
    }
}

bool Client::isInChannel(const string &channel) const
{
    for (size_t i = 0; i < _channels.size(); i++)
    {
        if (_channels[i] == channel)
            return true;
    }
    return false;
}

bool Client::tryRegister()
{
    if (_hasProvidedPass && !_nickName.empty() && !_userName.empty())
    {
        _registered = true;    
        return true;
    }
    return false;
}