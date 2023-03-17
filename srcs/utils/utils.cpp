#include "utils.hpp"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cstdlib>

// function to check if a directory exists
bool dirExists(const std::string& dirName) {
    struct stat st = {};
    if (stat(dirName.c_str(), &st) == 0) {
        return (st.st_mode & S_IFDIR) != 0;
    }
    return false;
}

// function to create a directory
bool createDir(const std::string& dirName) {
    int status = mkdir(dirName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return status == 0;
}

bool isDirectory(const std::string &dirPath) {
    struct stat info = {};
    return stat(dirPath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}


std::string getBasename(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos) {
        return path.substr(pos + 1);
    }
    return path;
}

void error(const std::string &msg, int err)  {
    std::string errorMsg = msg + (!err ? "." : (std::string(" | ") + strerror(errno)));
    throw std::runtime_error(errorMsg);
}

std::string getFileContent(const std::string& path)  {
    std::ifstream file_(path, std::ios::binary);

    std::stringstream fileContent_;
    std::string line_;
    while (std::getline(file_, line_)) {
        fileContent_ << line_ << std::endl;
    }
    return fileContent_.str();
}

// Convert a string in the format "<number>k", "<number>m", or "<number>g" to bytes.
size_t convertToBytes(const std::string& str)
{
    const size_t multiplier = 1024;
    size_t factor = 1;
    std::string numStr = str.substr(0, str.length() - 1); // remove the last character ("k", "m", or "g")
    long num = strtol(numStr.c_str(), NULL, 10); // convert the number string to a long
    if (num <= 0)
    {
        throw std::invalid_argument("Invalid size: " + str);
    }
    switch (str[str.length() - 1])
    {
        case 'k':
        case 'K':
            factor *= multiplier;
        case 'm':
        case 'M':
            factor *= multiplier;
        case 'g':
        case 'G':
            factor *= multiplier;
            break;
        default:
            throw std::invalid_argument("Invalid size: " + str);
    }
    return static_cast<size_t>(num * factor);
}