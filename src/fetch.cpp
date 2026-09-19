/**
 * @file
 */
#include <mutex>
#include <pwd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <unistd.h>
#include "Command.hpp"
#include "Context.hpp"
#include "Crayon.hpp"

std::vector<std::thread> Command::ths;
std::vector<std::runtime_error> Command::exceptions;
std::mutex Command::mtx;
std::string Context::PACKAGE_DELIM = "; ";
/**
 * @returns gets the username
 * @throws runtime_error failed to get username
 */
std::string getuser()
{
    auto *p = getpwuid(getuid());
    if (p == nullptr)
    {
        std::string error = strerror(errno);
        throw std::runtime_error("Could not get struct passwd: " + error);
    }
    return p->pw_name;
}

/**
 * @returns gets the hostname
 * @param path
 */
std::string gethostname(const std::string &path)
{
    std::fstream fptr;
    fptr.open(path, std::ios::in);
    std::string hostname;
    getline(fptr, hostname);
    return hostname;
}

/**
 * @returns gets name of Operating System
 * @param path
 */
std::string getOS(std::string path)
{
    std::fstream fptr;
    fptr.open(path, std::ios::in);
    std::string os_name, variable_name, quote;

    while (fptr)
    {
        getline(fptr, os_name);
        variable_name = os_name.substr(0, 13);
        if (variable_name == "PRETTY_NAME=\"")
        {
            quote = "\"";
            break;
        }
        if (variable_name == "PRETTY_NAME=\'")
        {
            quote = "\'";
            break;
        }
    }

    os_name = os_name.substr(os_name.find(quote) + 1);
    os_name = os_name.substr(0, os_name.find(quote));

    return os_name;
}

/**
 * @returns gets HardWare Platform
 */
std::string getHardwarePlatform()
{
    auto cmd = Command::exec("uname -m");
    std::string s = cmd.getOutput();

    s = s.substr(0, s.find("\n"));
    return " " + s;
}

/**
 * @returns gets Host
 * @param path
 */
std::string getHost(std::string path)
{
    std::fstream f1, f2;
    std::string p1, p2, n1, n2;
    p1 = path + "product_name";
    p2 = path + "product_version";

    f1.open(p1, std::ios::in);
    getline(f1, n1);
    f1.close();

    f2.open(p2, std::ios::in);
    getline(f2, n2);
    f2.close();

    std::string host = n1 + " " + n2;

    return host;
}

/**
 * @returns gets kernel
 * @param path
 */
std::string getKernel(std::string path)
{
    std::fstream fptr;
    fptr.open(path, std::ios::in);
    std::string kernel;
    getline(fptr, kernel);
    return kernel;
}

/**
 * @returns get Uptime
 * @param path
 */
std::string getUpTime(std::string path)
{
    std::fstream fptr;
    fptr.open(path, std::ios::in);
    std::string time;

    getline(fptr, time);
    time = time.substr(0, time.find(" "));

    int m = stoi(time) / 60;
    int h = m / 60;
    int d = h / 24;

    std::string timeS;
    if (h == 0)
    {
        timeS = std::to_string(m % 60) + " mins";
    }
    else if (d == 0)
    {
        timeS = std::to_string(h % 24) + " hours, " + std::to_string(m % 60) + " mins";
    }
    else
    {
        timeS = std::to_string(d) + " d, " + std::to_string(h % 24) + " hours, " +
                std::to_string(m % 60) + " mins";
    }

    return timeS;
}

/**
 * @returns gets RAM usage details
 * @param path
 */
std::string getRAM(std::string path)
{
    std::fstream fptr;
    fptr.open(path, std::ios::in);
    std::string line, sub;
    std::string total, avail;
    while (fptr)
    {
        getline(fptr, line);
        sub = line.substr(0, line.find(":"));
        if (sub == "MemTotal")
        {
            total = line;
        }
        else if (sub == "MemAvailable")
        {
            avail = line;
            break;
        }
    }

    size_t i;
    for (i = 0; i < total.size(); i++)
    {
        if (isdigit(total[i]))
        {
            break;
        }
    }
    total = total.substr(i);
    total = total.substr(0, total.find(" "));

    for (i = 0; i < avail.size(); i++)
    {
        if (isdigit(avail[i]))
            break;
    }
    avail = avail.substr(i);
    avail = avail.substr(0, avail.find(" "));

    int memTotal = stoi(total);
    int memAvail = stoi(avail);
    int memUsed = memTotal - memAvail;

    return std::to_string(memUsed / 1024) + "MiB / " + std::to_string(memTotal / 1024) +
           "MiB";
}

