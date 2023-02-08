#include "table.hpp"
#include <string>
#include <utility>
#include <iostream>
#include "utils.h"

using namespace toml;

table::table(enum e_toml type) {
	this->type = type;
}

table::table(TomlMap& mp) {
	this->mp = mp;
	this->type = TABLE;
}

table::table(std::string& str)
	: type(STRING)
	, str(str) { }

void* table::operator[](std::string& idx) {
	TomlMap::iterator f = mp.find(idx);
	if (f == mp.end())
		return NULL;
	return f->second;
}

void table::push(table t) {
	vec.push_back(t);
}

void table::insert(std::string s, table* t) {
	mp.insert(std::make_pair(s, t));
}

void table::print(int indent) {
	std::string s(indent, ' ');
	// std::cout << s << "type: " << type << std::endl;

	switch (type) {
		case STRING:
			std::cout << s << "  " << str << std::endl;
			break;
		case TABLE:
			std::cout << s << "{" << std::endl;
			ITER_FOREACH(TomlMap, mp, it) {
				std::cout << s << "  " << it->first << ": ";
				it->second->print(indent + 2);
			}
			std::cout << s << "}" << std::endl;
			break;
		case ARRAY:
			std::cout << s << "[" << std::endl;
			ITER_FOREACH(std::vector<table>, vec, it) {
				it->print(indent + 2);
			}
			std::cout << s << "]" << std::endl;
			break;
	}
}

table::~table() { }
