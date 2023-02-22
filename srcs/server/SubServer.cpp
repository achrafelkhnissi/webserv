//
// Created by Achraf El khnissi on 2/18/23.
//

#include "SubServer.hpp"

SubServer::SubServer(const ServerConfig &config) {

    _server_name = config.server_name;
    _root = config.root;
    _error_pages = config.error_page;
    _client_max_body_size = config.client_max_body_size;

    std::vector<LocationConfig>::const_iterator it = config.locations.begin();
    std::vector<LocationConfig>::const_iterator ite = config.locations.end();

    for (; it != ite; ++it) {
        location location;
        fill_location(*it, location);
        _locations.push_back(location);
    }
}

SubServer::~SubServer() {

}

const std::vector<std::string>& SubServer::getServerName() const {
    return _server_name;
}

const std::string& SubServer::getRoot() const {
    return _root;
}

const std::string& SubServer::getIndex() const {
    return _index;
}

const std::string& SubServer::getErrorPages() const {
    return _error_pages;
}

const std::string& SubServer::getClientMaxBodySize() const {
    return _client_max_body_size;
}

const std::vector<location>& SubServer::getLocation() const {
    return _locations;
}



void SubServer::fill_location(const  LocationConfig& location_config, location& location) {

//    _location.allowed_methods = ft::split(location_config.methods, ' ');
    location.root = location_config.root;
    location.index = location_config.index;
    location.client_max_body_size = location_config.client_max_body_size;
    location.error_pages = location_config.error_page;
    location.prefix = location_config.path;
    location.auto_index = location_config.autoindex == "on";
}

void SubServer::print_data() const {
    std::cout << "\t--- Sub server ---" << std::endl;
    std::cout << "\t- server_name: ";
    std::vector<std::string>::const_iterator itt = _server_name.begin();
    std::vector<std::string>::const_iterator itte = _server_name.end();
    while (itt != itte) {
        std::cout << *itt << ", ";
        ++itt;
    }
    std::cout << std::endl;
    std::cout << "\t- root: " << _root << std::endl;
    std::cout << "\t- index: " << _index << std::endl;
    std::cout << "\t- error_pages: " << _error_pages << std::endl;
    std::cout << "\t- client_max_body_size: " << _client_max_body_size << std::endl;
    std::cout << "\t- location: " << std::endl;
    std::vector<location>::const_iterator it = _locations.begin();
    std::vector<location>::const_iterator ite = _locations.end();
    while (it != ite) {
        std::cout << "\t\tprefix: " << it->prefix << std::endl;
        std::cout << "\t\troot: " << it->root << std::endl;
        std::cout << "\t\tindex: " << it->index << std::endl;
        std::cout << "\t\tclient_max_body_size: " << it->client_max_body_size << std::endl;
        std::cout << "\t\terror_pages: " << it->error_pages << std::endl;
        std::cout << "\t\tauto_index: " << it->auto_index << std::endl;
        ++it;
    }
    std::cout << "\t--- End Sub server ---" << std::endl;
    std::cout << std::endl;
}
