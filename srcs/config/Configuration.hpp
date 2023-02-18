#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "../lib/toml/data/table.hpp"
#include "../lib/toml/toml.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <string>

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
	std::string error_page;

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
		std::cout << s << "client_max_body_size: " << client_max_body_size << std::endl;
		std::cout << s << "error_page: " << error_page << std::endl;
		std::cout << std::endl;
	}
};

struct ServerConfig {

	std::string listen;
	std::string host;
	std::string server_name;
	std::string root;
	std::string error_page;
	std::string client_max_body_size;

	std::vector<LocationConfig> locations;

	void print() {
		std::cout << "listen: " << listen << std::endl;
		std::cout << "host: " << host << std::endl;
		std::cout << "server_name: " << server_name << std::endl;
		std::cout << "root: " << root << std::endl;
		std::cout << "error_page: " << error_page << std::endl;
		std::cout << "client_max_body_size: " << client_max_body_size << std::endl;
		for (size_t i = 0; i < locations.size(); i++) {
			std::cout << "location " << i << ":" << std::endl;
			locations[i].print(1);
		}
		std::cout << std::endl;
	}
};

class Configuration {

private:
	vector<ServerConfig> servers;
	int _port;
	std::string _host;
	std::string _root;
	std::string _log_path;

	void parse_config_file(std::string config_file);

public:
	Configuration(toml::table& config);

	int get_port();

	std::string get_host();
	std::string get_root();
	std::string get_log_path();
};

#endif // CONFIGURATION_HPP
