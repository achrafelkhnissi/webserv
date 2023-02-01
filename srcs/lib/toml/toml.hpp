#pragma once
#include "data/table.hpp"
using namespace std;

namespace toml {
	table *parse_file(istream &);
}
