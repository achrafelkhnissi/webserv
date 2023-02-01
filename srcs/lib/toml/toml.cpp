#include "data/table.hpp"
#include <fstream>
#include <iostream>
#include "toml.hpp"

using namespace toml;

table *toml::parse_file(std::istream &is)
{
	table *t = new table();

	return t;
}
