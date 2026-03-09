/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clalopez <clalopez@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 14:54:15 by clalopez          #+#    #+#             */
/*   Updated: 2026/03/09 15:12:16 by clalopez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel()
{

}

Channel::Channel(const string &name): _name(name)
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