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
 * Created on May 24, 2005
 */
package net.sf.launch4j.ant;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import org.apache.tools.ant.BuildException;

import net.sf.launch4j.config.Config;
import net.sf.launch4j.config.Msg;
import net.sf.launch4j.config.SingleInstance;
import net.sf.launch4j.config.Splash;
import net.sf.launch4j.config.VersionInfo;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class AntConfig extends Config {
	private final List wrappedHeaderObjects = new ArrayList();
	private final List wrappedLibs = new ArrayList();
	private final List wrappedVariables = new ArrayList();

	public void setJarPath(String path) {
		setJar(new File(path));
	}

	public void addObj(StringWrapper obj) {
		wrappedHeaderObjects.add(obj);
	}

	public void addLib(StringWrapper lib) {
		wrappedLibs.add(lib);
	}
	
	public void addVar(StringWrapper var) {
		wrappedVariables.add(var);
	}

	// __________________________________________________________________________________

	public void addSingleInstance(SingleInstance singleInstance) {
		checkNull(getSingleInstance(), "singleInstance");
		setSingleInstance(singleInstance);
	}

	public void addClassPath(AntClassPath classPath) {
		checkNull(getClassPath(), "classPath");
		setClassPath(classPath);
	}

	public void addJre(AntJre jre) {
		checkNull(getJre(), "jre");
		setJre(jre);
	}

	public void addSplash(Splash splash) {
		checkNull(getSplash(), "splash");
		setSplash(splash);
	}

	public void addVersionInfo(VersionInfo versionInfo) {
		checkNull(getVersionInfo(), "versionInfo");
		setVersionInfo(versionInfo);
	}
	
	public void addMessages(Msg messages) {
		checkNull(getMessages(), "messages");
		setMessages(messages);
	}

	// __________________________________________________________________________________

	public void unwrap() {
		setHeaderObjects(StringWrapper.unwrap(wrappedHeaderObjects));
		setLibs(StringWrapper.unwrap(wrappedLibs));
		setVariables(StringWrapper.unwrap(wrappedVariables));
		if (getClassPath() != null) {
			((AntClassPath) getClassPath()).unwrap();
		}
		if (getJre() != null) {
			((AntJre) getJre()).unwrap();
		}
	}

	private void checkNull(Object o, String name) {
		if (o != null) {
			throw new BuildException(
					Messages.getString("AntConfig.duplicate.element")
					+ ": "
					+ name);
		}
	}
}
