#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

typedef std::string string;

class Client
{
    private:
        int         _fd;
        string      _userName;
        string      _nickName;
        bool        _registered;
        bool        _hasProvidedPass;
        std::vector<string> _channels;

    public:
        Client();
        Client(int fd);
        ~Client();

        int     getFd() const;

        string  getUserName() const;
        void    setUserName(const string &username);

        string  getNickName() const;
        void    setNickName(const string &nickname);

        bool    isRegistered() const;
        void    setRegistered(bool value);
        bool    tryRegister();

        bool    hasProvidedPass() const;
        void    setHasProvidedPass(bool value);

        void    joinChannel(const string &channel);
        void    leaveChannel(const string &channel);
        bool    isInChannel(const string &channel) const;

        std::vector<string> getChannels() const;
};

#endif