#include "Server.hpp"

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

//    for (virtualServerMapIterator_t it_ = _virtualServer.begin(); it_ != _virtualServer.end(); ++it_) {
//        it_->second.printData();
//    }

}

void Server::_setupVirtualServer(VirtualServer& vserver) {
    // Create a socket
    int serverFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd_ == -1)
        _error("socket");

    // Set the server socket to non-blocking mode
    if (fcntl(serverFd_, F_SETFL, O_NONBLOCK) < 0)
        _error("fcntl");

    // Set the server socket to reuse the address
    int optionValue_ = 1;
    if (setsockopt(serverFd_, SOL_SOCKET, SO_REUSEADDR, &optionValue_, sizeof(optionValue_)) < 0)
        _error("setsockopt");

    // Bind the socket to the port and host
    struct sockaddr_in serverAddr_ = {};
    memset(&serverAddr_, 0, sizeof(serverAddr_));
    serverAddr_.sin_family = AF_INET;
    serverAddr_.sin_addr.s_addr = INADDR_ANY;
    serverAddr_.sin_port = htons(vserver.getPort());

    if (bind(serverFd_, (struct sockaddr *) &serverAddr_, sizeof(serverAddr_)) == -1)
        strerror(errno);

    // Listen for incoming connections
    if (listen(serverFd_, SOMAXCONN) == -1)
        _error("listen");

    struct pollfd serverPollFd_ = {};
    serverPollFd_.fd = serverFd_;
    serverPollFd_.events = POLLIN;
    _fds.push_back(serverPollFd_);

}

void Server::start() {
//    int timeout = 60 * 1000; // 60 seconds

    // Start the server loop
    while (true) {
        int ready_ = poll(_fds.data(), _fds.size(), -1);

        if (ready_ == -1)
            _error("poll");

        if (ready_ == 0)
            _error("poll timeout");


        // Loop through the server fds and handle connections
        for (size_t i = 0; i < _virtualServer.size(); i++) {
            if (_fds[i].revents & POLLIN) {
                _handleConnections(_fds[i].fd);
            }
        }

        // Loop through the client fds and handle requests
        for (pollfdsVectorIterator_t it = _fds.begin() + _virtualServer.size(); it != _fds.end(); ++it) {
            if (it->revents & POLLIN) {
                _handleRequest(it);
            }
        }
    }

}

Server::~Server() {
    _clearPollfds();
}

void Server::_handleConnections(int sockfd) {

    while (true) {
        struct sockaddr_in sockAddrIn_ = {};
        socklen_t clientLen_ = sizeof(sockAddrIn_);
        int clientFd_ = accept(sockfd, (struct sockaddr*)&sockAddrIn_, &clientLen_);
        if (clientFd_ == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            } else {
                _error("accept");
            }
        }
        // Add the clientPollFd_ socket to the pollfds list
        struct pollfd clientPollFd_ = {};
        clientPollFd_.fd = clientFd_;
        clientPollFd_.events = POLLIN;
        _fds.push_back(clientPollFd_);
    }
}

void Server::_handleRequest(pollfdsVectorIterator_t it) {
    // Handle the request

    string method_ = _request.getMethod();


    char buffer_[BUFFER_SIZE];
    ssize_t bytesRead_ = recv(it->fd, buffer_, BUFFER_SIZE, 0);

    if (bytesRead_ == -1)
        _error("recv");

    if (bytesRead_ == 0) {
        close(it->fd);
        _fds.erase(it);
        return;
    }

    buffer_[bytesRead_] = '\0';

    std::cout << "=== REQUEST RECEIVED ===" << std::endl;
    std::cout << buffer_ << std::endl;
    _request.setRequest();
    std::cout << "=== END OF REQUEST ===" << std::endl;

    // Match the port and host to the correct server
    virtualServerMapIterator_t vserverIter_ = _virtualServer.find(_request.getPort());

    subServersIterator_t subServerIter_ = vserverIter_->second.matchSubServer(_request.getHost());

//    subServerIter_->printData();

    if (_request.getMethod() == "GET") {
        _handleGET(it->fd, subServerIter_, _request);
    } else if (_request.getMethod() == "POST") {
//        _handlePOST(it->fd, _request);
    } else if (_request.getMethod() == "DELETE") {
        _handleDELETE(it->fd,subServerIter_,  _request);
	}
}

void Server::_clearPollfds() {
    pollfdsVectorIterator_t iter_ = _fds.begin();
    pollfdsVectorIterator_t iterEnd_ = _fds.end();

    for(; iter_ != iterEnd_; iter_++)
        close(iter_->fd);
    _fds.clear();
}

