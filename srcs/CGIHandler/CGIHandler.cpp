//
// Created by Achraf El khnissi on 3/4/23.
//

#include "CGIHandler.hpp"
#include <algorithm>


CGIHandler::CGIHandler(const stringMap_t &env, const string &body, location_t* location)
	: _requestBody(body), _envMap(env) {

	_cgiPath = location->cgi_path;
    _envSize = env.size();
    _env = new char*[_envSize + 1];
    _env[_envSize] = NULL; // NULL terminate the array is expected by execve
	_responseBody = "";
    int i = 0;
    for(stringMap_t::const_iterator it = env.begin(); it != env.end(); it++, i++) {
        *(_env + i) = strdup((it->first + "=" + it->second).c_str());
    }
}

string CGIHandler::getCmd() {

    string path_ = _envMap["PATH_INFO"];
    string scriptName_ = path_.substr(path_.find_last_of('/') + 1);
    string extension_ = ".py";
    string cmd_ = "./www/cgi-bin/";


    stringVectorIterator_t it = find(_cgiPath.begin(), _cgiPath.end(), scriptName_);
    if (it != _cgiPath.end()) {
        return  cmd_ += *it;
    } else {
        std::size_t pos = path_.find_last_of('.');
        if (pos != std::string::npos) {
            extension_ = path_.substr(pos);
        }

        for (stringVectorIterator_t it = _cgiPath.begin(); it != _cgiPath.end(); ++it) { //
            if (it->size() >= extension_.length() && it->substr(it->size() - extension_.length()) == extension_) {
               return cmd_ += *it;
            }
        }
    }

   return cmd_ += "helloCGI.py" ;
}

void CGIHandler::setArgs(string cmd){
    _argv = new char*[2];
    _argv[0] = strdup(cmd.c_str());
    _argv[1] = NULL;
}

string CGIHandler::CGIExecuter() {

	string cmd_ = getCmd();
    setArgs(cmd_);

	int fdForResponse[2];
    int fdForRequest[2];

	if (pipe(fdForResponse) == -1 || pipe(fdForRequest) == -1)
		throw std::runtime_error("pipe() failed");


    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("fork() failed");
    } else if (pid == 0) { // child process

		if (dup2(fdForResponse[1], STDOUT_FILENO) == -1 //dup the write end of the pipe to stdout to write the response
            || close(fdForResponse[0]) == -1 || close(fdForResponse[1]) == - 1
            || dup2(fdForRequest[0], STDIN_FILENO) == -1 //dup the read end of the pipe to stdin to read the request
            || close(fdForRequest[1]) == -1 || close(fdForRequest[0]) == -1 )// close the read and write end of the pipe
			throw std::runtime_error("dup2() of close() failed");

        if (execve(_argv[0], _argv, _env) == -1) {
            throw std::runtime_error(strerror(errno));
        }
    } else {

        close(fdForRequest[0]); // close the read end of the pipe
        write(fdForRequest[1], _requestBody.c_str(), _requestBody.size());
        close(fdForRequest[1]);

		close(fdForResponse[1]); // close the write end of the pipe
        int status;
        waitpid(pid, &status, 0);

		char buffer_[1024];
		int ret = 0;
		do {
			::memset(buffer_, 0, 1024);
			ret = read(fdForResponse[0], buffer_, 1024) ;
			_responseBody.append(buffer_, ret);

		} while (ret > 0);
		close(fdForResponse[0]);
	}
	return _responseBody;
}

CGIHandler::~CGIHandler() {
    for(size_t i = 0; i < _envSize; i++) {
        free(_env[i]);
    }

    for (size_t i = 0; i < 2; i++) {
        free(_argv[i]);
    }
    delete[] _env;
}

