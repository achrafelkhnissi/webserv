#include "Server.hpp"

/*
 * Server class - This class is responsible for creating a server object
 *
 * @param config: Configuration object that contains all the information from the config file
 */
 Server::Server(Configuration config): _config(config) {

    vector<ServerConfig> servers = _config.getServers();
    for (vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); ++it) {
        if (_vserver.find(it->port) != _vserver.end()) {
            _vserver[it->port].addSubServer(*it);
        } else {
            _vserver[it->port] = Vserver(*it);
        }
    }
    for (vserver_it it = _vserver.begin(); it != _vserver.end(); ++it) {
        _setup_vserver(it->second);
    }

    for (vserver_it it = _vserver.begin(); it != _vserver.end(); ++it) {
        it->second.print_data();
    }

}

void Server::_setup_vserver(Vserver& vserver) {
    // Create a socket
    int _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1)
        _error("socket");

    // Set the server socket to non-blocking mode
    if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) < 0)
        _error("fcntl");

    // Set the server socket to reuse the address
    int optval = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        _error("setsockopt");

    // Bind the socket to the port and host
    struct sockaddr_in server_addr = {};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(vserver.getPort());

    if (bind(_server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
        strerror(errno);

    // Listen for incoming connections
    if (listen(_server_fd, SOMAXCONN) == -1)
        _error("listen");

    struct pollfd server_fd = {};
    server_fd.fd = _server_fd;
    server_fd.events = POLLIN;
    _fds.push_back(server_fd);

}

void Server::start() {
//    int timeout = 60 * 1000; // 60 seconds

    // Start the server loop
    while (true) {
        int ready = poll(_fds.data(), _fds.size(), -1);

        if (ready == -1)
            _error("poll");

        if (ready == 0)
            _error("poll timeout");


        // Loop through the server fds and handle connections
        for (size_t i = 0; i < _vserver.size(); i++) {
            if (_fds[i].revents & POLLIN) {
                _handle_connections(_fds[i].fd);
            }
        }

        // Loop through the client fds and handle requests
        for (pollfds_it it = _fds.begin() + _vserver.size(); it != _fds.end(); ++it) {
            if (it->revents & POLLIN) {
                _handle_request(it);
            }
        }
    }

}

Server::~Server() {
    _clear_pollfds();
}

void Server::_handle_connections(int sockfd) {

    while (true) {
        struct sockaddr_in client_addr = {};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            } else {
                _error("accept");
            }
        }
        // Add the client socket to the pollfds list
        struct pollfd client = {};
        client.fd = client_fd;
        client.events = POLLIN;
        _fds.push_back(client);

        std::cout << "Accepted connection on fd " << client_fd << std::endl;
    }
}

std::pair<std::string, int> getHostPortFromRequest(const std::string& request) {
    std::string host;
    int port = 80; // default HTTP port

    // Find the Host header in the request
    size_t hostPos = request.find("Host:");
    if (hostPos != std::string::npos) {
        hostPos += 6; // move to the start of the host string
        size_t hostEndPos = request.find("\r\n", hostPos);
        if (hostEndPos != std::string::npos) {
            host = request.substr(hostPos, hostEndPos - hostPos);
        }
    }

    // Find the port number in the host string, if specified
    size_t portPos = host.find(':');
    if (portPos != std::string::npos) {
        port = std::stoi(host.substr(portPos + 1));
        host = host.substr(0, portPos);
    }

    return std::make_pair(host, port);
}

void Server::_handle_request(pollfds_it it) {
    // Handle the request

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = recv(it->fd, buffer, BUFFER_SIZE, 0);

    std::pair<std::string, int> host_port = getHostPortFromRequest(buffer);

    if (bytes_read == -1)
        _error("recv");

    if (bytes_read == 0) {
        std::cout << "Closed connection on fd " << it->fd << std::endl;
        close(it->fd);
        _fds.erase(it);
        return;
    }

    buffer[bytes_read] = '\0';
    std::cout << "Received " << bytes_read << " bytes on fd " << it->fd << std::endl;
    std::cout << buffer << std::endl;

    // Send the response to the client
    _handle_get(it->fd, host_port);
}

