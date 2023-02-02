#include "toml.hpp"
#include "data/table.hpp"
#include <fstream>
#include <iostream>
#include <istream>
#include <string>

using namespace toml;

table* toml::parse_stream(std::ifstream& is) {
	(void)is;
	table* t = new table();

	return t;
}

table* toml::parse_file(std::string& filename) {
	std::ifstream ifs(filename);

	if (ifs.is_open())
		return NULL;
	return parse_stream(ifs);
}
