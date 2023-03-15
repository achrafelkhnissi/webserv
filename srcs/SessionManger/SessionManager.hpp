#ifndef WEBSERV_SESSIONMANAGER_HPP
#define WEBSERV_SESSIONMANAGER_HPP

#include <map>
#include <string>
#include <iostream>

/*
 * How to test SessionManager:
 * - When a user logs in, a session is created and a cookie is set
 * - When a user logs out, the session is deleted
 * - When a user tries to access a page, the cookie is checked
 * - If the cookie is valid, the user is logged in
 * - If the cookie is invalid, the user is logged out
 * - Store session information in a map
 * - Store the body of the post in a string
 * - When a new post request is received, Check the cookie/session
 * - If the cookie is valid, the user is logged in using the stored data.
 *
 * Example:
 * POST /login HTTP/1.1
 *
 * 1. Check the cookie
 * 2. If the cookie is valid, the user is logged in using the stored data (display /homepage directly)
 * 3. If the cookie is invalid, the user is logged out (display /login)
 *
 */

class SessionManager {

private:
	std::map<std::string, std::string> _sessions;

	// todo: check how to store session state information
	// std::map<std::string, std::map<std::string, std::string> > _sessionState;
	// std::map<std::string, std::string> _sessionState;

public:
	SessionManager();
	~SessionManager();

	void addSession(std::string key, std::string value);
	void removeSession(std::string key);
	std::string getSession(std::string key);

	void printSessions();

	void clearSessions();

	void createSession(std::string userId);
	void deleteSession(std::string userId);

	std::string getUserId(std::string sessionId);
	bool isValidSession(std::string sessionId);

private:
	std::string _generateSessionId();
	void _setCookie(std::string sessionId);
};

#endif //WEBSERV_SESSIONMANAGER_HPP
