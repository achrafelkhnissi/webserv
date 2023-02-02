#pragma once

#include <string>

class Token {

public:
	enum e_token {
		TK_KEY = 1,
		TK_ASSIGN = 1 << 1,
		TK_VALUE = 1 << 2,
		TK_EOF = 1 << 4,
	};
	enum e_token type;
	std::string value;

	bool is(enum e_token type) {
		return this->type | type;
	}
};
