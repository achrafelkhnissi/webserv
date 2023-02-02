#pragma once

#include <string>

class ParseError {
public:
	ParseError();
	ParseError(std::string&, int);

private:
	std::string message;
	int line;
};
