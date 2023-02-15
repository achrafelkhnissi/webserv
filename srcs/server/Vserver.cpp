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
Vserver::Vserver(int port, const std::string &host): _port(port), _end_server(), _host(host) {
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

    pollfd listener_fd = {};
    _end_server = false;
    int optval = 1; // used for setsockopt (option value)

    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //make the socket non-blocking
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    // SO_REUSEADDR 	Allows the socket to be bound to an address that is already in use.
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        strerror(errno);
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a port and host
    struct sockaddr_in server_address = {};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(_port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        strerror(errno);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 10) == -1) {
        strerror(errno);
        exit(EXIT_FAILURE);
    }

    listener_fd.fd = sockfd;
    listener_fd.events = POLLIN;
    _fds.push_back(listener_fd);

    _start_polling(sockfd);
}

/*
 *
 * Start polling method - This method is responsible for waiting for any I/O operation on the _fds using the poll function
 *
 * @param listener_socket - socket that will be used to listen for incoming connections
 *
 */
void Vserver::_start_polling(int listener_socket) {
    int timeout = 60 * 1000; // 1 min for now
    int r;
    do {
        std::cout << "waiting on poll..." << std::endl;
        r = poll(_fds.data(), _fds.size(), timeout);

        if (r < 0){
            strerror(errno);
            break;
        }
        if (r == 0){
            std::cout << "poll() timed out." << std::endl;
            break;
        }

        //loop through to find the descriptors that returned
        pollfds_it it = _fds.begin();
        pollfds_it end = _fds.end();

        // Loop through the pollfd list and check which ones have revents set
        for(; it != end; it++){
            if (it->revents == 0)
                continue;
            if (it->revents != POLLIN) {
                strerror(errno);
                _end_server = true;
                break;
            }
            if (it->fd == listener_socket){
                std::cout << "listener socket is readable" << std::endl;

                /* call the handle_connections() to accept all the incoming connections
                queued up in the listening socket then call pall again */
                _handle_connections( listener_socket);
            }
            else
                /* This is not the listening socket, therefore an
                 existing connection must be readable */
                _handle_request(it);
        }
    } while (_end_server == false);
    // Clean up all the sockets that are open
    _clear_pollfds();
}

void Vserver::_handle_connections(int sockfd) {
    // Accept an incoming connection
    int new_fd;
    do {
        /* accept each incoming connection if accept fails with EWOULDBLOCK, then
        we have accepted all of them, else we need to end the server ( to do later) */
        new_fd = accept(sockfd, nullptr, nullptr);

        if (new_fd < 0)
        {
            if (errno != EWOULDBLOCK){
                strerror(errno);
                _end_server = true;
            }
            break;
        }
        std::cout << "Connection accepted." << std::endl;
        pollfd connection = {};
        connection.fd = new_fd;
        connection.events = POLLIN;
        _fds.push_back(connection);
    } while (true);
}

void Vserver::_handle_request(pollfds_it it) {


    // Read the incoming request
    char buffer[1024] = {};

    do {
        /* receive data in this connection until the rcv fails with EWOULDBLOCK
         * if any other failure occurs, we will close the connetion*/
        ssize_t bytes_read = recv(it->fd, buffer, sizeof(buffer), 0);

        //
        if (bytes_read < 0) {
            strerror(errno);
            break;
        }
        if (bytes_read == 0) {
            std::cout << "Connection closed!" << std::endl;
            break;
        }

        // Parse the request and call the appropriate method
        std::string request(buffer, bytes_read);
        std::string method = request.substr(0, request.find(' '));

        printf("Request:\n\n%s\n", request.c_str());

        if (method == "GET") {
            _handle_get(it->fd);
        } else if (method == "POST") {
            _handle_post(it->fd);
        } else if (method == "DELETE") {
            _handle_delete(it->fd);
        } else {
            _handle_error(it->fd);
        }
    } while(true);

    // Close the connection
    close(it->fd);
    _fds.erase(it);
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
}

void Vserver::_handle_delete(int fd) {
    // Handle the DELETE request
    // ...
}

void Vserver::_handle_error(int fd) {
    // Handle the error
    // ...
}

Vserver::~Vserver() {
    // Close all open file descriptors
}

/* ------------------------ HELPER FUNCTIONS ----------------------------------
 *
 * _errror() - prints an error message and exits the program with EXIT_FAILURE
 *
 * @param msg: the error message to print
 *
 * ----------------------------------------------------------------------------*/
void Vserver::_error(const std::string &msg) {
//    strerror(errno);
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}
