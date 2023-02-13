
#include "../parser/Parser.hpp"

bool is_key(TokenList::iterator& it, TokenList::iterator end) {
	while (it != end) {
		switch ((*it)->type) {
		case Token::KEY:
			it++;
			if ((*it)->type == Token::DOT)
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

bool syntax_checker(TokenList& tokens) {
	TokenList::iterator it = tokens.begin();
	TokenList::iterator end = tokens.end();

	while (it != end) {
		switch ((*it)->type) {
		case Token::OPENBRACKET: {
			bool is_array = (*it)->is(Token::OPENBRACKET);
			it++; // skip OPENBRACKET
			if (is_array)
				it++;
			if (!(is_key(it, end) && (*it)->is(Token::CLOSEBRACKET)))
				return false;
			if (is_array)
				it++;
			break;
		}
		case Token::KEY: {
			if (!(is_key(it, end) && (*it)->is(Token::ASSIGN)))
				return false;
			it++; // skip ASSIGN
			if (!(*it)->is(Token::VALUE) || !(*it)->is(Token::QOUTED))
				return false;
			break;
		}
		case Token::NEWLINE:
		case Token::COMMENT:
			break;
		default:
			return false;
			break;
		}
		if ((*it)->is(Token::COMMENT))
			it++;
		if (!(*it)->is(Token::NEWLINE))
			return false;
		++it;
	}
	return true;
}
