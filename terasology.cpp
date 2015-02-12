
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <conio.h>
#include <cstdlib>

#include "jli_library.h"
#include "platform.h"
#include "config_file.h"
#include "jredetector.h"
#include "log.h"

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	
	string basepath = Platform::getBaseDir();
	log() << "Program base dir is '" << basepath << "'\n";

	SetCurrentDirectoryA(basepath.c_str());

	ConfigFile config;
	try {
		config.load(Platform::getConfigPath());
	} catch (const ConfigException &e) {
		Platform::showError("Configuration Error",
			"Configuration error occured in " + Platform::getConfigPath() + ":\n"
			+ e.message());
		return -1;
	}
	

	if (!config.launchLog().empty()) {
		HANDLE hLogFile = CreateFile(config.launchLog().c_str(), GENERIC_WRITE, // open for writing
			0,                      // do not share
			NULL,                   // default security
			CREATE_ALWAYS,             // create new file only
			FILE_ATTRIBUTE_NORMAL,  // normal file
			NULL);
		SetStdHandle(STD_OUTPUT_HANDLE, hLogFile);
		SetStdHandle(STD_ERROR_HANDLE, hLogFile);
	}
	
	string jreHome = JreDetector::detectJre(config.jreDirectory());

	// No suitable JRE has been found
	if (jreHome.empty()) {
		// User installation error will be reported by detectJre
		if (config.jreDirectory().empty()) {
			Platform::showError("No Java Installation Found",
				"Could not find a suitable Java 8 installation on your computer.");
		}
		return -1;
	}

	string startupJarPath = Platform::getBaseDir() + "libs" + Platform::directorySep + "Terasology.jar";

	if (!Platform::fileExists(startupJarPath)) {
		Platform::showError("Installation Error",
			"The required file " + startupJarPath + " is missing.");
		return -1;
	}
	
	try {
		JliLibrary jliLibrary(jreHome);
		return jliLibrary.launch(startupJarPath, config.jvmArguments(), config.programArguments());
	}
	catch (const JliException &e) {
		Platform::showError("Launch Error", e.message());
		return -1;
	}

}
