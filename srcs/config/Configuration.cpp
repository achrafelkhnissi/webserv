#include "Configuration.hpp"
#include <iostream>
#include <sys/_types/_size_t.h>

LocationConfig fill_location(toml::table& location) {
	(void)location;
	LocationConfig l;

	l.path = location["path"].as_str("/");
	l.root = location["root"].as_str("/var/www/html");
	l.index = location["index"].as_str("index.html");
	l.autoindex = location["autoindex"].as_str("off");
	l.upload_path = location["upload_path"].as_str("/var/www/html/upload");
	l.upload_store = location["upload_store"].as_str("on");
	l.cgi_path = location["cgi_path"].as_str("/var/www/html/cgi");
	l.cgi_extension = location["cgi_extension"].as_str(".cgi");
	l.cgi_pass = location["cgi_pass"].as_str("");
	l.client_max_body_size = location["client_max_body_size"].as_str("10m");
	l.error_page = location["error_page"].as_str("404 /404.html");
	return l;
}

ServerConfig fill_server(toml::table& server) {
	ServerConfig s;

	s.listen = server["listen"].as_int(s.listen);
	s.host = server["host"].as_str("127.0.0.1");
	s.server_name = server["server_name"].as_str("example.com");
	s.root = server["root"].as_str("/var/www/html");
	s.error_page = server["error_page"].as_str("404 /404.html");
	s.client_max_body_size = server["client_max_body_size"].as_str("10m");

	toml::table& t = server["location"];
	for (size_t i = 0; i < t.vec.size(); i++) {
		s.locations.push_back(fill_location(t[i]));
	}
	return s;
}

Configuration::Configuration(toml::table& config) {

	toml::table& t = config["server"];
	if (t.is_type(toml::table::NONE))
		return;
	for (size_t i = 0; i < t.vec.size(); i++) {
		_servers.push_back(fill_server(t[i]));
//		_servers.back().print();
	}
}

Configuration::~Configuration() {

}

const vector<ServerConfig> &Configuration::getServers() const {
    return _servers;
}
