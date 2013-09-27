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
 * Created on Oct 8, 2006
 */
package net.sf.launch4j.config;

import net.sf.launch4j.binding.IValidatable;
import net.sf.launch4j.binding.Validator;

/**
 * @author Copyright (C) 2006 Grzegorz Kowal
 */
public class Msg implements IValidatable {
	private String startupErr;
	private String bundledJreErr;
	private String jreVersionErr;
	private String launcherErr;
	private String instanceAlreadyExistsMsg;

	public void checkInvariants() {
		Validator.checkOptString(startupErr, 1024, "startupErr",
				Messages.getString("Msg.startupErr"));
		Validator.checkOptString(bundledJreErr, 1024, "bundledJreErr",
				Messages.getString("Msg.bundledJreErr"));
		Validator.checkOptString(jreVersionErr, 1024, "jreVersionErr",
				Messages.getString("Msg.jreVersionErr"));
		Validator.checkOptString(launcherErr, 1024, "launcherErr",
				Messages.getString("Msg.launcherErr"));
		Validator.checkOptString(instanceAlreadyExistsMsg, 1024, "instanceAlreadyExistsMsg",
				Messages.getString("Msg.instanceAlreadyExistsMsg"));
	}

	public String getStartupErr() {
		return !Validator.isEmpty(startupErr) ? startupErr
				: "An error occurred while starting the application.";
	}
	
	public void setStartupErr(String startupErr) {
		this.startupErr = startupErr;
	}

	public String getBundledJreErr() {
		return !Validator.isEmpty(bundledJreErr) ? bundledJreErr
				: "This application was configured to use a bundled Java Runtime" +
						" Environment but the runtime is missing or corrupted.";
	}

	public void setBundledJreErr(String bundledJreErr) {
		this.bundledJreErr = bundledJreErr;
	}

	public String getJreVersionErr() {
		return !Validator.isEmpty(jreVersionErr) ? jreVersionErr
				: "This application requires a Java Runtime Environment";
	}

	public void setJreVersionErr(String jreVersionErr) {
		this.jreVersionErr = jreVersionErr;
	}
	
	public String getLauncherErr() {
		return !Validator.isEmpty(launcherErr) ? launcherErr
				: "The registry refers to a nonexistent Java Runtime Environment" +
						" installation or the runtime is corrupted.";
	}
	
	public void setLauncherErr(String launcherErr) {
		this.launcherErr = launcherErr;
	}

	public String getInstanceAlreadyExistsMsg() {
    	return !Validator.isEmpty(instanceAlreadyExistsMsg) ? instanceAlreadyExistsMsg
    			: "An application instance is already running.";
    }

	public void setInstanceAlreadyExistsMsg(String instanceAlreadyExistsMsg) {
    	this.instanceAlreadyExistsMsg = instanceAlreadyExistsMsg;
    }
}
