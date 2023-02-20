#ifndef PARSER_HPP
#define PARSER_HPP

#include "../data/table.hpp"
#include "../error/ParseError.hpp"
#include "../result/Result.hpp"
#include "../tokenizer/token/Token.hpp"
#include <list>
#include <map>
#include <string>
#include <vector>

// clang-format off
typedef std::list<Token> TokenList;
typedef std::vector<std::pair<TokenList, std::string> > TokenMap;
typedef std::pair<TokenList, std::string> TokenPair;
// clang-format on

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

TokenList til_ignore(TokenList::iterator& cur, Token::e_token until, Token::e_token ignore);

TokenList::iterator til(TokenList::iterator& cur, Token::e_token until);
typedef Result<toml::table*, ParseError> ChekerResult;
ChekerResult syntax_checker(TokenList& tokens);
#endif
