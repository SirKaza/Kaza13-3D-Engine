#pragma once
#include "Globals.h"
#include "EditorMenu.h"
#include "ConsoleEditor.h"
#include <string>

void log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	if (strcmp(format, "<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>\n") != 0) { // not Gl debug spam
		myConsole.AddLog(tmp_string);
	}
}