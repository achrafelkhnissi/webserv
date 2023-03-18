//
// Created by Achraf El khnissi on 2/18/23.
//

#ifndef WEBSERV_SUBSERVER_H
#define WEBSERV_SUBSERVER_H

#include <iostream>
#include <vector>
#include "CGIHandler.hpp"
#include "Configuration.hpp"  // Configuration class
#include "typedefs.hpp"

struct location_s{
	stringVector_t  allowedMethods;
    stringVector_t  index;
	string          redirection; // 301, 302, 307
	string          root;
	string          clientMaxBodySize;
	string          errorPages;
	string          prefix;
    string          uploadPath;
	string          autoIndex;
    vector<string>  cgi_path;
};

class SubServer {

private:
	stringVector_t      _serverName;
    locationVector_t    _locations;
	string              _root;
	stringVector_t      _index;
	vector<string>		_errorPages;
	string              _clientMaxBodySize;
    stringVector_t     _allowedMethods;
    // TODO: add Cgi

public:
    explicit SubServer(const ServerConfig& config);
    ~SubServer();

    void                    printData() const;
    void                    fillLocation(const LocationConfig& locationConfig, location_t& location);
    const string&           getRoot() const;
    const stringVector_t&   getIndex() const;
    const vector<string>&           getErrorPages() const;
    const string&           getClientMaxBodySize() const;
    const stringVector_t&   getServerName() const;
    const locationVector_t& getLocation() const;
};


#endif //WEBSERV_SUBSERVER_H
