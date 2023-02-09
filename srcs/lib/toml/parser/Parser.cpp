#include "Parser.hpp"
#include <algorithm>
#include <list>
#include <numeric>
#include <string>

int is_assign(Token* t) {
	return t->type == Token::ASSIGN || t->type == Token::COMMENT;
}
int is_newline(Token* t) {
	return t->type == Token::NEWLINE || t->type == Token::COMMENT;
}
int is_close(Token* t) {
	return t->type == Token::CLOSEBRACKET || t->type == Token::COMMENT;
}

template <class T>
T ft_next(T it, typename std::iterator_traits<T>::difference_type n = 1) {
	std::advance(it, n);
	return it;
}

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

Parser::Parser(TokenList tks) {
	TokenList::iterator cur = tks.begin();
	TokenMap* lastmp = &mp;
	while (cur != tks.end()) {
		if ((*cur)->is(Token::KEY)) {
			TokenList res = til_ignore(cur, Token::ASSIGN | Token::COMMENT, Token::DOT);
			cur++;
			lastmp->insert(TokenPair(
				res,
				std::accumulate(cur,
								til(cur, Token::NEWLINE | Token::COMMENT),
								std::string(""),
								[](std::string& acc, Token* t) { return acc + t->value; })));
		} else if ((*cur)->is(Token::OPENBRACKET)) {
			cur++;
			std::vector<TomlBlock>* tm = &this->tables;
			if ((*cur)->is(Token::OPENBRACKET)) {
				tm = &array;
				cur++;
			}
			tm->push_back(TomlBlock(TokenList(cur, til(cur, Token::CLOSEBRACKET | Token::COMMENT)),
									TokenMap()));
			lastmp = &tm->rbegin()->mp;
		}
		cur++;
	}
}

void _printKeyValue(TokenMap& mp) {
	for (auto& m : mp) {
		//auto m = mp.begin();
		for (auto& it : m.first) {
			std::cout << it->value << " ";
		}
		std::cout << " = ";
		std::cout << m.second;
		std::cout << std::endl;
	}
}

void _print_list(TokenList& list) {
	auto begin = list.begin();
	while (begin != list.end()) {
		std::cout << (*begin)->value << ">";
		begin++;
	}
	std::cout << std::endl;
}

void _print_block(std::vector<TomlBlock>& vec) {

	std::cout << "================================================" << std::endl;
	for (auto& block : vec) {
		_print_list(block.prefix);
		_printKeyValue(block.mp);
	}
}

void Parser::print() {
	_printKeyValue(mp);
	_print_block(array);
	_print_block(tables);
}
