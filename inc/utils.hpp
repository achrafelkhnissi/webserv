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
#endif

