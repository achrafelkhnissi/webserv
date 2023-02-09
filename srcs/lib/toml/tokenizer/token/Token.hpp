#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

class Token {

public:
	enum e_token {
		KEY = 1,
		ASSIGN = 1 << 1,
		VALUE = 1 << 9,
		QOUTED = 1 << 2,
		DOT = 1 << 3,
		OPENBRACKET = 1 << 4,
		CLOSEBRACKET = 1 << 5,
		NEWLINE = 1 << 6,
		COMMENT = 1 << 8,
		_EOF = 1 << 20,
	};
	enum e_token type;
	std::string value;
	int line;
	int offset;

	bool is(enum e_token type) {
		return this->type == type;
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
		}
		s += '\n';
		return s;
	}
};
