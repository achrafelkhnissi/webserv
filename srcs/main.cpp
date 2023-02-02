#include "config/Configuration.hpp"
#include "server/Server.hpp"

#include <iostream>
#include <string>

int main(int ac, char** av) {

    std::string config_file = "default.conf";

    if (ac == 2) {
        config_file = av[1];
    }

    try {

        Configuration config(config_file);

        // Initialise the server.
        Server server(config.get_port(), config.get_host());
        server.start();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
