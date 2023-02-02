
#include <cstdlib>
template<typename T, typename E>
class Result {
public:

	Result(T value) {
		fine = true;
		this->value = value;
	}

	Result(E error)
	{
		fine = false;
		this->error = error;
	}

	T ok() {
		if (!fine)
			abort();
		return value;
	}
	E err()
	{
		if (fine)
			abort();
		return error;
	}

	bool is_ok() {
		return is_ok;
	}


private:
	bool fine;
	T value;
	E error;
};
