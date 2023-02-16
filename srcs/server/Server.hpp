#ifndef SERVER_HPP
#define SERVER_HPP

#include "Vserver.hpp"                  // Vserver class
#include "../config/Configuration.hpp"  // Configuration class
#include <vector>                       // std::vector

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
    typedef std::vector<Vserver>                vserver;
    typedef std::vector<Vserver>::iterator      vserver_it;

private:
    vserver                 _vserver;
    Configuration           _config;

public:
    explicit Server(Configuration config);
    ~Server();

    void start();
};

#endif
