/*
	Launch4j (http://launch4j.sourceforge.net/)
	Cross-platform Java application wrapper for creating Windows native executables.

	Copyright (c) 2004, 2008 Grzegorz Kowal,
							 Ian Roberts (jdk preference patch)
							 Sylvain Mina (single instance patch)

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

#include "resource.h"
#include "head.h"

HMODULE hModule;
FILE* hLog;
BOOL console = FALSE;
BOOL wow64 = FALSE;
int foundJava = NO_JAVA_FOUND;

struct _stat statBuf;
PROCESS_INFORMATION pi;
DWORD priority;

char mutexName[STR] = {0};

char errUrl[256] = {0};
char errTitle[STR] = "Launch4j";
char errMsg[BIG_STR] = {0};

char javaMinVer[STR] = {0};
char javaMaxVer[STR] = {0};
char foundJavaVer[STR] = {0};
char foundJavaKey[_MAX_PATH] = {0};

char oldPwd[_MAX_PATH] = {0};
char workingDir[_MAX_PATH] = {0};
char cmd[_MAX_PATH] = {0};
char args[MAX_ARGS] = {0};

FILE* openLogFile(const char* exePath, const int pathLen) {
	char path[_MAX_PATH] = {0};
	strncpy(path, exePath, pathLen);
	strcat(path, "\\launch4j.log");
	return fopen(path, "a");
}

void closeLogFile() {
	if (hLog != NULL) {
		fclose(hLog);	
	}
}

void setWow64Flag() {
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
			GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (fnIsWow64Process != NULL) {
		fnIsWow64Process(GetCurrentProcess(), &wow64);
	}
	debug("WOW64:\t\t%s\n", wow64 ? "yes" : "no"); 
}

void setConsoleFlag() {
     console = TRUE;
}

void msgBox(const char* text) {
    if (console) {
        printf("%s: %s\n", errTitle, text);
    } else {
    	MessageBox(NULL, text, errTitle, MB_OK);
    }
}

void signalError() {
	DWORD err = GetLastError();
	if (err) {
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
						| FORMAT_MESSAGE_FROM_SYSTEM
						| FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				err,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			    (LPTSTR) &lpMsgBuf,
			    0,
			    NULL);
		debug("Error:\t\t%s\n", (LPCTSTR) lpMsgBuf);
		strcat(errMsg, "\n\n");
		strcat(errMsg, (LPCTSTR) lpMsgBuf);
		msgBox(errMsg);
		LocalFree(lpMsgBuf);
	} else {
		msgBox(errMsg);
	}
	if (*errUrl) {
		debug("Open URL:\t%s\n", errUrl);
		ShellExecute(NULL, "open", errUrl, NULL, NULL, SW_SHOWNORMAL);
	}
	closeLogFile();
}

BOOL loadString(const int resID, char* buffer) {
	HRSRC hResource;
	HGLOBAL hResourceLoaded;
	LPBYTE lpBuffer;

	hResource = FindResourceEx(hModule, RT_RCDATA, MAKEINTRESOURCE(resID),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
	if (NULL != hResource) {
		hResourceLoaded = LoadResource(hModule, hResource);
		if (NULL != hResourceLoaded) {
			lpBuffer = (LPBYTE) LockResource(hResourceLoaded);            
			if (NULL != lpBuffer) {     
				int x = 0;
				do {
					buffer[x] = (char) lpBuffer[x];
				} while (buffer[x++] != 0);
				// debug("Resource %d:\t%s\n", resID, buffer);
				return TRUE;
			}
		}    
	} else {
		SetLastError(0);
	}
	return FALSE;
}

BOOL loadBool(const int resID) {
	char boolStr[20] = {0};
	loadString(resID, boolStr);
	return strcmp(boolStr, TRUE_STR) == 0;
}

int loadInt(const int resID) {
	char intStr[20] = {0};
	loadString(resID, intStr);
	return atoi(intStr);
}

BOOL regQueryValue(const char* regPath, unsigned char* buffer,
		unsigned long bufferLength) {
	HKEY hRootKey;
	char* key;
	char* value;
	if (strstr(regPath, HKEY_CLASSES_ROOT_STR) == regPath) {
		hRootKey = HKEY_CLASSES_ROOT;
	} else if (strstr(regPath, HKEY_CURRENT_USER_STR) == regPath) {
		hRootKey = HKEY_CURRENT_USER;
	} else if (strstr(regPath, HKEY_LOCAL_MACHINE_STR) == regPath) {
		hRootKey = HKEY_LOCAL_MACHINE;
	} else if (strstr(regPath, HKEY_USERS_STR) == regPath) {
		hRootKey = HKEY_USERS;
	} else if (strstr(regPath, HKEY_CURRENT_CONFIG_STR) == regPath) {
		hRootKey = HKEY_CURRENT_CONFIG;
	} else {
		return FALSE;
	}
	key = strchr(regPath, '\\') + 1;
	value = strrchr(regPath, '\\') + 1;
	*(value - 1) = 0;

	HKEY hKey;
	unsigned long datatype;
	BOOL result = FALSE;
	if ((wow64 && RegOpenKeyEx(hRootKey,
								key,
								0,
	        					KEY_READ | KEY_WOW64_64KEY,
								&hKey) == ERROR_SUCCESS)
			|| RegOpenKeyEx(hRootKey,
								key,
								0,
	        					KEY_READ,
								&hKey) == ERROR_SUCCESS) {
		result = RegQueryValueEx(hKey, value, NULL, &datatype, buffer, &bufferLength)
				== ERROR_SUCCESS;
		RegCloseKey(hKey);
	}
	*(value - 1) = '\\';
	return result;
}

void regSearch(const HKEY hKey, const char* keyName, const int searchType) {
	DWORD x = 0;
	unsigned long size = BIG_STR;
	FILETIME time;
	char buffer[BIG_STR] = {0};
	while (RegEnumKeyEx(
				hKey,			// handle to key to enumerate
				x++,			// index of subkey to enumerate
				buffer,			// address of buffer for subkey name
				&size,			// address for size of subkey buffer
				NULL,			// reserved
				NULL,			// address of buffer for class string
				NULL,			// address for size of class buffer
				&time) == ERROR_SUCCESS) {
		
		if (strcmp(buffer, javaMinVer) >= 0
				&& (!*javaMaxVer || strcmp(buffer, javaMaxVer) <= 0)
				&& strcmp(buffer, foundJavaVer) > 0) {
			strcpy(foundJavaVer, buffer);
			strcpy(foundJavaKey, keyName);
			appendPath(foundJavaKey, buffer);	
			foundJava = searchType;
			debug("Match:\t\t%s\\%s\n", keyName, buffer);
		} else {
			debug("Ignore:\t\t%s\\%s\n", keyName, buffer);
		}
		size = BIG_STR;
	}
}

void regSearchWow(const char* keyName, const int searchType) {
	HKEY hKey;
	debug("64-bit search:\t%s...\n", keyName);
	if (wow64 && RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			keyName,
			0,
            KEY_READ | KEY_WOW64_64KEY,
			&hKey) == ERROR_SUCCESS) {
		
		regSearch(hKey, keyName, searchType | KEY_WOW64_64KEY);
		RegCloseKey(hKey);
		if ((foundJava & KEY_WOW64_64KEY) != NO_JAVA_FOUND)
		{
			debug("Using 64-bit runtime.\n");
			return;
		}
	}
	debug("32-bit search:\t%s...\n", keyName);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			keyName,
			0,
            KEY_READ,
			&hKey) == ERROR_SUCCESS) {
		regSearch(hKey, keyName, searchType);
		RegCloseKey(hKey);
	}
}

void regSearchJreSdk(const char* jreKeyName, const char* sdkKeyName,
		const int jdkPreference) {
	if (jdkPreference == JDK_ONLY || jdkPreference == PREFER_JDK) {
		regSearchWow(sdkKeyName, FOUND_SDK);
		if (jdkPreference != JDK_ONLY) {
			regSearchWow(jreKeyName, FOUND_JRE);
		}
	} else { // jdkPreference == JRE_ONLY or PREFER_JRE
		regSearchWow(jreKeyName, FOUND_JRE);
		if (jdkPreference != JRE_ONLY) {
			regSearchWow(sdkKeyName, FOUND_SDK);
		}
	}
}

BOOL findJavaHome(char* path, const int jdkPreference) {
	regSearchJreSdk("SOFTWARE\\JavaSoft\\Java Runtime Environment",
					"SOFTWARE\\JavaSoft\\Java Development Kit",
					jdkPreference);
	if (foundJava == NO_JAVA_FOUND) {
		regSearchJreSdk("SOFTWARE\\IBM\\Java2 Runtime Environment",
						"SOFTWARE\\IBM\\Java Development Kit",
						jdkPreference);
	}
	if (foundJava != NO_JAVA_FOUND) {
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				foundJavaKey,
				0,
	            KEY_READ | (foundJava & KEY_WOW64_64KEY),
				&hKey) == ERROR_SUCCESS) {
			unsigned char buffer[BIG_STR] = {0};
			unsigned long bufferlength = BIG_STR;
			unsigned long datatype;
			if (RegQueryValueEx(hKey, "JavaHome", NULL, &datatype, buffer,
					&bufferlength) == ERROR_SUCCESS) {
				int i = 0;
				do {
					path[i] = buffer[i];
				} while (path[i++] != 0);
                // (foundJava & FOUND_SDK) {  // removed by fry
                //    appendPath(path, "jre");
                //
				RegCloseKey(hKey);
				return TRUE;
			}
			RegCloseKey(hKey);
		}
	}
	return FALSE;
}

/*
 * Extract the executable name, returns path length.
 */
