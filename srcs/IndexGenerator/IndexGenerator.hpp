#ifndef WEBSERV_INDEXGENERATOR_HPP
#define WEBSERV_INDEXGENERATOR_HPP

#include <string>
#include <vector>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

class IndexGenerator {

public:
	explicit IndexGenerator(const std::string&, const std::string&);
	std::string generate();

private:
    std::string _path;
	std::string _root;
	std::string _uri;
	std::string _tab;

    // List all regular files in a directory
    std::vector<std::string> _listDirectoryFiles(const std::string& path);

    // Get the size of a file
    off_t _getFileSize(const std::string& filename);

    // Get the modification time of a file
    time_t _getFileMtime(const std::string& filename);

    // Check if a file is a directory
    bool _isDirectory(const std::string& filename);

    // Format a time_t value as a string
    std::string _formatTime(time_t t);
};

#endif //WEBSERV_INDEXGENERATOR_HPP
