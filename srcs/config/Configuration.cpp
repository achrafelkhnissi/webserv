#include "Configuration.hpp"

Configuration::Configuration(const std::string& config_file) {
    parse_config_file(config_file);

    _port = 1337;
    _host = "localhost";
}

int Configuration::get_port() {
    return _port;
}

std::string Configuration::get_host() {
    return _host;
}

std::string Configuration::get_root() {
    return _root;
}

std::string Configuration::get_log_path() {
    return _log_path;
}

void Configuration::parse_config_file(const std::string& config_file) {
    // TODO: parse config file
    (void)config_file;
}
