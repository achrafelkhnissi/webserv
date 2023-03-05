
#include "CGIHandler.hpp"

int main() {
    stringMap_t env;
    env["PATH_INFO"] = "./www/cgi-bin/hel";
    env["QUERY_STRING"] = "name=achraf&age=23";
    env["REQUEST_METHOD"] = "GET";

    CGIHandler cgi(env);
    cgi._cgiPath.push_back("helloCGI.py");
    cgi._cgiPath.push_back("helloCGI.php");
    cgi._cgiPath.push_back("helloCGI.sh");
    cgi._cgiPath.push_back("helloCGI.rb");

    std::cout << cgi.getCmd() << std::endl;

    return 0;
}
