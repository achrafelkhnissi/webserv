//
// Created by Achraf El khnissi on 2/18/23.
//

#include "SubServer.h"

SubServer::SubServer() {

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

const location& SubServer::getLocation() const {
    return _location;
}
