// Parser IRC simple y claro

#include "SimpleParser.hpp"
#include <cctype>

using std::string;

// Quita CRLF del final si existen
static string stripCRLF(const string &s)
{
    string r = s;
    if (!r.empty() && r.back() == '\n')
        r.pop_back();
    if (!r.empty() && r.back() == '\r')
        r.pop_back();
    return r;
}
// coge una línea completa y la parte en prefix/command/params/trailing
SimpleParsedMessage SimpleParser::parseLine(const std::string &rawLine)
{
    SimpleParsedMessage out;
    string line = stripCRLF(rawLine);
    size_t i = 0;
    size_t n = line.size();

    // 1) si empieza por ':'
    if (i < n && line[i] == ':')
    {
        i++; // saltar ':'
        size_t start = i;
        while (i < n && line[i] != ' ')
            i++;
        out.prefix = line.substr(start, i - start);
        while (i < n && line[i] == ' ') //saltar espacios
            i++;
    }

    // 2) siguiente token
    if (i < n)
    {
        size_t start = i;
        while (i < n && line[i] != ' ')
            i++;
        out.command = line.substr(start, i - start);
        for (size_t k = 0; k < out.command.size(); ++k)
            out.command[k] = static_cast<char>(std::toupper(static_cast<unsigned char>(out.command[k]))); //iasjfaf
    }

    // 3) Params y trailing
    // saltar espacios
    while (i < n && line[i] == ' ')
        i++;

    while (i < n)
    {
        if (line[i] == ':')
        {
            // trailing: el resto sin ':'
            out.trailing = line.substr(i + 1);
            break;
        }
        // siguiente parámetro (sin espacios dentro)
        size_t start = i;
        while (i < n && line[i] != ' ')
            i++;
        out.params.push_back(line.substr(start, i - start));
        while (i < n && line[i] == ' ')
            i++;
    }

    return out;
}
