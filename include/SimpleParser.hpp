#pragma once

#include <string>
#include <vector>

struct SimpleParsedMessage
{
    std::string prefix;   // quien lo manda (si hay ':' al inicio)
    std::string command;  // comando (PRIVMSG, NICK...)
    std::vector<std::string> params; // parámetros sin espacios
    std::string trailing; // texto final que puede tener espacios
};

// Función principal: parseLine toma una línea completa y devuelve partes.
class SimpleParser
{
public:
    static SimpleParsedMessage parseLine(const std::string &rawLine);
};
