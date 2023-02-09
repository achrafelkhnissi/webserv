#include "Server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

Server::Server(int port, const std::string& host) : _port(port), _host(host) { }

void Server::start() {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Bind the socket to a port and host
    struct sockaddr_in server_address = {};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(_port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("[ERROR]: webserv");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 10) == -1) {
        perror("[ERROR]: webserv");
        exit(EXIT_FAILURE);
    }

    // handle_connection(sockfd);

    handle_request(sockfd);
}

void Server::handle_connection(int sockfd) {
    // Accept an incoming connection
    int new_fd = accept(sockfd, nullptr, nullptr);
    std::cout << "Connection accepted." << std::endl;
}

void Server::handle_request(int fd) {

    int new_fd = accept(fd, nullptr, nullptr); // remove this line later.

    // Read the incoming request
    char buffer[1024];
    int bytes_read = recv(new_fd, buffer, sizeof(buffer), 0);

    // Parse the request and call the appropriate method
    std::string request(buffer, bytes_read);
    std::string method = request.substr(0, request.find(" "));

    printf("Request:\n\n%s\n", request.c_str());

    if (method == "GET") {
        handle_get(new_fd);
    } else if (method == "POST") {
        handle_post(new_fd);
    } else if (method == "DELETE") {
        handle_delete(new_fd);
    } else {
        handle_error(new_fd);
    }
}

void Server::send_response(int fd) {
// Send the response to the client
    std::string response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n\r\n"
                           "<h1>Hello World!</h1>";
    send(fd, response.c_str(), response.length(), 0);
}

void Server::handle_get(int fd) {
// Handle the GET request
// ...
    send_response(fd);
}

void Server::handle_post(int fd) {
// Handle the POST request
// ...
}

void Server::handle_delete(int fd) {
// Handle the DELETE request
// ...
}

void Server::handle_error(int fd) {
// Handle the error
// ...
}

Server::~Server() {
    // Close all open file descriptors
}
