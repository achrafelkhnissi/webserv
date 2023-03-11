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
	string prefix;
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
    vector<string> allowed_methods;

	void print(int indent = 0) {
		string s(indent, ' ');
		cout << s << "\t\tprefix: " << prefix << endl;
		cout << s << "\t\troot: " << root << endl;
		ITER_FOREACH(vector<string>, index, it) {
			cout << "\t\t" << *it << " ";
		}
		cout << endl;
		cout << s << "\t\tautoindex: " << autoindex << endl;
		cout << s << "\t\tupload_path: " << upload_path << endl;
		cout << s << "\t\tupload_store: " << upload_store << endl;
		cout << s << "\t\tcgi_path: " << cgi_path << endl;
		cout << s << "\t\tcgi_extension: " << cgi_extension << endl;
		cout << s << "\t\tcgi_pass: " << cgi_pass << endl;
		cout << s << "\t\tclient_max_body_size: " << client_max_body_size << endl;
		cout << s << "\t\terror_page: " << error_page << endl;
	}
};

struct ServerConfig {

	unsigned short port;
	string host;
	vector<string> server_name;
	string root;
	string error_page;
	string client_max_body_size;
    vector<string> allowed_methods;
    vector<string> index;

	vector<LocationConfig> locations;

	void print() {

		cout << "\t==== ServerConfig ====" << endl;
		cout << "\tlisten: " << port << endl;
		cout << "\thost: " << host << endl;
		cout << "\tserver_name: ";
		ITER_FOREACH(string_vector, server_name, it) {
			cout << "\t" << *it << " ";
		}
		cout << endl;
		cout << "\troot: " << root << endl;
		cout << "\terror_page: " << error_page << endl;
		cout << "\tclient_max_body_size: " << client_max_body_size << endl;
		for (size_t i = 0; i < locations.size(); i++) {
			cout << "\t\t" << "==== LocationConfig ====" << endl;
			cout << "\t\tlocation " << i << ":" << endl;
			locations[i].print(1);
			cout << "\t\t" << "==== END LocationConfig ====\n" << endl;
		}
		cout << "\t==== END ServerConfig ====" << endl;
	}
};

class Configuration {

private:
	vector<ServerConfig> _servers;
//	int _port;
//	string _host;
//	string _root;
//	string _log_path;

	void parse_config_file(string config_file);

public:
	Configuration(toml::table& config);
    ~Configuration();

    const vector<ServerConfig>& getServers() const;
//	int get_port();
//
//	string get_host();
//	string get_root();
//	string get_log_path();

	void print(); // todo: remove this
};

#endif // CONFIGURATION_HPP
