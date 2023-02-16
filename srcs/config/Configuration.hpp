#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <fstream>
#include <iostream>
#include <map>
#include <string>

class Configuration {

private:
    int             _port;
    std::string     _host;
    std::string     _root;
    std::string     _log_path;

    void parse_config_file(const std::string& config_file);

public:
    Configuration(const std::string& config_file);

    int get_port();

    std::string get_host();
    std::string get_root();
    std::string get_log_path();
};

#endif // CONFIGURATION_HPP