#ifndef UTILS_H
#define UTILS_H

#include <string>

bool dirExists(const std::string& dirName);
bool createDir(const std::string& dirName);

#define ITER_FOREACH(Klass, list, varname)                                                         \
	for (Klass::iterator varname = list.begin(); varname != list.end(); varname++)
#define ITER_FOREACH_CONST(Klass, list, varname)                                                   \
	for (Klass::const_iterator varname = list.begin(); varname != list.end(); varname++)

bool isDirectory(const std::string &dirPath) ;
std::string getBasename(const std::string& path);
void error(const std::string &msg, int err = 0);
std::string getFileContent(const std::string& path);
size_t convertToBytes(const std::string& str);
std::string removeTrailingSlashes(std::string str);

#endif

