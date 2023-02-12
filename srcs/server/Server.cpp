#include "Server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

#define  FALSE 1
#define  TRUE 0
Server::Server(int port, const std::string &host): _port(port), _host(host) { }

void Server::set_up() {

    pollfd listner_fd;
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    fcntl(sockfd, F_SETFL, O_NONBLOCK);

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

    listner_fd.fd = sockfd;
    listner_fd.events = POLLIN;
    _fds.push_back(listner_fd);
    // handle_connection(sockfd);

//    handle_request(sockfd);
    start_polling(sockfd);
}

void Server::start_polling(int listener_socket) {
    int timeout = 60 * 1000; // 1 min for now
    int r;
    do{
        std::cout << "waiting on poll..." << std::endl;
        r = poll(_fds.data(), _fds.size(), timeout);

        if (r < 0){
            perror(" poll system call failed");
            break;
        }
        if (r == 0){
            std::cout << "poll() timed out.";
            break;
        }
        //loop through to find the descriptors that returned

        std::vector<pollfd>::iterator it = _fds.begin();
        std::vector<pollfd>::iterator end = _fds.end();
//        std::vector<pollfd>::iterator it;
        for(; it != end; it++){
            if (it->revents == 0)
                continue;
            if (it->revents != POLLIN){
                perror(" Error! revents ");
                break;
            }
            if (it->fd == listener_socket){
                std::cout << "listener socket is readable" << std::endl;
                //call the handle_connections() to accept all the incoming connections queued up in the listening socket then call pall again
                handle_connections( listener_socket);
            }
            else
                // This is not the listening socket, therefore an
                // existing connection must be readable
                handle_request(it);
        }
    } while (true);

}

void Server::handle_connections(int sockfd) {
    // Accept an incoming connection
    int new_fd;
    do {
        new_fd = accept(sockfd, nullptr, nullptr);
        /* accept each incomming connection if accept fails with EWOULDBLOCK, then
         * we have accepted all of them, else we need to end the server ( to do later) */

        if (new_fd < 0)
        {
            perror("accept failed!");
            break;
        }
        std::cout << "Connection accepted." << std::endl;
        pollfd connection;
        connection.fd = new_fd;
        connection.events = POLLIN;
        _fds.push_back(connection);
    } while (new_fd != -1);
}

void Server::handle_request(std::vector<pollfd>::iterator it) {

//    int new_fd = accept(fd, nullptr, nullptr); // remove this line later.

    // Read the incoming request
    int close_conn = FALSE;
    char buffer[1024];
    do {
        /* receive data in this connection until the rcv fails with EWOULDBLOCK
         * if any other failure occurs, we will close the connetion*/

        int bytes_read = recv(it->fd, buffer, sizeof(buffer), 0);
        if (bytes_read < 0)
        {
            if (errno != EWOULDBLOCK)
                close_conn = TRUE;
            perror("recv failed!");
            break;
        }
        if (bytes_read == 0){ // check to see if the connection has been closed by the client
            std::cout << "Connection closed!";
            close_conn = TRUE;
            break;
        }

        // Parse the request and call the appropriate method
        std::string request(buffer, bytes_read);
        std::string method = request.substr(0, request.find(" "));

        printf("Request:\n\n%s\n", request.c_str());

        if (method == "GET") {
            handle_get(it->fd);
        } else if (method == "POST") {
            handle_post(it->fd);
        } else if (method == "DELETE") {
            handle_delete(it->fd);
        } else {
            handle_error(it->fd);
        }
    }while(true);
    if (close_conn){
        close(it->fd);
        _fds.erase(it);
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
