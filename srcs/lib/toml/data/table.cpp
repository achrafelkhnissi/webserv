#include "table.hpp"
#include "utils.h"
#include <iostream>
#include <string>
#include <utility>

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

void table::push(table& t) {
	vec.push_back(t);
}

void table::insert(std::string s, table* t) {
	mp.insert(std::make_pair(s, t));
}

table* table::get(std::string& s) {
	TomlMap::iterator f = mp.find(s);
	if (f == mp.end())
		return NULL;
	return f->second;
}

void table::create(std::string& s) {
	if (get(s) != NULL)
		return;
	mp.insert(std::make_pair(s, new table(TABLE)));
}

void table::emplace(std::string& s, table* t) {
	mp.emplace(s, t); // TODO: C++11
}

bool table::is_type(enum e_toml type) {
	return this->type == type;
}

void table::set_type(enum e_toml type) {
	this->type = type;
}

void table::set_string(std::string& str) {
	this->str = str;
}

void table::print(int indent) {
	std::string s(indent, ' ');
	// std::cout << s << "type: " << type << std::endl;

	switch (type) {
	case STRING:
		std::cout << str << std::endl;
		break;
	case TABLE:
		std::cout << "{" << std::endl;
		ITER_FOREACH(TomlMap, mp, it) {
			std::cout << s << "  " << it->first << ": ";
			it->second->print(indent + 2);
		}
		std::cout << s << "}" << std::endl;
		break;
	case ARRAY:
		std::cout << "[" << std::endl;
		ITER_FOREACH(std::vector<table>, vec, it) {
			it->print(indent + 2);
		}
		std::cout << s << "]" << std::endl;
		break;
	}
}

table::~table() { }
