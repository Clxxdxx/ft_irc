/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clalopez <clalopez@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 12:41:13 by clalopez          #+#    #+#             */
/*   Updated: 2026/03/09 15:12:52 by clalopez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <iostream>

typedef std::string string;
using std::cout;
using std::endl;
class Channel
{
private:
    string _name;
    string _topic;

    std::vector<int> _clients;
    std::vector<int> _operators;

    bool _inviteOnly;

public:
    Channel();
    Channel(const string &name);
    ~Channel();

    string getName() const;
    void setName(const string &name);

    string getTopic() const;
    void setTopic(const string &topic);

    std::vector<int> getClients() const;
    std::vector<int> getOperators() const;

    void addClient(int fd);
    void removeClient(int fd);
    bool isClient(int fd) const;

    void addOperator(int fd);
    bool isOperator(int fd) const;

    bool isInviteOnly() const;
    void setInviteOnly(bool value);
};

#endif