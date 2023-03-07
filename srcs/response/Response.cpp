#include "Response.hpp"

Response::Response() {
    statusCode = 200;
    protocol = "HTTP";
    version = "1.1";
    body = "Hello World";

    http_errors[400] = "Bad Request";
    http_errors[401] = "Unauthorized";
    http_errors[403] = "Forbidden";
    http_errors[404] = "Not Found";
    http_errors[405] = "Method Not Allowed";
    http_errors[415] = "Unsupported Media Type";
    http_errors[500] = "Internal Server Error";
    http_errors[501] = "Not Implemented";
    http_errors[503] = "Service Unavailable";
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
    this->statusCode = statusCode;
}

void Response::setStatusCode(const string& filePath, std::map<string, string> &mimTypes) {
    std::string extension = filePath.substr(filePath.find_last_of(".") );
    std::string M = mimTypes[extension];
    std::ifstream file(filePath);

    if (M.empty()) {
        statusCode = 415;
    }
    if (!is_regular_file(filePath.c_str()) || !file.is_open()) {
        statusCode = 404;
    }
    else {
        statusCode = 200;
    }

    file.close();
}

int Response::getStatusCode() const {
    return statusCode;
}

void Response::setContentLength(string &path) {

    std::ifstream file_stream(path, std::ios::binary);
    stringstream string_stream;
    string_stream << file_stream.rdbuf();

    //get file size
    file_stream.seekg(0, std::ios::end);
    content_length = file_stream.tellg();
    file_stream.seekg(0, std::ios::beg);

    file_stream.close();
}

void Response::setHeaders() {
    headers["Content-Type"] = content_type;
    headers["Content-Length"] = std::to_string(content_length);
}

const stringMap_t& Response::getHeaders() const {
    for (std::map<string, string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << " : " << it->second << std::endl;
    }
    return headers;
}
void Response::setContentType(const string& extension, std::map<string, string> &mimTypes) {

    content_type =  mimTypes[extension];

}

size_t Response::getContentLength() const{
    return content_length;
}
const string& Response::getContentType() const{
    return content_type;
}

const string &Response::getBody() const {
    return body;
}

void Response::setBody(const string &s) {
    this->body = s;

}
