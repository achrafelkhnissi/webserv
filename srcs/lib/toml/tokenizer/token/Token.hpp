#pragma once

#include <string>

class Token {

public:
	enum e_token {
		KEY = 1,
		ASSIGN = 1 << 1,
		VALUE = 1 << 2,
		DOT = 1 << 3,
		OpenBracket = 1 << 4,
		CloseBracket = 1 << 5,
		COMMENT = 1 << 8,
		_EOF = 1 << 30,
	};
	enum e_token type;
	std::string value;
	int line;
	int offset;

	bool is(enum e_token type) {
		return this->type | type;
	}
};
