#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class Request {
public:
	Request();
	const string& getProtocol() const;
	int getVersion() const;
	const string& getMethod() const;
	const string& getUri() const;
	const string& getBody() const;
	void setProtocol(const std::string& protocol);
	void setVersion(int major_version);
	void setMethod(const std::string& method);
	void setUri(const std::string& uri);
	void setBody(const std::string& body);
	~Request();

	void print() {
		cout << "Method: " << method << endl;
		cout << "URI: " << uri << endl;
		cout << "Version: " << version << endl;
		cout << "Query: " << query << endl;
	}

private:
	string protocol;
	string method;
	string uri;
	string body;
	string query;
	int version;
};

#endif
