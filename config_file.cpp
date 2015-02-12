
#include <fstream>
#include <exception>

#include "util.h"
#include "config_file.h"

void ConfigFile::load(string configFilePath) {

	ifstream infile(configFilePath);
	string line;

	while (getline(infile, line)) {
		// Trim line first
		line = trim(line);

		if (line.empty() || line[0] == ';') {
			continue;
		}

		// Skip lines without =
		auto posOfAssignment = line.find('=');
		if (posOfAssignment == string::npos) {
			throw ConfigException("Found malformed config line: " + line);
			continue;
		}

		// Extract key / value
		auto key = trim(line.substr(0, posOfAssignment));
		auto value = trim(line.substr(posOfAssignment + 1));

		if (key == "jredir") {
			mJreDirectory = value;
		} else if (key == "jvmarg") {
			mJvmArguments.push_back(value);
		} else if (key == "arg") {
			mProgramArguments.push_back(value);
		} else if (key == "log") {
			mLaunchLog = value;
		} else {
			throw ConfigException("Invalid config option: " + key);
		}
	}
	
}
