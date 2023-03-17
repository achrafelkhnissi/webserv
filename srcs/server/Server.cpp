#include "Server.hpp"
#include "Request.hpp"
#include "utils.hpp"
#include "Response.hpp"

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
//	_mimeTypes[".ico"] = "image/x-icon";

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
    int timeout = 60 * 1000; // 60 seconds

	// Start the server loop
	while (true) {
		int ready_ = poll(_fds.data(), _fds.size(), timeout);

		if (ready_ == -1)
			_error("poll", 1);

		if (ready_ == 0)
			_error("poll timeout", 1);

        for (size_t i = 0; i < _fds.size(); i++) {
            if ((_fds[i].revents & POLLIN) || (_fds[i].revents & POLLOUT)) {
                if (i < _virtualServer.size()){
                    _handleConnections(_fds[i].fd);
                }
                else
                    _handleRequest(_fds.begin() + i);
            }
        }
    }
}

Server::~Server() {
	_clearPollfds();
}

void Server::_handleConnections(int sockfd) {

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

		// Add the clientPollFd_ socket to the pollfds list
		struct pollfd clientPollFd_ = {};
		clientPollFd_.fd = clientFd_;
		clientPollFd_.events = POLLIN;
		_fds.push_back(clientPollFd_);
		_clientHttpParserMap[clientFd_] = HttpParser();
}

void Server::_handleRequest(pollfdsVectorIterator_t it) {

	string requestBuffer_;
	ssize_t bytesRead_ = 0;


	// Versio 1 - Recv until BUFFER_SIZE bytes of data
	char buffer_[BUFFER_SIZE + 1];

	memset(buffer_, 0, BUFFER_SIZE);
	bytesRead_ = recv(it->fd, buffer_, BUFFER_SIZE, 0);
	if (bytesRead_ == -1 || bytesRead_ == 0) {
        close(it->fd);
        _fds.erase(it);
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
			return ;
		}
		// Match the port and host to the correct server
		virtualServerMapIterator_t vserverIter_ = _virtualServer.find(_request.getHost().second);
		subServersIterator_t subServerIter_ = vserverIter_->second.matchSubServer(_request.getHost().first);

		if (_request.getMethod() == "GET") {
			_handleGET(it, subServerIter_, _request);
		} else if (_request.getMethod() == "POST") {
            _handlePOST(it, subServerIter_, _request);
		} else if (_request.getMethod() == "DELETE") {
			_handleDELETE(it, subServerIter_, _request);
		}
        else {
            Response response_ = Response();
            response_.setStatusCode(501);
            response_.setHeaders(_request, _mimeTypes, _getErrorPage(501));
            sendResponseHeaders(it, response_);
            sendResponseBody(it, _getErrorPage(501));
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
		std::size_t found_ = str_.find_last_of('/');
		if (found_ != std::string::npos){
            found_ = found_ == 0 ? 1 : found_;
			str_ = str_.substr(0, found_);
		}

	} while (!str_.empty());
	return nullptr;
}

void Server::sendResponseHeaders(pollfdsVectorIterator_t it, const Response& response) {
	// send HTTP response header
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

	int ret = send(it->fd, response_header.c_str(), response_header.size(), 0);
    if (ret == -1 || ret == 0) {
        close(it->fd);
        _fds.erase(it);
        return ;
    }

}

void Server::sendResponseBody(pollfdsVectorIterator_t it, const string& resourcePath) {

	ifstream fileStream(resourcePath, ios::in | ios::binary);

	if (!fileStream) {
		cerr << "Unable to open file" << endl;
		return;
	}


	char buffer[BUFFER_SIZE];
	while (fileStream) {
        ::memset(buffer, 0, BUFFER_SIZE);
		fileStream.read(buffer, BUFFER_SIZE);

		int bytesRead = fileStream.gcount();
		int sent = send(it->fd, buffer, bytesRead, 0);
        if (sent == -1 || sent == 0) {
            close(it->fd);
            _fds.erase(it);
            break;
        }
	}

	fileStream.close();
}

