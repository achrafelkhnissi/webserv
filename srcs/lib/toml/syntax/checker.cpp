
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

/* bool syntax_checker(TokenList& tokens) {
	TokenList::iterator it = tokens.begin();

	while (it != tokens.end()) {
		switch ((*it)->type) {
		case Token::OPENBRACKET:
			bool is_array = (*it)->is(Token::OPENBRACKET);
			it++;
			if (is_array)
				it++;
			if (!(is_key(it, tokens.end()) && (*it)->is(Token::CLOSEBRACKET)))
				return false;
			if (is_array)
				it++;
			break;
		case Token::KEY:
			if ((*it)->type != Token::CLOSEBRACKET)
				it++;

			break;

		default:
			return false;
			break;
		}
		++it;
	}
	return true;
} */
