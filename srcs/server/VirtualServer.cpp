//
// Created by Fathiyat olatokunbo Jamia on 2/15/23.
//

#include "VirtualServer.hpp"

VirtualServer::VirtualServer(ServerConfig serverConfig) {

    _hostPort = std::make_pair(serverConfig.host, serverConfig.port);
        this->addSubServer(serverConfig);
    std::cout << "\nCreating a virtual server on "
              << "port '" << _hostPort.second
              << "' and host '" << _hostPort.first << "'\n" << std::endl;
}


VirtualServer::~VirtualServer() {
}


void VirtualServer::addSubServer(ServerConfig &config) {
    _subServers.push_back(SubServer(config));
}

VirtualServer::VirtualServer() {

}

int VirtualServer::getServerFd() const {
    return _serverFd;
}

void VirtualServer::printData() const {
    std::cout << "--- Virtual Server Data ---" << std::endl;
    std::cout << "Server fd: " << _serverFd << std::endl;
    std::cout << "Host: " << _hostPort.first << std::endl;
    std::cout << "Port: " << _hostPort.second << std::endl;
    // print sub servers
    std::cout << "Sub servers: " << std::endl;
    for (subServersConstIterator_t it = _subServers.begin(); it != _subServers.end(); ++it) {
        it->printData();
    }
    std::cout << "--- End Virtual Server ---" << std::endl;
    std::cout << std::endl;
}

int VirtualServer::getPort() const {
    return _hostPort.second;
}

hostPortPair_t VirtualServer::getHostPort() const {
    return _hostPort;
}

const subServersIterator_t VirtualServer::matchSubServer(const string& host) {

    for (subServersIterator_t iter1_ = _subServers.begin(); iter1_ != _subServers.end(); ++iter1_) {
        for (stringVectorConstIterator_t iter2_ = iter1_->getServerName().begin();
             iter2_ != iter1_->getServerName().end(); ++iter2_) {
            if (host == *iter2_)
                return iter1_;
        }
    }
    return _subServers.begin();
}
