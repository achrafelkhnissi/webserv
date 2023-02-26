

#include "Request.hpp"
#include <string>
#include <iostream> // TODO: remove this

Request::Request() {
    host = "localhost"; // TODO: remove this
    port = 1337; // TODO: remove this
	protocol = "HTTP";
	version = "1.1";
	method = "GET";
	uri = "/";
	query = "name=ismail&age=23";
	body = "Hello World";
}

void    Request::setRequest() {
//    std::cout << "Host: "; std::cin >> host;
//    std::cout << "Port: "; std::cin >> port;
//    std::cout << "Protocol: "; std::cin >> protocol;
//    std::cout << "Version: "; std::cin >> version;
//    std::cout << "Method: "; std::cin >> method;
    std::cout << "URI: "; std::cin >> uri;
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

const std::string& Request::getProtocol() const {
	return protocol;
}

const std::string& Request::getVersion() const {
	return version;
}

const std::string& Request::getMethod() const {
	return method;
}

const std::string& Request::getUri() const {
	return uri;
}

const std::string& Request::getBody() const {
	return body;
}

void Request::setProtocol(const std::string& protocol) {
	this->protocol = protocol;
}

void Request::setVersion(int version) {
	this->version = version;
}

void Request::setMethod(const std::string& method) {
	this->method = method;
}

void Request::setUri(const std::string& uri) {
	this->uri = uri;
}

void Request::setBody(const std::string& body) {
	this->body = body;
}

const std::string& Request::getHost() const { return host; } // TODO: remove this
const int& Request::getPort() const { return port; } // TODO: remove this


Request::~Request() { }
