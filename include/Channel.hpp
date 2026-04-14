#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>

typedef std::string string;

class Channel
{
private:
    string              _name;
    string              _topic;
    string              _password;      // mode k

    std::vector<int>    _clients;
    std::vector<int>    _operators;
    std::vector<int>    _inviteList;    // fds invitados (modo i)

    bool    _inviteOnly;        // mode i
    bool    _topicRestricted;   // mode t: solo operadores pueden cambiar el topic
    int     _userLimit;         // mode l: 0 = sin limite

public:
    Channel();
    Channel(const string &name);
    ~Channel();

    // Nombre
    string  getName() const;
    void    setName(const string &name);

    // Topic
    string  getTopic() const;
    void    setTopic(const string &topic);

    // Password (mode k)
    string  getPassword() const;
    void    setPassword(const string &password);
    bool    hasPassword() const;

    // Clientes
    std::vector<int>    getClients() const;
    void    addClient(int fd);
    void    removeClient(int fd);
    bool    isClient(int fd) const;
    int     getClientCount() const;

    // Operadores (mode o)
    std::vector<int>    getOperators() const;
    void    addOperator(int fd);
    void    removeOperator(int fd);
    bool    isOperator(int fd) const;

    // Lista de invitados (mode i)
    void    addInvite(int fd);
    void    removeInvite(int fd);
    bool    isInvited(int fd) const;

    // Modos
    bool    isInviteOnly() const;
    void    setInviteOnly(bool value);

    bool    isTopicRestricted() const;
    void    setTopicRestricted(bool value);

    int     getUserLimit() const;
    void    setUserLimit(int limit);
    bool    hasUserLimit() const;
};

#endif