int getExePath(char* exePath) {
    if (GetModuleFileName(hModule, exePath, _MAX_PATH) == 0) {
        return -1;
    }
	return strrchr(exePath, '\\') - exePath;
}

void appendPath(char* basepath, const char* path) {
	if (basepath[strlen(basepath) - 1] != '\\') {
		strcat(basepath, "\\");
	}
	strcat(basepath, path);
}

void appendJavaw(char* jrePath) {
    if (console) {
	    appendPath(jrePath, "bin\\java.exe");
    } else {
        appendPath(jrePath, "bin\\javaw.exe");
    }
}

void appendLauncher(const BOOL setProcName, char* exePath,
		const int pathLen, char* cmd) {
	if (setProcName) {
		char tmpspec[_MAX_PATH];
		char tmpfile[_MAX_PATH];
		strcpy(tmpspec, cmd);
		strcat(tmpspec, LAUNCH4J_TMP_DIR);
		tmpspec[strlen(tmpspec) - 1] = 0;
		if (_stat(tmpspec, &statBuf) == 0) {
			// Remove temp launchers and manifests
			struct _finddata_t c_file;
			long hFile;
			appendPath(tmpspec, "*.exe");
			strcpy(tmpfile, cmd);
			strcat(tmpfile, LAUNCH4J_TMP_DIR);
			char* filename = tmpfile + strlen(tmpfile);
			if ((hFile = _findfirst(tmpspec, &c_file)) != -1L) {
				do {
					strcpy(filename, c_file.name);
					debug("Unlink:\t\t%s\n", tmpfile);
					_unlink(tmpfile);
					strcat(tmpfile, MANIFEST);
					debug("Unlink:\t\t%s\n", tmpfile);
					_unlink(tmpfile);
				} while (_findnext(hFile, &c_file) == 0);
			}
			_findclose(hFile);
		} else {
			if (_mkdir(tmpspec) != 0) {
				debug("Mkdir failed:\t%s\n", tmpspec);
				appendJavaw(cmd);
				return;
			}
		}
		char javaw[_MAX_PATH];
		strcpy(javaw, cmd);
		appendJavaw(javaw);
		strcpy(tmpfile, cmd);
		strcat(tmpfile, LAUNCH4J_TMP_DIR);
		char* tmpfilename = tmpfile + strlen(tmpfile);
		char* exeFilePart = exePath + pathLen + 1;

		// Copy manifest
		char manifest[_MAX_PATH] = {0};
		strcpy(manifest, exePath);
		strcat(manifest, MANIFEST);
		if (_stat(manifest, &statBuf) == 0) {
			strcat(tmpfile, exeFilePart);
			strcat(tmpfile, MANIFEST);
			debug("Copy:\t\t%s -> %s\n", manifest, tmpfile);
			CopyFile(manifest, tmpfile, FALSE);
		}

		// Copy launcher
		strcpy(tmpfilename, exeFilePart);
		debug("Copy:\t\t%s -> %s\n", javaw, tmpfile);
		if (CopyFile(javaw, tmpfile, FALSE)) {
			strcpy(cmd, tmpfile);
			return;
		} else if (_stat(javaw, &statBuf) == 0) {
			long fs = statBuf.st_size;
			if (_stat(tmpfile, &statBuf) == 0 && fs == statBuf.st_size) {
				debug("Reusing:\t\t%s\n", tmpfile);
				strcpy(cmd, tmpfile);
				return;
			}
		}
	}
	appendJavaw(cmd);
}

