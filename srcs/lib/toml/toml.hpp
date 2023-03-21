#pragma once
#include "table.hpp"
using namespace std;

namespace toml {
table* parse_stream(ifstream&);
table* parse_file(std::string&);
} // namespace toml
