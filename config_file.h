
#include <string>
#include <vector>
#include <exception>

using namespace std;

class ConfigException : public exception {
public:
	ConfigException(string message) : mMessage(message) {
	}

	const string &message() const {
		return mMessage;
	}
private:
	string mMessage;
};

class ConfigFile {
public:
	string jreDirectory() {
		return mJreDirectory;
	}

	string homeDirectory() {
		return mHomeDirectory;
	}

	vector<string> programArguments() {
		return mProgramArguments;
	}

	vector<string> jvmArguments() {
		return mJvmArguments;
	}

	string launchLog() {
		return mLaunchLog;
	}

	void load(string configFilePath);

private:
	string mLaunchLog;
	string mJreDirectory;
	string mHomeDirectory;
	vector<string> mJvmArguments;
	vector<string> mProgramArguments;
};