void appendAppClasspath(char* dst, const char* src, const char* classpath) {
	strcat(dst, src);
	if (*classpath) {
		strcat(dst, ";");
	}
}

BOOL isJrePathOk(const char* path) {
	char javaw[_MAX_PATH];
	BOOL result = FALSE;
	if (*path) {
		strcpy(javaw, path);
		appendJavaw(javaw);
		result = _stat(javaw, &statBuf) == 0;
	}	
	debug("Check launcher:\t%s %s\n", javaw, result ? "(OK)" : "(n/a)");
	return result;
}

/* 
 * Expand environment %variables%
 */
BOOL expandVars(char *dst, const char *src, const char *exePath, const int pathLen) {
    char varName[STR];
    char varValue[MAX_VAR_SIZE];
    while (strlen(src) > 0) {
        char *start = strchr(src, '%');
        if (start != NULL) {
            char *end = strchr(start + 1, '%');
            if (end == NULL) {
                return FALSE;
            }
            // Copy content up to %VAR%
            strncat(dst, src, start - src);
            // Insert value of %VAR%
            *varName = 0;
            strncat(varName, start + 1, end - start - 1);
            // Remember value start for logging
            char *varValue = dst + strlen(dst);
            if (strcmp(varName, "EXEDIR") == 0) {
                strncat(dst, exePath, pathLen);
            } else if (strcmp(varName, "EXEFILE") == 0) {
                strcat(dst, exePath);
            } else if (strcmp(varName, "PWD") == 0) {
                GetCurrentDirectory(_MAX_PATH, dst + strlen(dst));
            } else if (strcmp(varName, "OLDPWD") == 0) {
                strcat(dst, oldPwd);
			} else if (strstr(varName, HKEY_STR) == varName) {
				regQueryValue(varName, dst + strlen(dst), BIG_STR);
            } else if (GetEnvironmentVariable(varName, varValue, MAX_VAR_SIZE) > 0) {
                strcat(dst, varValue);
            }
            debug("Substitute:\t%s = %s\n", varName, varValue);
            src = end + 1;
        } else {
            // Copy remaining content
            strcat(dst, src);
            break;
        }
	}
	return TRUE;
}

