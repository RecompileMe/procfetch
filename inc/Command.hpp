#pragma once
#include <vector>
#include <string>
#include <functional>
#include <sstream>
#include <thread>
#include <mutex>
#include <cstring>
#include <csignal>
#include <sys/wait.h>
#include "Path.hpp"

/**
 * Command executes a command in a subshell(shell runs on a forked process).
 *
 * Sample code:
 * ```cpp
 * auto c = Command::exec("ls -l");
 * if (c.getExitcode() == 0) {
 *     std::cout << c.getOutput();
 * }
 * ```
 */
class Command
{
  public:
    typedef std::function<void(Command)> func_type;
    /**
     * Wait for all threads to be finished.
     */
    static void wait();

    /**
     * @returns exceptions
     */
    static std::vector<std::runtime_error> &getExceptions();

    /**
     * Executes the specified command in a new thread.
     * @param cmd containing the command to call and its arguments
     * @param func to be performed after cmd is finished
     */
    static void exec_async(const std::string &cmd, const func_type &func);

    /**
     * Executes the specified command in a new thread.
     * @param cmd containing the command
     * @param arg arguments
     * @param func to be performed after cmd is finished
     */
    static void exec_async(const Path &cmd, const std::string &arg,
                           const func_type &func);

    /**
     * Executes the specified command.
     * @param cmd containing the command to call and its arguments
     * @returns Command object for getting the results.
     * @throws runtime_error failed to popen(3)
     */
    static Command exec(const std::string &cmd);

    /**
     * @returns get contents written by the command to standard output
     */
    std::string getOutput();

    /**
     * @returns get contents written by the command to standard error output
     */
    std::string getErrorOutput();

    /**
     * @returns get the new line counts of the output.
     */
    int getOutputLines();

    /**
     * @returns get the exit code of the command.
     */
    int getExitCode();

private:
    int exit_code;
    std::string output;
    std::string error_output;
    int lines;
    static std::vector<std::thread> ths;
    static std::vector<std::runtime_error> exceptions;
    static std::mutex mtx;

    Command();
    static void split(std::vector<char *> &v, std::string cmd);
};
