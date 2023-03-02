#include "config/Configuration.hpp"
#include "lib/toml/data/table.hpp"
#include "lib/toml/toml.hpp"
#include "request/Request.hpp"
#include "request/parser/HttpParser.hpp"

#include <iostream>
#include <string>

using namespace std;

int main(int ac, char** av) {
	HttpParser* t = new HttpParser();
	string s0 = string(av[ac - 1]);
	if(t->push(s0) != HttpParser::FAILED){
		t->print();
	}
	else 
		cout << "FAILED" << endl;
	delete t;
	return 0;
}
