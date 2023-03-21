#ifndef WEBSERV_CGIHANDLER_HPP
#define WEBSERV_CGIHANDLER_HPP

#include <iostream>
#include <string>
#include "Request.hpp"
#include "typedefs.hpp"

class CGIHandler {
public:
    CGIHandler(const stringMap_t &env, const string &body, location_t *location);

private:

    stringVector_t  _cgiPath;
	string          _responseBody;
    string          _requestBody;
    size_t          _envSize;
    stringMap_t     _envMap;

    char            **_env;
    char            **_argv;


public:
	~CGIHandler();
    string getCmd();
    void setArgs(string);
    string CGIExecuter();
};


#endif //WEBSERV_CGIHANDLER_HPP
