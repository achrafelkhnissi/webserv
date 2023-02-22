

#include "Request.hpp"
#include <string>

Request::Request() {
	protocol = "HTTP";
	version = "1.1";
	method = "GET";
	uri = "/users/me";
	query = "name=ismail&age=23";
	body = "Hello World";
}

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

Request::~Request() { }
