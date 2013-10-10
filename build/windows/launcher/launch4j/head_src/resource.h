/*
	Launch4j (http://launch4j.sourceforge.net/)
	Cross-platform Java application wrapper for creating Windows native executables.

	Copyright (c) 2004, 2008 Grzegorz Kowal
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

// ICON
#define APP_ICON 					1

// BITMAP
#define SPLASH_BITMAP				1

// RCDATA
#define JRE_PATH					1
#define JAVA_MIN_VER				2
#define JAVA_MAX_VER				3
#define SHOW_SPLASH					4
#define SPLASH_WAITS_FOR_WINDOW		5
#define SPLASH_TIMEOUT				6 
#define SPLASH_TIMEOUT_ERR			7
#define CHDIR						8
#define SET_PROC_NAME				9
#define ERR_TITLE					10
#define GUI_HEADER_STAYS_ALIVE		11
#define JVM_OPTIONS					12
#define CMD_LINE					13
#define JAR							14
#define MAIN_CLASS					15
#define CLASSPATH					16
#define WRAPPER						17
#define JDK_PREFERENCE				18
#define ENV_VARIABLES				19
#define PRIORITY_CLASS				20
#define DOWNLOAD_URL				21
#define SUPPORT_URL					22
#define MUTEX_NAME					23
#define INSTANCE_WINDOW_TITLE		24
#define INITIAL_HEAP_SIZE			25
#define INITIAL_HEAP_PERCENT		26
#define MAX_HEAP_SIZE				27
#define MAX_HEAP_PERCENT			28

#define STARTUP_ERR					101
#define BUNDLED_JRE_ERR				102
#define JRE_VERSION_ERR				103
#define LAUNCHER_ERR				104
#define INSTANCE_ALREADY_EXISTS_MSG	105
