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

#include "SubServer.hpp"
#include "../config/Configuration.hpp"  // Configuration class


#define BUFFER_SIZE 1024

class Vserver {

private:

	typedef std::pair<std::string, int>			host_port;
    typedef std::vector<SubServer>              subServers;
    typedef subServers::iterator                subServers_it;

private:

    host_port 			_host_port;
    int                 _server_fd;

	subServers          _sub_servers;	// for handling multiple virtual servers


public:
    void _setup();
    void _error(const std::string& msg);
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

    const std::vector<SubServer>::iterator matchSubServer(const std::string& host);
    const location matchLocation(const std::vector<SubServer>& );

};

#endif //WEBSERV_VSERVER_HPP
