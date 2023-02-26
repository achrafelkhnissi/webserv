#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "../lib/toml/data/table.hpp"
#include "../lib/toml/toml.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

typedef std::vector<std::string> string_vector;
struct LocationConfig {

	std::string path;
	std::string root;
	std::string index;
	std::string autoindex;
	std::string upload_path;
	std::string upload_store;
	std::string cgi_path;
	std::string cgi_extension;
	std::string cgi_pass;
	std::string client_max_body_size;
	std::string error_page; // error_page can be a list of pages

	void print(int indent = 0) {
		std::string s(indent, ' ');
		std::cout << s << "path: " << path << std::endl;
		std::cout << s << "root: " << root << std::endl;
		std::cout << s << "index: " << index << std::endl;
		std::cout << s << "autoindex: " << autoindex << std::endl;
		std::cout << s << "upload_path: " << upload_path << std::endl;
		std::cout << s << "upload_store: " << upload_store << std::endl;
		std::cout << s << "cgi_path: " << cgi_path << std::endl;
		std::cout << s << "cgi_extension: " << cgi_extension << std::endl;
		std::cout << s << "cgi_pass: " << cgi_pass << std::endl;
		std::cout << s << "_clientMaxBodySize: " << client_max_body_size << std::endl;
		std::cout << s << "error_page: " << error_page << std::endl;
		std::cout << std::endl;
	}
};

struct ServerConfig {

	unsigned short port;
	std::string host;
	std::vector<std::string> server_name;
	std::string root;
	std::string error_page; // error_page can be a list of pages
	std::string client_max_body_size;

	std::vector<LocationConfig> locations;

	void print() {
		std::cout << "port: " << port << std::endl;
		std::cout << "host: " << host << std::endl;
		std::cout << "server_name: ";
		ITER_FOREACH(string_vector, server_name, it) {
			std::cout << *it << " ";
		}
		std::cout << std::endl;
		std::cout << "root: " << root << std::endl;
		std::cout << "error_page: " << error_page << std::endl;
		std::cout << "_clientMaxBodySize: " << client_max_body_size << std::endl;
		for (size_t i = 0; i < locations.size(); i++) {
			std::cout << "location " << i << ":" << std::endl;
			locations[i].print(1);
		}
		std::cout << std::endl;
	}
};

class Configuration {

private:
	vector<ServerConfig> _servers;

public:
	Configuration(toml::table& config);
    ~Configuration();

    const vector<ServerConfig>& getServers() const;
};

#endif // CONFIGURATION_HPP
