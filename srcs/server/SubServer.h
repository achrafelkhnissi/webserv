//
// Created by Achraf El khnissi on 2/18/23.
//

#ifndef WEBSERV_SUBSERVER_H
#define WEBSERV_SUBSERVER_H

#include <iostream>
#include <vector>

typedef struct {

	std::vector<std::string>    accepted_methods;
    std::vector<std::string>    index;
	std::string                 redirection; // 301, 302, 307
	std::string                 root;
	std::string                 client_max_body_size;
	std::string                 error_pages;
	std::string                 prefix;
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
    location                    _location;
    // TODO: add Cgi

public:
    SubServer();
    ~SubServer();

    const std::vector<std::string>&    getServerName() const;
    const std::string&                 getRoot() const;
    const std::string&                 getIndex() const;
    const std::string&                 getErrorPages() const;
    const std::string&                 getClientMaxBodySize() const;
    const location&                    getLocation() const;

};


#endif //WEBSERV_SUBSERVER_H