/**
 * @returns gets type of shell
 * @param path
 */
std::string getSHELL(std::string path)
{
    std::fstream fptr;
    fptr.open(path, std::ios::in);
    std::string line, sub;
    while (fptr)
    {
        getline(fptr, line);
        sub = line.substr(0, line.find(":"));
        if (sub == getuser())
            break;
    }
    reverse(line.begin(), line.end());
    line = line.substr(0, line.find("/"));
    reverse(line.begin(), line.end());
    return line;
}

/**
 * @returns gets the Desktop Environment
 */
std::string getDE()
{
    const char *de;

    de = getenv("XDG_CURRENT_DESKTOP");
    if (de != nullptr)
        return de;

    de = getenv("XDG_SESSION_DESKTOP");
    if (de != nullptr)
        return de;

    return "";
}

/**
 * @returns checks for Resolution file
 */
bool resCheck()
{
    return Path::of("/sys/class/graphics/fb0/modes").isRegularFile();
}

/**
 * @returns gets current Screen Resolution
 * @param path
 */
std::string getRES(std::string path)
{
    std::fstream fptr;
    fptr.open(path, std::ios::in);
    std::string res;
    getline(fptr, res);
    res = res.substr(2);
    return res.substr(0, res.find("p"));
}

/**
 * @returns gets current Theme info
 */
std::string getTheme()
{
    auto args = "gsettings get org.gnome.desktop.interface gtk-theme";
    auto cmd = Command::exec(args);
    auto s = cmd.getOutput();

    return s.substr(1, s.find("\'", 1) - 1);
}

/**
 * @returns gets current Icon info
 */
std::string getIcons()
{
    auto args = "gsettings get org.gnome.desktop.interface icon-theme";
    auto cmd = Command::exec(args);
    auto s = cmd.getOutput();

    return s.substr(1, s.find("\'", 1) - 1);
}

/**
 * @returns gets CPU info
 * @param path
 */
std::string getCPU(std::string path)
{
    std::fstream fptr;
    fptr.open(path, std::ios::in);
    std::string cpu, line, sub;

    while (fptr)
    {
        getline(fptr, line);
        sub = line.substr(0, 10);
        if (sub == "model name")
            break;
    }
    cpu = line.substr(line.find(":") + 2);

    return cpu;
}

/**
 * @returns checks for CPUtemp file
 */
bool CpuTempCheck()
{
    return Path::of("/sys/class/thermal/thermal_zone1").isDirectory();
}

/**
 * @returns gets CPU temp
 * @param path
 */
int getCPUtemp(std::string path)
{
    std::fstream fptr;
    fptr.open(path, std::ios::in);
    std::string temp;
    getline(fptr, temp);
    return stoi(temp);
}

/**
 * @returns gets vendor of Internal and External GPU
 */
std::vector<std::string> getGPU()
{
    std::vector<std::string> gpu;
    auto cmd = Command::exec("lspci");
    std::istringstream ss(cmd.getOutput());
    std::string s;

    while (std::getline(ss, s))
    {
        if (s.find("VGA") != std::string::npos || s.find("3D") != std::string::npos ||
            s.find("Display") != std::string::npos)
        {
            auto start = s.find(": ") + 2;
            auto end = s.find(" (", start);
            gpu.push_back(s.substr(start, end - start));
        }
    }

    return gpu;
}

/**
 * @returns gets count of all packages installed
 */
