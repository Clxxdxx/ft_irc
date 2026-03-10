/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ancarret <ancarret@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:54:15 by clalopez          #+#    #+#             */
/*   Updated: 2026/03/10 18:38:45 by ancarret         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(): _inviteOnly(false), _topicRestricted(false), _userLimit(0)
{

}

Channel::Channel(const string &name): _name(name), _inviteOnly(false), _topicRestricted(false), _userLimit(0)
{
    
}

Channel::~Channel()
{
    
}

string Channel::getName() const
{
    return _name;
}

string Channel::getTopic() const
{
    return _topic;
}

std::vector<int> Channel::getClients() const
{
    return _clients;
}

std::vector<int> Channel::getOperators() const
{
    return _operators;
}

bool Channel::isInviteOnly() const
{
    //Si se necesita invitacion, devolver true, sino es publico
    if (_inviteOnly == true)
        return true;
    return false;
}

void Channel::setName(const string &name)
{
    _name = name;
}

void Channel::setTopic(const string &topic)
{
    _topic = topic;
}

void Channel::setInviteOnly(bool value)
{
    _inviteOnly = value;
}

// --- Password (mode k) ---

string Channel::getPassword() const
{
    return _password;
}

void Channel::setPassword(const string &password)
{
    _password = password;
}

bool Channel::hasPassword() const
{
    return !_password.empty();
}

// --- Clientes ---

void Channel::addClient(int fd)
{
    _clients.push_back(fd);
}

void Channel::removeClient(int fd)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i] == fd)
        {
            _clients.erase(_clients.begin() + i);
            return;
        }
    }
}

bool Channel::isClient(int fd) const
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i] == fd)
            return true;
    }
    return false;
}

int Channel::getClientCount() const
{
    return static_cast<int>(_clients.size());
}

// --- Operadores (mode o) ---

void Channel::addOperator(int fd)
{
    if (!isOperator(fd))
        _operators.push_back(fd);
}

void Channel::removeOperator(int fd)
{
    for (size_t i = 0; i < _operators.size(); i++)
    {
        if (_operators[i] == fd)
        {
            _operators.erase(_operators.begin() + i);
            return;
        }
    }
}

bool Channel::isOperator(int fd) const
{
    for (size_t i = 0; i < _operators.size(); i++)
    {
        if (_operators[i] == fd)
            return true;
    }
    return false;
}

// --- Lista de invitados (mode i) ---

void Channel::addInvite(int fd)
{
    if (!isInvited(fd))
        _inviteList.push_back(fd);
}

void Channel::removeInvite(int fd)
{
    for (size_t i = 0; i < _inviteList.size(); i++)
    {
        if (_inviteList[i] == fd)
        {
            _inviteList.erase(_inviteList.begin() + i);
            return;
        }
    }
}

bool Channel::isInvited(int fd) const
{
    for (size_t i = 0; i < _inviteList.size(); i++)
    {
        if (_inviteList[i] == fd)
            return true;
    }
    return false;
}

// --- Modo t (topic restringido) ---

bool Channel::isTopicRestricted() const
{
    return _topicRestricted;
}

void Channel::setTopicRestricted(bool value)
{
    _topicRestricted = value;
}

// --- Modo l (limite de usuarios) ---

int Channel::getUserLimit() const
{
    return _userLimit;
}

void Channel::setUserLimit(int limit)
{
    _userLimit = limit;
}

bool Channel::hasUserLimit() const
{
    return _userLimit > 0;
}