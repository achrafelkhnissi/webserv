#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class Request {
public:
	Request();
	void push(std::string);

    const string& getHost() const; // TODO: remove this
    const string& getQuery() const; // TODO: remove this
    const int& getPort() const; // TODO: remove this
    void printData() const; // TODO: remove this
    void setRequest(); // TODO: remove this


    const string& getProtocol() const;
	const string& getVersion() const;
	const string& getMethod() const;
	const string& getUri() const;
	const string& getBody() const;

	void setProtocol(const std::string& protocol);
	void setVersion(int major_version);
	void setMethod(const std::string& method);
	void setUri(const std::string& uri);
	void setBody(const std::string& body);

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

    string host;
    int port;

	string protocol;
	string method;
	string uri;
	string body;
	string query;

	int version;
	string host;
	multimap<string, string> headers;
};

#endif
