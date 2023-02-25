#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>                       // std::vector

#include "Vserver.hpp"                  // Vserver class
#include "SubServer.hpp"                // SubServer class
#include "Configuration.hpp"  // Configuration class
#include "Request.hpp"       // Request class
#include "typedefs.hpp"

#include <sstream>

/*
 * Server class - This class is responsible for creating a server object
 *
 * @var _vserver - vector of Vserver objects (one for each virtual server)
 * @var _config - Configuration object that contains all the information from the config file
 *
 * @method Server - constructor that takes a Configuration object as an argument
 * @method ~Server - destructor
 * @method start - method to start the server
 */
class Server {

//private:
//    typedef std::vector<pollfd>                         pollfdsVector_t;
//    typedef std::vector<pollfd>::iterator               pollfdsVectorIterator_t;
//    typedef std::vector<pollfd>::const_iterator         pollfdsVectorConstIterator_t;

private:
    virtualServerMap_t      _vserver;
    Configuration           _config;
    pollfdsVector_t         _fds;
	errorPagesMap_t 	    _error_pages;
    Request                 _request;


public:
    Server(Configuration config);
    ~Server();

    void start();
    void _handle_connections(int fd);
    void _handle_request(pollfdsVectorIterator_t it);
    void _send_response(int fd);
    void _handle_get(int fd, const subServersIterator_t&, const Request&);
    void _handle_post(int fd);
    void _handle_delete(int fd);
    void _handle_error(int fd);
    void _clear_pollfds();
    void _error(const string& msg);
    void _setup_vserver(Vserver& vserver);
    void print_data() const;

    string getFileContent(const string& path) const;

};

#endif
