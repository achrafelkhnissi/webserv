#include "utils.hpp"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>

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

