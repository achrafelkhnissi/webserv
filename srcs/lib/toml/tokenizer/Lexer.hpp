#pragma once
#include "../error/ParseError.hpp"
#include "../result/Result.hpp"
#include "token/Token.hpp"
#include <iostream>
#include <string>

typedef Result<Token*, ParseError> ParseResult;

class Lexer {

public:
	Lexer(std::istream&);
	ParseResult next();

private:
	std::istream *in;
	std::string line;
	int cursor;
	int nc;
	Token *last_token;

	// utils
	
	ParseResult _next(Token *t);

	// cursor
	char next_char();
	char current();
	void next_line();
	void make_token(Token *t, std::string value, Token::e_token type);

	// scanners
	int scan_qoute();
	int scan_word();
};
