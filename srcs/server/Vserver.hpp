//
// Created by Fathiyat olatokunbo Jamia on 2/15/23.
//

#ifndef WEBSERV_VSERVER_HPP
#define WEBSERV_VSERVER_HPP

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <cerrno>
#include <string>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

/*
 * Vserver class - This class is responsible for creating a virtual server object
 *
 * @var _port - port number
 * @var _end_server - boolean to check if the server is running or not
 * @var _host - hostname
 * @var _fds - vector to store file descriptor information
 *
 * @method Vserver - constructor that takes port and host as an argument
 * @method ~Vserver - destructor
 * @method setup - method to start the server, it will listen for incoming connections
 * @method start_polling - waiting for any I/O operation on the _fds using the poll function
 * @method clear_pollfds - method to clear the pollfds
 * @method handle_connections - method to handle incoming connections, it will accept new connections and add them to the poll list
 * @method handle_request - method to handle incoming requests, it will read the request, parse it and call the appropriate method
 * @method send_response - method to send HTTP responses, it will send the response to the client
 * @method handle_get - method to handle GET requests
 * @method handle_post - method to handle POST requests
 * @method handle_delete - method to handle DELETE requests
 * @method handle_error - method to handle errors
 */
class Vserver {

private:
    typedef std::vector<pollfd>                 pollfds;
    typedef std::vector<pollfd>::iterator       pollfds_it;

private:
    int                 _port;
    std::string         _host;
    pollfds             _fds;

private:
    void _setup();
    void _clear_pollfds();
    void _error(const std::string& msg);

    void _handle_connections(int fd);
    void _handle_request(pollfds_it it);
    void _send_response(int fd);
    void _handle_get(int fd);
    void _handle_post(int fd);
    void _handle_delete(int fd);
    void _handle_error(int fd);

public:
    Vserver(int port, const std::string& host);
    ~Vserver();

    void start();
};

#endif //WEBSERV_VSERVER_HPP
