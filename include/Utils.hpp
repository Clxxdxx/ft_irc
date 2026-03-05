#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>
#include <cstdlib>
#include <cctype>
using std::cout;
using std::endl;

typedef std::string string;

bool isNumber(const string &str);
bool checkArgs(const string &portStr, const string &password);


#endif