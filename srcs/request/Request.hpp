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


	const string& getProtocol() const;
	string getVersion() const;
	const string& getMethod() const;
	const string& getUri() const;
    const string& getQuery() const;
	const string& getBody() const;
	void setProtocol(const std::string& protocol);
	void setVersion(int major_version);
	void setMethod(std::string& method);
	void setUri(std::string& uri);
	void setBody(std::string& body);
	void setHeaders(multimap<string, string>& headers);
	void setQuery(string& query);

	pair<string, unsigned short> getHost() const;
	const string &getContentType();

	~Request();

	void print();

private:

    string version;

	string protocol;
    string host;
	string method;
	string uri;
	string body;
	string query;

	multimap<string, string> headers;
};
