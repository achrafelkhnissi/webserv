#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>                       // std::vector
#include <sys/stat.h>				   // stat
#include "utils.hpp"
#include "VirtualServer.hpp"                  // VirtualServer class
#include "SubServer.hpp"                // SubServer class
#include "Configuration.hpp"  // Configuration class
#include "Request.hpp"       // Request class
#include "typedefs.hpp"
#include "HttpParser.hpp"
#include "Response.hpp"

#include <sstream>

#define MSG(msg) (std::string(getBasename((__FILE__)) + ":" + std::string(std::to_string(__LINE__)) + ": " + msg))



/*
 * Server class - This class is responsible for creating a server object
 *
 * @var _virtualServer - vector of VirtualServer objects (one for each virtual server)
 * @var _config - Configuration object that contains all the information from the config file
 *
 * @method Server - constructor that takes a Configuration object as an argument
 * @method ~Server - destructor
 * @method start - method to start the server
 */
class Server {

private:
    virtualServerMap_t      _virtualServer;
    Configuration           _config;
    pollfdsVector_t         _fds;
	errorPagesMap_t 	    _errorPages;
    Request                 _request;
    string                  _uploadPath;
    stringMap_t             _CGIEnv;
    clientHttpParserMap_t   _clientHttpParserMap;
    std::map<string, string> _mimeTypes;
public:
    Server(Configuration config);
    ~Server();

    void    start();
    void    printData() const;

private:
    void    _handleConnections(int fd);
    void    _handleRequest(pollfdsVectorIterator_t it);
    void    _sendResponse(int fd);
    void    _handleGET(int, const subServersIterator_t&, const Request&);
    void    _handlePOST(int, const subServersIterator_t&, const Request&);
	void 	_handleDELETE(int , const subServersIterator_t &, const Request& );
	bool 	is_regular_file(const char* path) const;
    void    _handleError(int fd);
    void    _clearPollfds();
    void    _error(const string& msg, int err) const;
    void    _setupVirtualServer(VirtualServer& vserver);
    void    _setCGIEnv(const Request& request, const location_t& location, const string& path);
    string  _getErrorPage(int code) const;
    string  _getFileContent(const string& path) const;
    void    _setMimeTypes();


    void _handleError(int fd, int statusCode);

    string _getStatusMessage(int statusCode) const;


    void sendResponseHeaders(int fd, const Response &response);

    void sendResponseBody(int fd, const string &resourcePath);

    void sendResponseBody(int fd, const Response& response);

    const string handleFormData(const Request &request, Response &response);

    const string handleFileUploads(const Request &request, Response &response, const string &uploadPath);

    location_t *matchLocation(const locationVector_t &locations, const string &uri);
};

#endif
