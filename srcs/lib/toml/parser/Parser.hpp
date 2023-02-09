#ifndef PARSER_HPP
#define PARSER_HPP

#include "../tokenizer/token/Token.hpp"
#include <list>
#include <map>
#include <string>
#include <vector>

typedef std::list<Token*> TokenList;
typedef std::map<TokenList, std::string> TokenMap;
typedef std::pair<TokenList, std::string> TokenPair;

struct TomlBlock {

	TomlBlock(TokenList prefix, TokenMap mp) {
		this->mp = mp;
		this->prefix = prefix;
	}

	TokenList prefix;
	TokenMap mp;
};

class Parser {
public:
	TokenMap mp;
	std::vector<TomlBlock> array;
	std::vector<TomlBlock> tables;
	Parser(TokenList);
	void print();
};
#endif
