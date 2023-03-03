#include "config/Configuration.hpp"
#include "lib/toml/data/table.hpp"
#include "lib/toml/toml.hpp"
#include "request/Request.hpp"
#include "request/parser/HttpParser.hpp"

#include <iostream>
#include <string>

using namespace std;

int main() {
	HttpParser* t = new HttpParser();
	string s0;
	while (getline(cin, s0)) {
		s0.push_back('\n');
		HttpParser::e_status r = t->push(s0);
		if (r == HttpParser::FAILED) {
			cout << "FAILED" << endl;
			break;
		} else if (r == HttpParser::DONE)
			cout << "DONE" << endl;
	}
	delete t;
	return 0;
}