void appendHeapSizes(char *dst) {
	MEMORYSTATUS m;
	memset(&m, 0, sizeof(m));
	GlobalMemoryStatus(&m);

	appendHeapSize(dst, INITIAL_HEAP_SIZE, INITIAL_HEAP_PERCENT,
			m.dwAvailPhys, "-Xms");
	appendHeapSize(dst, MAX_HEAP_SIZE, MAX_HEAP_PERCENT,
			m.dwAvailPhys, "-Xmx");
}

void appendHeapSize(char *dst, const int absID, const int percentID,
		const DWORD freeMemory, const char *option) {
	
	const int mb = 1048576;		// 1 MB
	int abs = loadInt(absID);
	int percent = loadInt(percentID);
	int free = (long long) freeMemory * percent / (100 * mb);	// 100% * 1 MB
	int size = free > abs ? free : abs;
	if (size > 0) {
		debug("Heap %s:\t%d MB / %d%%, Free: %d MB, Heap size: %d MB\n",
				option, abs, percent, freeMemory / mb, size);
		strcat(dst, option);
		_itoa(size, dst + strlen(dst), 10);						// 10 -- radix
		strcat(dst, "m ");
	}
}

int prepare(const char *lpCmdLine) {
	char tmp[MAX_ARGS] = {0};
	hModule = GetModuleHandle(NULL);
	if (hModule == NULL) {
		return FALSE;
	}

	// Get executable path
	char exePath[_MAX_PATH] = {0};
	int pathLen = getExePath(exePath);
	if (pathLen == -1) {
		return FALSE;
	}

	// Initialize logging 
    if (strstr(lpCmdLine, "--l4j-debug") != NULL) {
		hLog = openLogFile(exePath, pathLen);
		if (hLog == NULL) {
			return FALSE;
		}
		debug("\n\nCmdLine:\t%s %s\n", exePath, lpCmdLine);
	}

    setWow64Flag();

	// Set default error message, title and optional support web site url.
	loadString(SUPPORT_URL, errUrl);
	loadString(ERR_TITLE, errTitle);
	if (!loadString(STARTUP_ERR, errMsg)) {
		return FALSE;			
	}

	// Single instance
	loadString(MUTEX_NAME, mutexName);
	if (*mutexName) {
		SECURITY_ATTRIBUTES security;
		security.nLength = sizeof(SECURITY_ATTRIBUTES);
		security.bInheritHandle = TRUE;
		security.lpSecurityDescriptor = NULL;
		CreateMutexA(&security, FALSE, mutexName);
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			debug("Instance already exists.");
			return ERROR_ALREADY_EXISTS;
		}
	}
	
	// Working dir
	char tmp_path[_MAX_PATH] = {0};
	GetCurrentDirectory(_MAX_PATH, oldPwd);
	if (loadString(CHDIR, tmp_path)) {
		strncpy(workingDir, exePath, pathLen);
		appendPath(workingDir, tmp_path);
		_chdir(workingDir);
		debug("Working dir:\t%s\n", workingDir);
	}

	// Use bundled jre or find java
	if (loadString(JRE_PATH, tmp_path)) {
		char jrePath[MAX_ARGS] = {0};
		expandVars(jrePath, tmp_path, exePath, pathLen);
		debug("Bundled JRE:\t%s\n", jrePath);
		if (jrePath[0] == '\\' || jrePath[1] == ':') {
			// Absolute
			strcpy(cmd, jrePath);
		} else {
			// Relative
			strncpy(cmd, exePath, pathLen);
			appendPath(cmd, jrePath);
		}
    }
	if (!isJrePathOk(cmd)) {
		if (!loadString(JAVA_MIN_VER, javaMinVer)) {
			loadString(BUNDLED_JRE_ERR, errMsg);
			return FALSE;
		}
		loadString(JAVA_MAX_VER, javaMaxVer);
		if (!findJavaHome(cmd, loadInt(JDK_PREFERENCE))) {
			loadString(JRE_VERSION_ERR, errMsg);
			strcat(errMsg, " ");
			strcat(errMsg, javaMinVer);
			if (*javaMaxVer) {
				strcat(errMsg, " - ");
				strcat(errMsg, javaMaxVer);
			}
			loadString(DOWNLOAD_URL, errUrl);
			return FALSE;
		}
		if (!isJrePathOk(cmd)) {
			loadString(LAUNCHER_ERR, errMsg);
			return FALSE;
		}
	}

    // Append a path to the Path environment variable
	char jreBinPath[_MAX_PATH];
	strcpy(jreBinPath, cmd);
	strcat(jreBinPath, "\\bin");
	if (!appendToPathVar(jreBinPath)) {
		return FALSE;
	}

	// Set environment variables
	char envVars[MAX_VAR_SIZE] = {0};
	loadString(ENV_VARIABLES, envVars);
	char *var = strtok(envVars, "\t");
	while (var != NULL) {
		char *varValue = strchr(var, '=');
		*varValue++ = 0;
		*tmp = 0;
		expandVars(tmp, varValue, exePath, pathLen);
		debug("Set var:\t%s = %s\n", var, tmp);
		SetEnvironmentVariable(var, tmp);
		var = strtok(NULL, "\t"); 
	}
	*tmp = 0;

	// Process priority
	priority = loadInt(PRIORITY_CLASS);

	// Custom process name
	const BOOL setProcName = loadBool(SET_PROC_NAME)
			&& strstr(lpCmdLine, "--l4j-default-proc") == NULL;
	const BOOL wrapper = loadBool(WRAPPER);

    char jdk_path[_MAX_PATH] = {0};  // fry
    strcpy(jdk_path, cmd);
    //msgBox(jdk_path);

	appendLauncher(setProcName, exePath, pathLen, cmd);

	// Heap sizes
	appendHeapSizes(args);
	
    // JVM options
	if (loadString(JVM_OPTIONS, tmp)) {
		strcat(tmp, " ");
	} else {
        *tmp = 0;
    }
	/*
	 * Load additional JVM options from .l4j.ini file
	 * Options are separated by spaces or CRLF
	 * # starts an inline comment
	 */
	strncpy(tmp_path, exePath, strlen(exePath) - 3);
	strcat(tmp_path, "l4j.ini");
	long hFile;
	if ((hFile = _open(tmp_path, _O_RDONLY)) != -1) {
		const int jvmOptLen = strlen(tmp);
		char* src = tmp + jvmOptLen;
		char* dst = src;
		const int len = _read(hFile, src, MAX_ARGS - jvmOptLen - BIG_STR);
		BOOL copy = TRUE;
		int i;
		for (i = 0; i < len; i++, src++) {
			if (*src == '#') {
				copy = FALSE;
			} else if (*src == 13 || *src == 10) {
				copy = TRUE;
				if (dst > tmp && *(dst - 1) != ' ') {
					*dst++ = ' ';
				}
			} else if (copy) {
				*dst++ = *src;
			}
		}
		*dst = 0;
		if (len > 0 && *(dst - 1) != ' ') {
			strcat(tmp, " ");
		}
		_close(hFile);
	}

    // Expand environment %variables%
	expandVars(args, tmp, exePath, pathLen);

	// MainClass + Classpath or Jar
	char mainClass[STR] = {0};
	char jar[_MAX_PATH] = {0};
	loadString(JAR, jar);
	if (loadString(MAIN_CLASS, mainClass)) {
		if (!loadString(CLASSPATH, tmp)) {
			return FALSE;
		}
		char exp[MAX_ARGS] = {0};
		expandVars(exp, tmp, exePath, pathLen);
		strcat(args, "-classpath \"");
		if (wrapper) {
			appendAppClasspath(args, exePath, exp);
		} else if (*jar) {
			appendAppClasspath(args, jar, exp);
		}

	// add tools.jar for JDK  [fry]
	char tools[_MAX_PATH] = { 0 };
	sprintf(tools, "%s\\lib\\tools.jar", jdk_path);
	appendAppClasspath(args, tools, exp);

		// Deal with wildcards or >> strcat(args, exp); <<
		char* cp = strtok(exp, ";");
		while(cp != NULL) {
			debug("Add classpath:\t%s\n", cp);
			if (strpbrk(cp, "*?") != NULL) {
				int len = strrchr(cp, '\\') - cp + 1;
				strncpy(tmp_path, cp, len);
				char* filename = tmp_path + len;
				*filename = 0;
				struct _finddata_t c_file;
				long hFile;
				if ((hFile = _findfirst(cp, &c_file)) != -1L) {
					do {
						strcpy(filename, c_file.name);
						strcat(args, tmp_path);
						strcat(args, ";");
						debug("      \"      :\t%s\n", tmp_path);
					} while (_findnext(hFile, &c_file) == 0);
				}
				_findclose(hFile);
			} else {
				strcat(args, cp);
				strcat(args, ";");
			}
			cp = strtok(NULL, ";");
		} 
		*(args + strlen(args) - 1) = 0;

		strcat(args, "\" ");
		strcat(args, mainClass);
	} else if (wrapper) {
       	strcat(args, "-jar \"");
		strcat(args, exePath);
   		strcat(args, "\"");
    } else {
       	strcat(args, "-jar \"");
        strncat(args, exePath, pathLen);
        appendPath(args, jar);
       	strcat(args, "\"");
    }

	// Constant command line args
	if (loadString(CMD_LINE, tmp)) {
		strcat(args, " ");
		strcat(args, tmp);
	}

	// Command line args
	if (*lpCmdLine) {
		strcpy(tmp, lpCmdLine);
		char* dst;
		while ((dst = strstr(tmp, "--l4j-")) != NULL) {
			char* src = strchr(dst, ' ');
			if (src == NULL || *(src + 1) == 0) {
				*dst = 0;
			} else {
				strcpy(dst, src + 1);
			}
		}
		if (*tmp) {
			strcat(args, " ");
			strcat(args, tmp);
		}
	}

	debug("Launcher:\t%s\n", cmd);
	debug("Launcher args:\t%s\n", args);
	debug("Args length:\t%d/32768 chars\n", strlen(args));
	return TRUE;
}

