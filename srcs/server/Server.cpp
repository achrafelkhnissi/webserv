#include "Server.hpp"


/*
 * Server class - This class is responsible for creating a server object
 *
 * @param config: Configuration object that contains all the information from the config file
 */
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

    for (virtualServerMapIterator_t it_ = _virtualServer.begin(); it_ != _virtualServer.end(); ++it_) {
        it_->second.printData();
    }

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

        std::cout << "Accepted connection on fd " << clientFd_ << std::endl;
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
        std::cout << "Closed connection on fd " << it->fd << std::endl;
        close(it->fd);
        _fds.erase(it);
        return;
    }

    buffer_[bytesRead_] = '\0';
    std::cout << "Received " << bytesRead_ << " bytes on fd " << it->fd << std::endl;

    std::cout << "=== REQUEST RECEIVED ===" << std::endl;
    std::cout << buffer_ << std::endl;
    _request.setRequest();
    std::cout << "=== END OF REQUEST ===" << std::endl;

    // Match the port and host to the correct server
    virtualServerMapIterator_t vserverIter_ = _virtualServer.find(_request.getPort());

    subServersIterator_t subServerIter_ = vserverIter_->second.matchSubServer(_request.getHost());

    subServerIter_->printData();

    if (_request.getMethod() == "GET")
        _handleGET(it->fd, subServerIter_, _request);
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




void Server::_handleGET(int fd, const subServersIterator_t &subServersIterator, const Request& request) {


    string root_ = subServersIterator->getRoot();
    string uri_ = request.getUri();
    string index_ = "index.html";
    location_t    location_;

    // Match the uri to the correct location
    locationVectorConstIterator_t locIter_ = subServersIterator->getLocation().begin();
    locationVectorConstIterator_t locIterEnd_ = subServersIterator->getLocation().end();
    for (; locIter_ != locIterEnd_; ++locIter_) {
        if (locIter_->prefix == uri_) {
            location_ = *locIter_;
            break;
        }
    }


    root_ = location_.root;

    string resourcePath_ = root_ + uri_ + "/" + index_;

    std::cout << "index_path: " << resourcePath_ << std::endl;

    std::ifstream resourceFile_;
    std::stringstream responseHeaderStream_;
    string responseHeader_;
    string responseBody_;
    int contentLength_, responseSize_, bytesSent_;

    // open the requested resource file
    resourceFile_.open(resourcePath_, std::ios::binary);
    if (!resourceFile_.is_open()) {
        // if the file does not exist, send a 404 response_
        responseHeaderStream_ << "HTTP/1.1 404 Not Found\r\n\r\n";
        responseHeader_ = responseHeaderStream_.str();
        send(fd, responseHeader_.c_str(), responseHeader_.length(), 0);
        return;
    }

    std::string response_ = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n\r\n" +
        _getFileContent(resourcePath_); // TODO: <-- Test this.

    resourceFile_.seekg(0, std::ios::end);
    contentLength_ = resourceFile_.tellg();
    resourceFile_.seekg(0, std::ios::beg);


    // allocate memory for the response_ body and read the resource file contents into it
    responseBody_.resize(contentLength_);
    resourceFile_.read(&responseBody_[0], contentLength_);

    // construct the response_ header with the content length and send it to the client
    responseHeaderStream_ << "HTTP/1.1 200 OK\r\nContent-Length: " << contentLength_ << "\r\n\r\n";
    responseHeader_ = responseHeaderStream_.str();
    send(fd, responseHeader_.c_str(), responseHeader_.length(), 0);

    // send the response_ body to the client in chunks
    bytesSent_ = 0;
    while (bytesSent_ < contentLength_) {
        responseSize_ = send(fd, &responseBody_[bytesSent_], contentLength_ - bytesSent_, 0);
        if (responseSize_ < 0) {
            // error sending response_ to client
            break;
        }
        bytesSent_ += responseSize_;
    }
//    _sendResponse(fd);
}


//
///*
// *  @disc _handlePOST method - This method is responsible for handling POST requests
// * 							and sending a response to the client
// * 	@param fd: the file descriptor of the client socket
// */
//void Server::_handlePOST(int fd) {
//    // Handle the POST request
//    // ...
//    (void)fd;
//}
//
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

string Server::_getErrorPage(int code) const {
    return "www/errors/error-" + std::to_string(code) + ".html";
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
//
///*
//* _error() - prints an error message and exits the program with EXIT_FAILURE
//*
//* @param msg: the error message to print
//*/
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


std::string Server::_getFileContent(const std::string& path) const {
    std::ifstream file_(path);

    std::cout << "path: " << path << std::endl;

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
