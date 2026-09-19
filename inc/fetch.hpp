/**
 * @file
 */
#pragma once
#include <vector>
#include <string>

std::string getuser();
std::string gethostname(const std::string &path);
std::string getOS(const std::string &path);
std::string getHardwarePlatform();
std::string getHost(const std::string &path);
std::string getKernel(const std::string &path);
std::string getUpTime(const std::string &path);
std::string getRAM(const std::string &path);
std::string getSHELL(const std::string &path);
std::string getDE();
std::string getRES(const std::string &path);
std::string getTheme();
std::string getIcons();
std::string getCPU(const std::string &path);
std::vector<std::string> getGPU();
std::string getPackages();
std::string getColor(const std::string &color);
bool resCheck();
bool CpuTempCheck();
int getCPUtemp(const std::string &path);
void printBattery(const std::string &path);
void print(std::string &color, std::string &distro_name);