void Server::_sendResponse(int fd) {
// Send the response_ to the client
    string response_ = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n\r\n"
                           "<h1>Hello World!</h1>";
    send(fd, response_.c_str(), response_.length(), 0);
}

bool Server::_isDirectory(const std::string &dirPath) const {
        struct stat info = {};
        return stat(dirPath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
    }

void Server::_handleGET(int fd, const subServersIterator_t &subServersIterator, const Request& request) {

	string root_ = subServersIterator->getRoot();
	string uri_ = request.getUri().empty() ? "/" : request.getUri();
	string index_ = subServersIterator->getIndex();
	std::cout << "index: " <<  index_ << std::endl;
	location_t location_ = {};

	// Match the uri to the correct location

	string str_ = uri_;

	bool isMatch_ = false;
	do {
		locationVectorConstIterator_t locIter_ = subServersIterator->getLocation().begin();
		locationVectorConstIterator_t locIterEnd_ = subServersIterator->getLocation().end();
		for (; locIter_ != locIterEnd_; ++locIter_) {
			if (locIter_->prefix == str_) {
				location_ = *locIter_;
				isMatch_ = true;
				break;
			}
		}
		std::size_t found_ = str_.find_last_of("/");
		if (found_ != std::string::npos){
			str_ = str_.substr(0, found_);
		}

	} while (!str_.empty() && !isMatch_);

//	if (!isMatch_) {
////        std::stringstream response_stream;
////        response_stream << "HTTP/1.1 404 Not Found\r\n\r\n"; //TODO: send 404.html
////		// 	string resourcePath_ = "www/errors/404/error-404.html";
////        std::string response = response_stream.str();
////        send(fd, response.c_str(), response.length(), 0);
////        return;
//	}

// TODO: if is match then use the location infos
	if (isMatch_){
		root_ = location_.root;
		index_ = location_.index;
	}


	// Check if the uri is a directory

    string resourcePath_ = root_ + uri_;
	if (_isDirectory(resourcePath_)){
		if (resourcePath_.back() != '/')
			resourcePath_ += "/";
		resourcePath_ += index_;
	}

	std::cout << "index_path: " << resourcePath_ << std::endl;

	std::ifstream file_stream(resourcePath_, std::ios::binary);
    if (!file_stream.is_open()) {
		std::cout << "File not opened!" << std::endl;
		::perror("open");
		// file not found, send 404 error
		file_stream.close();
		exit(1);
    }

	stringstream string_stream;
	string_stream << file_stream.rdbuf();

    //get file size
    file_stream.seekg(0, std::ios::end);
    size_t content_length = file_stream.tellg();
    file_stream.seekg(0, std::ios::beg);

    // set content type based on file extension
    std::string content_type;
    std::string extension = resourcePath_.substr(resourcePath_.find_last_of(".") + 1);
	if (extension == "html") {
		content_type = "text/html";
	} else if (extension == "jpg" || extension == "jpeg") {
        content_type = "image/jpeg";
    } else if (extension == "png") {
        content_type = "image/png";
    } else if (extension == "css") {
        content_type = "text/css";
    } else if (extension == "mov") {
        content_type = "video/mov";
    } else if (extension == "mp3") {
        content_type = "audio/mpeg";
	} else if (extension == "mp4") {
        content_type = "video/mp4";
    } else if (extension == "webm") {
        content_type = "video/webm";
    } else {
        // unknown file type, send 415 error
        std::stringstream response_stream;
        response_stream << "HTTP/1.1 415 Unsupported Media Type\r\n\r\n"; //TODO: test without stream
        std::string response = response_stream.str();
        send(fd, response.c_str(), response.length(), 0);
		file_stream.close();
		response_stream.clear();
        return;
    }

    // send HTTP response header
    std::stringstream response_stream;
    response_stream << "HTTP/1.1 200 OK\r\n";
    //response_stream << "Content-Length: " << string_stream.str().size() << "\r\n";
	response_stream << "Content-Length: " << content_length << "\r\n";
    response_stream << "Content-Type: " << content_type << "\r\n\r\n";
    std::string response_header = response_stream.str();
    send(fd, response_header.c_str(), response_header.length(), 0);

    // send file contents in chunks
    const size_t CHUNK_SIZE = 1024;
    char buffer[CHUNK_SIZE];

	std::cout << "Sending file..." << std::endl;

    while (string_stream.good()) {
        string_stream.read(buffer, CHUNK_SIZE);
        size_t bytes_read = string_stream.gcount();
        if (bytes_read <= 0) {
            break;
        }
        send(fd, buffer, bytes_read, 0);
		std::cout << "bytes_read: " << bytes_read << std::endl;
    }
	std::cout << "Done sending file" << std::endl;
	response_stream.clear();
	file_stream.close();
}

//void Server::_handlePOST(int clientSocket, const Request& request) {
//    (void) request;
//    // read request headers from client
//    std::string request_headers;
//    char buffer[1024];
//    int bytes_received;
//    do {
//        bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
//        if (bytes_received < 0) {
//            // error reading request headers
//            return;
//        }
//        request_headers.append(buffer, bytes_received);
//    } while (bytes_received == sizeof(buffer));
//
//    // parse request headers to get content length and content type
//    std::istringstream request_stream(request_headers);
//    std::string line;
//    size_t content_length = 0;
//    std::string content_type;
//    while (std::getline(request_stream, line)) {
//        if (line.find("Content-Length:") == 0) {
//            content_length = std::stoi(line.substr(16));
//        } else if (line.find("Content-Type:") == 0) {
//            content_type = line.substr(14);
//        }
//    }
//    // handle request based on content type
//    if (content_type == "application/x-www-form-urlencoded") {
//        // handle form data
//        std::string form_data(&request_body[0], &request_body[0] + content_length);
//        // process form data...
//        // create response message
//        std::stringstream response_stream;
//        response_stream << "HTTP/1.1 200 OK\r\n\r\n";
//        response_stream << "<html><body><h1>Form Submitted Successfully</h1></body></html>";
//        std::string response = response_stream.str();
//        send(clientSocket, response.c_str(), response.length(), 0);
//    } else if (content_type == "multipart/form-data") {
//        // handle file upload
//        std::string boundary = "--" + content_type.substr(content_type.find("boundary=") + 9);
//        std::istringstream request_body_stream(std::string(&request_body[0], &request_body[0] + content_length));
//        std::string line;
//        std::getline(request_body_stream, line);
//        while (std::getline(request_body_stream, line)) {
//            if (line == boundary || line == boundary + "--") {
//                // end of file upload
//                break;
//            } else if (line.find("Content-Disposition:") == 0) {
//                std::string filename;
//                size_t filename_start = line.find("filename=\"") + 10;
//                size_t filename_end = line.find("\"", filename_start);
//                if (filename_start != std::string::npos && filename_end != std::string::npos) {
//                    filename = line.substr(filename_start, filename_end - filename_start);
//                }
//                std::getline(request_body_stream, line); // skip Content-Type line
//                std::getline(request_body_stream, line); // skip empty line
//                std::ofstream file_stream(_uploadPath + filename, std::ios::binary);
//                if (file_stream.is_open()) {
//                    while (std::getline(request_body_stream, line)) {
//                        if (line == boundary || line == boundary + "--") {
//                            // end of file upload
//                            break;
//                        }
//                        file_stream << line << "\n";
//                    }
//                    file_stream.close();
//                    // create response message
//                    std::stringstream response_stream;
//                    response_stream << "HTTP/1.1 200 OK\r\n\r\n";
//                    response_stream << "<html><body><h1>File Uploaded Successfully</h1></body></html>";
//                    std::string response = response_stream.str();
//                    send(clientSocket, response.c_str(), response.length(), 0);
//                } else {
//                    // error opening file stream
//                    return;
//                }
//            }
//        }
//    } else {
//        // unsupported content type
//        std::stringstream response_stream;
//        response_stream << "HTTP/1.1 400 Bad Request\r\n\r\n";
//        response_stream << "<html><body><h1>Unsupported Content Type</h1></body></html>";
//        std::string response = response_stream.str();
//        send(clientSocket, response.c_str(), response.length(), 0);
//    }
//}

///*
// * @disc _handleDELETE method - This method is responsible for handling DELETE requests
// * @param fd: the file descriptor of the client socket
// */
//void Server::_handleDELETE(int fd) {
//    // Handle the DELETE request
//    // ...
//    (void)fd;
//}
//
///*
// * @disc _handleEerror method - This method is responsible for handling errors
// *
// * @param fd: the file descriptor of the client socket
// */

bool Server::is_regular_file(const char* path) const {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        // error occurred while checking file status
        return false;
    }
    return S_ISREG(path_stat.st_mode);
}

