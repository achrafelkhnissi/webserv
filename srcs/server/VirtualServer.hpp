//
// Created by Fathiyat olatokunbo Jamia on 2/15/23.
//

#ifndef WEBSERV_VIRTUALSERVER_HPP
#define WEBSERV_VIRTUALSERVER_HPP

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


class VirtualServer {

private:

    subServerVector_t   _subServers;	// for handling multiple virtual servers
    hostPortPair_t      _hostPort;
    int                 _serverFd;

public:
    VirtualServer();
    VirtualServer(ServerConfig serverConfig);
    ~VirtualServer();

    int     getServerFd() const;
    int     getPort() const;
    void    addSubServer(ServerConfig &config);
    void    printData() const;

    const subServersIterator_t  matchSubServer(const string& host);
};

#endif //WEBSERV_VIRTUALSERVER_HPP
