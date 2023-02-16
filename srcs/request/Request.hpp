#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>

class Request {
public:
	Request();
	const std::string& getProtocol() const;
	const std::string& getVersion() const;
	const std::string& getMethod() const;
	const std::string& getUri() const;
	const std::string& getBody() const;
	void setProtocol(const std::string& protocol);
	void setVersion(int major_version);
	void setMethod(const std::string& method);
	void setUri(const std::string& uri);
	void setBody(const std::string& body);
	~Request();

private:
	std::string protocol;
	std::string version;
	std::string method;
	std::string uri;
	std::string body;
};
#endif
