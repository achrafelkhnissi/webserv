#include "Lexer.hpp"
#include "token/Token.hpp"
#include <_ctype.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <istream>
#include <list>
#include <string.h>
#include <string>

Lexer::Lexer(std::istream& in) {
	last_token = NULL;
	this->in = &in;
	nc = 0;
}

void Lexer::skip_space() {
	while (current() == ' ' || current() == '\t') {
		next_char();
	}
}

bool Lexer::tokenize(std::string& line, Token& tk, size_t& pos) {
	if (pos >= line.size())
		return false;
	while (line[pos] == ' ' || line[pos] == '\t')
		pos++;
	switch (line[pos]) {
	case '.':
		make_token(&tk, ".", Token::DOT);
		break;
	case '[':
		make_token(&tk, "[", Token::OPENBRACKET);
		break;
	case ']':
		make_token(&tk, "]", Token::CLOSEBRACKET);
		break;
	case '=':
		make_token(&tk, "=", Token::ASSIGN);
		break;
	case '\n':
		make_token(&tk, "\n", Token::NEWLINE);
	case '"':
	case '\'': {
		size_t size = line.find(line[pos], pos + 1);
		if (size == std::string::npos)
			return false;
		make_token(&tk, line.substr(pos + 1, size), Token::QOUTED);
		pos = size + 1;
		break;
	}
	case '#': {
		size_t size = line.find('\n', pos);
		if (size == std::string::npos)
			return false;
		make_token(&tk, line.substr(pos + 1, size), Token::COMMENT);
		pos = size + 1;
		break;
	}
	default:
		if (isalnum(line[pos]) || strchr("_-", line[pos])) {
			size_t size = line.find_first_not_of(
				"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_", pos);
			if (size == std::string::npos)
				return false;
			make_token(&tk, line.substr(pos, size), Token::KEY);
			pos = size;
		} else
			return false;
		break;
	}
	return true;
}

Token::e_token Lexer::expect() {
	switch (last_token) {
	case Token::UNINTILIASED:
		return Token::KEY | Token::COMMENT | Token::NEWLINE | Token::OPENBRACKET;
	case Token::NEWLINE:
		return Token::KEY;
	case Token::KEY:
		return Token::ASSIGN | Token::DOT;
	case Token::ASSIGN:
		return Token::QOUTED | Token::KEY | Token::OPENBRACKET;
	case Token::QOUTED:
		return Token::NEWLINE | Token::COMMA;
	case Token::DOT:
		return Token::KEY;
	case Token::OPENBRACKET:
		return Token::KEY | Token::QOUTED | Token::OPENBRACKET;
	case Token::CLOSEBRACKET:
		return Token::NEWLINE | Token::CLOSEBRACKET;
	case Token::COMMENT:
		return Token::NEWLINE;
	case Token::VALUE:
		return Token::NEWLINE | Token::COMMA;
	case Token::COMMA:
		return Token::VALUE | Token::QOUTED;
	case Token::_EOF:
		abort();
	}
}

bool Lexer::getnextline(std::string& line) {
	if (in->eof())
		return false;
	std::getline(*in, line);
	line.push_back('\n');
	return !in->fail();
}

TokenListResult Lexer::parse() {
	std::string line;
	Token t;
	size_t pos;
	std::list<Token> list;
	while (getnextline(line)) {
		pos = 0;
		while (tokenize(line, t, pos)) {
			if (last_token->type == Token::ASSIGN) {
				break;
			}
			if (!(t.type & expect()))
				return ParseError("unexpected token `" + t.value + "`", nc);
			last_token = t.type;
			list.push_back(t);
		}
	}
	return TokenListResult(list);
}

void Lexer::make_token(Token* t, std::string value, Token::e_token type) {
	t->value = value;
	t->type = type;
	t->line = nc;
	t->offset = cursor;
}
