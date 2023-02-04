#ifndef PARSER_HPP
# define PARSER_HPP

#include <map>
#include <list>
#include <vector>
#include "../tokenizer/token/Token.hpp"

typedef std::list<Token*> TokenList;
typedef std::map<TokenList, TokenList> TokenMap ;
typedef std::pair<TokenList, TokenList> TokenPair ;

struct TomlBlock {

	TomlBlock(TokenList prefix, TokenMap mp)
	{
		this->mp = mp;
		this->prefix = prefix;
	}

	TokenList prefix;
	TokenMap mp;
};

class Parser {
private:
	TokenMap mp;
	std::vector<TomlBlock> array;
	std::vector<TomlBlock> tables;
public:
	Parser(TokenList);
	void print();
};
#endif
