
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdarg.h>
#include <iostream>

#include "log.h"

static HANDLE mLogHandle = NULL;

void Log::setFile(string filename) {
	
	(*this) << "Enabling debug logging to " << filename << "\n";

	mLogHandle = CreateFile(filename.c_str(), 
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (mLogHandle == INVALID_HANDLE_VALUE) {
		(*this) << "Unable to open debug logfile: " << GetLastError() << "\n";
		return;
	}

	// Redirect STDOUT / STDERR (even for subsequent CRT invocations such as the JRE's)
	SetStdHandle(STD_OUTPUT_HANDLE, mLogHandle);
	SetStdHandle(STD_ERROR_HANDLE, mLogHandle);
}

Log &Log::operator<<(const string &msg) {
	OutputDebugString(msg.c_str());
	if (mLogHandle) {
		// Situations like these really remind me how barebones C++ is
		string msgCopy = msg;
		auto pos = msgCopy.find("\n");
		if (pos != string::npos) {
			msgCopy.replace(pos, 1, "\r\n");
		}
		LPCVOID buffer = msgCopy.c_str();
		DWORD bytesToWrite = (DWORD)msgCopy.size();
		DWORD bytesWritten = 0;
		if (!WriteFile(mLogHandle, buffer, bytesToWrite, &bytesWritten, NULL)) {
			mLogHandle = NULL;
			(*this) << "Failed writing to debugging logfile: " << GetLastError() << "\n";
		} else {
			FlushFileBuffers(mLogHandle);
		}
	}
		
	return *this;
}

