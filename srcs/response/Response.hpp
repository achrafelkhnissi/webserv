#pragma once

#include <string>
#include "typedefs.hpp"
#include "iostream"

class Response {
private:
    int statusCode;
    string protocol;
    string version;
    string body;
    size_t content_length;
    string content_type;
    std::map<string, string> headers;
    std::map<int, string> http_errors;



public:
    Response();
    ~Response();

    void setStatusCode(const string& filePath, std::map<string, string> &mimTypes);
    void setProtocol(const string& protocol);
    void setVersion(const string& version);
    void setBody(const string& body);
    void setHeaders(const stringMap_t& headers);
    void setContentLength(string &path);
    void setContentType(const string& filePath, std::map<string, string> &mimTypes);
    void setHeaders();

    int getStatusCode() const;
    const string& getProtocol() const;
    const string& getVersion() const;
    const string& getBody() const;
    const stringMap_t& getHeaders() const;
    size_t getContentLength() const;
    const string& getContentType() const;
    std::map<int, string> getHttpErrors() const;
    void printData() const;

    bool is_regular_file(const char *path) const;

    void setStatusCode(int statusCode);
};