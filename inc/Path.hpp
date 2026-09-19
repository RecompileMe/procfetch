#pragma once
#include <filesystem>
#include <vector>

/**
 * A Path represents a path.
 */
class Path
{
public:
    /**
     * @returns Path object
     * @param path
     */
    static Path of(const std::string &path);

    /**
     * @returns the filename of the path.
     */
    Path getFilename();

    /**
     * @returns exists and is a regular file
     */
    bool isRegularFile();

    /**
     * @returns exists and is a executable(or searchable)
     */
    bool isExecutable();

    /**
     * @returns exists and is a directory
     */
    bool isDirectory();

    /**
     * @returns readable reprsentation for dev.
     */
    std::string toString() const;

    /**
     * @returns a vector containing absolute paths to contents at the given path
     */
    std::vector<Path> getDirectoryContents();

private:
    std::filesystem::path path;
    std::filesystem::file_status status;

    Path(std::filesystem::path path, std::filesystem::file_status status);
};