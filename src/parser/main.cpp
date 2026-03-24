/* Programa de prueba mínimo para SimpleParser
 * Uso:
 *  - Pasar la línea como argumentos: ./simple_parser_main NICK pepe
 *  - O no pasar nada y escribir la línea por stdin
 */

#include "SimpleParser.hpp"
#include <iostream>
#include <sstream>

int main(int argc, char **argv)
{
    std::string line;

    if (argc > 1)
    {
        // unir los argumentos en una sola línea
        std::ostringstream oss;
        for (int i = 1; i < argc; ++i)
        {
            if (i > 1) oss << ' ';
            oss << argv[i];
        }
        line = oss.str();
    }
    else
    {
        // leer una línea de stdin
        if (!std::getline(std::cin, line))
            return 0;
    }

    // añadir CRLF por si acaso
    line += "\r\n";

    SimpleParsedMessage msg = SimpleParser::parseLine(line);

    std::cout << "prefix: '" << msg.prefix << "'\n";
    std::cout << "command: '" << msg.command << "'\n";
    std::cout << "params:";
    for (size_t i = 0; i < msg.params.size(); ++i)
        std::cout << " [" << msg.params[i] << "]";
    std::cout << "\n";
    std::cout << "trailing: '" << msg.trailing << "'\n";

    return 0;
}
