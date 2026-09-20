#include <getopt.h>
#include "Options.hpp"

Options::Options(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "a:d:vb")) != -1)
    {

        switch (opt)
        {
            case 'a':
                color_name = std::string(optarg);
                break;
            case 'd':
                distro_name = std::string(optarg);
                break;
            case 'b':
                show_battery = true;
                break;
            default:
                break;
        }
    }
}