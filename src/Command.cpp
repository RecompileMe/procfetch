#include "Command.hpp"

Command::Command(): output(std::string()), lines(0), exit_code(0){ }

void Command::split(std::vector<char *> &v, std::string cmd)
{
    std::istringstream ss{cmd};
    for (std::string arg{}; getline(ss, arg, ' '); ) {
        if (arg == "")
            continue;

        auto s = strdup(arg.c_str());
        if (s == NULL)
            throw std::runtime_error("strdup failed");
        v.push_back(s);
    }
    v.push_back((char *)0);
}

void Command::wait()
{
    for (auto &t : ths)
        if (t.joinable())
            t.join();
}

std::vector<std::runtime_error>& Command::getExceptions()
{
    return exceptions;
}

void Command::exec_async(const std::string &cmd, const Command::func_type &func)
{
    ths.push_back(std::thread([=]() {
        try
        {
            auto result = exec(cmd);
            func(result);
        }
        catch (const std::runtime_error &e)
        {
            std::lock_guard<std::mutex> lock(mtx);
            exceptions.push_back(e);
        }
    }));
}

void Command::exec_async(const Path &cmd, const std::string &arg, const Command::func_type &func)
{
    exec_async(cmd.toString() + " " + arg, func);
}

Command Command::exec(const std::string &cmd)
{
    auto result = Command{};
    int out_fd[2], err_fd[2];
    pid_t pid;

    if (pipe(out_fd) == -1)
        throw std::runtime_error("pipe failed");
    if (pipe(err_fd) == -1)
        throw std::runtime_error("pipe failed");

    if ((pid = fork()) < 0)
        throw std::runtime_error("fork faliled");
    else if (pid == 0)
    { // child
        if (close(out_fd[0]) == -1)
            throw std::runtime_error("close failed");
        if (close(err_fd[0]) == -1)
            throw std::runtime_error("close failed");
        if (dup2(out_fd[1], fileno(stdout)) == -1)
            throw std::runtime_error("dup2 failed");
        if (dup2(err_fd[1], fileno(stderr)) == -1)
            throw std::runtime_error("dup2 failed");

        std::vector<char *>v{};
        split(v, cmd);
        auto argv = v.data();
        execvp(argv[0], argv);

        // If execvp() returns, an error have occured.
        // As the process terminates immediately, it does not free the dynamically allocated memory tied to the argv.
        switch (errno)
        {
            case ENOENT:
                exit(127);
            case EACCES:
                exit(126);
            default:
                throw std::runtime_error("execvp failed: " +
                                         std::string(strerror(errno)) + ": " + argv[0]);
        }
    }

    // parent
    if (close(out_fd[1]) == -1)
        throw std::runtime_error("close failed");
    if (close(err_fd[1]) == -1)
        throw std::runtime_error("close failed");
    FILE *out = fdopen(out_fd[0], "r");
    if (out == NULL)
        throw std::runtime_error("fdopen failed");
    FILE *err = fdopen(err_fd[0], "r");
    if (err == NULL)
        throw std::runtime_error("fdopen failed");

    int c;
    while ((c = fgetc(out)) != EOF)
    {
        if (c == '\n')
            result.lines += 1;
        result.output += c;
    }

    while ((c = fgetc(err)) != EOF)
        result.error_output += c;

    if (fclose(out) == EOF)
        throw std::runtime_error("fclose failed: " + std::string(strerror(errno)));

    if (fclose(err) == EOF)
        throw std::runtime_error("fclose failed: " + std::string(strerror(errno)));

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
        result.exit_code = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
    {
        int sig = WTERMSIG(status);
        throw std::runtime_error("abnormal termination, signal number = " +
                                 std::to_string(sig));
    }
    else if (WIFSTOPPED(status))
    {
        int sig = WSTOPSIG(status);
        throw std::runtime_error("child stopped, signal number = " +
                                 std::to_string(sig));
    }
    else
        throw std::runtime_error("must not be here");

    return result;
}

std::string Command::getOutput()
{
    return output;
}

std::string Command::getErrorOutput()
{
    return error_output;
}

int Command::getOutputLines()
{
    return lines;
}

int Command::getExitCode()
{
    return exit_code;
}