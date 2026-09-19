#pragma once
#include <string>

enum class Mode
{
    NORMAL,
    SHOW_VERSION,
};

class Options
{
public:
    Mode mode = Mode::NORMAL;
    std::string color_name = "def";
    std::string distro_name = "def";
    bool show_battery = false;

    Options() = default;
    Options(int argc, char *argv[]);
};