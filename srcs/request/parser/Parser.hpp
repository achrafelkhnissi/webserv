#ifndef PARSER_HPP
#define PARSER_HPP

#include <queue>
#include <string>

class Parser {

public:
	Parser();
	~Parser();
	void push(std::string chunk);
	Parser(const Parser& other);
	Parser& operator=(const Parser& other);

private:
	std::string chunk;
	std::queue<std::string> queue;
};

#endif
