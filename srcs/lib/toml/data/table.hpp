#pragma once

#include <string>

namespace toml {
	class table;
};

class toml::table {
	std::string value;

private:

public:
	table();
	~table();
};
