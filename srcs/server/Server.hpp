#ifndef SERVER_HPP
#define SERVER_HPP

#include "Vserver.hpp"                  // Vserver class
#include "../config/Configuration.hpp"  // Configuration class
#include <vector>                       // std::vector
#include "../request/Request.hpp"       // Request class

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

private:
    typedef std::vector<pollfd>                     pollfds;
    typedef std::vector<pollfd>::iterator           pollfds_it;
    typedef std::vector<pollfd>::const_iterator     const_pollfds_it;
    typedef std::map<int, Vserver>                  vserver;
    typedef vserver::iterator                       vserver_it;
    typedef vserver::const_iterator                 const_vserver_it;
	typedef std::map<int, std::string>			    error_pages;

private:
    vserver                 _vserver;
    Configuration           _config;
    pollfds                 _fds;
	error_pages 			_error_pages;
    Request                 _request;


public:
    Server(Configuration config);
    ~Server();

    void start();
    void _handle_connections(int fd);
    void _handle_request(pollfds_it it);
    void _send_response(int fd);
    void _handle_get(int fd, std::pair<std::string, int> host_port);
    void _handle_post(int fd);
    void _handle_delete(int fd);
    void _handle_error(int fd);
    void _clear_pollfds();
    void _error(const std::string& msg);
    void _setup_vserver(Vserver& vserver);
    void print_data() const;
};

#endif
