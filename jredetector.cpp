
#include "jredetector.h"
#include "platform.h"
#include "log.h"

string JreDetector::detectJre(string userJrePath) {

	string systemJrePath = JreDetector::detectSystemJre();
	string localJrePath = JreDetector::detectLocalJre();
	userJrePath = Platform::ensureDirectorySep(userJrePath);

	log() << "User JRE: " << userJrePath << "\n";
	log() << "System JRE: " << systemJrePath << "\n";
	log() << "Local JRE: " << localJrePath << "\n";

	// A user supplied VM always takes precedence
	if (!userJrePath.empty()) {
		if (!isValidJre(userJrePath)) {
			Platform::showError(
				"Configured Java is not suitable",
				"The configured Java installation at '" + userJrePath + "' cannot be found or is incompatible with Terasology."
				);
			return "";
		}
		return userJrePath;
	}

	if (!localJrePath.empty()) {
		return localJrePath;
	}

	return systemJrePath;

}
