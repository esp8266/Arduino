/*
	Launch4j (http://launch4j.sourceforge.net/)
	Cross-platform Java application wrapper for creating Windows native executables.

	Copyright (c) 2004, 2007 Grzegorz Kowal
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

#include "../resource.h"
#include "../head.h"
#include "guihead.h"

extern FILE* hLog;
extern PROCESS_INFORMATION pi;

HWND hWnd;
DWORD dwExitCode = 0;
BOOL stayAlive = FALSE;
BOOL splash = FALSE;
BOOL splashTimeoutErr;
BOOL waitForWindow;
int splashTimeout = DEFAULT_SPLASH_TIMEOUT;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow) {
	int result = prepare(lpCmdLine);
	if (result == ERROR_ALREADY_EXISTS) {
		HWND handle = getInstanceWindow();
		ShowWindow(handle, SW_SHOW);
		SetForegroundWindow(handle);
		closeLogFile();
		return 2;
	}
	if (result != TRUE) {
		signalError();
		return 1;
	}

	splash = loadBool(SHOW_SPLASH)
			&& strstr(lpCmdLine, "--l4j-no-splash") == NULL;
	stayAlive = loadBool(GUI_HEADER_STAYS_ALIVE)
			&& strstr(lpCmdLine, "--l4j-dont-wait") == NULL;
	if (splash || stayAlive) {
		hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, "STATIC", "",
				WS_POPUP | SS_BITMAP,
				0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
		if (splash) {
			char timeout[10] = {0};
			if (loadString(SPLASH_TIMEOUT, timeout)) {
				splashTimeout = atoi(timeout);
				if (splashTimeout <= 0 || splashTimeout > MAX_SPLASH_TIMEOUT) {
					splashTimeout = DEFAULT_SPLASH_TIMEOUT;
				}
			}
			splashTimeoutErr = loadBool(SPLASH_TIMEOUT_ERR)
					&& strstr(lpCmdLine, "--l4j-no-splash-err") == NULL;
			waitForWindow = loadBool(SPLASH_WAITS_FOR_WINDOW);
			HANDLE hImage = LoadImage(hInstance,	// handle of the instance containing the image
					MAKEINTRESOURCE(SPLASH_BITMAP),	// name or identifier of image
					IMAGE_BITMAP,					// type of image
					0,								// desired width
					0,								// desired height
					LR_DEFAULTSIZE);
			if (hImage == NULL) {
				signalError();
				return 1;
			}
			SendMessage(hWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hImage);
			RECT rect;
			GetWindowRect(hWnd, &rect);
			int x = (GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
			int y = (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;
			SetWindowPos(hWnd, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
			ShowWindow(hWnd, nCmdShow);
			UpdateWindow (hWnd);
		}
		if (!SetTimer (hWnd, ID_TIMER, 1000 /* 1s */, TimerProc)) {
			signalError();
			return 1;
		}
	}
	if (execute(FALSE) == -1) {
		signalError();
		return 1;
	}
	if (!(splash || stayAlive)) {
		debug("Exit code:\t0\n");
		closeHandles();
		return 0;
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	debug("Exit code:\t%d\n", dwExitCode);
	closeHandles();
	return dwExitCode;
}

HWND getInstanceWindow() {
	char windowTitle[STR];
	char instWindowTitle[STR] = {0};
	if (loadString(INSTANCE_WINDOW_TITLE, instWindowTitle)) {
		HWND handle = FindWindowEx(NULL, NULL, NULL, NULL); 
		while (handle != NULL) {
			GetWindowText(handle, windowTitle, STR - 1);
			if (strstr(windowTitle, instWindowTitle) != NULL) {
				return handle;
			} else {
				handle = FindWindowEx(NULL, handle, NULL, NULL);
			}
		}
	}
	return NULL;   
}

BOOL CALLBACK enumwndfn(HWND hwnd, LPARAM lParam) {
	DWORD processId;
	GetWindowThreadProcessId(hwnd, &processId);
	if (pi.dwProcessId == processId) {
		LONG styles = GetWindowLong(hwnd, GWL_STYLE);
		if ((styles & WS_VISIBLE) != 0) {
			splash = FALSE;
			ShowWindow(hWnd, SW_HIDE);
			return FALSE;
		}
	}
	return TRUE;
}

VOID CALLBACK TimerProc(
	HWND hwnd,			// handle of window for timer messages
	UINT uMsg,			// WM_TIMER message
	UINT idEvent,		// timer identifier
	DWORD dwTime) {		// current system time
	
	if (splash) {
		if (splashTimeout == 0) {
			splash = FALSE;
			ShowWindow(hWnd, SW_HIDE);
			if (waitForWindow && splashTimeoutErr) {
				KillTimer(hwnd, ID_TIMER);
				signalError();
				PostQuitMessage(0);
			}
		} else {
			splashTimeout--;
			if (waitForWindow) {
				EnumWindows(enumwndfn, 0);
			}
		}
	}
	GetExitCodeProcess(pi.hProcess, &dwExitCode);
	if (dwExitCode != STILL_ACTIVE
			|| !(splash || stayAlive)) {
		KillTimer(hWnd, ID_TIMER);
		PostQuitMessage(0);
	}
}
