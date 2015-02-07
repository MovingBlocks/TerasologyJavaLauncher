
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <conio.h>

#include "jli_library.h"
#include "platform.h"
#include "config_file.h"
#include "jredetector.h"
#include "log.h"

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
	if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
		log() << "Unable to attach to the console of the parent process.\n";
	}

	string basepath = Platform::getBaseDir();
	log() << "Program base dir is '" << basepath << "'\n";

	ConfigFile config;
	config.load(basepath + Platform::directorySep + "launch.cfg");
	
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
		return jliLibrary.launch(startupJarPath);
	}
	catch (const JliException &e) {
		Platform::showError("Launch Error", e.message());
		return -1;
	}

}
