//
// Created by Achraf El khnissi on 3/4/23.
//

#ifndef WEBSERV_CGIHANDLER_HPP
#define WEBSERV_CGIHANDLER_HPP

#include <iostream>
#include <string>

typedef std::string string;

/*
 * TODO:
 * 	- make the CGIHandler turn the request into env variables
 * 	- store the env variables in a map
 * 	- turn the map into a char** to pass it to execve
 * 	- make sure to free the char** after execve
 * 	- save the standard in and out
 * 	- create a pipe to communicate with the CGI script
 * 	- make the fd_out of the child process point to the pipe write end
 * 	- make the fd_in of the main process point to the pipe read end
 * 	- make sure to close the pipe write end in the main process
 * 		and the pipe read end in the child process
 * 	- create a new process to run the CGI script
 *
 *  -The method of CGI activation : [virtual path][extra path information]?[query string]
 * 	- Accessing CGI Programs Through URLs
 * 		- set up a specific directory for CGI programs ( for security reasons ) ( ex: /cgi-bin )
 * 	the location of the data varies depending on the method attribute specified in the form tag:
 * 		- GET: the data is appended to the URL ==> the data is sent in the query string
 * 		- POST: the data is sent in the body of the request ==> the data is sent as command line arguments
 *
 * The following are environment variables passed to CGI programs:

Server specific variables:
- SERVER_SOFTWARE: name/version of HTTP server.
- SERVER_NAME: host name of the server, may be dot-decimal IP address.
- GATEWAY_INTERFACE: CGI/version.

TODO: Request specific variables:
    - SERVER_PROTOCOL: HTTP/version.
    - SERVER_PORT: TCP port (decimal).
    - REQUEST_METHOD: name of HTTP method (see above).
    - PATH_INFO: path suffix, if appended to URL after program name and a slash.
    - PATH_TRANSLATED: corresponding full path as supposed by server, if PATH_INFO is present.
    - SCRIPT_NAME: relative path to the program, like /cgi-bin/script.cgi.
    - QUERY_STRING: the part of URL after ? character. The query string may be composed of *name=value pairs separated with ampersands (such as var1=val1&var2=val2...) when used to submit form data transferred via GET method as defined by HTML application/x-www-form-urlencoded.
    - REMOTE_HOST: host name of the client, unset if server did not perform such lookup.
    - REMOTE_ADDR: IP address of the client (dot-decimal).
    - AUTH_TYPE: identification type, if applicable.
    - REMOTE_USER used for certain AUTH_TYPEs.
    - REMOTE_IDENT: see ident, only if server performed such lookup.
    - CONTENT_TYPE: Internet media type of input data if PUT or POST method are used, as provided via HTTP header.
    - CONTENT_LENGTH: similarly, size of input data (decimal, in octets) if provided via HTTP header.

Variables passed by user agent (HTTP_ACCEPT, HTTP_ACCEPT_LANGUAGE, HTTP_USER_AGENT, HTTP_COOKIE and possibly others) contain values of corresponding HTTP headers and therefore have the same sense.

 *
 * REMARKS:
 *  - The file requested by the client is passed to the CGI program as the first argument. (URL does not map to a flat file, but instead somehow to a scripting area)
 *  - The CGIPASS directive is used to specify the directory where the CGI programs are located.
 *
 *  http://example.com/cgi-bin/myprogram.cgi?file=index.html example of a CGI request
 */

#include "../request/Request.hpp"
//#include "../server/SubServer.hpp"
#include "../../inc/typedefs.hpp"

class CGIHandler {

public: // make it private later
	stringVector_t _cgiPath;
	string _responseBody;

    stringMap_t _envMap; // todo: delete this later
    char **_env;
    char **_argv;
    size_t _envSize;


public:
//	CGIHandler(const string& path, const Request& request, const string& body);
    CGIHandler(const stringMap_t& env);
	~CGIHandler();

	void handle();
	const string& getResponse() const;
    string getCmd();
    void setArgs(string);
    string CGIExecuter();
};


#endif //WEBSERV_CGIHANDLER_HPP
