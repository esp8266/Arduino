/*
	Launch4j (http://launch4j.sourceforge.net/)
	Cross-platform Java application wrapper for creating Windows native executables.

	Copyright (c) 2004, 2007 Grzegorz Kowal

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

#include "../resource.h"
#include "../head.h"

int main(int argc, char* argv[])
{
    setConsoleFlag();
	LPTSTR cmdLine = GetCommandLine();
	if (*cmdLine == '"') {
		if (*(cmdLine = strchr(cmdLine + 1, '"') + 1)) {
			cmdLine++;
		}
	} else if ((cmdLine = strchr(cmdLine, ' ')) != NULL) {
		cmdLine++;
	} else {
		cmdLine = "";
	}
	int result = prepare(cmdLine);
	if (result == ERROR_ALREADY_EXISTS) {
		char errMsg[BIG_STR] = {0};
		loadString(INSTANCE_ALREADY_EXISTS_MSG, errMsg);
		msgBox(errMsg);
		closeLogFile();
		return 2;
	}
	if (result != TRUE) {
		signalError();
		return 1;
	}

	result = (int) execute(TRUE);
	if (result == -1) {
		signalError();
	} else {
		return result;
	}
}
