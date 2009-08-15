/*
	Launch4j (http://launch4j.sourceforge.net/)
	Cross-platform Java application wrapper for creating Windows native executables.

	Copyright (c) 2004, 2008 Grzegorz Kowal,
							 Ian Roberts (jdk preference patch)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	Except as contained in this notice, the name(s) of the above copyright holders
	shall not be used in advertising or otherwise to promote the sale, use or other
	dealings in this Software without prior written authorization.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#ifndef _LAUNCH4J_HEAD__INCLUDED_
#define _LAUNCH4J_HEAD__INCLUDED_

#define WIN32_LEAN_AND_MEAN		// VC - Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <shellapi.h>
#include <direct.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <io.h>
#include <process.h>

#define NO_JAVA_FOUND 0
#define FOUND_JRE 1
#define FOUND_SDK 2

#define JRE_ONLY 0
#define PREFER_JRE 1
#define PREFER_JDK 2
#define JDK_ONLY 3

#define LAUNCH4J_TMP_DIR "\\launch4j-tmp\\"
#define MANIFEST ".manifest"

#define KEY_WOW64_64KEY 0x0100

#define HKEY_STR "HKEY"
#define HKEY_CLASSES_ROOT_STR "HKEY_CLASSES_ROOT"
#define HKEY_CURRENT_USER_STR "HKEY_CURRENT_USER"
#define HKEY_LOCAL_MACHINE_STR "HKEY_LOCAL_MACHINE"
#define HKEY_USERS_STR "HKEY_USERS"
#define HKEY_CURRENT_CONFIG_STR "HKEY_CURRENT_CONFIG"

#define STR 128
#define BIG_STR 1024
#define MAX_VAR_SIZE 32767
#define MAX_ARGS 32768

#define TRUE_STR "true"
#define FALSE_STR "false"

#define debug(args...) if (hLog != NULL) fprintf(hLog, ## args); 

typedef void (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

FILE* openLogFile(const char* exePath, const int pathLen);
void closeLogFile();
void msgBox(const char* text);
void signalError();
BOOL loadString(const int resID, char* buffer);
BOOL loadBool(const int resID);
int loadInt(const int resID);
BOOL regQueryValue(const char* regPath, unsigned char* buffer,
		unsigned long bufferLength);
void regSearch(const HKEY hKey, const char* keyName, const int searchType);
void regSearchWow(const char* keyName, const int searchType);
void regSearchJreSdk(const char* jreKeyName, const char* sdkKeyName,
		const int jdkPreference);
BOOL findJavaHome(char* path, const int jdkPreference);
int getExePath(char* exePath);
void appendPath(char* basepath, const char* path);
void appendJavaw(char* jrePath);
void appendAppClasspath(char* dst, const char* src, const char* classpath);
BOOL isJrePathOk(const char* path);
BOOL expandVars(char *dst, const char *src, const char *exePath, const int pathLen);
void appendHeapSizes(char *dst);
void appendHeapSize(char *dst, const int absID, const int percentID,
		const DWORD freeMemory, const char *option);
int prepare(const char *lpCmdLine);
void closeHandles();
BOOL appendToPathVar(const char* path);
DWORD execute(const BOOL wait);

#endif // _LAUNCH4J_HEAD__INCLUDED_