void closeHandles() {
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	closeLogFile();
}

/*
 * Append a path to the Path environment variable
 */
BOOL appendToPathVar(const char* path) {
	char chBuf[MAX_VAR_SIZE] = {0};

	const int pathSize = GetEnvironmentVariable("Path", chBuf, MAX_VAR_SIZE);
	if (MAX_VAR_SIZE - pathSize - 1 < strlen(path)) {
		return FALSE;
	}
	strcat(chBuf, ";");
	strcat(chBuf, path);
	return SetEnvironmentVariable("Path", chBuf);
}

// may need to ignore STILL_ACTIVE (error code 259) here
// http://msdn.microsoft.com/en-us/library/ms683189(VS.85).aspx
DWORD execute(const BOOL wait) {
	STARTUPINFO si;
    memset(&pi, 0, sizeof(pi));
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

	DWORD dwExitCode = -1;
	char cmdline[MAX_ARGS];
    strcpy(cmdline, "\"");
	strcat(cmdline, cmd);
	strcat(cmdline, "\" ");
	strcat(cmdline, args);
	if (CreateProcess(NULL, cmdline, NULL, NULL,
			TRUE, priority, NULL, NULL, &si, &pi)) {
		if (wait) {
			WaitForSingleObject(pi.hProcess, INFINITE);
			GetExitCodeProcess(pi.hProcess, &dwExitCode);
			debug("Exit code:\t%d\n", dwExitCode);
			closeHandles();
		} else {
			dwExitCode = 0;
		}
	}
	return dwExitCode;
}
