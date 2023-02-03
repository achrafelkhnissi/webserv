#pragma once
#include "../error/ParseError.hpp"
#include "../result/Result.hpp"
#include "token/Token.hpp"
#include <iostream>
#include <string>

class Lexer {

public:
	Lexer(std::istream&);
	Result<Token *, ParseError> next();

private:
	std::istream *in;
	std::string line;
	int cursor;
	int nc;

	// cursor
	char next_char();
	char current();
	void next_line();
	void make_token(Token *t, std::string value, Token::e_token type);

	// 

	// scanners
	int scan_qoute();
};