void Server::sendResponseBody(pollfdsVectorIterator_t it, const Response& response) {

	std::stringstream body_stream;
	std::ifstream file_stream(response.getBody().c_str(), std::ios::binary);
	body_stream << file_stream.rdbuf();
	const size_t CHUNK_SIZE = 1024;
	char buffer[CHUNK_SIZE];


	while (body_stream.good()) {
		body_stream.read(buffer, CHUNK_SIZE);
		size_t bytes_read = body_stream.gcount();
		if (bytes_read <= 0) {
			break;
		}
		int ret = send(it->fd, buffer, bytes_read, 0);
        if (ret == -1 || ret == 0) {
            close(it->fd);
            _fds.erase(it);
            break;
        }
	}
	body_stream.clear();
	file_stream.close();
}

void Server::_handleGET(pollfdsVectorIterator_t it, const subServersIterator_t &subServersIterator, const Request& request) {

    Response response_ = Response();
    string resourcePath_;
    string root_ = subServersIterator->getRoot();
    string uri_ = request.getUri().empty() ? "/" : request.getUri();
    stringVector_t index_ = subServersIterator->getIndex();
    location_t *location_ = matchLocation(subServersIterator->getLocation(), uri_);

    if (location_->prefix.find("cgi-bin") != std::string::npos) {
        _handleCGI(it, subServersIterator, request, location_);
        return;
    }

    if (location_ != nullptr) {
        root_ = location_->root;
        index_ = location_->index;
    }
    if (find(location_->allowedMethods.begin(), location_->allowedMethods.end(), "GET") ==
        location_->allowedMethods.end()) {
        response_.setStatusCode(405);
        response_.setHeaders(request, _mimeTypes, _getErrorPage(405));
        resourcePath_ = _getErrorPage(405);
    } else {
        // Check if the uri is a directory
        resourcePath_ = root_ + uri_;
        if (isDirectory(resourcePath_)) {
            if (resourcePath_.back() != '/')
                resourcePath_ += "/";
            resourcePath_ += index_[0]; //todo: return the first index that exists
        }

        response_.setStatusCode(request, resourcePath_, _mimeTypes);
        int statusCode_ = response_.getStatusCode();
        if (statusCode_ != 200)
            resourcePath_ = _getErrorPage(statusCode_);
        response_.setHeaders(request, _mimeTypes, resourcePath_);

    }
        sendResponseHeaders(it, response_);
        sendResponseBody(it, resourcePath_);
}

const string Server::handleFormData(  const Request& request, Response& response){
    int contentLength = std::stoi(request.getHeaders().find("Content-Length")->second);
    const std::string& requestBody = request.getBody();

    std::string form_data = requestBody.substr(0, contentLength); // TODO: store form data
    response.setStatusCode(200);
    response.setHeaders(request, _mimeTypes, "www/html/success.html");
    return "www/html/success.html";
}

const string Server::handleFileUploads( const Request& request, Response& response, const string& uploadPath){

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
                    // note: when you remove the newline all the files are getting uploaded successfully but empty.
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
                response.setHeaders(request, _mimeTypes, _getErrorPage(500));
                return _getErrorPage(500);
            }
        }
    }
    return "";
}

