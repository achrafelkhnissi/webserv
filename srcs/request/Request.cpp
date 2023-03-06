

#include "Request.hpp"
#include <string>
#include <iostream> // TODO: remove this

Request::Request() {
    host = "localhost"; // TODO: remove this
    port = 1337; // TODO: remove this
	protocol = "HTTP";
	version = "1.1";
	method = "GET";
	uri = "";
	query = "name=ismail&age=23";
	body = "Hello World";
}

void    Request::setRequest() {
//    std::cout << "Host: "; std::cin >> host;
//    std::cout << "Port: "; std::cin >> port;
//    std::cout << "Protocol: "; std::cin >> protocol;
//    std::cout << "Version: "; std::cin >> version;
    std::cout << "Method: "; std::cin >> method;
    std::cout << "URI: "; std::cin >> uri;
	if (uri == "empty") { //TODO: remove this later
		uri = "";
	}
} // TODO: remove this

void Request::printData() const {

    std::cout << "\n==== Request Start ====" << std::endl;
    std::cout << "Host: " << host << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Protocol: " << protocol << std::endl;
    std::cout << "Version: " << version << std::endl;
    std::cout << "Method: " << method << std::endl;
    std::cout << "URI: " << uri << std::endl;
    std::cout << "Query: " << query << std::endl;
    std::cout << "Body: " << body << std::endl;
    std::cout << "==== Request End ====\n" << std::endl;
} // TODO: remove this

void Request::push(std::string) { }

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

const std::string& Request::getHost() const { return host; } // TODO: remove this
const int& Request::getPort() const { return port; } // TODO: remove this


Request::~Request() { }

const string &Request::getQuery() const {
    return query;
}
