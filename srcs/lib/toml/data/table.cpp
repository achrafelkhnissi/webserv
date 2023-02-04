#include "table.hpp"
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

void table::push(table t) {
	vec.push_back(t);
}

void table::insert(std::string s, void* t) {
	mp.insert(std::make_pair(s, t));
}

table::~table() { }
