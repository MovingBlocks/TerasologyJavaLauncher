
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdarg.h>
#include <iostream>

#include "log.h"

Log Log::instance;

Log &Log::operator<<(const string &msg) {
	OutputDebugString(msg.c_str());
	cout << msg;
	return *this;
}
