//
// Created by Fathiyat olatokunbo Jamia on 2/15/23.
//

#include "Vserver.hpp"

Vserver::Vserver(ServerConfig server_config) {

    _host_port = std::make_pair(server_config.host, server_config.port);
        this->addSubServer(server_config);
    std::cout << "\nCreating a virtual server on "
              << "port '" << _host_port.second
              << "' and host '"<< _host_port.first << "'\n" << std::endl;
}


void Vserver::_error(const string &msg) {
//    strerror(errno);
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}
Vserver::~Vserver() {
    // Close all open file descriptors
}


void Vserver::addSubServer(ServerConfig &config) {
    _sub_servers.push_back(SubServer(config));
}

Vserver::Vserver() {

}

int Vserver::getServerFd() const {
    return _server_fd;
}

void Vserver::print_data() const {
    std::cout << "--- Virtual Server Data ---" << std::endl;
    std::cout << "Server fd: " << _server_fd << std::endl;
    std::cout << "Host: " << _host_port.first << std::endl;
    std::cout << "Port: " << _host_port.second << std::endl;
    // print sub servers
    std::cout << "Sub servers: " << std::endl;
    for (subServersConstIterator_t it = _sub_servers.begin(); it != _sub_servers.end(); ++it) {
        it->print_data();
    }
    std::cout << "--- End Virtual Server ---" << std::endl;
    std::cout << std::endl;
}

int Vserver::getPort() const {
    return _host_port.second;
}

const subServersIterator_t Vserver::matchSubServer(const string& host) {

    for (subServersIterator_t it = _sub_servers.begin(); it != _sub_servers.end(); ++it) {
        for (stringVectorConstIterator_t it2 = it->getServerName().begin();
             it2 != it->getServerName().end(); ++it2) {
            if (host == *it2)
                return it;
        }
    }
    return _sub_servers.begin();
}
