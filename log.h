
#include <sstream>
#include <string>
using namespace std;

class Log {
public:
	static Log instance;

	Log &operator<<(const string &msg);

	template<typename T>
	inline Log &operator<<(T val) {
		ostringstream stream;
		stream << val;
		return (*this) << stream.str();
	}
};

inline Log &log() {
	return Log::instance;
}
