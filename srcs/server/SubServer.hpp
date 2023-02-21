//
// Created by Achraf El khnissi on 2/18/23.
//

#ifndef WEBSERV_SUBSERVER_H
#define WEBSERV_SUBSERVER_H

#include <iostream>
#include <vector>
#include "../config/Configuration.hpp"  // Configuration class

typedef struct {

	std::vector<std::string>    allowed_methods;
    std::vector<std::string>    index;
	std::string                 redirection; // 301, 302, 307
	std::string                 root;
	std::string                 client_max_body_size;
	std::string                 error_pages;
	std::string                 prefix;
    std::string                 path;
	bool                        auto_index;
    // TODO: add Cgi

} location;

class SubServer {

private:
	std::vector<std::string>    _server_name;
	std::string                 _root;
	std::string                 _index;
	std::string                 _error_pages;
	std::string                 _client_max_body_size;
    std::vector<location>       _locations;
    // TODO: add Cgi

public:
    explicit SubServer(const ServerConfig& config);
    ~SubServer();

    const std::vector<std::string>&    getServerName() const;
    const std::string&                 getRoot() const;
    const std::string&                 getIndex() const;
    const std::string&                 getErrorPages() const;
    const std::string&                 getClientMaxBodySize() const;
    const vector<location>&            getLocation() const;
    void    fill_location(const LocationConfig& location_config, location& location);
    void    print_data() const;

};


#endif //WEBSERV_SUBSERVER_H
