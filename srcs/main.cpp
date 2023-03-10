#include "config/Configuration.hpp"
#include "lib/toml/data/table.hpp"
#include "lib/toml/toml.hpp"
#include "request/Request.hpp"
#include "request/parser/HttpParser.hpp"

#include <iostream>
#include <string>

using namespace std;

#include "./server/Server.hpp"

#include <iostream>
#include <string>

int main(int ac, char** av) {

    std::string config_file = "default.toml";

    if (ac == 2) {
        config_file = av[1];
    }

    toml::table* t = toml::parse_file(config_file);
	if (t == nullptr) {
		std::cerr << "Error parsing file" << std::endl;
		return 1;
	}

    try {
        Configuration config(*t);

		config.print();

		Server server(config);

		server.start();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    delete t;

    return 0;
}



#include "CGIHandler.hpp"

//int main() {
//    stringMap_t env;
//    env["PATH_INFO"] = "./www/cgi-bin/hel";
//    env["QUERY_STRING"] = "name=achraf&age=23";
//    env["REQUEST_METHOD"] = "GET";
//
//    CGIHandler cgi(env, "this is a body");
//    cgi._cgiPath.push_back("helloCGI.py");
//    cgi._cgiPath.push_back("helloCGI.php");
//    cgi._cgiPath.push_back("helloCGI.sh");
//    cgi._cgiPath.push_back("helloCGI.rb");
//
////    std::cout << cgi.getCmd() << std::endl;
//
//    cgi.CGIExecuter();
//    return 0;
//}
////
//int main() {
//	HttpParser* t = new HttpParser();
//	string s0;
//	while (getline(cin, s0)) {
//		s0.push_back('\n');
//		HttpParser::e_status r = t->push(s0);
//		if (r == HttpParser::FAILED) {
//			cout << "FAILED" << endl;
//			break;
//		} else if (r == HttpParser::DONE)
//		{
//			Request &req = t->into_request();
//			req.print();
//			delete &req;
//			cout << "DONE" << endl;
//		}
//	}
//	delete t;
//	return 0;
//}
