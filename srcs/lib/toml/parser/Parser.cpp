#include "Parser.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <list>
#include <numeric>
#include <string>

TokenList til_ignore(TokenList::iterator& cur, Token::e_token until, Token::e_token ignore) {
	TokenList res;
	while (!cur->is(until)) {
		if (!cur->is(ignore)) {
			res.push_back(*cur);
		}
		cur++;
	}
	return res;
}

TokenList::iterator til(TokenList::iterator& cur, Token::e_token until) {
	while (!cur->is(until)) {
		cur++;
	}
	return cur;
}

std::string accum(std::string& acc, Token& t) {
	return acc + t.value;
};

Parser::Parser(TokenList tks) {
	TokenList::iterator cur = tks.begin();
	TokenMap* lastmp = &mp;

	while (cur != tks.end()) {
		switch (cur->type) {
		case Token::KEY: {
			TokenList res = til_ignore(cur, Token::ASSIGN, Token::DOT);
			cur++; // skip assign
			bool is_array = cur->is(Token::OPENBRACKET);
			TokenList values = til_ignore(cur,
										  Token::NEWLINE | Token::COMMENT,
										  Token::COMMA | Token::CLOSEBRACKET | Token::OPENBRACKET);
			lastmp->push_back(TokenPair(res, values, is_array));
			break;
		}
		case Token::OPENBRACKET: {
			cur++;
			bool is_array = cur->is(Token::OPENBRACKET);
			std::vector<TomlBlock>* tm = &this->tables;
			TomlBlock::blockType type = TomlBlock::TABLE;
			if (is_array) {
				type = TomlBlock::ARRAY;
				cur++;
			}
			TokenList res = til_ignore(cur, Token::CLOSEBRACKET | Token::COMMENT, Token::DOT);
			tm->push_back(TomlBlock(res, TokenMap(), type));
			lastmp = &tm->back().mp;
			break;
		}
		default:
			break;
		}
		cur++;
	}
}

void _printKeyValue(TokenMap& mp) {
	ITER_FOREACH(TokenMap, mp, m) {
		ITER_FOREACH_CONST(TokenList, m->key, it) {
			std::cout << it->value << " ";
		}
		std::cout << " = ";
		if (m->is_array) {
			std::cout << "[";
			ITER_FOREACH_CONST(TokenList, m->value, it) {
				std::cout << it->value << ", ";
			}
			std::cout << "]";
			continue;
		}
		std::cout << m->value.front().value;
		std::cout << std::endl;
	}
}

void _print_list(TokenList& list) {
	TokenList::iterator begin = list.begin();
	while (begin != list.end()) {
		std::cout << begin->value << ">";
		begin++;
	}
	std::cout << std::endl;
}

void _print_block(std::vector<TomlBlock>& vec) {

	std::cout << "================================================" << std::endl;
	ITER_FOREACH(std::vector<TomlBlock>, vec, block) {
		_print_list(block->prefix);
		_printKeyValue(block->mp);
	}
}

void Parser::print() {
	_printKeyValue(mp);
	_print_block(tables);
}
