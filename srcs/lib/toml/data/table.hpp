#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace toml {

class table;
typedef std::map<std::string, table*> TomlMap;

class table {

public:
	enum e_toml {
		TABLE,
		ARRAY,
		STRING,
		NONE,
	};
	static table* empty_;

	table(enum e_toml type);
	table(TomlMap& mp);
	table(std::string& str);
	void push(table&);
	void insert(std::string s, table* t);
	void create(std::string& s);
	void print(int indent = 0);

	bool is_type(enum e_toml type);
	void set_type(enum e_toml type);

	void set_string(std::string& str);

	table& operator[](std::string idx);
	table& operator[](size_t idx);
	table& get(std::string s);
	table& get(size_t idx);

	table& last();

	std::string& as_str();
	std::string& as_str(std::string& s);

	~table();

	//private:
	enum e_toml type;
	TomlMap mp;
	std::vector<table> vec;
	std::string str;
};
}; // namespace toml
