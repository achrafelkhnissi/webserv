#include "Configuration.hpp"
#include "table.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sys/_types/_size_t.h>

void fill_array(toml::table& t, string key, vector<string>& v, ServerConfig& s) {
    if (t[key].is_type(toml::table::NONE))
    {
        v = s.allowed_methods;
        return;
    }
    ITER_FOREACH(vector<toml::table>, t[key].vec, it) {
        v.push_back(it->as_str(""));
    }
}

LocationConfig fill_location(toml::table& location, ServerConfig& s) {
	(void)location;
	LocationConfig l;

	l.prefix = location["prefix"].as_str("/");
	l.root = location["root"].as_str("s.root");

    fill_array(location, "index", l.index, s);
    fill_array(location, "allowed_methods", l.allowed_methods, s);
    fill_array(location, "error_page", l.error_page, s);
    fill_array(location, "redirect", l.redirect, s);
    fill_array(location, "cgi_path", l.cgi_path, s);

	l.autoindex = location["autoindex"].as_str("off");
	l.upload_path = location["upload_path"].as_str(s.upload_path);
	l.client_max_body_size = location["_clientMaxBodySize"].as_str(s.client_max_body_size);
	return l;
}

ServerConfig fill_server(toml::table& server) {
	ServerConfig s;
	s.port = server["port"].as_int(s.port);
	s.host = server["host"].as_str("127.0.0.1");
	s.upload_path = server["upload_path"].as_str("www/upload");
	s.root = server["root"].as_str("www");
	s.client_max_body_size = server["_clientMaxBodySize"].as_str("1m");

    ITER_FOREACH(vector<toml::table>, server["index"].vec, it) {
        s.index.push_back(it->as_str("default.com"));
    }

	ITER_FOREACH(vector<toml::table>, server["redirect"].vec, it) {
		s.redirect.push_back(it->as_str(""));
	}

	ITER_FOREACH(vector<toml::table>, server["server_name"].vec, it) {
		s.server_name.push_back(it->as_str());
	}
    ITER_FOREACH(vector<toml::table>, server["allowed_methods"].vec, it) {
        s.allowed_methods.push_back(it->as_str(""));
    }
    ITER_FOREACH(vector<toml::table>, server["error_page"].vec, it) {
        s.error_page.push_back(it->as_str(""));
    }

	toml::table& t = server["location"];
	for (size_t i = 0; i < t.vec.size(); i++) {
		s.locations.push_back(fill_location(t[i], s));
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

Configuration::e_error validate_location(toml::table &location)
{
	string location_keys[] = {"prefix", "root", "upload_path", "autoindex", "redirect", "allowed_methods", "index", "cgi_path", "error_page", "_clientMaxBodySize"};
	ITER_FOREACH(toml::table::TomlMap, location.mp, it) {
		if (find(begin(location_keys), end(location_keys), it->first) == end(location_keys))
			return Configuration::ERROR_UNKNOWN_KEY;
	}

	if (!optional_is(location, "prefix", toml::table::STRING))
		return Configuration::ERROR_INVALID_PORT;
	else if (!optional_is(location, "root", toml::table::STRING))
		return Configuration::ERROR_INVALID_ROOT;
	else if (!optional_is(location, "upload_path", toml::table::STRING))
		return Configuration::ERROR_INVALID_UPLOAD_PATH;
	else if (!optional_is(location, "autoindex", toml::table::STRING))
		return Configuration::ERROR_INVALID_AUTOINDEX;
	else if (!optional_is(location, "client_max_body_size", toml::table::STRING))
		return Configuration::ERROR_INVALID_CLIENT_MAX_BODY_SIZE;
	else if (!optional_str_arr(location, "redirect"))
		return Configuration::ERROR_INVALID_REDIRECT;
	else if (!optional_str_arr(location, "cgi_path"))
		return Configuration::ERROR_INVALID_CGI_PATH;
	else if (!optional_str_arr(location, "index"))
		return Configuration::ERROR_INVALID_INDEX;
	else if (!optional_str_arr(location, "allowed_methods"))
		return Configuration::ERROR_INVALID_ALLOWED_METHODS;
	else if (!optional_str_arr(location, "error_page"))
		return Configuration::ERROR_INVALID_ERROR_PAGE;
	return Configuration::ERROR_NONE;
}

Configuration::e_error validate_location_list(toml::table &location) {
	Configuration::e_error error;
	if (!location.is_type(toml::table::ARRAY))
		return Configuration::ERROR_INVALID_LOCATION;
	for (size_t i = 0; i < location.vec.size(); i++) {
		error = validate_location(location[i]);
		if (error != Configuration::ERROR_NONE)
			return error;
	}
	return Configuration::ERROR_NONE;
}

Configuration::e_error validate_server(toml::table& server) {
	Configuration::e_error error;
	string server_keys[] = {"port", "host", "index", "server_name", "allowed_methods", "root", "error_page", "_clientMaxBodySize", "location", "upload_path", "redirect"};
	ITER_FOREACH(toml::table::TomlMap, server.mp, it) {
		if (find(begin(server_keys), end(server_keys), it->first) == end(server_keys))
			return Configuration::ERROR_UNKNOWN_KEY;
	}

	if (!optional_is(server, "port", toml::table::STRING))
		return Configuration::ERROR_INVALID_PORT;
	else if (!optional_is(server, "upload_path", toml::table::STRING))
		return Configuration::ERROR_INVALID_PORT;
	else if (!optional_is(server, "host", toml::table::STRING))
		return Configuration::ERROR_INVALID_HOST;
	else if (!optional_is(server, "_clientMaxBodySize", toml::table::STRING))
		return Configuration::ERROR_INVALID_CLIENT_MAX_BODY_SIZE;
	else if (!optional_str_arr(server, "redirect"))
		return Configuration::ERROR_INVALID_REDIRECT;
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
	else if ((error = validate_location_list(server["location"])) != Configuration::ERROR_NONE)
			return error;
	return Configuration::ERROR_NONE;
}

Configuration::e_error Configuration::pre_validate(toml::table& config) {
	
	toml::table& t = config["server"];
	if (t.is_type(toml::table::NONE) || !t.is_type(toml::table::ARRAY))
		return Configuration::ERROR_INVALID_SERVER;

	for (size_t i = 0; i < t.vec.size(); i++) {
		if ((error = validate_server(t[i])) != Configuration::ERROR_NONE)
			return error;
	}

	return Configuration::ERROR_NONE;
}

Configuration::e_error Configuration::post_validate() {
	if (_servers.empty())
		return Configuration::ERROR_INVALID_SERVER;
	for (size_t i = 0; i < _servers.size(); i++) {
		if (_servers[i].port < 0 || _servers[i].port > 65535 )
			return Configuration::ERROR_INVALID_PORT;
		if (_servers[i].error_page.size() < 2 && !_servers[i].error_page.empty())
			return Configuration::ERROR_INVALID_ERROR_PAGE;
		for (size_t j = 0; j < _servers[i].error_page.size() - 1; j++) {
			int code = atoi(_servers[i].error_page[j].c_str());
			if (code < 100 || code > 599)
				return Configuration::ERROR_INVALID_ERROR_PAGE;
		}
	}
	return Configuration::ERROR_NONE;
}

Configuration::Configuration(toml::table& config) {
	if ((error = pre_validate(config) )!= Configuration::ERROR_NONE)
	{
		print();
		cout << "ERROR: Invalid configuration file" << endl;
		exit(1);
	}
	toml::table& t = config["server"];
	for (size_t i = 0; i < t.vec.size(); i++) {
		_servers.push_back(fill_server(t[i]));
	}
	error = post_validate();
	if (error != Configuration::ERROR_NONE) {
		print();
		cout << "ERROR: Invalid configuration file" << endl;
		exit(1);
	}
}

Configuration::~Configuration() {

}

const vector<ServerConfig> &Configuration::getServers() const {
    return _servers;
}

void Configuration::print() {
	cout << "\n<==================== Configuration ====================>" << endl;

	for (size_t i = 0; i < _servers.size(); i++)
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
		case ERROR_INVALID_CGI_PATH:
			cout << "ERROR_INVALID_CGI_PATH" << endl;
			break;
		case ERROR_INVALID_AUTOINDEX:
			cout << "ERROR_INVALID_AUTOINDEX" << endl;
			break;
		case ERROR_INVALID_UPLOAD_PATH:
			cout << "ERROR_INVALID_UPLOAD_PATH" << endl;
			break;
		case ERROR_INVALID_REDIRECT:
			cout << "ERROR_INVALID_REDIRECT" << endl;
			break;
		case ERROR_INVALID_PREFIX:
			cout << "ERROR_INVALID_PREFIX" << endl;
			break;

	}

	cout << "\n<==================== END Configuration ====================>\n" << endl;
}
