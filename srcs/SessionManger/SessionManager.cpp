
#include "SessionManager.hpp"

/* NOT USED */

SessionManager::SessionManager() {
	srand(time(NULL));
}

SessionManager::~SessionManager() {
	_sessions.clear();
}

void SessionManager::addSession(string key, t_sessionData value) {
	_sessions[key] = value;
}

void SessionManager::removeSession(string key) {
	_sessions.erase(key);
}

t_sessionData SessionManager::getSession(string key) {
	return _sessions[key];
}

void SessionManager::printSessions() {
	std::cout << "--- Sessions ---";
	for (t_sessionMap::iterator it = _sessions.begin(); it != _sessions.end(); ++it) {
		std::cout << '\t' << it->first << " => " << it->second["userId"] << std::endl;
    }
	std::cout << "----------------" << std::endl;
}

bool SessionManager::isValidSession(string sessionId) {
	return _sessions.count(sessionId) > 0;
}

string SessionManager::getUserId(string sessionId) {
	return _sessions[sessionId]["userId"];
}

void SessionManager::clearSessions() {
	_sessions.clear();
}

void SessionManager::createSession(string userId, t_sessionData sessionData) {
	string sessionId = _generateSessionId();

	sessionData["userId"] = userId;
	_sessions[sessionId] = sessionData;

	// todo: set cookie
	_setCookie(sessionId);
}

string SessionManager::_generateSessionId() {
	string sessionId;
	const int SESSION_ID_LENGTH = 32;
	std::string charset = "0123456789"
						  "abcdefghijklmnopqrstuvwxyz"
						  "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	srand(time(NULL));
	for (int i = 0; i < SESSION_ID_LENGTH; i++) {
		sessionId += charset[rand() % charset.length()];
	}
	return sessionId;
}

void SessionManager::_setCookie(string sessionId) {
	std::cout << "Set-Cookie: webserv-sessionId=" << sessionId << std::endl;
}
