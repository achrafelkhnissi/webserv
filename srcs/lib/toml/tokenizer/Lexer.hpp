#pragma once
#include "ParseError.hpp"
#include "Result.hpp"
#include "token/Token.hpp"
#include <iostream>
#include <list>
#include <stddef.h>
#include <string>

typedef Result<Token*, ParseError> ParseResult;
typedef Result<std::list<Token>, ParseError> TokenListResult;

class Lexer {

public:
	Lexer(std::istream&);
	TokenListResult parse();

private:
	std::istream* in;
	std::string line;
	size_t pos;
	size_t nc;
	Token::e_token last_token;

	// utils

	bool skip_space(std::string& line);

	// cursor
	char next_char();
	char current();
	bool getnextline(std::string& line);
	void make_token(Token* t, std::string value, Token::e_token type);
	Token::e_token expect();
	Token::e_token expect_value();

	// new
	TokenListResult parse_value(std::string& line);
	// tokenizer
	bool tokenize(std::string& line, Token& tk);
	bool tokenize_value(std::string& line, Token& tk);
	// validation
	//bool expected(Token::e_token type);
};
