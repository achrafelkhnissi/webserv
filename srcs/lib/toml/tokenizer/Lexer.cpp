#include "Lexer.hpp"
#include "token/Token.hpp"
#include <cstring>
#include <iostream>
#include <istream>
#include <string.h>
#include <string>

Lexer::Lexer(std::istream& in) {
	last_token = NULL;
	this->in = &in;
	nc = 0;
	next_line();
}

void Lexer::next_line() {
	nc++;
	std::getline(*in, line);
	line.push_back('\n');
	cursor = 0;
}

char Lexer::next_char() {
	if (current() == '\n') {
		next_line();
		return current();
	}
	return line[++cursor];
}

char Lexer::current() {
	return line[cursor];
}

ParseResult Lexer::_next(Token* t) {
	switch (current()) {
	case '.':
		make_token(t, "", Token::DOT);
		break;
	case '[':
		make_token(t, "", Token::OPENBRACKET);
		break;
	case ']':
		make_token(t, "", Token::CLOSEBRACKET);
		break;
	case '=':
		make_token(t, "", Token::ASSIGN);
		break;
	case '\n':
		make_token(t, "", Token::NEWLINE);
		break;
	case '"':
	case '\'': {
		int b = cursor;
		int size = scan_qoute();
		if (size == -1)
			return ParseResult(ParseError("unclose quote", nc));
		make_token(t, line.substr(b, size), Token::QOUTED);
		break;
	}
	case '#': {
		int size = 0;
		int b = cursor;
		while (next_char() != '\n')
			size++;
		make_token(t, line.substr(b, size), Token::COMMENT);
		return ParseResult(t);
		break;
	}
	default:
		if (isalnum(current()) || strchr("_-", current())) {
			make_token(t, line.substr(cursor, scan_word()), Token::KEY);
			cursor--;
		} else
			return ParseResult(ParseError("unexpected token `" + line + "`", nc));
		break;
	}
	next_char();
	return ParseResult(t);
}

ParseResult Lexer::next() {
	Token* t = new Token();

	while (current() == ' ')
		next_char();
	if (last_token && last_token->type == Token::ASSIGN && current() != '"') {
		int b = cursor;
		int size = 1;
		while (next_char() != '\n')
			size++;
		make_token(t, line.substr(b, size), Token::VALUE);
		last_token = t;
		return ParseResult(t);
	}

	if (in->eof())
		return (make_token(t, "", Token::_EOF), ParseResult(t));
	else if (in->fail()) {
		delete t;
		return ParseResult(ParseError("broken input", nc));
	}

	ParseResult res = _next(t);
	if (res.is_ok())
		last_token = res.ok();
	else {
		delete t;
		last_token = NULL;
	}
	return res;
}

void Lexer::make_token(Token* t, std::string value, Token::e_token type) {
	t->value = value;
	t->type = type;
	t->line = nc;
	t->offset = cursor;
}

int Lexer::scan_qoute() {
	char q = current();
	int i = 1;

	while (next_char() != q) {
		if (current() == '\0')
			return -1;
		i++;
	}
	return i + 1;
}

int Lexer::scan_word() {
	int i = 0;

	while (isalnum(current()) || strchr("_-", current())) {
		if (next_char() == '\0')
			return i;
		i++;
	}
	return i;
}
