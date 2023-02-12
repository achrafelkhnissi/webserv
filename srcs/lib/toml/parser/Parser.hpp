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

	enum blockType { ARRAY, TABLE };

	TomlBlock(TokenList prefix, TokenMap mp, enum blockType type) {
		this->mp = mp;
		this->prefix = prefix;
		this->type = type;
	}

	TokenList prefix;
	TokenMap mp;
	blockType type;
};

class Parser {
public:
	TokenMap mp;
	std::vector<TomlBlock> tables;
	Parser(TokenList);
	void print();
};
#endif
