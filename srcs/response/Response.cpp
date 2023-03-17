#include "Response.hpp"

Response::Response() {
    _statusCode = 200;
    _protocol = "HTTP";
    _version = "1.1";
    _body = "<h1>Hello World</h1>";

    _httpErrors[200] = "OK";
    _httpErrors[400] = "Bad Request";
    _httpErrors[401] = "Unauthorized";
    _httpErrors[403] = "Forbidden";
    _httpErrors[404] = "Not Found";
    _httpErrors[405] = "Method Not Allowed";
    _httpErrors[413] = "Request Entity Too Large";
    _httpErrors[415] = "Unsupported Media Type";
    _httpErrors[500] = "Internal Server Error";
    _httpErrors[501] = "Not Implemented";
    _httpErrors[503] = "Service Unavailable";
}

Response::~Response() {}


bool Response::is_regular_file(const char* path) const {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        // error occurred while checking file status
        return false;
    }
    return S_ISREG(path_stat.st_mode);
}

void Response::setStatusCode(int statusCode) {
    this->_statusCode = statusCode;
}

void Response::setStatusCode(const Request& request, const string& filePath, std::map<string, string> &mimTypes) {

    std::string M = mimTypes[_extractExtension(filePath)];
    std::ifstream file(filePath);

    //TODO: check if we should support a file with no extension
    if (!is_regular_file(filePath.c_str()) || !file.is_open()) {
        _statusCode = 404;
    } else if (M.empty() && request.getMethod() == "GET") {
        _statusCode = 415;
    } else {
        _statusCode = 200;
    }

//    if (M.empty())
//   {
//        _statusCode = 415;
//        if (!is_regular_file(filePath.c_str()) || !file.is_open())
//            _statusCode = 404;
//    } else {
//        _statusCode = 200;
//    }

    file.close();
}

int Response::getStatusCode() const {
    return _statusCode;
}

void Response::setContentLength(const string &path) {

    std::ifstream file_stream(path, std::ios::in | std::ios::binary);

    //get file size
    file_stream.seekg(0, std::ios::end);
    _headers["Content-Length"] = std::to_string(file_stream.tellg());
    file_stream.seekg(0, std::ios::beg);

    file_stream.close();
}

void Response::setContentLength(::size_t size) {
    _headers["Content-Length"] = std::to_string(size);

}

void Response::setContentType(string type) {
    _headers["Content-Type"] = type;
}

void Response::setHeaders(const Request &request,  std::map<string, string> &mimTypes, const string &path) {

    if (_statusCode == 200){
        setContentType(path, mimTypes);
        setContentLength(path);
        setLastModified(path);
    }
    setProtocol(request.getProtocol());
    setVersion(request.getVersion());
    setContentType(path, mimTypes);
    setContentLength(path);
    setDate();
    setServer("webserv 1.0");
    setAcceptRanges("bytes");
    setConnection("Keep-Alive");

}

const stringMap_t& Response::getHeaders() const {
//    for (std::map<string, string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
//        std::cout << it->first << " : " << it->second << std::endl;
//    }
    return _headers;
}
void Response::setContentType(const string& path, std::map<string, string> &mimTypes) {

    // todo: check if the content type doesn't exist
    // if it doesn't exist, set it to text/plain or respond with the following
    /*
     *  std::stringstream response_stream;
     *  response_stream << "HTTP/1.1 415 Unsupported Media Type\r\n\r\n";
     *  std::string response = response_stream.str();
     *  send(fd, response.c_str(), response.length(), 0);
	 * file_stream.close();
	 * response_stream.clear();
     */
    _headers["Content-Type"] =  mimTypes[_extractExtension(path)];

}

size_t Response::getContentLength() {
    return ::atoi(_headers["Content-Length"].c_str());
}
const string& Response::getContentType() {
    return _headers["Content-Type"];
}

const string &Response::getBody() const {
    return _body;
}

void Response::setBody(const string &s) {
    this->_body = s;
}

void Response::setDate() {
    std::time_t now = std::time(NULL);
    std::tm *gmt = std::gmtime(&now);

    char buf[80];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gmt);

    _headers["Date"] = buf;
}

void Response::setServer(const string& server) {
    _headers["Server"] = server;
}

void Response::setLastModified(const string &path) {

    struct stat file_stat;
    char date_str[30];

    if (stat(path.c_str(), &file_stat) == 0) {
        // Convert the last modification time to a string in the Last-Modified format
        time_t mod_time = file_stat.st_mtime;
        struct tm *tm_info = gmtime(&mod_time);
        strftime(date_str, 30, "%a, %d %b %Y %H:%M:%S GMT", tm_info);
    }

    _headers["Last-Modified"] = string(date_str);
}

void Response::setProtocol(const string &prtcl) {
    this->_protocol = prtcl;
}

void Response::setVersion(const string &v) {
    this->_version = v;
}

void Response::setAcceptRanges(const string &acceptRanges) {
    _headers["Accept-Ranges"] = acceptRanges;
}

void Response::setConnection(const string &cnx) {

    /**
     * HTTP/1.1
     * 1. By default connection is Keep-Alive
     * 2. if the client sends a Connection: close header, then the connection is closed
     * 3. if the client sends a Connection: keep-alive header, then the connection is kept alive
     */

    _headers["Connection"] = cnx;
}

std::map<int, string> Response::getHttpErrors() const{
    return _httpErrors;
}

string Response::_extractExtension(const string &path) const {
    string extension_ = "";

    std::size_t found_ = path.find_last_of('.');
    if (found_ != std::string::npos){
        extension_ = path.substr(found_);
    }
    return extension_;
}

const string &Response::getProtocol() const {
    return _protocol;
}

const string &Response::getVersion() const {
    return _version;
}

void Response::handleError(int fd, int statusCode) {

    string errorPage = _getErrorPage(statusCode);
    string errorName = _httpErrors[statusCode];

    std::ifstream file_stream(errorPage, std::ios::in | std::ios::binary);

    if (!file_stream.is_open()) {
        std::cerr << "Error: " << errorName << std::endl;
        return;
    }

    //get file size

    setContentLength(errorPage);

    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 " << statusCode << " " << errorName << "\r\n";
    response_stream << "Content-Type: text/html\r\n";
    response_stream << "Content-Length: " << getContentLength() << "\r\n";
    response_stream << "Connection: close\r\n\r\n";
    response_stream << file_stream.rdbuf();

    std::cout << response_stream.str() << std::endl;
    std::string response = response_stream.str();
    send(fd, response.c_str(), response.length(), 0);

    std::cout << "handleError" << std::endl;
    file_stream.close();
//    send(fd, file_content.c_str(), file_content.length(), 0);

}

string Response::_getErrorPage(int code) const {
    return "www/error_pages/" + std::to_string(code) + ".html";
}


