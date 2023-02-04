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
	
	return new toml::table();
}

table* toml::parse_file(std::string& filename) {
	std::ifstream ifs(filename);

	if (!ifs.is_open())
		return NULL;
	return parse_stream(ifs);
}
