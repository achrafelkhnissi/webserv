#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <utility>
#include <vector>

using namespace std;

struct RequestTokenParser {
	std::string token;
	void (*done)(std::string);
};

class Request {
public:
	enum Method { GET, POST, PUT, DELETE, HEAD, OPTIONS, TRACE, CONNECT };

	enum Status {
		DONE,
		FAILED,
		CONTINUE,

	};

	Request();
	void push(std::string);

    const string& getHost() const; // TODO: remove this
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
	~Request();

private:

    string host;
    int port;

	string protocol;
	string version;
	string method;
	string uri;
	string body;
	string query;
	vector<pair<string, string> > headers;
	//Status status;
	//int index;
	//RequestTokenParser token_parser[6];
};
#endif
