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

using namespace std;

typedef vector<string> string_vector;
struct LocationConfig {
	string path;
	string root;
	string autoindex;
	string upload_path;
	string upload_store;
	string cgi_path;
	string cgi_extension;
	string cgi_pass;
	string client_max_body_size;
	string error_page;
	vector<string> index;

	void print(int indent = 0) {
		string s(indent, ' ');
		cout << s << "path: " << path << endl;
		cout << s << "root: " << root << endl;
		ITER_FOREACH(vector<string>, index, it) {
			cout << *it << " ";
		}
		cout << endl;
		cout << s << "autoindex: " << autoindex << endl;
		cout << s << "upload_path: " << upload_path << endl;
		cout << s << "upload_store: " << upload_store << endl;
		cout << s << "cgi_path: " << cgi_path << endl;
		cout << s << "cgi_extension: " << cgi_extension << endl;
		cout << s << "cgi_pass: " << cgi_pass << endl;
		cout << s << "client_max_body_size: " << client_max_body_size << endl;
		cout << s << "error_page: " << error_page << endl;
		cout << endl;
	}
};

struct ServerConfig {

	unsigned short port;
	string host;
	vector<string> server_name;
	string root;
	string error_page;
	string client_max_body_size;

	vector<LocationConfig> locations;

	void print() {
		cout << "listen: " << port << endl;
		cout << "host: " << host << endl;
		cout << "server_name: ";
		ITER_FOREACH(string_vector, server_name, it) {
			cout << *it << " ";
		}
		cout << endl;
		cout << "root: " << root << endl;
		cout << "error_page: " << error_page << endl;
		cout << "client_max_body_size: " << client_max_body_size << endl;
		for (size_t i = 0; i < locations.size(); i++) {
			cout << "location " << i << ":" << endl;
			locations[i].print(1);
		}
		cout << endl;
	}
};

class Configuration {

private:
	vector<ServerConfig> _servers;
	int _port;
	string _host;
	string _root;
	string _log_path;

	void parse_config_file(string config_file);

public:
	Configuration(toml::table& config);
    ~Configuration();

    const vector<ServerConfig>& getServers() const;
	int get_port();

	string get_host();
	string get_root();
	string get_log_path();
};

#endif // CONFIGURATION_HPP
