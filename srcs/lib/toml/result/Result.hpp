#pragma once

#include <cstdlib>
#include <iostream>
#include <ostream>

template <typename T, typename E>
class Result {
public:
	Result(T value) {
		fine = true;
		this->value = value;
	}

	Result() {
		fine = false;
	}

	Result(E error) {
		fine = false;
		this->error = error;
	}

	T& ok() {
		if (!fine) {
			std::cerr << "abort: " << std::endl;
			std::cerr << this->error.as_str() << std::endl;
			abort();
		}
		return value;
	}
	E err() {
		if (fine) {
			std::cerr << "abort: access Result err while Result is VALUE" << std::endl;
			abort();
		}
		return error;
	}

	bool is_ok() {
		return fine;
	}

private:
	bool fine;
	T value;
	E error;
};
