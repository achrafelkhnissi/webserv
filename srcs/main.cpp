#include "server/Server.hpp"
#include "config/Configuration.hpp"
#include "lib/toml/data/table.hpp"
#include "lib/toml/toml.hpp"

#include <iostream>
#include <string>

int main(int ac, char** av) {

    //std::string config_file = "default.conf";

	toml::table *t = toml::parse_file(std::cin);
	
	std::cout << t << std::endl;

	delete t;
    return 0;
}
