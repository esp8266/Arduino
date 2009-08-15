/*
	Launch4j (http://launch4j.sourceforge.net/)
	Cross-platform Java application wrapper for creating Windows native executables.

	Copyright (c) 2004, 2007 Grzegorz Kowal

	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	    * Redistributions of source code must retain the above copyright notice,
	      this list of conditions and the following disclaimer.
	    * Redistributions in binary form must reproduce the above copyright notice,
	      this list of conditions and the following disclaimer in the documentation
	      and/or other materials provided with the distribution.
	    * Neither the name of the Launch4j nor the names of its contributors
	      may be used to endorse or promote products derived from this software without
	      specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
	PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * Created on Apr 21, 2005
 */
package net.sf.launch4j.config;

import java.io.File;

import net.sf.launch4j.binding.IValidatable;
import net.sf.launch4j.binding.Validator;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class Splash implements IValidatable {
	
	// 1.x config properties_____________________________________________________________
	public static final String SPLASH_FILE = "splash";
	public static final String WAIT_FOR_TITLE = "waitForTitle";
	public static final String TIMEOUT = "splashTimeout";
	public static final String TIMEOUT_ERR = "splashTimeoutErr";

	// __________________________________________________________________________________
	private File file;
	private boolean waitForWindow = true;
	private int timeout = 60;
	private boolean timeoutErr = true;

	public void checkInvariants() {
		Validator.checkFile(file, "splash.file",
				Messages.getString("Splash.splash.file"));
		Validator.checkRange(timeout, 1, 60 * 15, "splash.timeout",
				Messages.getString("Splash.splash.timeout"));
	}

	/** Splash screen in BMP format. */
	public File getFile() {
		return file;
	}

	public void setFile(File file) {
		this.file = file;
	}

	/** Splash timeout in seconds. */
	public int getTimeout() {
		return timeout;
	}

	public void setTimeout(int timeout) {
		this.timeout = timeout;
	}

	/** Signal error on splash timeout. */
	public boolean isTimeoutErr() {
		return timeoutErr;
	}

	public void setTimeoutErr(boolean timeoutErr) {
		this.timeoutErr = timeoutErr;
	}

	/** Hide splash screen when the child process displayes the first window. */
	public boolean getWaitForWindow() {
		return waitForWindow;
	}

	public void setWaitForWindow(boolean waitForWindow) {
		this.waitForWindow = waitForWindow;
	}
}
