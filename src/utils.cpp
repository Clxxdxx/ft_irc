#include "Utils.hpp"

bool isNumber(const string &str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!std::isdigit(static_cast<unsigned char>(str[i])))
            return false;
    }
    return true;
}

bool checkArgs(const string &portStr, const string &password)
{
    if (!isNumber(portStr))
    {
        std::cerr << "Error: Port must be numeric" << endl;
        return false;
    }

    int portInt = std::atoi(portStr.c_str());

    if (portInt < 1024 || portInt > 49151)
    {
        std::cerr << "Error: Port must be between 1024 and 49151" << endl;
        return false;
    }

    if (password.empty())
    {
        std::cerr << "Error: Password cannot be empty" << endl;
        return false;
    }

    return true;
}