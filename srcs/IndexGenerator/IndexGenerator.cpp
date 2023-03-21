
#include "IndexGenerator.hpp"
#include "utils.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

IndexGenerator::IndexGenerator(const std::string& root, const std::string& uri) {
	_tab = "&#9;";
	_path = removeTrailingSlashes(root + uri) + "/";
	_root = root;
	_uri = uri;
}

std::string IndexGenerator::generate() {
	std::vector<std::string> filenames = _listDirectoryFiles(_path);

	std::string html;
	html += "<html><head><title>Index of " + _path + "</title></head><body>\n";
	html += "<h1>Index of " + _path + "</h1><hr><pre>\n";
	html += "<span style=\"display: inline-block; width: 30em;\">Name</span>";
	html += "<span style=\"display: inline-block; width: 8em;\">Size</span>";
	html += "<span style=\"display: inline-block; width: 15em; text-align: right;\">Last Modified</span>";
	html += "<hr>\n";

	for (std::vector<std ::string>::iterator filename = filenames.begin(); filename != filenames.end(); ++filename) {
		std::string filepath = _path + *filename;

		bool is_dir = _isDirectory(filepath);
		std::string name = *filename;
		if (is_dir && name.back() != '/') {
			name += "/";
		}
		std::string uri_path = (_uri == "/") ? "" : _uri;
		std::string size_str = is_dir ? "-" : std::to_string(_getFileSize(filepath));
		std::string mtime_str = _tab + _formatTime(_getFileMtime(filepath));
		std::string link = "<a href=\"" + uri_path + "/" + *filename + "\">" + name + "</a>";
		html += "<span style=\"display: inline-block; width: 30em;\">" + link + "</span>";
		html += "<span style=\"display: inline-block; width: 8em;\">" + size_str + "</span>";
		html += "<span style=\"display: inline-block; width: 15em; text-align: right;\">" + mtime_str + "</span>\n";
	}

	html += "</pre><hr></body></html>\n";
	return html;
}

// List all regular files in a directory
std::vector<std::string> IndexGenerator::_listDirectoryFiles(const std::string& path) {
	std::vector<std::string> filenames;

	DIR* dir = opendir(path.c_str());
	if (!dir) {
		std::cerr << "Failed to open directory " << path << "\n";
		return filenames;
	}
	dirent* entry;
	while ((entry = readdir(dir))) {
		filenames.push_back(entry->d_name);
	}
	closedir(dir);
	return filenames;
}

// Get the size of a file
off_t IndexGenerator::_getFileSize(const std::string& filename) {
	struct stat sb;
	if (stat(filename.c_str(), &sb) == -1) {
		std::cerr << "Failed to get file status for " << filename << "\n";
		return -1;
	} else {
		return sb.st_size;
	}
}

// Get the modification time of a file
time_t IndexGenerator::_getFileMtime(const std::string& filename) {
	struct stat sb;
	if (stat(filename.c_str(), &sb) == -1) {
		std::cerr << "Failed to get file status for " << filename << "\n";
		return -1;
	} else {
		return sb.st_mtime;
	}
}

// Check if a file is a directory
bool IndexGenerator::_isDirectory(const std::string& filename) {
	struct stat sb;
	if (stat(filename.c_str(), &sb) == -1) {
		std::cerr << "Failed to get file status for " << filename << "\n";
		return false;
	} else {
		return S_ISDIR(sb.st_mode);
	}
}

// Format a time_t value as a string
std::string IndexGenerator::_formatTime(time_t t) {
	char buf[80];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
	return std::string(buf);
}