void Server::_handlePOST(pollfdsVectorIterator_t it, const subServersIterator_t &subServersIterator, const Request& request) {

    string resourcePath_ ;
    Response response_ = Response();
    string root_ = subServersIterator->getRoot();
    string uri_ = request.getUri().empty() ? "/" : request.getUri();
    stringVector_t index_ = subServersIterator->getIndex();
    location_t *location_ = matchLocation(subServersIterator->getLocation(), uri_);

    if (location_ != nullptr) {
        if (find(location_->allowedMethods.begin(), location_->allowedMethods.end(), "POST") ==
        location_->allowedMethods.end()) {
        response_.setStatusCode(405);
        response_.setHeaders(request, _mimeTypes, _getErrorPage(405));
        resourcePath_ = _getErrorPage(405);
        } else if (request.getBody().size() > convertToBytes(location_->clientMaxBodySize)){
            response_.setStatusCode(413);
            response_.setHeaders(request, _mimeTypes, _getErrorPage(413));
            resourcePath_ = _getErrorPage(413);
        } else if (location_->prefix.find("cgi-bin") != std::string::npos) {
            _handleCGI(it, subServersIterator, request, location_);
            return;
        } else if (location_->prefix.find("upload") != std::string::npos)
            _uploadPath = location_->root + uri_;
        else if (!dirExists(_uploadPath)) {
            if (!createDir(_uploadPath))
                throw std::runtime_error("Failed to create upload directory");
        }
    }

    if (response_.getStatusCode() == 200) {
        string content_type = request.getHeaders().find("Content-Type")->second;
        content_type = content_type.substr(0, content_type.find(';'));

        if (content_type == "application/x-www-form-urlencoded") {
            resourcePath_ = handleFormData(request, response_);
        } else if (content_type == "multipart/form-data") {
            resourcePath_ = handleFileUploads(request, response_, _uploadPath);
        } else {
            // unsupported content type
            response_.setStatusCode(400);
            response_.setHeaders(request, _mimeTypes, _getErrorPage(400));
            resourcePath_ = _getErrorPage(400);
        }
    }
    sendResponseHeaders(it, response_);
    sendResponseBody(it, resourcePath_);

}

void Server::_handleDELETE(pollfdsVectorIterator_t it, const subServersIterator_t &subServersIterator, const Request& request) {

	string root_ = subServersIterator->getRoot();
	string uri_ = request.getUri().empty() ? "/" : request.getUri();
	stringVector_t index_ = subServersIterator->getIndex();
	// Match the uri to the correct location
	location_t *location_ = matchLocation(subServersIterator->getLocation(), uri_);

	if (location_ != nullptr)
		root_ = location_->root;

	string resourcePath_ = root_ + uri_;
	Response response_   = Response();

    response_.setStatusCode(request, resourcePath_.c_str(), _mimeTypes);

    // check if to delete is allowed
    if (response_.getStatusCode() == 200 && location_ != nullptr) {
        if (find(location_->allowedMethods.begin(), location_->allowedMethods.end(), "DELETE") ==
            location_->allowedMethods.end()) {
            response_.setStatusCode(405);
        } else {
            if (remove(resourcePath_.c_str()) != 0)
                response_.setStatusCode(404);
        }
    }
    response_.setHeaders(request, _mimeTypes, resourcePath_);
	sendResponseHeaders(it, response_);
}

string Server::_getErrorPage(int code) const {
	return "www/error_pages/" + std::to_string(code) + ".html";
}

void Server::_error(const std::string &msg, int err) const {
	std::string errorMsg = msg + (!err ? "." : (std::string(" | ") + strerror(errno)));
	throw std::runtime_error(errorMsg);
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

void Server::_handleCGI(pollfdsVectorIterator_t it, const subServersIterator_t &iter, const Request &request, location_t *location) {

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
//    _CGIEnv["REMOTE_ADDR"] = request.getHost().first;
//    _CGIEnv["REMOTE_PORT"] = request.getHost().second;
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
    response.setContentLength(response_body.size());
    response.setContentType("text/html");
    response.setConnection(request.getHeaders().find("Connection")->second);
    response.setDate();
    response.setServer("webserv/1.0");
    response.setBody(response_body);

    sendResponseHeaders(it, response);
    int ret = send(it->fd, response.getBody().c_str(), response.getBody().size(), 0);
    if (ret == -1 || ret == 0) {
        close(it->fd);
        _fds.erase(it);
    }
}

