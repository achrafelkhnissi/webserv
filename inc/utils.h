#ifndef UTILS_H
#define UTILS_H

#define ITER_FOREACH(Klass, list, varname)                                                         \
	for (Klass::iterator varname = list.begin(); varname != list.end(); varname++)
#define ITER_FOREACH_CONST(Klass, list, varname)                                                   \
	for (Klass::const_iterator varname = list.begin(); varname != list.end(); varname++)

#endif
