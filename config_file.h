
#include <string>
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

	string jvmArguments() {
		return mJvmArguments;
	}

	void setJreDirectory(string jreDirectory) {
		mJreDirectory = jreDirectory;
	}

	void setHomeDiretory(string homeDirectory) {
		mHomeDirectory = homeDirectory;
	}

	void setJvmArguments(string jvmArguments) {
		mJvmArguments = jvmArguments;
	}

	void load(string configFilePath);

private:
	string mJreDirectory;
	string mHomeDirectory;
	string mJvmArguments;
};
