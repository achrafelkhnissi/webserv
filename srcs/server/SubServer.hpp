
#ifndef WEBSERV_SUBSERVER_H
#define WEBSERV_SUBSERVER_H

#include <iostream>
#include <vector>
#include "CGIHandler.hpp"
#include "Configuration.hpp"
#include "typedefs.hpp"

struct location_s{
	stringVector_t  allowedMethods;
    stringVector_t  index;
    stringVector_t  cgi_path;
    stringVector_t  errorPages;
    stringVector_t  redirect;
	string          redirection;
	string          root;
	string          clientMaxBodySize;
	string          prefix;
    string          uploadPath;
	string          autoIndex;
};

class SubServer {

private:
	stringVector_t      _serverName;
    stringVector_t      _redirect;
    locationVector_t    _locations;
	string              _root;
	stringVector_t      _index;
	stringVector_t 		_errorPages;
	string              _clientMaxBodySize;
    stringVector_t      _allowedMethods;
    string              _uploadPath;

public:
    explicit SubServer(const ServerConfig& config);
    ~SubServer();

    void                    printData() const;
    void                    fillLocation(const LocationConfig& locationConfig, location_t& location);
    const string&           getRoot() const;
    const stringVector_t&   getIndex() const;
    const stringVector_t&   getErrorPages() const;
    const string&           getClientMaxBodySize() const;
    const stringVector_t&   getServerName() const;
    const locationVector_t& getLocation() const;
    const stringVector_t&   getAllowedMethods() const;
    const string&           getUploadPath() const;
    const stringVector_t&   getRedirect() const;
};


#endif //WEBSERV_SUBSERVER_H
