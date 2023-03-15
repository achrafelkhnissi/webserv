#ifndef WEBSERV_SESSIONMANAGER_HPP
#define WEBSERV_SESSIONMANAGER_HPP

#include <map>
#include <string>
#include <iostream>

class SessionManager {

private:
	std::map<std::string, std::string> _sessions;

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
