#include "Configuration.hpp"
#include <iostream>
#include <sys/_types/_size_t.h>

LocationConfig fill_location(toml::table& location) {
	(void)location;
	LocationConfig l;
	l.path = "/";
	l.root = "/var/www/html";
	l.index = "index.html";
	l.autoindex = "off";
	l.upload_path = "/var/www/html/upload";
	l.upload_store = "on";
	l.cgi_path = "/var/www/html/cgi";
	l.cgi_extension = ".cgi";
	l.cgi_pass = "";
	l.client_max_body_size = "10m";
	l.error_page = "404 /404.html";

	l.path = location["path"].as_str(l.path);
	l.root = location["root"].as_str(l.root);
	l.index = location["index"].as_str(l.index);
	l.autoindex = location["autoindex"].as_str(l.autoindex);
	l.upload_path = location["upload_path"].as_str(l.upload_path);
	l.upload_store = location["upload_store"].as_str(l.upload_store);
	l.cgi_path = location["cgi_path"].as_str(l.cgi_path);
	l.cgi_extension = location["cgi_extension"].as_str(l.cgi_extension);
	l.cgi_pass = location["cgi_pass"].as_str(l.cgi_pass);
	l.client_max_body_size = location["client_max_body_size"].as_str(l.client_max_body_size);
	l.error_page = location["error_page"].as_str(l.error_page);
	return l;
}

ServerConfig fill_server(toml::table& server) {
	ServerConfig s;
	s.listen = "80";
	s.host = "10.0.0.1";
	s.server_name = "example.com";
	s.root = "/var/www/html";
	s.error_page = "404 /404.html";
	s.client_max_body_size = "10m";

	s.listen = server["listen"].as_str(s.listen);
	s.host = server["host"].as_str(s.host);
	s.server_name = server["server_name"].as_str(s.server_name);
	s.root = server["root"].as_str(s.root);
	s.error_page = server["error_page"].as_str(s.error_page);
	s.client_max_body_size = server["client_max_body_size"].as_str(s.client_max_body_size);

	toml::table& t = server["location"];
	std::cout << t.vec.size() << std::endl;
	for (size_t i = 0; i < t.vec.size(); i++) {
		s.locations.push_back(fill_location(server["location"]));
	}
	return s;
}

Configuration::Configuration(toml::table& config) {

	toml::table& t = config["server"];
	if (t.is_type(toml::table::NONE))
		return;
	for (size_t i = 0; i < t.vec.size(); i++) {
		servers.push_back(fill_server(t[i]));
		servers.back().print();
	}
}

int Configuration::get_port() {
	return _port;
}

std::string Configuration::get_host() {
	return _host;
}

std::string Configuration::get_root() {
	return _root;
}

std::string Configuration::get_log_path() {
	return _log_path;
}

void Configuration::parse_config_file(std::string config_file) {
	(void)config_file;
	// TODO: parse config file
}
