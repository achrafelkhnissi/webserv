#include "Parser.hpp"
#include "utils.h"
#include <algorithm>
#include <list>
#include <numeric>
#include <string>


TokenList til_ignore(TokenList::iterator& cur, Token::e_token until, Token::e_token ignore) {
	TokenList res;
	while (!(*cur)->is(until)) {
		if (!(*cur)->is(ignore)) {
			res.push_back(*cur);
		}
		cur++;
	}
	return res;
}

TokenList::iterator til(TokenList::iterator& cur, Token::e_token until) {
	while (!(*cur)->is(until)) {
		cur++;
	}
	return cur;
}

std::string accum(std::string& acc, Token* t) {
	return acc + t->value;
};

Parser::Parser(TokenList tks) {
	TokenList::iterator cur = tks.begin();
	TokenMap* lastmp = &mp;
	while (cur != tks.end()) {
		if ((*cur)->is(Token::KEY)) {
			TokenList res = til_ignore(cur, Token::ASSIGN | Token::COMMENT, Token::DOT);
			cur++;
			lastmp->insert(TokenPair(
				res,
				std::accumulate(
					cur, til(cur, Token::NEWLINE | Token::COMMENT), std::string(""), accum)));
		} else if ((*cur)->is(Token::OPENBRACKET)) {
			cur++;
			std::vector<TomlBlock>* tm = &this->tables;
			TomlBlock::blockType type = TomlBlock::TABLE;
			if ((*cur)->is(Token::OPENBRACKET)) {
				type = TomlBlock::ARRAY;
				cur++;
			}
			TokenList res = til_ignore(cur, Token::CLOSEBRACKET | Token::COMMENT, Token::DOT);
			tm->push_back(TomlBlock(res, TokenMap(), type));
			lastmp = &tm->rbegin()->mp;
		}
		cur++;
	}
}

void _printKeyValue(TokenMap& mp) {
	ITER_FOREACH(TokenMap, mp, m) {
		ITER_FOREACH_CONST(TokenList, m->first, it) {
			std::cout << (*it)->value << " ";
		}
		std::cout << " = ";
		std::cout << m->second;
		std::cout << std::endl;
	}
}

void _print_list(TokenList& list) {
	TokenList::iterator begin = list.begin();
	while (begin != list.end()) {
		std::cout << (*begin)->value << ">";
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
