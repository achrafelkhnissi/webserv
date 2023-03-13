

#include "Request.hpp"
#include <string>
#include <iostream> // TODO: remove this

Request::Request() {
    host = "localhost"; // TODO: remove this
	protocol = "HTTP";
	version = "1.1";
	method = "GET";
	query = "name=ismail&age=23";
//    uri = "/media/video/video.mp4";
    uri = "/media/image/myimage.jpg";
	body = "Hello World";
}

const std::string& Request::getProtocol() const {
    return protocol;
}

string Request::getVersion() const {
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

void Request::setMethod(std::string& method) {
	this->method.swap(method);
}

void Request::setUri(std::string& uri) {
	this->uri.swap(uri);
}

void Request::setBody(std::string& body) {
	this->body.swap(body);
}

void Request::setHeaders(std::multimap<std::string, std::string>& headers) {
	this->headers.swap(headers);
}

void Request::setQuery(std::string& query) {
	this->query.swap(query);
}

pair<string, unsigned short> Request::getHost() const {
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

const string &Request::getContentType() {
    multimap<string, string>::iterator res = headers.find("Content-Type");
    if (res == headers.end())
        return *(new string("")); // leak
    return res->second;
}

void Request::print() {

	cerr << "\n==================== Request ====================" << endl;
	cerr << "Method: " << method << endl;
	cerr << "URI: " << uri << endl;
	cerr << "Version: " << version << endl;
	cerr << "Query: " << query << endl;
	cerr << "Host: " << getHost().first << endl;
	cerr << "Port: " << getHost().second << endl;

	for (multimap<string, string>::iterator it = headers.begin(); it != headers.end(); it++) {
		cerr << it->first << ": " << it->second << endl;
	}
	cerr << "Body: " << body << endl;
	cerr << "==================== END Request ====================\n\n" << endl;
}

//const std::string& Request::getHost() const { return host; } // TODO: remove this
//const int& Request::getPort() const { return port; } // TODO: remove this


Request::~Request() { }

const string &Request::getQuery() const {
    return query;
}

void Request::setRequest() {
    std::cout << "Enter uri: "; cin >> uri;
}

multimap<string, string> Request::getHeaders() const {
	return headers;
}

//const string &Request::getQuery() const {
//    return query;
//}
