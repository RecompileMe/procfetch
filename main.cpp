/**
 * @file main.cpp
 */
#include <csignal>
#include <iostream>
#include "fetch.hpp"
#include "Crayon.hpp"
#include "Options.hpp"

#define VERSION "@VERSION@"

/**
 * @returns Displays Info
 * @param show_battery
 */
void DisplayInfo(bool show_battery)
{
    std::string delim = ": ";
    auto title = Crayon{}.bright().green();
    auto ye = Crayon{}.yellow();
    std::string username = ye.text(getuser()) + "@" +
                      ye.text(gethostname("/proc/sys/kernel/hostname"));
    std::cout << Crayon{}.underscore().text(username) << std::endl;
    std::cout << std::endl;

    std::cout << title.text("OS") << delim << getOS("/etc/os-release")
         << getHardwarePlatform() << std::endl;
    std::cout << title.text("Host") << delim
         << getHost("/sys/devices/virtual/dmi/id/") << std::endl;
    std::cout << title.text("Kernel") << delim
         << getKernel("/proc/sys/kernel/osrelease") << std::endl;
    std::cout << title.text("UpTime") << delim << getUpTime("/proc/uptime") << std::endl;
    std::cout << title.text("RAM") << delim << getRAM("/proc/meminfo") << std::endl;
    std::cout << title.text("shell") << delim << getSHELL("/etc/passwd") << std::endl;
    std::cout << title.text("DE") << delim << getDE() << std::endl;

    if (resCheck())
    {
        std::cout << title.text("Resolution") << delim
             << getRES("/sys/class/graphics/fb0/modes") << std::endl;
    }

    std::cout << title.text("Theme") << delim << getTheme() << std::endl;
    std::cout << title.text("Icons") << delim << getIcons() << std::endl;
    std::cout << title.text("CPU") << delim << getCPU("/proc/cpuinfo") << std::endl;

    if (CpuTempCheck())
    {
        int temp = getCPUtemp("/sys/class/thermal/thermal_zone0/temp");
        std::cout << title.text("CPU Temperature") << delim << float(temp / 1000.0)
             << " °C" << std::endl;
    }

    std::vector<std::string> gpus = getGPU();
    for (auto gpu : gpus)
        std::cout << title.text("GPU") << delim << gpu << std::endl;

    std::cout << title.text("Packages") << delim << getPackages() << std::endl;

    if (show_battery)
        printBattery("/sys/class/power_supply/");

    std::cout << std::endl;
}

/**
 * @returns process status
 * @param argc
 * @param argv
 */
int main(int argc, char *argv[])
{
    auto options = Options(argc, argv);
    if (optind != argc)
    {
        std::cout << "Error: " << argv[0] << ": unknown argument: " << argv[optind]
             << std::endl;
        return 1;
    }

    switch (options.mode)
    {
    case Mode::NORMAL:
        // no-op
        break;
    case Mode::SHOW_VERSION:
        std::cout << VERSION << std::endl;
        return 0;
    default:
        // MUST NOT BE HERE
        return 1;
    }

    // Mode::NORMAL
    print(options.color_name, options.distro_name);
    DisplayInfo(options.show_battery);

    return 0;
}


