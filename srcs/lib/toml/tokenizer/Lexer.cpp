#include "Lexer.hpp"
#include "Token.hpp"
#include <_ctype.h>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <istream>
#include <list>
#include <string.h>
#include <string>

Lexer::Lexer(std::istream& in) {
	last_token = Token::UNINTILIASED;
	this->in = &in;
	nc = 0;
	pos = 0;
}

bool Lexer::skip_space(std::string& line) {
	if (pos >= line.size())
		return false;
	while (line[pos] == ' ' || line[pos] == '\t')
		pos++;
	if (pos >= line.size())
		return false;
	return true;
}

bool Lexer::tokenize_value(std::string& line, Token& tk) {
	if (!skip_space(line))
		return false;
	switch (line[pos]) {
	case '[':
		make_token(&tk, "[", Token::OPENBRACKET);
		pos++;
		break;
	case ',':
		make_token(&tk, ",", Token::COMMA);
		pos++;
		break;
	case ']':
		make_token(&tk, "]", Token::CLOSEBRACKET);
		pos++;
		break;
	case '"':
	case '\'': {
		size_t size = line.find(line[pos], pos + 1);
		if (size == std::string::npos)
			return false;
		make_token(&tk, line.substr(pos + 1, size - pos - 1), Token::QOUTED);
		pos = size + 1;
		break;
	}
	default: {
		if (isalnum(line[pos]) || strchr("_-", line[pos])) {
			size_t size = line.find_first_not_of(
				"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_", pos);
			if (size == std::string::npos)
				return false;
			make_token(&tk, line.substr(pos, size - pos), Token::VALUE);
			pos = size;
		} else
			return false;
		break;
	}
	}
	return true;
}

bool Lexer::tokenize(std::string& line, Token& tk) {
	if (!skip_space(line))
		return false;
	switch (line[pos]) {
	case '.':
		make_token(&tk, ".", Token::DOT);
		pos++;
		break;
	case '[':
		make_token(&tk, "[", Token::OPENBRACKET);
		pos++;
		break;
	case ']':
		make_token(&tk, "]", Token::CLOSEBRACKET);
		pos++;
		break;
	case '=':
		make_token(&tk, "=", Token::ASSIGN);
		pos++;
		break;
	case '\n':
		make_token(&tk, "\n", Token::NEWLINE);
		pos++;
	case '"':
	case '\'': {
		size_t size = line.find(line[pos], pos + 1);
		if (size == std::string::npos)
			return false;
		make_token(&tk, line.substr(pos + 1, size - pos - 1), Token::QOUTED);
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
			make_token(&tk, line.substr(pos, size - pos), Token::KEY);
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
	case Token::KEY:
		return Token::ASSIGN | Token::DOT | Token::CLOSEBRACKET;
	case Token::ASSIGN:
		return Token::QOUTED | Token::KEY | Token::OPENBRACKET;
	case Token::QOUTED:
		return Token::NEWLINE | Token::COMMA;
	case Token::DOT:
		return Token::KEY;
	case Token::OPENBRACKET:
		return Token::KEY | Token::QOUTED | Token::OPENBRACKET;
	case Token::CLOSEBRACKET:
		return Token::NEWLINE | Token::CLOSEBRACKET | Token::COMMENT;
	case Token::COMMENT:
		return Token::NEWLINE;
	case Token::VALUE:
		return Token::NEWLINE | Token::COMMA;
	case Token::COMMA:
		return Token::VALUE | Token::QOUTED;
	case Token::_EOF:
	case Token::NEWLINE:
		abort();
	}
}

bool Lexer::getnextline(std::string& line) {
	if (in->eof())
		return false;
	std::getline(*in, line);
	line.push_back('\n');
	nc++;
	pos = 0;
	last_token = Token::UNINTILIASED;
	return !in->fail();
}

Token::e_token Lexer::expect_value() {
	switch (last_token) {
	case Token::QOUTED:
		return Token::NEWLINE | Token::COMMA | Token::COMMENT | Token::CLOSEBRACKET;
	case Token::OPENBRACKET:
		return Token::KEY | Token::QOUTED;
	case Token::CLOSEBRACKET:
		return Token::NEWLINE | Token::COMMENT;
	case Token::COMMENT:
		return Token::NEWLINE;
	case Token::VALUE:
		return Token::NEWLINE | Token::COMMA | Token::COMMENT;
	case Token::COMMA:
		return Token::VALUE | Token::QOUTED;
	case Token::ASSIGN:
		return Token::OPENBRACKET | Token::QOUTED | Token::VALUE;
	case Token::_EOF:
		abort();
	default:
		return (Token::e_token)0;
	}
}

TokenListResult Lexer::parse_value(std::string& line) {
	Token t;
	std::list<Token> list;
	while (tokenize_value(line, t)) {
		if (!(t.type & expect_value()))
			return ParseError("unexpected token `" + t.as_str() + "`", nc);
		last_token = t.type;
		list.push_back(t);
	}
	return TokenListResult(list);
}

TokenListResult Lexer::parse() {
	std::string line;
	Token t;
	std::list<Token> list;
	while (getnextline(line)) {
		while (tokenize(line, t)) {
			if (!(t.type & expect())) {
				return ParseError("unexpected token `" + t.as_str() + "`", nc);
			}
			last_token = t.type;
			list.push_back(t);
			if (last_token == Token::ASSIGN) {
				// = [value]
				TokenListResult res = parse_value(line);
				if (!res.is_ok())
					return res;
				list.splice(list.end(), res.ok());
				break;
			}
		}
		make_token(&t, "\n", Token::NEWLINE);
		list.push_back(t);
	}
	return TokenListResult(list);
}

void Lexer::make_token(Token* t, std::string value, Token::e_token type) {
	t->value = value;
	t->type = type;
	t->line = nc;
	t->offset = pos;
}