std::string getPackages()
{
    struct rec
    {
        std::string name;
        int count; // -1: not supported
    };
    std::vector<rec> pkgs;
    std::mutex mtx;

    if (Path::of("/bin/dpkg").isExecutable())
    {
        Command::exec_async("dpkg -l", [&](auto c) {
            std::lock_guard<std::mutex> lock(mtx);
            if (c.getOutputLines() > 5)
                pkgs.push_back(rec{"dpkg", c.getOutputLines() - 5});
        });
    }
    if (Path::of("/bin/snap").isExecutable())
    {
        Command::exec_async("snap list", [&](auto c) {
            std::lock_guard<std::mutex> lock(mtx);
            if (c.getOutputLines() > 1)
                pkgs.push_back(rec{"snap", c.getOutputLines()});
        });
    }
    if (Path::of("/bin/pacman").isExecutable())
    {
        Command::exec_async("pacman -Q", [&](auto c) {
            std::lock_guard<std::mutex> lock(mtx);
            if (c.getOutputLines() > 0)
                pkgs.push_back(rec{"pacman", c.getOutputLines()});
        });
    }
    if (Path::of("/bin/flatpak").isExecutable())
    {
        Command::exec_async("flatpak list", [&](auto c) {
            std::lock_guard<std::mutex> lock(mtx);
            if (c.getOutputLines() > 1)
                pkgs.push_back(rec{"flatpak", c.getOutputLines()});
        });
    }
    if (Path::of("/var/lib/rpm").isExecutable())
    {
        Command::exec_async("rpm -qa", [&](auto c) {
            std::lock_guard<std::mutex> lock(mtx);
            if (c.getOutputLines() > 0)
                pkgs.push_back(rec{"rpm", c.getOutputLines()});
        });
    }
    if (Path::of("/bin/emerge").isExecutable()) // gentoo
    {
        pkgs.push_back(rec{"portage", -1});
    }
    if (Path::of("/bin/xbps-install").isExecutable()) // void linux
    {
        Command::exec_async("flatpak list", [&](auto c) {
            std::lock_guard<std::mutex> lock(mtx);
            if (c.getOutputLines() > 0)
                pkgs.push_back(rec{"xbps", c.getOutputLines()});
        });
    }
    if (Path::of("/bin/zypper").isExecutable()) // opensuse
    {
        Command::exec_async("zypper se --installed-only", [&](auto c) {
            std::lock_guard<std::mutex> lock(mtx);
            if (c.getOutputLines() > 0)
                pkgs.push_back(rec{"zypper", c.getOutputLines()});
        });
    }

    Path cmd = Path::of("/home/linuxbrew/.linuxbrew/bin/brew");
    if (cmd.isExecutable())
    {
        Command::exec_async(cmd, "list", [&](auto c) {
            std::lock_guard<std::mutex> lock(mtx);
            if (c.getOutputLines() > 0)
                pkgs.push_back(
                    rec{cmd.getFilename().toString(), c.getOutputLines()});
        });
    }

    Command::wait();

    sort(pkgs.begin(), pkgs.end(),
         [](auto a, auto b) { return a.count > b.count; });

    auto red = Crayon{}.red();
    std::string pkg = "";
    for (auto p : pkgs)
    {
        if (p.count < 0)
        {
            pkg += "not supported " + red.text(p.name) + Context::PACKAGE_DELIM;
            continue;
        }
        pkg += std::to_string(p.count) + " " + red.text(p.name) + Context::PACKAGE_DELIM;
    }

    return pkg;
}

/**
 * @brief Utility to check if battery is charging or not
 * @returns status of battery
 */
bool isCharging(std::string path)
{
    std::fstream fptr;
    fptr.open(path, std::ios::in);
    std::string status;
    getline(fptr, status);

    return status == "Charging";
}

/**
 * @brief Utility to print battery perecentage bar
 */
void printBar(std::string status_path, int battery)
{
    auto red = Crayon{}.bright().red();
    auto green = Crayon{}.bright().green();
    std::string emoji = "\n🔋 ";

    if (isCharging(status_path))
        emoji = "\n🔌 ";

    int width = 40;
    int pos = width * battery / 100.0;

    std::cout << emoji << green.text(std::to_string(battery) + "% ") << green.text("[");
    for (int i = 0; i < width; i++)
    {
        if (i < pos)
            std::cout << green.text("=");
        else if (i == pos)
            std::cout << green.text(">");
        else
            std::cout << red.text("-");
    }
    std::cout << green.text("]") << std::endl;

    return;
}

