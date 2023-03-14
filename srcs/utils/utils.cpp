#include "utils.hpp"
#include <sys/stat.h>
#include <iostream>

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