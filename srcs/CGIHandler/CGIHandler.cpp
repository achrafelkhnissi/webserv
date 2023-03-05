//
// Created by Achraf El khnissi on 3/4/23.
//

#include "CGIHandler.hpp"
#include <algorithm>



CGIHandler::CGIHandler(const stringMap_t &env) {
    _envSize = env.size();
    _env = new char*[_envSize + 1];

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
//void CGIHandler::CGIExecuter() const {
//
//    pid_t pid = fork();
//    if (pid == -1) {
//        throw std::runtime_error("fork() failed");
//    } else if (pid == 0) {
//        // child process
//        if (execve(_cgiPath.c_str(), NULL, _env) == -1) {
//            throw std::runtime_error("execve() failed");
//        }
//    } else {
//        // parent process
//        int status;
//        waitpid(pid, &status, 0);
//        if (WIFEXITED(status)) {
//            std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
//        } else if (WIFSIGNALED(status)) {
//            std::cout << "Child terminated by signal " << WTERMSIG(status) << std::endl;
//        }
//    }
//
//}


CGIHandler::~CGIHandler() {
    for(int i = 0; i < _envSize; i++) {
        free(_env[i]);
    }
    delete[] _env;
}

