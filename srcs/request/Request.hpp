#pragma once

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class Request {
public:
	Request();
	const string& get_protocol() const;
	int get_version() const;
	const string& get_method() const;
	const string& get_uri() const;
	const string& get_body() const;
	void set_protocol(const std::string& protocol);
	void set_version(int major_version);
	void set_method(std::string& method);
	void set_uri(std::string& uri);
	void set_body(std::string& body);
	void set_headers(multimap<string, string>& headers);
	void set_query(string& query);

	pair<string, unsigned short> get_host();
	string &get_content_type();

	~Request();

	void print();

private:
	string protocol;
	string method;
	string uri;
	string body;
	string query;
	int version;
	string host;
	multimap<string, string> headers;
};
