#include "table.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

using namespace toml;

table* table::empty_ = new table(NONE);

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

void table::push(table& t) {
	vec.push_back(t);
}

void table::insert(std::string s, table* t) {
	if (t->is_type(ARRAY))
		get(vec.size() - 1).insert(s, t);
	else
		mp.insert(std::make_pair(s, t));
}

void table::create(std::string& s) {
	table& t = *this;
	if (t.is_type(ARRAY)) {
		t = t.get(t.vec.size() - 1);
	}
	if (t.get(s).is_type(NONE)) {
		t.insert(s, new table(TABLE));
	}
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

table& table::last() {
	if (type != ARRAY)
		abort();
	if (vec.size() == 0)
		vec.push_back(table(TABLE));
	return this->vec.back();
}

void table::print(int indent) {
	std::string s(indent, ' ');
	// std::cout << s << "type: " << type << std::endl;

	switch (type) {
	case STRING:
		std::cout << str << std::endl;
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
	case NONE:
		std::cout << s << "NONE" << std::endl;
		break;
	}
}

table& table::operator[](std::string idx) {
	return get(idx);
}

table& table::operator[](size_t idx) {
	return get(idx);
}

const std::string& table::as_str(const std::string& s) {
	if (type != STRING)
		return s;
	return str;
}

int table::as_int(int i) {
	char* end;
	long res = strtol(str.c_str(), &end, 10);
	if (type != STRING || (*end != '\0'))
		return i;
	return res;
}

bool table::as_bool(bool b) {
	if (type != STRING)
		return b;
	if (str == "true" || str == "on" || str == "yes")
		return true;
	else if (str == "false" || str == "off" || str == "no")
		return false;
	return b;
}

//int table::as_int(int s) {
//    if (type != STRING)
//        return s;
//    return std::stoi(str);
//}

table& table::get(std::string s) {
	TomlMap::iterator f = mp.find(s);
	if (type != TABLE || f == mp.end())
		return *empty_;
	return *f->second;
}

table& table::get(size_t idx) {
	if (type != ARRAY || idx >= vec.size())
		return *empty_;
	return vec[idx];
}

table::~table() { }
