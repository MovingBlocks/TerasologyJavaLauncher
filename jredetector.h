
#include <string>

using namespace std;

class JreDetector {
public:
	static string detectJre(string userJrePath);

	/*
		Attempts to detect a system wide installation of a suitable JRE.
	*/
	static string detectSystemJre();

	/*
		Attempts to detect a local installation of a suitable JRE (relative to this executable).
	*/
	static string detectLocalJre();
	
private:
	JreDetector();

	static bool isValidJre(string jreHome);
};
