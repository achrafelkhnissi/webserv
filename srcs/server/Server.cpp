#include "Server.hpp"

/*
 * Server class - This class is responsible for creating a server object
 *
 * @param config: Configuration object that contains all the information from the config file
 */
Server::Server(Configuration config): _config(config) {
    _vserver.push_back(Vserver(config.get_port(), config.get_host()));
    _vserver[0].start();
}

void Server::start() {
    for (std::vector<Vserver>::iterator it = _vserver.begin(); it != _vserver.end(); ++it) {
        it->start();
    }
}

Server::~Server() { }
