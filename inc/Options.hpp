#pragma once
#include <string>

class Options
{
public:
    std::string color_name;
    std::string distro_name;
    bool show_battery;

    Options() = default;
    Options(int argc, char *argv[]);
};