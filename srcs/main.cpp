#include "lib/toml/data/table.hpp"
#include "lib/toml/toml.hpp"

#include <iostream>
#include <string>

int main() {

    //std::string config_file = "default.conf";

	toml::table *t = toml::parse_file(std::cin);
	
	std::cout << t << std::endl;

	delete t;
    return 0;
}
