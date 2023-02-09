#pragma once

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
	};

	table(enum e_toml type);
	table(TomlMap& mp);
	table(std::string& str);
	void push(table&);
	void insert(std::string s, table* t);
	void emplace(std::string& s, table* t);
	void create(std::string& s);
	void print(int indent = 0);

	bool is_type(enum e_toml type);
	void set_type(enum e_toml type);

	void set_string(std::string& str);

	table& operator[](std::string idx);
	table& operator[](int idx);
	table& get(std::string s);
	table& get(int idx);

	std::string& as_str();

	~table();

	//private:
	enum e_toml type;
	TomlMap mp;
	std::vector<table> vec;
	std::string str;
};
}; // namespace toml
