
#include "SubServer.hpp"
#include "CGIHandler.hpp"
#include <vector>

SubServer::SubServer(const ServerConfig &config) {

    _serverName = config.server_name;
    _root = config.root;
    _errorPages = config.error_page;
    _clientMaxBodySize = config.client_max_body_size;
    _allowedMethods = config.allowed_methods;
    _index = config.index;
    _uploadPath = config.upload_path;
    _redirect = config.redirect;


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

const stringVector_t& SubServer::getIndex() const {
    return _index;
}

const vector<string>& SubServer::getErrorPages() const {
    return _errorPages;
}

const string& SubServer::getClientMaxBodySize() const {
    return _clientMaxBodySize;
}

const locationVector_t& SubServer::getLocation() const {
    return _locations;
}



void SubServer::fillLocation(const  LocationConfig& locationConfig, location_t& location) {

    location.root = locationConfig.root;
    location.index = locationConfig.index;
    location.clientMaxBodySize = locationConfig.client_max_body_size;
    location.errorPages = locationConfig.error_page;
    location.prefix = locationConfig.prefix;
    location.autoIndex = locationConfig.autoindex;
    location.allowedMethods = locationConfig.allowed_methods; // todo: wait for ismail to add it.
    location.cgi_path = locationConfig.cgi_path;
    location.uploadPath = locationConfig.upload_path;
    location.redirect = locationConfig.redirect;
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
    std::cout << "\t- index: " << _index[0] << std::endl; // todo: print all index
	for (size_t i = 1; i < _errorPages.size(); i++)
    	std::cout << "\t- _errorPages: " << _errorPages[i] << std::endl;
    std::cout << "\t- _clientMaxBodySize: " << _clientMaxBodySize << std::endl;

    std::cout << "\t- Allowed methods: ";
    stringVectorConstIterator_t ittt = _allowedMethods.begin();
    stringVectorConstIterator_t ittte = _allowedMethods.end();
    for (; ittt != ittte; ++ittt) {
        std::cout << *ittt << ", ";
    }

    std::cout << "\t- location: " << std::endl;
    locationVectorConstIterator_t it = _locations.begin();
    locationVectorConstIterator_t ite = _locations.end();
    while (it != ite) {
        std::cout << "\t\tprefix: " << it->prefix << std::endl;
        std::cout << "\t\troot: " << it->root << std::endl;
        std::cout << "\t\tindex: " << it->index[0] << std::endl; // todo: print all index
        std::cout << "\t\t_clientMaxBodySize: " << it->clientMaxBodySize << std::endl;
//        std::cout << "\t\t_errorPages: " << it->errorPages << std::endl;
        std::cout << "\t\tauto_index: " << it->autoIndex << std::endl;
        std::cout << "\t\tupload_path: " << it->uploadPath << std::endl;
        std::cout << "\t\tallowed_methods: ";
        stringVectorConstIterator_t itttt = it->allowedMethods.begin();
        stringVectorConstIterator_t itttte = it->allowedMethods.end();
        for (; itttt != itttte; ++itttt) {
            std::cout << *itttt << ", ";
        }
        std::cout << std::endl;
        ++it;
    }
    std::cout << "\t--- End Sub server ---" << std::endl;
    std::cout << std::endl;
}

const stringVector_t &SubServer::getAllowedMethods() const {
    return _allowedMethods;
}

const string &SubServer::getUploadPath() const {
    return _uploadPath;
}

const stringVector_t &SubServer::getRedirect() const {
    return _redirect;
}