void Server::_handleDELETE(int clientSocket , const subServersIterator_t &subServersIterator, const Request& request) {

	string root_ = subServersIterator->getRoot();
	string uri_ = request.getUri().empty() ? "/" : request.getUri();
	string index_ = subServersIterator->getIndex();
	std::cout << "index: " <<  index_ << std::endl;
	location_t location_ = {};

	// Match the uri to the correct location

	string str_ = uri_;

	bool isMatch_ = false;
	do {
		locationVectorConstIterator_t locIter_ = subServersIterator->getLocation().begin();
		locationVectorConstIterator_t locIterEnd_ = subServersIterator->getLocation().end();
		for (; locIter_ != locIterEnd_; ++locIter_) {
			if (locIter_->prefix == str_) {
				location_ = *locIter_;
				isMatch_ = true;
				break;
			}
		}
		std::size_t found_ = str_.find_last_of("/");
		if (found_ != std::string::npos){
			str_ = str_.substr(0, found_);
		}

	} while (!str_.empty() && !isMatch_);

	if (isMatch_){
		root_ = location_.root;
		index_ = location_.index;
	}

	string resourcePath_ = root_ + uri_;
	std::cout << "resourcePath in the handleDelete function : " <<  resourcePath_ << std::endl;
	if (_isDirectory(resourcePath_)){
		if (resourcePath_.back() != '/')
			resourcePath_ += "/";
		resourcePath_ += index_;
	}

	// check if the file is regular file
	if (!is_regular_file(resourcePath_.c_str())) {
		// create response message
		std::stringstream response_stream;
		response_stream << "HTTP/1.1 404 Not Found\r\n\r\n";
		response_stream << "<html><body><h1>File Not Found</h1></body></html>";
		std::string response = response_stream.str();
		send(clientSocket, response.c_str(), response.length(), 0);
		return;
	}

	if (remove(resourcePath_.c_str()) != 0) {
		// create response message
		std::stringstream response_stream;
		response_stream << "HTTP/1.1 500 Internal Server Error\r\n\r\n";
		response_stream << "<html><body><h1>File Not Found</h1></body></html>";
		std::string response = response_stream.str();
		send(clientSocket, response.c_str(), response.length(), 0);
	} else {
	// create response message
	std::stringstream response_stream;
	response_stream << "HTTP/1.1 200 OK\r\n\r\n";
	response_stream << "<html><body><h1>File Deleted Successfully</h1></body></html>";
	std::string response = response_stream.str();
	send(clientSocket, response.c_str(), response.length(), 0);
}

}
string Server::_getErrorPage(int code) const {
    return "www/errors/404/error-" + std::to_string(code) + ".html";
}


