#include "Server.hpp"
#include "Request.hpp"
#include "utils.hpp"
#include "Response.hpp"
#include "IndexGenerator.hpp"
#include <algorithm>

Server::Server(Configuration config): _config(config), _request() {

	vector<ServerConfig> servers_ = _config.getServers();

	for (vector<ServerConfig>::iterator it_ = servers_.begin(); it_ != servers_.end(); ++it_) {
		if (_virtualServer.find(it_->port) != _virtualServer.end()) {
			_virtualServer[it_->port].addSubServer(*it_);
		} else {
			_virtualServer[it_->port] = VirtualServer(*it_);
		}
	}
	for (virtualServerMapIterator_t it_ = _virtualServer.begin(); it_ != _virtualServer.end(); ++it_) {
		_setupVirtualServer(it_->second);
	}

	_setMimeTypes();
}

void Server::_setMimeTypes() {

	_mimeTypes[".bmp"] = "image/bmp";
	_mimeTypes[".css"] = "text/css";
	_mimeTypes[".epub"] = "application/epub+zip";
	_mimeTypes[".gif"] = "image/gif";
	_mimeTypes[".htm"] = "text/html";
	_mimeTypes[".html"] = "text/html";
    _mimeTypes[".ico"] = "image/vnd.microsoft.icon";
	_mimeTypes[".jpeg"] = "image/jpeg";
	_mimeTypes[".jpg"] = "image/jpeg";
	_mimeTypes[".js"] = "text/javascript";
	_mimeTypes[".json"] = "application/json";
	_mimeTypes[".mp3"] = "audio/mpeg";
	_mimeTypes[".mp4"] = "video/mp4";
	_mimeTypes[".mpeg"] = "video/mpeg";
	_mimeTypes[".png"] = "image/png";
	_mimeTypes[".pdf"] = "application/pdf";
	_mimeTypes[".php"] = "application/x-httpd-php";
	_mimeTypes[".svg"] = "image/svg+xml";
	_mimeTypes[".txt"] = "text/plain";
	_mimeTypes[".weba"] = "audio/webm";
	_mimeTypes[".webm"] = "video/webm";
    _mimeTypes[".webp"] = "image/webp";
    _mimeTypes[".mov"] = "video/quicktime";
    _mimeTypes[".xml"] = "application/xml";
	_mimeTypes[".php"] = "application/x-httpd-php";
}

void Server::_setupVirtualServer(VirtualServer& vserver) {
	// Create a socket
	int serverFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd_ == -1)
		error("socket", 1);

	// Set the server socket to reuse the address
	int optionValue_ = 1;
	if (setsockopt(serverFd_, SOL_SOCKET, SO_REUSEADDR, &optionValue_, sizeof(optionValue_)) < 0)
		error("setsockopt", 1);

    int nosigpipe = 1; // value for SO_NOSIGPIPE
    if (setsockopt(serverFd_, SOL_SOCKET, SO_NOSIGPIPE, &nosigpipe, sizeof(nosigpipe)) < 0)
        error("setsockopt", 1);

    // Bind the socket to the port and host
	struct sockaddr_in serverAddr_ = {};
	memset(&serverAddr_, 0, sizeof(serverAddr_));
	serverAddr_.sin_family = AF_INET;
	serverAddr_.sin_addr.s_addr = INADDR_ANY;
	serverAddr_.sin_port = htons(vserver.getPort());

	if (bind(serverFd_, (struct sockaddr *) &serverAddr_, sizeof(serverAddr_)) == -1)
		error("bind", 1);

	// Listen for incoming connections
	if (listen(serverFd_, SOMAXCONN) == -1)
		error(MSG("bind()"), 1);

	struct pollfd serverPollFd_ = {};
	serverPollFd_.fd = serverFd_;
	serverPollFd_.events = POLLIN;
	_fds.push_back(serverPollFd_);

}

void Server::start() {
    int timeout = -1; //60 * 1000; // 60 seconds

	// Start the server loop
	while (true) {
		int ready_ = poll(_fds.data(), _fds.size(), timeout);

		if (ready_ == -1) {
            _error("poll", 1);
        }

		if (ready_ == 0)
			_error("poll timeout", 1);

        for(size_t i = 0; i < _fds.size(); i++) {

            if ((_fds[i].revents & POLLIN) || (_fds[i].revents & POLLOUT)) {
                if (i < _virtualServer.size()){
                    _handleConnections(_fds[i].fd);
                }
                else{
                    _handleRequest(_fds.begin() + i);
                    if (_fds[i].fd == -1) {
                        _fds.erase(_fds.begin() + i);
                        i--;
                    }
                }
            }
        }
    }
}

