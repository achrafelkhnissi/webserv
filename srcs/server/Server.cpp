#include "Server.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int port, const std::string& host)
	: _port(port)
	, _host(host) { }

void Server::start() {
	// Create a socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// Bind the socket to a port and host
	struct sockaddr_in server_address = {};
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(_port);
	server_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
		perror("[ERROR]: webserv");
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if (listen(sockfd, 10) == -1) {
		perror("[ERROR]: webserv");
		exit(EXIT_FAILURE);
	}

	handle_connection(sockfd);
	handle_request(sockfd);
}

void Server::handle_connection(int sockfd) {
	// Accept an incoming connection
	int new_fd = accept(sockfd, nullptr, nullptr);
	(void)new_fd;
}

void Server::handle_request(int fd) {
	// Read the incoming request
	char buffer[1024];
	int bytes_read = recv(fd, buffer, sizeof(buffer), 0);

	// Parse the request and call the appropriate method
	std::string request(buffer, bytes_read);
	std::string method = request.substr(0, request.find(" "));

	if (method == "GET") {
		handle_get(fd);
	} else if (method == "POST") {
		handle_post(fd);
	} else if (method == "DELETE") {
		handle_delete(fd);
	} else {
		handle_error(fd);
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
	(void)fd;
}

void Server::handle_post(int fd) {
	// Handle the POST request
	// ...
	(void)fd;
}

void Server::handle_delete(int fd) {
	// Handle the DELETE request
	// ...
	(void)fd;
}

void Server::handle_error(int fd) {
	// Handle the error
	// ...
	(void)fd;
}

Server::~Server() {
	// Close all open file descriptors
}
