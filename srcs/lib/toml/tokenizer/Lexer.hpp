#pragma once
#include "../error/ParseError.hpp"
#include "../result/Result.hpp"
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
	Result<std::list<Token*>, ParseError> into();

private:
	std::istream* in;
	std::string line;
	size_t cursor;
	size_t nc;
	Token::e_token last_token;

	// utils

	void skip_space();
	ParseResult _next(Token* t);
	Result<std::list<Token*>, ParseError> next_value();
	ParseResult next();

	// cursor
	char next_char();
	char current();
	bool getnextline(std::string& line);
	void make_token(Token* t, std::string value, Token::e_token type);
	Token::e_token expect();

	// new
	TokenListResult parse();
	// tokenizer
	bool tokenize(std::string& line, Token& tk, size_t& pos);
	// validation
	//bool expected(Token::e_token type);
};