void Server::_clear_pollfds() {
    pollfds_it it = _fds.begin();
    pollfds_it end = _fds.end();

    for(; it != end; it++)
        close(it->fd);
    _fds.clear();
}

void Server::_send_response(int fd) {
// Send the response to the client
    std::string response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n\r\n"
                           "<h1>Hello World!</h1>";
    send(fd, response.c_str(), response.length(), 0);
}




void Server::_handle_get(int fd, std::pair<std::string, int> host_port) {


    std::string _root = _vserver[host_port.second].getRoot();
//
    std::cout << "root: " << _root << std::endl;
//    std::string _root = "www/";
    std::string _resource = "index.html";

    std::string resource_path = _root + _resource;

    std::cout << "resource_path: " << resource_path << std::endl;

    std::ifstream resource_file;
    std::stringstream response_header_stream;
    std::string response_header;
    std::string response_body;
    int content_length, response_size, bytes_sent;

    // open the requested resource file
    resource_file.open(resource_path, std::ios::binary);
    if (!resource_file.is_open()) {
        // if the file does not exist, send a 404 response
        response_header_stream << "HTTP/1.1 404 Not Found\r\n\r\n";
        response_header = response_header_stream.str();
        send(fd, response_header.c_str(), response_header.length(), 0);
        return;
    }

    resource_file.seekg(0, std::ios::end);
    content_length = resource_file.tellg();
    resource_file.seekg(0, std::ios::beg);

    // allocate memory for the response body and read the resource file contents into it
    response_body.resize(content_length);
    resource_file.read(&response_body[0], content_length);

    // construct the response header with the content length and send it to the client
    response_header_stream << "HTTP/1.1 200 OK\r\nContent-Length: " << content_length << "\r\n\r\n";
    response_header = response_header_stream.str();
    send(fd, response_header.c_str(), response_header.length(), 0);

    // send the response body to the client in chunks
    bytes_sent = 0;
    while (bytes_sent < content_length) {
        response_size = send(fd, &response_body[bytes_sent], content_length - bytes_sent, 0);
        if (response_size < 0) {
            // error sending response to client
            break;
        }
        bytes_sent += response_size;
    }
//    _send_response(fd);
}


//
///*
// *  @disc _handle_post method - This method is responsible for handling POST requests
// * 							and sending a response to the client
// * 	@param fd: the file descriptor of the client socket
// */
//void Server::_handle_post(int fd) {
//    // Handle the POST request
//    // ...
//    (void)fd;
//}
//
///*
// * @disc _handle_delete method - This method is responsible for handling DELETE requests
// * @param fd: the file descriptor of the client socket
// */
//void Server::_handle_delete(int fd) {
//    // Handle the DELETE request
//    // ...
//    (void)fd;
//}
//
///*
// * @disc _handle_error method - This method is responsible for handling errors
// *
// * @param fd: the file descriptor of the client socket
// */
//void Server::_handle_error(int fd) {
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
void Server::_error(const std::string &msg) {
//    std::cerr << msg << ": ";
//    strerror(errno);
    std::perror(msg.c_str());
    exit(EXIT_FAILURE);
}

void Server::print_data() const {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Server data:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "fds: " << std::endl;
    for (const_pollfds_it it = _fds.begin(); it != _fds.end(); it++) {
        std::cout << "fd: " << it->fd << std::endl;
        std::cout << "events: " << it->events << std::endl;
        std::cout << "revents: " << it->revents << std::endl;
    }
    for (const_vserver_it it = _vserver.begin(); it != _vserver.end(); it++) {
        it->second.print_data();
    }
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Server data end" << std::endl;

}
