#include "Crayon.hpp"

Crayon Crayon::bright()
{
    escape_codes += BRIGHT;
    return *this;
}

Crayon Crayon::underscore()
{
    escape_codes += UNDERSCORE;
    return *this;
}

Crayon Crayon::color(std::string color)
{
    escape_codes += getColor(color);
    return *this;
}

Crayon Crayon::red()
{
    escape_codes += RED;
    return *this;
}

Crayon Crayon::green()
{
    escape_codes += GREEN;
    return *this;
}

Crayon Crayon::yellow()
{
    escape_codes += YELLOW;
    return *this;
}

std::string Crayon::text(std::string s)
{
    return escape_codes + s + RESET;
}

std::string Crayon::getColor(std::string s)
{
    return m[s];
}