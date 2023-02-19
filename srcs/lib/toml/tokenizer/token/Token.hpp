#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

class Token {

public:
	// clang-format off
	enum e_token {
		UNINTILIASED 	= 1,
		KEY 			= 1 << 1,
		ASSIGN 			= 1 << 2,
		QOUTED 			= 1 << 3,
		DOT 			= 1 << 4,
		OPENBRACKET 	= 1 << 5,
		CLOSEBRACKET 	= 1 << 6,
		NEWLINE 		= 1 << 7,
		COMMA 			= 1 << 8,
		COMMENT 		= 1 << 9,
		VALUE 			= 1 << 10,
		_EOF 			= 1 << 20,
	};
	// clang-format on

	enum e_token type;
	std::string value;
	int line;
	int offset;

	bool is(enum e_token type) {
		return this->type & type;
	}

	std::string as_str() {
		std::string s;
		s = "TOKEN " + std::to_string(line) + ":" + std::to_string(offset) + " `" + value + "`";
		switch (type) {
		case KEY:
			s += "[KEY]";
			break;
		case VALUE:
			s += "[VALUE]";
			break;
		case QOUTED:
			s += "[QOUTED]";
			break;
		case ASSIGN:
			s += "[ASSIGN]";
			break;
		case DOT:
			s += "[DOT]";
			break;
		case OPENBRACKET:
			s += "[OPENBRACKET]";
			break;
		case CLOSEBRACKET:
			s += "[CLOSEBRACKET]";
			break;
		case NEWLINE:
			s += "[NEWLINE]";
			break;
		case COMMENT:
			s += "[COMMENT]";
			break;
		case _EOF:
			s += "[_EOF]";
			break;
		case COMMA:
			s += "[COMMA]";
			break;
		case UNINTILIASED:
			abort();
		}
		s += '\n';
		return s;
	}
};

inline Token::e_token operator|(Token::e_token t1, Token::e_token t2) {
	return (Token::e_token)((int)t1 | (int)t2);
}