Server::~Server() {
	_clearPollfds();
}

void Server::_handleConnections(int sockfd) {
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;

		struct sockaddr_in sockAddrIn_ = {};
		socklen_t clientLen_ = sizeof(sockAddrIn_);
		int clientFd_ = accept(sockfd, (struct sockaddr*)&sockAddrIn_, &clientLen_);
		if (clientFd_ == -1) {
			if (errno == EWOULDBLOCK || errno == EAGAIN) {
			    cerr << "EWOULDBLOCK" << endl;
                return;
            } else {
				_error("accept", 1);
			}
        }
    if (setsockopt(clientFd_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
        std::cerr << "Error setting timeout\n";
        exit(1);
    }

    if (setsockopt(clientFd_, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
        std::cerr << "Error setting timeout\n";
        exit(1);
    }
		// Add the clientPollFd_ socket to the pollfds list
		struct pollfd clientPollFd_ = {};
		clientPollFd_.fd = clientFd_;
		clientPollFd_.events = POLLIN;
		_fds.push_back(clientPollFd_);
		_clientHttpParserMap[clientFd_] = HttpParser();
}

void Server::_handleRequest(pollfdsVectorIterator_t it) {

    Response response_ = Response();
    string requestBuffer_;
	ssize_t bytesRead_ = 0;

	char buffer_[BUFFER_SIZE + 1];

	memset(buffer_, 0, BUFFER_SIZE);
	bytesRead_ = recv(it->fd, buffer_, BUFFER_SIZE, 0);

	if (bytesRead_ == -1 || bytesRead_ == 0) {
        close(it->fd);
        it->fd = -1;
        return ;
	}
	buffer_[bytesRead_] = '\0';
	requestBuffer_ = string(buffer_, bytesRead_);

	_clientHttpParserMap[it->fd].push(requestBuffer_);

	while (_clientHttpParserMap[it->fd].has_request())
	{
        _request = _clientHttpParserMap[it->fd].consume_request();
		if (_request.isInvalid()) {
            _request.debug_err();

             if (_request.getError() == err_invalid_version)
               response_.setStatusCode(505);
           else
                response_.setStatusCode(400);

            response_.setHeaders(_request, _mimeTypes, _getErrorPage(response_.getStatusCode(), stringVector_t()));

            sendResponse(it, _getErrorPage(response_.getStatusCode(), stringVector_t()),response_);
            close(it->fd);
            it->fd = -1;
			return ;
        }
        else {
            virtualServerMapIterator_t vserverIter_ = _virtualServer.find(_request.getHost().second);
            subServersIterator_t subServerIter_ = vserverIter_->second.matchSubServer(_request.getHost().first);

            if (_request.getMethod() == "GET") {
                _handleGET(it, subServerIter_, _request);
                if (it->fd != -1) {
                    close(it->fd);
                    it->fd = -1;
                }
            } else if (_request.getMethod() == "POST") {
                _handlePOST(it, subServerIter_, _request);
                if (it->fd != -1) {
                    close(it->fd);
                    it->fd = -1;
                }
            } else if (_request.getMethod() == "DELETE") {
                _handleDELETE(it, subServerIter_, _request);
                if (it->fd != -1) {
                    close(it->fd);
                    it->fd = -1;
                }
            } else{
                response_.setStatusCode(501);
                response_.setHeaders(_request, _mimeTypes, _getErrorPage(response_.getStatusCode(), stringVector_t()));

                sendResponse(it, _getErrorPage(response_.getStatusCode(), stringVector_t()),response_);
                close(it->fd);
                it->fd = -1;
            }
            if (it->fd == -1)
                break;
        }
	}

}

void Server::_clearPollfds() {
	pollfdsVectorIterator_t iter_ = _fds.begin();
	pollfdsVectorIterator_t iterEnd_ = _fds.end();

	for(; iter_ != iterEnd_; iter_++)
		close(iter_->fd);
	_fds.clear();
}

location_t* Server::matchLocation(const locationVector_t &locations, const std::string &uri) {

	location_t *location_ = new location_t(); //TODO FREE
	string str_ = uri;

	do {
        locationVectorConstIterator_t locIter_ = locations.begin();
		locationVectorConstIterator_t locIterEnd_ = locations.end();
		for (; locIter_ != locIterEnd_; ++locIter_) {
			if (locIter_->prefix == str_) {
				*location_ = *locIter_;
				return location_;
			}
		}
        if (str_ == "/")
            return nullptr;
		std::size_t found_ = str_.find_last_of('/');
		if (found_ != std::string::npos){
            found_ = found_ == 0 ? 1 : found_; // if found_ == 0, then str_ = "/"
			str_ = str_.substr(0, found_);
		}

	} while (!str_.empty());
	return nullptr;
}

void Server::sendResponse(pollfdsVectorIterator_t it, const string& resourcePath, const Response& response) {

    std::ostringstream response_stream;

    response_stream << response.getProtocol() + "/" + response.getVersion() << " ";
    response_stream << response.getStatusCode() << " "<< response.getHttpErrors().at(response.getStatusCode()) << "\r\n" ; //TODO: change to response.getStatus()

    std::map<std::string, std::string>::const_iterator headerIter_ = response.getHeaders().begin();
    std::map<std::string, std::string>::const_iterator headerIterEnd_ = response.getHeaders().end();
    for (; headerIter_ != headerIterEnd_; ++headerIter_) {
        response_stream << headerIter_->first  << ": " << headerIter_->second << "\r\n";
    }
    response_stream <<  "\r\n";

    const string& response_header = response_stream.str();

    ifstream fileStream(resourcePath, ios::in | ios::binary);

    if (!fileStream) {
        cerr << "Unable to open file" << endl;
        return;
    }

    const size_t response_header_size = response_header.size();

    fileStream.seekg(0, std::ios::end);
    ::size_t file_size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    // Send the response header and the file contents in a single call to send.
    const size_t total_size = response_header_size + file_size;
    char* buffer = new char[total_size];

    // Copy the response header to the buffer.
    ::memcpy(buffer, response_header.c_str(), response_header_size);

    // Copy the file contents to the buffer.
    fileStream.read(buffer + response_header_size, file_size);

    fileStream.close();

    // Send the buffer in a single call to send.
    int sent = send(it->fd, buffer, total_size, 0);
    if (sent == -1 || sent == 0) {
        close(it->fd);
        it->fd = -1;
    }
    delete[] buffer;
}

void Server::_handleGET(pollfdsVectorIterator_t it, const subServersIterator_t &subServersIterator, const Request& request) {

    Response response_ = Response();
    int redirected_ = 0;
    stringVector_t redirect_ = subServersIterator->getRedirect();
    stringVector_t errorPages_ = subServersIterator->getErrorPages();
    stringVector_t allowedMethods_ = subServersIterator->getAllowedMethods();
    string resourcePath_;
    string root_ = subServersIterator->getRoot();
    string uri_ = request.getUri().empty() ? "/" : request.getUri();
    stringVector_t index_ = subServersIterator->getIndex();
    location_t *location_ = matchLocation(subServersIterator->getLocation(), uri_);

    if (location_ != nullptr) {
        if (location_->prefix.find("cgi-bin") != std::string::npos) {
            _handleCGI(it, request, location_);
            return;
        }
        root_ = location_->root;
        index_ = location_->index;
        errorPages_ = location_->errorPages;
        allowedMethods_ = location_->allowedMethods;
        redirect_ = location_->redirect;
    }

    if (!allowedMethods_.empty() && find(allowedMethods_.begin(), allowedMethods_.end(), "GET") == allowedMethods_.end()) {
        response_.setStatusCode(405);
        response_.setHeaders(request, _mimeTypes, _getErrorPage(405, errorPages_));
        resourcePath_ = _getErrorPage(405, errorPages_);

    } else {
        if (!redirect_.empty() && redirect_.at(0) == request.getUri()) {
            uri_  = redirect_.at(1);
            redirected_ = 1;
        }
        resourcePath_ = root_ + uri_;

        if (isDirectory(resourcePath_)) {
            if (resourcePath_.back() != '/') {
				resourcePath_ += "/";
			}
			string indexPage_ = _getIndexPage(resourcePath_, index_);
			resourcePath_ = indexPage_.empty() ? resourcePath_ : indexPage_;
			if (indexPage_.empty() && location_ != nullptr && location_->autoIndex == "on") {
				_generateIndexPage(it, root_, removeTrailingSlashes(uri_));
				return ;
			}
        }

        response_.setStatusCode(request, resourcePath_, _mimeTypes);
        int statusCode_ = response_.getStatusCode();
        if (statusCode_ != 200)
            resourcePath_ = _getErrorPage(statusCode_, errorPages_);
        else if (redirected_)
            response_.setStatusCode(301);
        response_.setHeaders(request, _mimeTypes, resourcePath_);
    }
    sendResponse(it,resourcePath_, response_);
    if (location_ != nullptr )
        delete location_;
}

const string Server::handleFormData(  const Request& request, Response& response){
    int contentLength = std::stoi(request.getHeaders().find("Content-Length")->second);
    const std::string& requestBody = request.getBody();
    std::string form_data = requestBody.substr(0, contentLength);
    response.setStatusCode(200);
    response.setHeaders(request, _mimeTypes, "www/html/success.html");
    return "www/html/success.html";
}

const string Server::handleFileUploads( const Request& request, Response& response, const string& uploadPath, stringVector_t errorPages) {

    string content_type = request.getHeaders().find("Content-Type")->second;
    std::string boundary = "--" + content_type.substr(content_type.find("boundary=") + 9);

    string requestBody = request.getBody();
    int contentLength = std::stoi(request.getHeaders().find("Content-Length")->second);

    std::istringstream request_body_stream(std::string(&requestBody[0], &requestBody[0] + contentLength));
    std::string line;
    std::getline(request_body_stream, line);

    while (std::getline(request_body_stream, line)) {

        if (line.compare(boundary + "--") == 0) {
            // end of file upload
            break;
        } else if (line.find("Content-Disposition:") != std::string::npos) {
            std::string filename;
            if (line.find("filename=\"") != std::string::npos) {
                // file upload
                size_t filename_start = line.find("filename=\"") + 10;
                size_t filename_end = line.find('\"', filename_start);
                if (filename_start != std::string::npos && filename_end != std::string::npos) {
                    filename = line.substr(filename_start, filename_end - filename_start);
                }
                std::getline(request_body_stream, line); // skip Content-Type line
                std::getline(request_body_stream, line); // skip empty line
            } else {
                // form field
                filename = "form_field.txt";
                std::getline(request_body_stream, line); // skip empty line
            }
            std::ofstream file_stream(uploadPath + "/" + filename, std::ios::binary);
            if (file_stream.is_open()) {
                while (std::getline(request_body_stream, line)) {
                    if (line.find(boundary) != std::string::npos || line.find( boundary + "--") != std::string::npos) {
                        // end of file upload
                        break;
                    }
                    if (line == "\r")
                        continue;

                    file_stream << line << std::endl;
                }
                file_stream.close();
                // create response message
                if (line.find( boundary + "--") != std::string::npos) {
                    std::stringstream response_stream;

                    response.setStatusCode(200);
                    response.setHeaders(request, _mimeTypes, "www/html/success_upload.html");
                    return "www/html/success_upload.html";
                }
            } else {
                response.setStatusCode(500);
                response.setHeaders(request, _mimeTypes, _getErrorPage(500, errorPages));
                return _getErrorPage(500, errorPages);
            }
        }
    }
    return "";
}

void Server::_handlePOST(pollfdsVectorIterator_t it, const subServersIterator_t &subServersIterator, const Request& request) {

    string resourcePath_ ;
    Response response_ = Response();
    string clientMaxBodySize_ = subServersIterator->getClientMaxBodySize();
    string uploadPath_ = subServersIterator->getUploadPath();
    stringVector_t allowedMethods_ = subServersIterator->getAllowedMethods();
    stringVector_t errorPages_ = subServersIterator->getErrorPages();
    string root_ = subServersIterator->getRoot();
    string uri_ = request.getUri().empty() ? "/" : request.getUri();
    stringVector_t index_ = subServersIterator->getIndex();
    location_t *location_ = matchLocation(subServersIterator->getLocation(), uri_);

    if (location_ != nullptr) {

        uploadPath_ = location_->uploadPath;
        allowedMethods_ = location_->allowedMethods;
        errorPages_ = location_->errorPages;
        root_ = location_->root;
        clientMaxBodySize_ = location_->clientMaxBodySize;
    }
        if ( !allowedMethods_.empty() && find(allowedMethods_.begin(), allowedMethods_.end(), "POST") == allowedMethods_.end()) {
            response_.setStatusCode(405);
        } else if (request.getBody().size() > convertToBytes(clientMaxBodySize_)) {
            response_.setStatusCode(413);
        } else if (location_ != nullptr && location_->prefix.find("cgi-bin") != std::string::npos) {
            _handleCGI(it, request, location_);
            return;
        } else if (!dirExists(uploadPath_)) {
            if (!createDir(uploadPath_)){
                response_.setStatusCode(500);
            }
        }

    if (response_.getStatusCode() == 200) {
        string content_type = request.getHeaders().find("Content-Type")->second;
        content_type = content_type.substr(0, content_type.find(';'));

        if (content_type == "application/x-www-form-urlencoded") {
            resourcePath_ = handleFormData(request, response_);
        } else if (content_type == "multipart/form-data") {
            resourcePath_ = handleFileUploads(request, response_, uploadPath_, errorPages_);
        } else {
            // unsupported content type
            response_.setStatusCode(400);
        }
    }
    if (response_.getStatusCode() != 200) {
        resourcePath_ = _getErrorPage(response_.getStatusCode(), errorPages_);
        response_.setHeaders(request, _mimeTypes, resourcePath_);
    }

    sendResponse(it, resourcePath_, response_);
    if (location_ != nullptr )
        delete location_;
}

void Server::_handleDELETE(pollfdsVectorIterator_t it, const subServersIterator_t &subServersIterator, const Request& request) {

    Response response_   = Response();
    stringVector_t errorPages_ = subServersIterator->getErrorPages();
    string root_ = subServersIterator->getRoot();
	string uri_ = request.getUri().empty() ? "/" : request.getUri();
	stringVector_t index_ = subServersIterator->getIndex();
	location_t *location_ = matchLocation(subServersIterator->getLocation(), uri_);

	if (location_ != nullptr){
        root_ = location_->root;
        errorPages_ = location_->errorPages;
    }
	string resourcePath_ = root_ + uri_;

    // check if to delete is allowed
        if ((location_ != nullptr) && (find(location_->allowedMethods.begin(), location_->allowedMethods.end(), "DELETE") ==
            location_->allowedMethods.end())) {
            response_.setStatusCode(405);
        } else if (remove(resourcePath_.c_str()) != 0)
            response_.setStatusCode(404);

        if (response_.getStatusCode() == 200)
            resourcePath_ = "www/html/success_delete.html";
        else
            resourcePath_ = _getErrorPage(response_.getStatusCode(), errorPages_);

    response_.setHeaders(request, _mimeTypes, resourcePath_);

    sendResponse(it, resourcePath_, response_);
    if (location_ != nullptr )
        delete location_;
}

string Server::_getErrorPage(int code, stringVector_t error_page) const {
    string error_page_;


    if (!error_page.empty() && to_string(code) == error_page[0])
        return error_page[1];

    return "www/error_pages/" + std::to_string(code) + ".html";
}

void Server::_error(const std::string &msg, int err) const {
	std::string errorMsg = msg + (!err ? "." : (std::string(" | ") + strerror(errno)));
    std::cout << errorMsg << std::endl;
    exit(1);
}

void Server::printData() const {
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Server data:" << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "fds: " << std::endl;
	for (pollfdsVectorConstIterator_t it = _fds.begin(); it != _fds.end(); it++) {
		std::cout << "fd: " << it->fd << std::endl;
		std::cout << "events: " << it->events << std::endl;
		std::cout << "revents: " << it->revents << std::endl;
	}
	for (virtualServerMapConstIterator_t it = _virtualServer.begin(); it != _virtualServer.end(); it++) {
		it->second.printData();
	}
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Server data end" << std::endl;

}

void Server::sendCGIResponse(pollfdsVectorIterator_t it, const Response& response, const string& body) {

    std::ostringstream response_stream;

    response_stream << response.getProtocol() + "/" + response.getVersion() << " ";
    response_stream << response.getStatusCode() << " "<< response.getHttpErrors().at(response.getStatusCode()) << "\r\n" ; //TODO: change to response.getStatus()

    std::map<std::string, std::string>::const_iterator headerIter_ = response.getHeaders().begin();
    std::map<std::string, std::string>::const_iterator headerIterEnd_ = response.getHeaders().end();
    for (; headerIter_ != headerIterEnd_; ++headerIter_) {
        response_stream << headerIter_->first  << ": " << headerIter_->second << "\r\n";
    }
    const string& resp = response_stream.str() + body;

    int sent = send(it->fd, resp.c_str(), resp.size(), 0);
    if (sent == -1 || sent == 0) {
        close(it->fd);
        it->fd = -1;
    }
}

void Server::_handleCGI(pollfdsVectorIterator_t it, const Request &request, location_t *location) {

    Response response;
    _CGIEnv["SERVER_SOFTWARE"] = "webserv/1.0";
    _CGIEnv["SERVER_NAME"] = request.getHost().first;
    _CGIEnv["GATEWAY_INTERFACE"] = "CGI/1.1";
    _CGIEnv["SERVER_PROTOCOL"] = "HTTP/1.1";
    _CGIEnv["SERVER_PORT"] = request.getHost().second;
    _CGIEnv["REQUEST_METHOD"] = request.getMethod();
    _CGIEnv["REQUEST_URI"] = request.getUri();
    _CGIEnv["PATH_INFO"] = location->root + request.getUri();
    _CGIEnv["SCRIPT_NAME"] = request.getUri();
    _CGIEnv["QUERY_STRING"] = request.getQuery();
    _CGIEnv["CONTENT_LENGTH"] = request.getHeaders().find("Content-Length")->second;
    _CGIEnv["CONTENT_TYPE"] = request.getHeaders().find("Content-Type")->second;
    _CGIEnv["HTTP_ACCEPT"] = request.getHeaders().find("Accept")->second;
    _CGIEnv["HTTP_ACCEPT_LANGUAGE"] = request.getHeaders().find("Accept-Language")->second;
    _CGIEnv["HTTP_ACCEPT_ENCODING"] = request.getHeaders().find("Accept-Encoding")->second;
    _CGIEnv["HTTP_CONNECTION"] = request.getHeaders().find("Connection")->second;
    _CGIEnv["HTTP_HOST"] = request.getHeaders().find("Host")->second;
    _CGIEnv["HTTP_USER_AGENT"] = request.getHeaders().find("User-Agent")->second;
    _CGIEnv["HTTP_REFERER"] = request.getHeaders().find("Referer")->second;
    _CGIEnv["HTTP_COOKIE"] = request.getHeaders().find("Cookie")->second;

    CGIHandler  cgiHandler(_CGIEnv, request.getBody(), location );
    string response_body = cgiHandler.CGIExecuter();
    response.setStatusCode(200);
    response.setConnection(request.getHeaders().find("Connection")->second);
    response.setDate();
    response.setServer("webserv/1.0");
    response.setBody(response_body);

    sendCGIResponse(it, response, response_body);
}

string Server::_getIndexPage(const string &path_, const stringVector_t &index_) const {

	stringVectorConstIterator_t it = index_.begin();
	while (it != index_.end()) {
		string index = path_ + *it;
		if (access(index.c_str(), F_OK) == 0)
			return index;
		it++;
	}
	return "";
}

void Server::_generateIndexPage(const pollfdsVectorIterator_t& it, const string &root, const string &uri) {

	IndexGenerator indexGenerator(root, uri);
	string indexPage = indexGenerator.generate();

	ostringstream response;

	response << "HTTP/1.1 200 OK\r\n";
	response << "Content-Type: text/html\r\n";
	response << "Content-Length: " << indexPage.size() << "\r\n";
	response << "Connection: Keep-Alive\r\n";
	response << "Server: webserv/1.0\r\n";
	response << "\r\n";
    response << indexPage;

	int ret = send(it->fd, response.str().c_str(), response.str().size(), 0);
	if (ret == -1 || ret == 0) {
		close(it->fd);
        it->fd = -1;
		return;
	}
}
