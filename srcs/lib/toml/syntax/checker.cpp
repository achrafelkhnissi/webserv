#include "Parser.hpp"

bool is_key(TokenList::iterator& it, TokenList::iterator end) {
	while (it != end) {
		switch (it->type) {
		case Token::KEY:
			it++;
			if (it->type == Token::DOT)
				it++;
			break;
		case Token::CLOSEBRACKET:
		case Token::ASSIGN:
			return true;
			break;
		default:
			return false;
			break;
		}
	}
	return true;
}

bool is_array(TokenList::iterator& it, TokenList::iterator end) {
	if (!it->is(Token::OPENBRACKET))
		return false;
	it++;
	while (it != end) {
		switch (it->type) {
		case Token::VALUE:
		case Token::QOUTED:
		case Token::KEY:
			it++;
			if (it->type == Token::COMMA)
				it++;
			break;
		case Token::CLOSEBRACKET:
			return it != end && it->is(Token::CLOSEBRACKET);
			break;
		default:
			return false;
			break;
		}
	}
	abort(); // should never reach here
	return false;
}

ChekerResult syntax_checker(TokenList& tokens) {
	TokenList::iterator it = tokens.begin();
	TokenList::iterator end = tokens.end();

	while (it != end) {
		switch (it->type) {
		case Token::OPENBRACKET: {
			it++; // skip OPENBRACKET
			bool is_array = it->is(Token::OPENBRACKET);
			if (is_array)
				it++;
			if (!(is_key(it, end) && it->is(Token::CLOSEBRACKET)))
				return ChekerResult(ParseError("Invalid key [1000] ", it->line));
			;
			if (is_array)
				it++;
			it++;
			break;
		}
		case Token::KEY: {
			if (!(is_key(it, end) && it->is(Token::ASSIGN)))
				return ChekerResult(ParseError("Invalid key [1001] ", it->line));
			it++; // skip ASSIGN
			if (!it->is(Token::VALUE) && !it->is(Token::QOUTED) && !is_array(it, end))
				return ChekerResult(ParseError("Invalid key [1002] ", it->line));
			it++; // skip VALUE QOUTED or CLOSEBRACKET
			break;
		}
		case Token::NEWLINE:
		case Token::COMMENT:
			break;
		case Token::_EOF:
			return ChekerResult(NULL);
			break;
		default:
			return ChekerResult(ParseError("Invalid key [1003] ", it->line));
			break;
		}
		if (it->is(Token::COMMENT))
			it++;
		if (!it->is(Token::NEWLINE) && !it->is(Token::_EOF))
			return ChekerResult(ParseError("Invalid key [1004] ", it->line));
		++it;
	}
	return ChekerResult(NULL);
}
