//
// Created by Achraf El khnissi on 2/18/23.
//

#ifndef WEBSERV_SUBSERVER_H
#define WEBSERV_SUBSERVER_H

#include <iostream>
//#include <vector>
#include "Configuration.hpp"  // Configuration class
#include "typedefs.hpp"

struct location_s{

	stringVector_t          allowed_methods;
    string                  index;
	string                  redirection; // 301, 302, 307
	string                  root;
	string                  client_max_body_size;
	string                  error_pages;
	string                  prefix;
	bool                    auto_index;
    // TODO: add Cgi

};

class SubServer {

private:
	stringVector_t          _server_name;
    locationVector_t        _locations;
	string                 _root;
	string                 _index;
	string                 _error_pages;
	string                 _client_max_body_size;
    // TODO: add Cgi

public:
    explicit SubServer(const ServerConfig& config);
    ~SubServer();

    const stringVector_t&    getServerName() const;
    const string&                 getRoot() const;
    const string&                 getIndex() const;
    const string&                 getErrorPages() const;
    const string&                 getClientMaxBodySize() const;
    const locationVector_t &            getLocation() const;
    void    fill_location(const LocationConfig& location_config, location_t& location);
    void    print_data() const;

};


#endif //WEBSERV_SUBSERVER_H
