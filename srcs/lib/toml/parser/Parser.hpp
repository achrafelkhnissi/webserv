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
struct TokenPair;
typedef std::vector<TokenPair> TokenMap;
// clang-format on

struct TokenPair {
	TokenList key;
	TokenList value;
	bool is_array;

	TokenPair(TokenList key, TokenList value, bool is_array = false) {
		this->key = key;
		this->value = value;
		this->is_array = is_array;
	}
};

struct TomlBlock {

	enum blockType { ARRAY, TABLE, ARRAY_VALUE };

	TomlBlock(TokenList prefix, TokenMap mp, enum blockType type) {
		this->mp = mp;
		this->prefix = prefix;
		this->type = type;
	}

	TokenList prefix;
	TokenMap mp;
	blockType type;
	std::vector<std::string> values;
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
