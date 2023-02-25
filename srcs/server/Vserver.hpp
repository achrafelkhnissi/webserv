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
#include <fstream>
#include <sstream>

//#include "SubServer.hpp"
#include "typedefs.hpp"
#include "Configuration.hpp"  // Configuration class


#define BUFFER_SIZE 1024


class Vserver {

private:

    subServerVector_t         _sub_servers;	// for handling multiple virtual servers
    hostPortPair_t 			_host_port;
    int                     _server_fd;



public:
    void _setup();
    void _error(const string& msg);
    void start();


public:
    Vserver();
    Vserver(ServerConfig config);
    ~Vserver();

    int getServerFd() const;
    void addSubServer(ServerConfig &config);
    void print_data() const;
    int getPort() const;
//    const std::string& getRoot() const;

    const subServersIterator_t matchSubServer(const string& host);
    const location_t matchLocation(const subServerVector_t& );

};

#endif //WEBSERV_VSERVER_HPP
