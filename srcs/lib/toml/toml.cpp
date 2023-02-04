#include "toml.hpp"
#include "data/table.hpp"
#include "parser/Parser.hpp"
#include "tokenizer/Lexer.hpp"
#include "tokenizer/token/Token.hpp"
#include <fstream>
#include <iostream>
#include <istream>
#include <string>
#include <list>

using namespace toml;

table *list2map(TokenList list, table &t) {
	table *last_t = &t;

	for (TokenList::iterator it = list.begin(); it != list.end(); it++) {
		Token *node = *it;
		if (node->type == Token::DOT)
			continue;
		t.mp.emplace(node->value, new table(table::TABLE));
		last_t = (table *) last_t->mp.at(node->value);
	}
	return last_t;
}

table* build(Parser &p) {
	table *t = new table(table::TABLE);
	for (TokenMap::iterator it = p.mp.begin(); it != p.mp.end(); it++) {
		table *last = list2map(it->first, *t);
		last->type = table::STRING;
		last->str = it->second;
	}
	return t;
}

table* toml::parse_stream(std::ifstream& in) {
	Lexer lexer = Lexer(in);
	std::list<Token *> tks;
	Result<Token*, ParseError> r;
	
	do {
		r = lexer.next();
		if (r.is_ok())
			tks.push_back(r.ok());
		else
		{
			// free(list)
			cerr << r.err().as_str() << endl;
			break;
		}
	}
	while (!r.ok()->is(Token::_EOF));

	Parser p = Parser(tks);
	
	return build(p);
}

table* toml::parse_file(std::string& filename) {
	std::ifstream ifs(filename);

	if (!ifs.is_open())
		return NULL;
	return parse_stream(ifs);
}
