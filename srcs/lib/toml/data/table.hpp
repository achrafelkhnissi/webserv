#pragma once

#include <map>
#include <string>
#include <vector>

typedef std::map<std::string, void*> TomlMap ;

namespace toml {
class table {

public:
	enum e_toml {
		TABLE,
		ARRAY,
		STRING,
	};

	table(enum e_toml type);
	table(TomlMap &mp);
	table(std::string &str);
	void *operator[](std::string &);
	void push(table);
	void insert(std::string s, void *t);
	~table();

//private:
	enum e_toml type;
	std::map<std::string, void*> mp;
	std::vector<table> vec;
	std::string str;
};
}; // namespace toml
