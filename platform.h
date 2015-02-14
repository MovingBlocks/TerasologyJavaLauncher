
#include <string>

using namespace std;

typedef void* LibraryHandle;

class Platform {
public:
	/**
		Backslash on Windows
		Slash on all else
	*/
	static const char directorySep;

	static string ensureDirectorySep(const string &path);

	/**
		The configuration is expected to be at the executable path
		and be named as the executable. Instead of .exe it ends with
		.cfg though.
	*/
	static string getConfigPath();

	static string getBaseDir();

	static void showError(const string &title, const string &message);

	static bool fileExists(string path);

	static void setWorkingDir(string path);
	
private:
	Platform();
};

inline string Platform::ensureDirectorySep(const string &path) {
	if (!path.empty()) {
		char lastChar = path.at(path.size() - 1);
		if (lastChar != '/' && lastChar != '\\') {
			return path + directorySep;
		}
	}

	return path;
}
