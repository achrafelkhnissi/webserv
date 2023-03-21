#pragma once

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

enum e_error {
	err_none,
	err_invalid_method,
	err_not_implemented_method,
	err_invalid_uri,
	err_invalid_version,
	err_not_implemented_version,
	err_invalid_status_line,
	err_invalid_header,
	err_invalid_chunk_body,
};

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
	void setError(e_error error);
	e_error getError() const;

	bool isInvalid() const;
	pair<string, unsigned short> getHost() const;
	const string &getContentType();
    multimap<string, string> getHeaders() const;
	void debug_err();

	~Request();

	void print();
    void setRequest();

private:

    string version;

	string protocol;
    string host;
	string method;
	string uri;
	string body;
	string query;

	e_error error;

	multimap<string, string> headers;
};
