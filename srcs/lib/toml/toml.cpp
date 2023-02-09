#include "toml.hpp"
#include "data/table.hpp"
#include "parser/Parser.hpp"
#include "tokenizer/Lexer.hpp"
#include "tokenizer/token/Token.hpp"
#include "utils.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <istream>
#include <list>
#include <string>
#include <vector>

using namespace toml;

table* list2map(TokenList list, table& t) {
	table* last_t = &t;

	ITER_FOREACH(TokenList, list, it) {
		Token* node = *it;
		t.create(node->value);
		last_t = last_t->get(node->value);
	}
	return last_t;
}

void fill_map(TokenMap& mp, table& t) {
	ITER_FOREACH(TokenMap, mp, it) {
		table* last = list2map(it->first, t);
		last->set_type(table::STRING);
		last->set_string(it->second);
	}
}

table* build(Parser& p) {
	table* t = new table(table::TABLE);

	fill_map(p.mp, *t);

	ITER_FOREACH(std::vector<TomlBlock>, p.array, it) {
		table tmp = table(table::TABLE);
		table* last = list2map(it->prefix, *t);
		last->set_type(table::ARRAY);
		fill_map(it->mp, tmp);
		last->push(tmp);
	}

	ITER_FOREACH(std::vector<TomlBlock>, p.tables, it) {
		table* last = list2map(it->prefix, *t);
		last->set_type(table::TABLE);
		fill_map(it->mp, *last);
	}

	return t;
}

table* toml::parse_stream(std::ifstream& in) {
	Lexer lexer = Lexer(in);
	std::list<Token*> tks;
	Result<Token*, ParseError> r;

	do {
		r = lexer.next();
		if (r.is_ok())
			tks.push_back(r.ok());
		else {
			// free(list)
			cerr << r.err().as_str() << endl;
			break;
		}
	} while (!r.ok()->is(Token::_EOF));

	Parser p = Parser(tks);

	return build(p);
}

table* toml::parse_file(std::string& filename) {
	std::ifstream ifs(filename);

	if (!ifs.is_open())
		return NULL;
	return parse_stream(ifs);
}
