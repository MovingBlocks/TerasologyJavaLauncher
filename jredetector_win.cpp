
#include <cvt/wstring>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <atlbase.h>
#include <atlstr.h>

#include "jredetector.h"
#include "log.h"
#include "platform.h"

const int JRE_KEY_COUNT = 2;
const CString JRE_KEYS[2] =
{
	"SOFTWARE\\JavaSoft\\Java Runtime Environment\\1.8",
	"SOFTWARE\\JavaSoft\\Java Development Kit\\1.8"
};
const CString JAVA_HOME_KEY = "JavaHome";

bool JreDetector::isValidJre(string javaHome) {
	// Check that the JLI lib exists
	string jliLIbPath = javaHome + "bin\\jli.dll";

	if (!PathFileExists(jliLIbPath.c_str())) {
		log() << "JLI library does not exist @ " << jliLIbPath << "\n";
		return false;
	}

	// TODO: Check whether we can successfully load jli.lib (32-bit vs 64-bit, etc.)

	return true;
}

string JreDetector::detectSystemJre() {
	CRegKey regKey;
	LSTATUS status;

	for (int key = 0; key < JRE_KEY_COUNT; key++) {
		CString jreKey = JRE_KEYS[key];
		if ((status = regKey.Open(HKEY_LOCAL_MACHINE, jreKey, KEY_READ)) != ERROR_SUCCESS) {
			log() << "Could not find registry key " << jreKey.GetString() << " Status: " << status << "\n";
			if (key == JRE_KEY_COUNT - 1) {
				return "";
			}
		}
	}

	char javaHome[MAX_PATH + 1];
	memset(javaHome, 0, MAX_PATH + 1);
	DWORD size = MAX_PATH;
	if (regKey.QueryStringValue(JAVA_HOME_KEY, javaHome, &size) != ERROR_SUCCESS) {
		return "";
	}

	string result = Platform::ensureDirectorySep(javaHome);

	if (!isValidJre(result)) {
		log() << "Could not validate system jre " << result << "\n";
		return "";
	}

	return result;
}

string JreDetector::detectLocalJre() {
	// This is the candiate JRE Path
	string jrePath = Platform::getBaseDir() + "jre\\";

	if (!isValidJre(jrePath)) {
		return "";
	}

	return jrePath;
}
