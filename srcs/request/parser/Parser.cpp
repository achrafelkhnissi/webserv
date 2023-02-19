

#include "Parser.hpp"
void Parser::push(std::string chunk) {
	if (chunk.empty())
		return;

	queue.push(chunk);
}
