
#include "Lexer.hpp"
#include "token/Token.hpp"
#include <iostream>
#include <istream>
#include <string>

Lexer::Lexer(std::istream &in) {
	this->in = &in;
	nc = 0;
	next_line();
}

void Lexer::next_line()
{
	nc++;
	std::getline(*in, line);
	cursor = 0;
}

char Lexer::next_char()
{
	if (current() == '\0')
	{
		next_line();
		return current();
	}
	return line[++cursor];
}

char Lexer::current()
{
	return line[cursor];
}

Result<Token *, ParseError> Lexer::next() {
	Token *t = new Token();
	return Result<Token *, ParseError>(t);
}
