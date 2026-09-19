/**
 * @file
 */
#pragma once
#include <vector>
#include <string>

std::string getuser();
std::string gethostname(const std::string &path);
std::string getOS(std::string path);
std::string getHardwarePlatform();
std::string getHost(std::string path);
std::string getKernel(std::string path);
std::string getUpTime(std::string path);
std::string getRAM(std::string path);
std::string getSHELL(std::string path);
std::string getDE();
std::string getRES(std::string path);
std::string getTheme();
std::string getIcons();
std::string getCPU(std::string path);
std::vector<std::string> getGPU();
std::string getPackages();
std::string getColor(std::string);
bool resCheck();
bool CpuTempCheck();
int getCPUtemp(std::string path);
void printBattery(std::string path);
void print(std::string color, std::string distro_name);