/**
 * @returns prints battery percentage bar
 * @param path
 */
void printBattery(std::string path)
{
    std::string dir_path = "";
    std::string capacity_path;
    std::string status_path;

    std::vector<Path> contents = Path::of(path).getDirectoryContents();

    for (auto &dir : contents)
    {
        if (dir.isDirectory() &&
            dir.getFilename().toString().substr(0, 2) == "BA")
        {
            dir_path = dir.toString();
            break;
        }
    }

    /* we don't have battery information */
    if (dir_path == "")
        return;

    capacity_path = dir_path + "/capacity";
    status_path = dir_path + "/status";
    std::fstream fptr;
    fptr.open(capacity_path, std::ios::in);
    std::string percent;
    getline(fptr, percent);
    printBar(status_path, stoi(percent));
}

/**
 * @param art
 * @param color_name
 */
void printProcess(std::string art, std::string color_name)
{
    std::string LIB_DIR = "@LIB_DIR@";
    std::string path = LIB_DIR + "/ascii/" + art;
    std::fstream fptr;
    fptr.open(path, std::ios::in);
    std::string txt;
    getline(fptr, txt);
    auto style = Crayon{}.bright();
    if (color_name == "def")
        style = style.color(txt.substr(0, txt.find(" ")));
    else
    {
        transform(color_name.begin(), color_name.end(), color_name.begin(),
                  ::toupper);
        style = style.color(color_name.substr(0, color_name.find(" ")));
    }
    std::cout << style.text("") << std::endl;
    while (fptr)
    {
        getline(fptr, txt);
        std::cout << style.text(txt) << std::endl;
    }
    fptr.close();
}

/**
 * Utility to print ascii art of Distro
 * @param color_name
 * @param distro_name
 */
void print(std::string color_name, std::string distro_name)
{
    std::string os = distro_name;

    if (distro_name == "def")
        os = getOS("/etc/os-release");

    std::map<std::string, std::string> ascii_arts = {{"Ubuntu", "ubuntu.ascii"},
                                      {"Debian", "debian.ascii"},
                                      {"Fedora", "fedora.ascii"},
                                      {"Red Hat", "redhat.ascii"},
                                      {"Arch Linux", "arch.ascii"},
                                      {"Manjaro", "manjaro.ascii"},
                                      {"Archcraft", "archcraft.ascii"},
                                      {"Kali", "kali.ascii"},
                                      {"Parrot", "parrot.ascii"},
                                      {"openSUSE", "opensuse.ascii"},
                                      {"Linux Mint", "linuxmint.ascii"},
                                      {"EndeavourOS", "endeavouros.ascii"},
                                      {"Pop!_OS", "pop!_os.ascii"},
                                      {"Gentoo", "gentoo.ascii"},
                                      {"elementary OS", "elementaryos.ascii"},
                                      {"Slackware", "slackware.ascii"},
                                      {"Asahi Linux", "asahi.ascii"},
                                      {"Peppermint", "peppermintos.ascii"},
                                      {"CentOS", "centos.ascii"},
                                      {"Lubuntu", "lubuntu.ascii"},
                                      {"Navy Linux", "navylinux.ascii"},
                                      {"BlackArch", "blackarch.ascii"},
                                      {"SteamOS", "steamos.ascii"},
                                      {"MX", "mxlinux.ascii"},
                                      {"Linux Lite", "linuxlite.ascii"},
                                      {"Bodhi", "bodhilinux.ascii"},
                                      {"Xubuntu", "xubuntu.ascii"},
                                      {"Kubuntu", "kubuntu.ascii"},
                                      {"Rocky", "rocky.ascii"},
                                      {"Deepin", "deepin.ascii"},
                                      {"Zorin", "zorin.ascii"},
                                      {"Garuda", "garudalinux.ascii"}};

    for (const auto &[key, value] : ascii_arts)
    {
        if (os.find(key) != std::string::npos)
        {
            printProcess(value, color_name);
            return;
        }
    }

    printProcess("linux.ascii", color_name);

    return;
}
