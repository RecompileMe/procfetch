#pragma once
#include <map>
#include <string>
#include "color.hpp"
/**
 * String Styling for terminal.
 */
class Crayon
{
public:
    /**
     * default constructor
     */
    Crayon();
    /**
     * set bright mode
     */
    Crayon bright();
    /**
     * set underscore mode
     */
    Crayon underscore();
    /**
     * set color
     * @param color
     */
    Crayon color(std::string color);
    /** set color red */
    Crayon red();
    /** set color green */
    Crayon green();
    /** set color yellow */
    Crayon yellow();
    /**
     * @param s
     * @returns styled text
     */
    std::string text(std::string s);

private:
    std::string escape_codes;
    inline static std::map<std::string, std::string> m =
    {
            {"RED", RED},         {"GREEN", GREEN},   {"BLACK", BLACK},
            {"YELLOW", YELLOW},   {"BLUE", BLUE},     {"MAGENTA", MAGENTA},
            {"CYAN", CYAN},       {"WHITE", WHITE},   {"BBLACK", BBLACK},
            {"BGRAY", BGRAY},     {"BRED", BRED},     {"BGREEN", BGREEN},
            {"BYELLOW", BYELLOW}, {"BBLUE", BBLUE},   {"BMAGENTA", BMAGENTA},
            {"BCYAN", BCYAN},     {"BWHITE", BWHITE},
    };
    static std::string getColor(std::string s);
};