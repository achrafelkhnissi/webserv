//
// Created by Achraf El khnissi on 3/4/23.
//

#include "CGIHandler.hpp"
#include <algorithm>



CGIHandler::CGIHandler(const stringMap_t &env) {
    _envSize = env.size();
    _env = new char*[_envSize + 1];

	_responseBody = "";
    int i = 0;
    for(stringMap_t::const_iterator it = env.begin(); it != env.end(); it++, i++) {
        _env[i] = strdup((it->first + "=" + it->second).c_str());
    }

    _envMap = env; // todo: remove this line
}

string CGIHandler::getCmd() {

    string path_ = _envMap["PATH_INFO"];
    string scriptName_ = path_.substr(path_.find_last_of('/') + 1);
    string extension_ = ".py";
    string cmd_ = "./www/cgi-bin/";


    stringVectorIterator_t it = find(_cgiPath.begin(), _cgiPath.end(), scriptName_);
    if (it != _cgiPath.end()) {
        cout << "Found file with" << extension_ << " extension: " << *it << endl;
       return  cmd_ += *it;
    } else {
        std::size_t pos = path_.find_last_of('.');
        if (pos != std::string::npos) {
            extension_ = path_.substr(pos);
        }

        for (stringVectorIterator_t it = _cgiPath.begin(); it != _cgiPath.end(); ++it) {
            if (it->size() >= extension_.length() && it->substr(it->size() - extension_.length()) == extension_) {
                cout << "Found file with" << extension_ << " extension: " << *it << endl;
               return cmd_ += *it;
            }
        }
    }
    cout << "Found file with" << extension_ << " extension: " << cmd_ + "helloCGI.py" << endl;
   return cmd_ += "helloCGI.py" ;
}

string CGIHandler::CGIExecuter() {

	string cmd_ = getCmd();
	int fd[2];
	int savedStdout_ = dup(1);
	int savedStdin_ = dup(0);

	if (pipe(fd) == -1)
		throw std::runtime_error("pipe() failed");


	 char* args[] = { "./www/cgi-bin/helloCGI.py", NULL };
	 char* env_[] = { NULL };
    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("fork() failed");
    } else if (pid == 0) {

				std::cout << "\n\nthe cmd : " << cmd_ << std::endl;


        // child process
		// dup the read end of the pipe to stdin ro read the body of the request
		if (dup2(fd[1], STDOUT_FILENO) == -1 ||  close(fd[0]) == -1 || close(fd[1]) == -1)
			throw std::runtime_error("dup2() of close() failed");
		//dup the write end of the pipe to stdout


	std::cerr << "args:" << _env << std::endl;
        if (execve(args[0], args, env_) == -1) {
            throw std::runtime_error(strerror(errno));
        }
    } else {
		close(fd[1]);
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
        } else if (WIFSIGNALED(status)) {
            std::cout << "Child terminated by signal " << WTERMSIG(status) << std::endl;
        }

		char buffer_[1024];
		int ret = 0;
		do {
			::memset(buffer_, 0, 1024);
			ret = read(fd[0], buffer_, 1024) ;
			_responseBody += buffer_;

		} while (ret > 0);
			std::cout << "_responseBody: " << _responseBody << std::endl;



		dup2(savedStdout_, STDOUT_FILENO); //restore stdout
		dup2(savedStdin_, STDIN_FILENO); //restore stdin
		close(fd[0]);
//		close(fd[1]);
		close(savedStdin_);
		close(savedStdout_);
	}

	return _responseBody;
}


CGIHandler::~CGIHandler() {
    for(int i = 0; i < _envSize; i++) {
        free(_env[i]);
    }
    delete[] _env;
}