//void Server::_handleEerror(int fd) {
//    // Handle the error
//    // ...
//    (void)fd;
//
//    // TODO: Structure to handle errors (not completed yet)
//    int status_code = 200;
//    switch (status_code) {
//        case 200:
//            std::cout << "HTTP/1.1 200 OK\r\n\r\n";
//            std::cout << "<html><body><h1>Hello, world!</h1></body></html>";
//            break;
//        case 403:
//            std::cout << "HTTP/1.1 403 Forbidden\r\n\r\n";
//            std::cout << "<html><body><h1>403 Forbidden</h1></body></html>";
//            break;
//        case 404:
//            std::cout << "HTTP/1.1 404 Not Found\r\n\r\n";
//            std::cout << "<html><body><h1>404 Not Found</h1></body></html>";
//            break;
//        case 500:
//            std::cout << "HTTP/1.1 500 Internal Server Error\r\n\r\n";
//            std::cout << "<html><body><h1>500 Internal Server Error</h1></body></html>";
//            break;
//        default:
//            std::cout << "HTTP/1.1 400 Bad Request\r\n\r\n";
//            std::cout << "<html><body><h1>400 Bad Request</h1></body></html>";
//            break;
//    }
//}

void Server::_error(const string &msg) {
//    std::cerr << msg << ": ";
//    strerror(errno);
    std::perror(msg.c_str());
    exit(EXIT_FAILURE);
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


string Server::_getFileContent(const std::string& path) const {
    std::ifstream file_(path);

    std::stringstream fileContent_;
    std::string line_;
    while (std::getline(file_, line_)) {
        fileContent_ << line_ << std::endl;
    }
    return fileContent_.str();
 }

/*
1. If Request-URI is an absoluteURI, the host is part of the
        Request-URI. Any Host header field value in the request MUST be
ignored.

2. If the Request-URI is not an absoluteURI, and the request includes
a Host header field, the host is determined by the Host header
        field value.

3. If the host as determined by rule 1 or 2 is not a valid host on
the server, the response MUST be a 400 (Bad Request) error message.
*/
