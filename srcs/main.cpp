#include "Configuration.hpp"
#include "table.hpp"
#include "toml.hpp"
#include "Request.hpp"
#include "HttpParser.hpp"
#include <iostream>
#include "Server.hpp"

#include <string>

using namespace std;

int main(int ac, char** av) {

    std::string config_file = "conf/default.toml";

    if (ac == 2) {
        config_file = av[1];
    }

    toml::table* t = toml::parse_file(config_file);
	if (t == nullptr) {
		std::cerr << "Error parsing file" << std::endl;
		return 1;
	}
	Configuration config(*t);

	Server server(config);

	server.start();

    delete t;
    return 0;
}

