#ifndef WEBSERV_TYPEDEFS_HPP
#define WEBSERV_TYPEDEFS_HPP

#include <string>
#include <map>
#include <vector>
#include <poll.h>
#include <iostream>

class SubServer;
class VirtualServer;
class Server;
class Request;
class Response;
class HttpParser;

typedef struct location_s location_t;

/* Server Typdefs */
typedef std::vector<pollfd>                         pollfdsVector_t;
typedef std::vector<pollfd>::iterator               pollfdsVectorIterator_t;
typedef std::vector<pollfd>::const_iterator         pollfdsVectorConstIterator_t;

typedef std::map<int, VirtualServer>                      virtualServerMap_t;
typedef virtualServerMap_t::iterator                virtualServerMapIterator_t ;
typedef virtualServerMap_t::const_iterator          virtualServerMapConstIterator_t;

typedef std::map<int, std::string>			        errorPagesMap_t;
typedef errorPagesMap_t::iterator			        errorPagesMapIterator_t;
typedef errorPagesMap_t::const_iterator		        errorPagesMapConstIterator_t;

typedef std::map<std::string, std::string>           stringMap_t; // env map for CGI

typedef std::map<int, HttpParser>                    clientHttpParserMap_t;
typedef clientHttpParserMap_t::iterator              clientHttpParserMapIterator_t;
typedef clientHttpParserMap_t::const_iterator        clientHttpParserMapConstIterator_t;

/* SubServer Typdefs */
typedef std::vector<location_t>                       locationVector_t;
typedef std::vector<location_t>::iterator             locationVectorIterator_t;
typedef std::vector<location_t>::const_iterator       locationVectorConstIterator_t;

/* VirtualServer Typdefs */
typedef std::pair<std::string, int>			            hostPortPair_t;

typedef std::vector<SubServer>                      subServerVector_t;
typedef std::vector<SubServer>::iterator            subServersIterator_t;
typedef std::vector<SubServer>::const_iterator      subServersConstIterator_t;

/* Utils Typdefs */
typedef std::vector<std::string>                    stringVector_t;
typedef std::vector<std::string>::iterator          stringVectorIterator_t;
typedef std::vector<std::string>::const_iterator    stringVectorConstIterator_t;

#include "Server.hpp"
#include "VirtualServer.hpp"
#include "SubServer.hpp"
#include "HttpParser.hpp"
#include "Request.hpp"
#include "Response.hpp"

#endif //WEBSERV_TYPEDEFS_HPP
