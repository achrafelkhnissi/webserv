#include "lib/toml/data/table.hpp"
#include "lib/toml/toml.hpp"

#include <iostream>
#include <string>

int main(int ac, char* av[]) {

	std::string config_file = "default.toml";

	if (ac == 2) {
		config_file = av[1];
	}

	toml::table* t = toml::parse_file(config_file);
	if (t == NULL) {
		std::cerr << "Error parsing file" << std::endl;
		return 1;
	}

	(*t)["server"][1].print();
	//t->print();

	delete t;
	return 0;
}
