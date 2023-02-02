
#include "Lexer.hpp"
#include "token/Token.hpp"
#include <_ctype.h>
#include <iostream>
#include <istream>
#include <string>

Lexer::Lexer(std::istream& in) {
	this->in = &in;
	nc = 0;
	next_line();
}

void Lexer::next_line() {
	nc++;
	std::getline(*in, line);
	cursor = 0;
}

char Lexer::next_char() {
	if (current() == '\0') {
		next_line();
		return current();
	}
	return line[++cursor];
}

char Lexer::current() {
	return line[cursor];
}

Result<Token*, ParseError> Lexer::next() {
	Token* t = new Token();
	char c = current();

	while (isspace(next_char()))
		;
	switch (current()) {
		case '.':
			make_token(t, ".", Token::DOT);
			break;
		case '[':
			make_token(t, "[", Token::OpenBracket);
			break;
		case ']':
			make_token(t, "]", Token::OpenBracket);
			break;
		case '=':
			make_token(t, "=", Token::OpenBracket);
			break;
		case '"':
		case '\'':
		{
			int size = scan_qoute();
			if (size == -1)
				return Result<Token*, ParseError>(ParseError("unclose quote", nc));
			make_token(t, line.substr(cursor, size), Token::COMMENT);
			break;
		}
		case '#':
		{
			int b = cursor;
			while (next_char() != '\0')
				;
			make_token(t, line.substr(b, cursor), Token::COMMENT);
			break;
		}
		case '\\':

			break;
		default:
			return Result<Token*, ParseError>(ParseError("unexpected token", nc));
			break;
	}
	return Result<Token*, ParseError>(t);
}

void Lexer::make_token(Token* t, std::string value, Token::e_token type) {
	t->value = value;
	t->type = type;
	t->line = nc;
	t->offset = cursor;
}

int Lexer::scan_qoute()
{
	char q = current();
	int i = 1;

	while (next_char() != q) {
 		if (current() == '\0')
			return -1;
		i++;
	}
	return i;
}
