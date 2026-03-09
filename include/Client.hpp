/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clalopez <clalopez@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 11:54:11 by clalopez          #+#    #+#             */
/*   Updated: 2026/03/09 12:40:18 by clalopez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
typedef std::string string;

class Client
{
    private:
        int _fd;
        string _userName;
        string _nickName;
        bool _registered;
        std::vector<string> _channels;
        
    public:
        Client(int fd);
        ~Client();

        int getFd() const;

        string getUserName() const;
        void setUserName(const string &username);

        string getNickName() const;
        void setNickName(const string &nickname);

        bool isRegistered() const;
        void setRegistered(bool value);

        void joinChannel(const string &channel);
        void leaveChannel(const string &channel);

        std::vector<string> getChannels() const;
};

#endif