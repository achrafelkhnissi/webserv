#include "ParseError.hpp"

ParseError::ParseError() { }

ParseError::ParseError(std::string message, int line) {
	this->message = message;
	this->line = line;
}
