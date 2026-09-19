#include "Path.hpp"

Path Path::of(const std::string &path)
{
    return Path(std::filesystem::path(path), std::filesystem::status(path));
}

Path Path::getFilename()
{
    std::filesystem::path p = this->path.filename();
    return Path(p, std::filesystem::status(p));
}

bool Path::isRegularFile()
{
    return std::filesystem::is_regular_file(status);
}

bool Path::isExecutable()
{
    if (status.permissions() == std::filesystem::perms::unknown)
        return false;
    return (status.permissions() & std::filesystem::perms::others_exec) !=
           std::filesystem::perms::none;
}

bool Path::isDirectory()
{
    return std::filesystem::is_directory(status);
}

std::string Path::toString() const
{
    return path.string();
}

std::vector<Path> Path::getDirectoryContents()
{
    std::vector<Path> contents;
    if (!this->isDirectory())
        return contents;

    for (const auto &entry : std::filesystem::directory_iterator(path))
        contents.push_back(Path::of(entry.path()));
    return contents;
}

Path::Path(std::filesystem::path path, std::filesystem::file_status status)
{
    this->path = path;
    this->status = status;
}

