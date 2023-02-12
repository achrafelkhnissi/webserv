#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <poll.h>

class Server {

private:
    int         _port; // port number
    std::string _host; // hostname
    std::vector<pollfd> _fds; // vector to store file descriptor information

public:
    Server(int port, const std::string& host); // constructor that takes port and host as an argument
    ~Server();

    void set_up(); // method to start the server, it will listen for incoming connections
    void start_polling(int listener_socket); // waiting for any I/O operation on the _fds using the poll function

    void handle_connections(int fd); // method to handle incoming connections, it will accept new connections and add them to the poll list
    void handle_request(std::vector<pollfd>::iterator it); // method to handle incoming requests, it will read the request, parse it and call the appropriate method
    void send_response(int fd); // method to send HTTP responses, it will send the response to the client
    void handle_get(int fd); // method to handle GET requests
    void handle_post(int fd); // method to handle POST requests
    void handle_delete(int fd); // method to handle DELETE requests
    void handle_error(int fd); // method to handle errors
};

#endif
