#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>                       // std::vector

#include "VirtualServer.hpp"                  // VirtualServer class
#include "SubServer.hpp"                // SubServer class
#include "Configuration.hpp"  // Configuration class
#include "Request.hpp"       // Request class
#include "typedefs.hpp"

#include <sstream>

/*
 * Server class - This class is responsible for creating a server object
 *
 * @var _virtualServer - vector of VirtualServer objects (one for each virtual server)
 * @var _config - Configuration object that contains all the information from the config file
 *
 * @method Server - constructor that takes a Configuration object as an argument
 * @method ~Server - destructor
 * @method start - method to start the server
 */
class Server {

private:
    virtualServerMap_t      _virtualServer;
    Configuration           _config;
    pollfdsVector_t         _fds;
	errorPagesMap_t 	    _errorPages;
    Request                 _request;


public:
    Server(Configuration config);
    ~Server();

    void    start();
    void    printData() const;

private:
    void    _handleConnections(int fd);
    void    _handleRequest(pollfdsVectorIterator_t it);
    void    _sendResponse(int fd);
    void    _handleGET(int fd, const subServersIterator_t&, const Request&);
    void    _handlePOST(int fd);
    void    _handleDELETE(int fd);
    void    _handleEerror(int fd);
    void    _clearPollfds();
    void    _error(const string& msg);
    void    _setupVirtualServer(VirtualServer& vserver);

    string  _getErrorPage(int code) const;
    string  _getFileContent(const string& path) const;

};

#endif
