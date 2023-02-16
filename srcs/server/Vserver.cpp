//
// Created by Fathiyat olatokunbo Jamia on 2/15/23.
//

#include "Vserver.hpp"


/*
 * Vserver class - This class is responsible for creating a virtual server object and start the server
 *
 * @param port - port number
 * @param host - hostname
 */
Vserver::Vserver(int port, const std::string &host): _port(port), _host(host) {
    std::cout << "\nCreating a virtual server on "
              << "port '" << port
              << "' and host '"<< host << "'\n" << std::endl;
    _setup();
}

/*
 * Setup method - This method is responsible for creating a socket, binding it to a port
 *              and host and listening for incoming connections
 */
void Vserver::_setup() {
    // Create a socket
    int _server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket == -1)
        _error("socket");

    // Set the server socket to non-blocking mode
    if (fcntl(_server_socket, F_SETFL, O_NONBLOCK) < 0)
        _error("fcntl");

    // Set the server socket to reuse the address
    int optval = 1;
    if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        _error("setsockopt");

    // Bind the socket to the port and host
    struct sockaddr_in server_addr = {};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(_port);

    if (bind(_server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        strerror(errno);

    // Listen for incoming connections
    if (listen(_server_socket, SOMAXCONN) == -1)
        _error("listen");

    // Add the server socket to the pollfds list
    struct pollfd server_fd = {};
    server_fd.fd = _server_socket;
    server_fd.events = POLLIN;
    _fds.push_back(server_fd);
}

void Vserver::start() {

    int timeout = 60 * 1000; // 60 seconds

    // Setting up the server
    _setup();

    // Start the server
    while (true) {
        int ready = poll(_fds.data(), _fds.size(), timeout);
        if (ready == -1)
            _error("poll");

        if (ready == 0)
            _error("poll timeout");

        if ( _fds[0].revents & POLLIN ) {
            struct sockaddr_in client_addr = {};
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(_fds[0].fd, (struct sockaddr*)&client_addr, &client_len);
            if (client_socket == -1) {
                perror("accept");
            }
            struct pollfd client = {};
            client.fd = client_socket;
            client.events = POLLIN;
            _fds.push_back(client);
//            _handle_connections(_fds[0].fd);
        }

        for (pollfds_it it = _fds.begin() + 1; it != _fds.end(); ++it) {
            if (it->revents & POLLIN) {
                _handle_request(it);
            }
        }

    }

//    _clear_pollfds();
}

void Vserver::_handle_connections(int sockfd) {
    // Accept an incoming connection
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

void Vserver::_handle_request(pollfds_it it) {
    // Handle the request
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = recv(it->fd, buffer, BUFFER_SIZE, 0);

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

    const char* report = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/html\r\n\r\n"
                         "<h1>Hello World!</h1>";

    send(it->fd, report, strlen(report), 0);
    close(it->fd);
}

void Vserver::_clear_pollfds() {
    pollfds_it it = _fds.begin();
    pollfds_it end = _fds.end();

    for(; it != end; it++)
        close(it->fd);
    _fds.clear();
}

void Vserver::_send_response(int fd) {
// Send the response to the client
    std::string response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n\r\n"
                           "<h1>Hello World!</h1>";
    send(fd, response.c_str(), response.length(), 0);
}

void Vserver::_handle_get(int fd) {
    // Handle the GET request
    // ...
    _send_response(fd);
}

void Vserver::_handle_post(int fd) {
    // Handle the POST request
    // ...
    (void)fd;
}

void Vserver::_handle_delete(int fd) {
    // Handle the DELETE request
    // ...
    (void)fd;
}

void Vserver::_handle_error(int fd) {
    // Handle the error
    // ...
    (void)fd;
}

Vserver::~Vserver() {
    // Close all open file descriptors
}

/* ------------------------ HELPER FUNCTIONS ----------------------------------
 *
 * _error() - prints an error message and exits the program with EXIT_FAILURE
 *
 * @param msg: the error message to print
 *
 * ----------------------------------------------------------------------------*/
void Vserver::_error(const std::string &msg) {
//    strerror(errno);
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}
