
#include "SessionManager.hpp"

SessionManager::SessionManager() {}

SessionManager::~SessionManager() {
	_sessions.clear();
}

void SessionManager::addSession(std::string key, std::string value) {
	_sessions[key] = value;
}

void SessionManager::removeSession(std::string key) {
	_sessions.erase(key);
}

std::string SessionManager::getSession(std::string key) {
	return _sessions[key];
}

void SessionManager::printSessions() {
	std::map<std::string, std::string>::iterator it;
	std::cout << "--- Sessions ---"
	for (it = _sessions.begin(); it != _sessions.end(); it++) {
		std::cout << it->first << " = " << it->second << std::endl;
	}
	std::cout << "----------------" << std::endl;
}

bool SessionManager::isValidSession(std::string sessionId) {
	bool result = false;
//	if (_sessions.find(sessionId) != _sessions.end()) {
//		result = true;
//	}
	if (_sessions.count(sessionId) > 0)
		result = true;
	return result;
}

std::string SessionManager::getUserId(std::string sessionId) {
	return _sessions[sessionId];
}

void SessionManager::clearSessions() {
	_sessions.clear();
}

void SessionManager::createSession(std::string userId) {
	std::string sessionId = _generateSessionId();
	_sessions[sessionId] = userId;
	_setCookie(sessionId);
}

std::string SessionManager::_generateSessionId() {
	std::string sessionId;
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

void SessionManager::_setCookie(std::string sessionId) {
	std::cout << "Set-Cookie: webserv-sessionId=" << sessionId << std::endl;
}
