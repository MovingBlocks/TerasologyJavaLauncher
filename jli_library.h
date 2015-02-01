
#include <string>
#include <exception>

using namespace std;

class JliException : public exception {
public:
	JliException(string message) : mMessage(message) {
	}

	const string &message() const {
		return mMessage;
	}
private:
	string mMessage;
};

class JliLibraryData;

extern "C" {
	typedef int(*FNJLI_Launch)(int argc, char ** argv,              /* main argc, argc */
		int jargc, const char** jargv,          /* java args */
		int appclassc, const char** appclassv,  /* app classpath */
		const char* fullversion,                /* full version defined */
		const char* dotversion,                 /* dot version defined */
		const char* pname,                      /* program name */
		const char* lname,                      /* launcher name */
		bool javaargs,                      /* JAVA_ARGS */
		bool cpwildcard,                    /* classpath wildcard*/
		bool javaw,                         /* windows-only javaw */
		int ergo                               /* ergonomics class policy */
		);
}

class JliLibrary {
public:
	JliLibrary(string jrePath);
	~JliLibrary();

	int launch(string startupJarPath);

private:
	JliLibraryData *d;
};
