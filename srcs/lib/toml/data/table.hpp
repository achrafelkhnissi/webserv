#pragma once

#include <map>
#include <string>
#include <vector>


namespace toml {

class table;
typedef std::map<std::string, table*> TomlMap ;

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
	void insert(std::string s, table *t);
	void print(int indent = 0);
	~table();

//private:
	enum e_toml type;
	TomlMap mp;
	std::vector<table> vec;
	std::string str;
};
}; // namespace toml
