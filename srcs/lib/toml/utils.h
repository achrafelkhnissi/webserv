#ifndef UTILS_H
# define UTILS_H
#include "parser/Parser.hpp"

TokenList til_ignore(TokenList::iterator& cur, Token::e_token until, Token::e_token ignore);

TokenList::iterator til(TokenList::iterator& cur, Token::e_token until);
#endif
