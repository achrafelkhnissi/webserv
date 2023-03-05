

#include "Request.hpp"
#include <string>

Request::Request() {
	protocol = "HTTP";
	version = 1;
}

const std::string& Request::get_protocol() const {
	return protocol;
}

int Request::get_version() const {
	return version;
}

const std::string& Request::get_method() const {
	return method;
}

const std::string& Request::get_uri() const {
	return uri;
}

const std::string& Request::get_body() const {
	return body;
}

void Request::set_protocol(const std::string& protocol) {
	this->protocol = protocol;
}

void Request::set_version(int version) {
	this->version = version;
}

void Request::set_method(std::string& method) {
	this->method.swap(method);
}

void Request::set_uri(std::string& uri) {
	this->uri.swap(uri);
}

void Request::set_body(std::string& body) {
	this->body.swap(body);
}

void Request::set_headers(std::multimap<std::string, std::string>& headers) {
	this->headers.swap(headers);
}

void Request::set_query(std::string& query) {
	this->query.swap(query);
}

pair<string, unsigned short> Request::get_host() {
	pair<string, unsigned short> host;
	const string &host_header = headers.find("Host")->second;
	string::size_type pos = host_header.find(':');
	if (pos != string::npos) {
		host.first = host_header.substr(0, pos);
		host.second = atoi(host_header.c_str() + pos + 1);
	}
	else abort();
	return host;
}

string &Request::get_content_type() {
	return headers.find("Content-Type")->second;
}

void Request::print() {
	cout << "Method: " << method << endl;
	cout << "URI: " << uri << endl;
	cout << "Version: " << version << endl;
	cout << "Query: " << query << endl;
	cout << "Host: " << get_host().first << endl;
	cout << "Port: " << get_host().second << endl;
	cout << "Content-Type: " << get_content_type() << endl;
}

Request::~Request() { }
