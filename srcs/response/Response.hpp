#pragma once

#include <string>
#include "typedefs.hpp"
#include "iostream"

class Response {

private:

    typedef std::map<string, string> stringMap_t;

    int _statusCode;
    string _protocol;
    string _version;
    string _body;

    stringMap_t _headers;
    std::map<int, string> _httpErrors;

public:
    Response();
    ~Response();

    void setStatusCode(const Request& request, const string& filePath, std::map<string, string> &mimTypes);
    void setProtocol(const string& protocol);
    void setVersion(const string& version);
    void setBody(const string& body);
    void setContentLength(const string &path);
    void setContentType(const string& filePath, std::map<string, string> &mimTypes);
    void setDate();
    void setServer(const string& server);
    void setLastModified(const string& filePath);
    void setAcceptRanges(const string& acceptRanges);
    void setConnection(const string& connection);
    void setHeaders(const Request& request, std::map<string, string> &mimTypes, const string &path);

    int getStatusCode() const;
    const string& getProtocol() const;
    const string& getVersion() const;
    const string& getBody() const;
    const stringMap_t& getHeaders() const;
    size_t getContentLength() ;
    const string& getContentType();
    std::map<int, string> getHttpErrors() const;
    void printData() const;

    bool is_regular_file(const char *path) const;
    string _extractExtension(const string &path) const ;


        void setStatusCode(int statusCode);
};