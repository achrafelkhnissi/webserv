#pragma once
#include "../error/ParseError.hpp"
#include "../result/Result.hpp"
#include "token/Token.hpp"
#include <iostream>
#include <list>
#include <stddef.h>
#include <string>

typedef Result<Token*, ParseError> ParseResult;

class Lexer {

public:
	Lexer(std::istream&);
	Result<std::list<Token*>, ParseError> into();

private:
	std::istream* in;
	std::string line;
	size_t cursor;
	size_t nc;
	Token* last_token;

	// utils

	ParseResult _next(Token* t);
	ParseResult next_value();
	ParseResult next();

	// cursor
	char next_char();
	char current();
	void next_line();
	void make_token(Token* t, std::string value, Token::e_token type);
	void into(std::list<Token*>& list);

	// scanners
	int scan_qoute();
	int scan_word();
};
