#include "Configuration.hpp"
#include "table.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sys/_types/_size_t.h>

LocationConfig fill_location(toml::table& location) {
	(void)location;
	LocationConfig l;

	l.prefix = location["prefix"].as_str("/");
	l.root = location["root"].as_str("/var/www/html");
    ITER_FOREACH(vector<toml::table>, location["index"].vec, it) {
        l.index.push_back(it->as_str("default.com"));
    }
    ITER_FOREACH(vector<toml::table>, location["allowed_methods"].vec, it) {
        l.allowed_methods.push_back(it->as_str(""));
    }
	l.autoindex = location["autoindex"].as_str("off");
	l.upload_path = location["upload_path"].as_str("www/html/upload");
	l.upload_store = location["upload_store"].as_str("on");
	l.cgi_path = location["cgi_path"].as_str("www/html/cgi");
	l.cgi_extension = location["cgi_extension"].as_str(".cgi");
	l.cgi_pass = location["cgi_pass"].as_str("");
	l.client_max_body_size = location["_clientMaxBodySize"].as_str("10m");
	l.error_page = location["error_page"].as_str("404 /errors/error-404.html");
	return l;
}

ServerConfig fill_server(toml::table& server) {
	ServerConfig s;
	s.port = server["port"].as_int(s.port);
	s.host = server["host"].as_str("127.0.0.1");

    ITER_FOREACH(vector<toml::table>, server["index"].vec, it) {
        s.index.push_back(it->as_str("default.com"));
    }

	ITER_FOREACH(vector<toml::table>, server["server_name"].vec, it) {
//		s.server_name.push_back(it->as_str("localhost"));
		s.server_name.push_back(it->as_str());
	}
    ITER_FOREACH(vector<toml::table>, server["allowed_methods"].vec, it) {
        s.allowed_methods.push_back(it->as_str(""));
    }
	s.root = server["root"].as_str("www");
	s.error_page = server["error_page"].as_str("404 /errors/error-404.html");
	s.client_max_body_size = server["_clientMaxBodySize"].as_str("10m");

	toml::table& t = server["location"];
	for (size_t i = 0; i < t.vec.size(); i++) {
		s.locations.push_back(fill_location(t[i]));
	}
	return s;
}

bool optional_is(toml::table &table, const string &key, toml::table::e_toml type) {
	return table[key].is_type(toml::table::NONE) || table[key].is_type(type);
}

bool optional_str_arr(toml::table &table, const string &key)
{
	toml::table& t = table[key];
	if (!optional_is(table, key, toml::table::ARRAY))
		return false;
	for (size_t i = 0; i < t.vec.size(); i++) {
		if (!t[i].is_type(toml::table::STRING))
			return false;
	}
	return true;
}

Configuration::e_error validate_server(toml::table& server) {
	string server_keys[] = {"port", "host", "index", "server_name", "allowed_methods", "root", "error_page", "_clientMaxBodySize", "location"};
	ITER_FOREACH(toml::table::TomlMap, server.mp, it) {
		if (find(begin(server_keys), end(server_keys), it->first) == end(server_keys))
			return Configuration::ERROR_UNKNOWN_KEY;
	}

	if (!optional_is(server, "port", toml::table::STRING))
		return Configuration::ERROR_INVALID_PORT;
	else if (!optional_is(server, "host", toml::table::STRING))
		return Configuration::ERROR_INVALID_HOST;
	else if (!optional_is(server, "_clientMaxBodySize", toml::table::STRING))
		return Configuration::ERROR_INVALID_CLIENT_MAX_BODY_SIZE;
	else if (!optional_str_arr(server, "index"))
		return Configuration::ERROR_INVALID_INDEX;
	else if (!optional_str_arr(server, "server_name"))
		return Configuration::ERROR_INVALID_SERVER_NAME;
	else if (!optional_str_arr(server, "allowed_methods"))
		return Configuration::ERROR_INVALID_ALLOWED_METHODS;
	else if (!optional_is(server, "root", toml::table::STRING))
		return Configuration::ERROR_INVALID_ROOT;
	else if (!optional_str_arr(server, "error_page"))
		return Configuration::ERROR_INVALID_ERROR_PAGE;
	return Configuration::ERROR_NONE;
}

Configuration::e_error Configuration::validate_keys(toml::table& config) {
	//string location_keys[] = {"prefix", "root", "index", "allowed_methods", "autoindex", "upload_path", "upload_store", "cgi_path", "cgi_extension", "cgi_pass", "error_page", "_clientMaxBodySize"};
	
	toml::table& t = config["server"];
	if (t.is_type(toml::table::NONE) || !t.is_type(toml::table::ARRAY))
		return Configuration::ERROR_INVALID_SERVER;

	for (size_t i = 0; i < t.vec.size(); i++) {
		if ((error = validate_server(t[i])) != Configuration::ERROR_NONE)
			return error;
	}

	return Configuration::ERROR_NONE;
}

Configuration::Configuration(toml::table& config) {
	if ((error = validate_keys(config) )!= Configuration::ERROR_NONE)
	{
		print();
		cout << "ERROR: Invalid configuration file" << endl;
		exit(1);
	}
	toml::table& t = config["server"];
	for (size_t i = 0; i < t.vec.size(); i++) {
		_servers.push_back(fill_server(t[i]));
	}
}

Configuration::~Configuration() {

}

const vector<ServerConfig> &Configuration::getServers() const {
    return _servers;
}

void Configuration::print() {
	cout << "\n<==================== Configuration ====================>" << endl;

	for (int i = 0; i < _servers.size(); i++)
		_servers[i].print();
	
	switch (error) {
		case ERROR_NONE:
			cout << "ERROR_NONE" << endl;
			break;
		case ERROR_INVALID_SERVER:
			cout << "ERROR_INVALID_SERVER" << endl;
			break;
		case ERROR_INVALID_PORT:
			cout << "ERROR_INVALID_PORT" << endl;
			break;
		case ERROR_INVALID_HOST:
			cout << "ERROR_INVALID_HOST" << endl;
			break;
		case ERROR_UNKNOWN_KEY:
			cout << "ERROR_UNKNOWN" << endl;
			break;
		case ERROR_INVALID_INDEX:
			cout << "ERROR_INVALID_INDEX" << endl;
			break;
		case ERROR_INVALID_SERVER_NAME:
			cout << "ERROR_INVALID_SERVER_NAME" << endl;
			break;
		case ERROR_INVALID_ALLOWED_METHODS:
			cout << "ERROR_INVALID_ALLOWED_METHODS" << endl;
			break;

		case ERROR_INVALID_ROOT:
			cout << "ERROR_INVALID_ROOT" << endl;
			break;
		case ERROR_INVALID_ERROR_PAGE:
			cout << "ERROR_INVALID_ERROR_PAGE" << endl;
			break;
		case ERROR_INVALID_CLIENT_MAX_BODY_SIZE:
			cout << "ERROR_INVALID_CLIENT_MAX_BODY_SIZE" << endl;
			break;
		case ERROR_INVALID_LOCATION:
			cout << "ERROR_INVALID_LOCATION" << endl;
			break;
	}

	cout << "\n<==================== END Configuration ====================>\n" << endl;
}
