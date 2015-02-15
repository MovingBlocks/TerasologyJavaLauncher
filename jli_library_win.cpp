
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "jli_library.h"
#include "config.h"
#include "log.h"

#include "MinHook.h"

/*
	JLI parses our command line for us into argc/argv, it returns the following structure though.
*/
typedef struct {
	char *value;
	unsigned char hasWildcard; //We really dont need this
} JLI_StdArg;

/*
	Typedefs for stuff found in jli.dll
*/
typedef void(*FNJLI_CmdToArgs)(char *cmdline);
typedef JLI_StdArg *(*FNJLI_GetStdArgs)();
typedef int(*FNJLI_GetStdArgc)();
typedef void(*FNJLI_SetTraceLauncher)();

#define FULL_VERSION "1.8"
#define DOT_VERSION "1.8"

class JliLibraryData {
public:
	JliLibraryData(string _jliPath) : jliPath(_jliPath), 
		handle(NULL), 
		JLI_Launch(NULL), 
		JLI_CmdToArgs(NULL), 
		JLI_GetStdArgs(NULL), 
		JLI_GetStdArgc(NULL) {
	}

	string jliPath;
	HMODULE handle;	
	FNJLI_Launch JLI_Launch;
	FNJLI_CmdToArgs JLI_CmdToArgs;
	FNJLI_GetStdArgs JLI_GetStdArgs;
	FNJLI_GetStdArgc JLI_GetStdArgc;
	FNJLI_SetTraceLauncher JLI_SetTraceLauncher;

	template<typename TFun>
	TFun getEntryPoint(string funName) {
		TFun result = (TFun)GetProcAddress(handle, funName.c_str());
		if (!result) {
			throw JliException("The " + funName + " function has not been found in the provided DLL "
				+ jliPath + ". Is your Java installation corrupted?");
		}
		return result;
	}
};

JliLibrary::JliLibrary(string jrePath) : d(new JliLibraryData(jrePath + "bin\\jli.dll")) {

	d->handle = LoadLibrary(d->jliPath.c_str());

	d->JLI_Launch = d->getEntryPoint<FNJLI_Launch>("JLI_Launch");
	d->JLI_CmdToArgs = d->getEntryPoint<FNJLI_CmdToArgs>("JLI_CmdToArgs");
	d->JLI_GetStdArgs = d->getEntryPoint<FNJLI_GetStdArgs>("JLI_GetStdArgs");
	d->JLI_GetStdArgc = d->getEntryPoint<FNJLI_GetStdArgc>("JLI_GetStdArgc");
}

JliLibrary::~JliLibrary() {
	if (d->handle) {
		FreeLibrary(d->handle);
		d->handle = NULL;
	}
}

/*
	This is an incredibly dirty hack.
	jli.dll tries to guess the application home by calling GetModuleHandle and then basically going
	up one directory. In our case, the exe is in the root so this will produce the wrong directory.
	Because of that, it does not detect our bundled JVM in the JRE subdirectory, which sucks.

	To get around this (without moving our exe file around), we instead fake return jli.dll's path
	for GetModuleFileName(NULL) by hooking into the Kernel32.dll function.
*/
static string detourExePath;
DWORD(__stdcall *GetModuleFileNameAOrg)(HMODULE module, LPSTR filename, DWORD size);
static DWORD __stdcall GetModuleFileNameDetour(HMODULE module, LPSTR filename, DWORD size) {	
	if (!module && detourExePath.size() < size) {
		memcpy(filename, detourExePath.c_str(), detourExePath.size());
		filename[detourExePath.size()] = '\0';
		return ERROR_SUCCESS;
	}

	return GetModuleFileNameAOrg(module, filename, size);
}

int JliLibrary::launch(string startupJarPath, vector<string> jvmArgs, vector<string> programArgs) {	
	detourExePath = d->jliPath;

	MH_Initialize();

	// Terrible hack, but JLI on Windows is also terribly limited when it comes to detecting the
	// Application home path. It just assumes that the binary lives in a "bin\" subdirectory.
	// We are going to fake this
	MH_STATUS status = MH_CreateHook(GetModuleFileNameA, GetModuleFileNameDetour, (LPVOID*)&GetModuleFileNameAOrg);
	if (status != MH_OK) {
		log() << "Could not initialize GetModuleHandle hook.\n";
	}
	MH_EnableHook(GetModuleFileNameA);

	int margc;
	char** margv;

	d->JLI_CmdToArgs(GetCommandLine());
	margc = d->JLI_GetStdArgc();
	// add one more to mark the end
	margv = (char **)malloc((margc + 1) * (sizeof(char *)));
	{
		int i = 0;
		JLI_StdArg *stdargs = d->JLI_GetStdArgs();
		for (i = 0; i < margc; i++) {
			margv[i] = stdargs[i].value;
		}
		margv[i] = NULL;
	}
	
	// We pass in custom arguments to java
	vector<string> customArgs;
	for (auto arg : jvmArgs) {
		customArgs.push_back(arg);
	}
	customArgs.push_back("-jar");
	customArgs.push_back(startupJarPath);
	for (auto arg : programArgs) {
		customArgs.push_back(arg);
	}
	const char** argptrs = new const char*[customArgs.size()];
	for (size_t i = 0; i < customArgs.size(); ++i) {
		argptrs[i] = customArgs[i].c_str();
	}

	int result = d->JLI_Launch(margc, margv,
		(int)customArgs.size(), argptrs,
		0, NULL, // We dont use app classpath
		FULL_VERSION,
		DOT_VERSION,
		*margv,
		*margv,
		1, // We use fixed custom arguments (see above)
		1, // Enable wildcard expansion in the classpath
		1, // Enable "javaw" mode, which enables output of errors as message boxes
		0 // Default policy for Server/Client VM selection
		);

	if (result != 0) {
		throw JliException("Could not launch the game.");
	}

	return result;

}
