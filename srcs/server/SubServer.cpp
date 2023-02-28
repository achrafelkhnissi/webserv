//
// Created by Achraf El khnissi on 2/18/23.
//

#include "SubServer.hpp"

SubServer::SubServer(const ServerConfig &config) {

    _serverName = config.server_name;
    _root = config.root;
    _errorPages = config.error_page;
    _clientMaxBodySize = config.client_max_body_size;


    std::vector<LocationConfig>::const_iterator iter_ = config.locations.begin();
    std::vector<LocationConfig>::const_iterator iterEnd_ = config.locations.end();

    for (; iter_ != iterEnd_; ++iter_) {
        location_t location_;
        fillLocation(*iter_, location_);
        _locations.push_back(location_);
    }
}

SubServer::~SubServer() {

}

const stringVector_t & SubServer::getServerName() const {
    return _serverName;
}

const string& SubServer::getRoot() const {
    return _root;
}

const string& SubServer::getIndex() const {
    return _index;
}

const string& SubServer::getErrorPages() const {
    return _errorPages;
}

const string& SubServer::getClientMaxBodySize() const {
    return _clientMaxBodySize;
}

const locationVector_t& SubServer::getLocation() const {
    return _locations;
}



void SubServer::fillLocation(const  LocationConfig& locationConfig, location_t& location) {

//    _location.allowedMethods = ft::split(locationConfig.methods, ' ');
    location.root = locationConfig.root;
    location.index = locationConfig.index;
    location.clientMaxBodySize = locationConfig.client_max_body_size;
    location.errorPages = locationConfig.error_page;
    location.prefix = locationConfig.path;
    location.autoIndex = locationConfig.autoindex == "on";
}

void SubServer::printData() const {
    std::cout << "\t--- Sub server ---" << std::endl;
    std::cout << "\t- server_name: ";
    stringVectorConstIterator_t itt = _serverName.begin();
    stringVectorConstIterator_t itte = _serverName.end();
    while (itt != itte) {
        std::cout << *itt << ", ";
        ++itt;
    }
    std::cout << std::endl;
    std::cout << "\t- root: " << _root << std::endl;
    std::cout << "\t- index: " << _index << std::endl;
    std::cout << "\t- _errorPages: " << _errorPages << std::endl;
    std::cout << "\t- _clientMaxBodySize: " << _clientMaxBodySize << std::endl;
    std::cout << "\t- location: " << std::endl;
    locationVectorConstIterator_t it = _locations.begin();
    locationVectorConstIterator_t ite = _locations.end();
    while (it != ite) {
        std::cout << "\t\tprefix: " << it->prefix << std::endl;
        std::cout << "\t\troot: " << it->root << std::endl;
        std::cout << "\t\tindex: " << it->index << std::endl;
        std::cout << "\t\t_clientMaxBodySize: " << it->clientMaxBodySize << std::endl;
        std::cout << "\t\t_errorPages: " << it->errorPages << std::endl;
        std::cout << "\t\tauto_index: " << it->autoIndex << std::endl;
        ++it;
    }
    std::cout << "\t--- End Sub server ---" << std::endl;
    std::cout << std::endl;
}

