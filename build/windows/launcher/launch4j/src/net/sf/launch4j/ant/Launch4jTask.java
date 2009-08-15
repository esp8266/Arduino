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

import net.sf.launch4j.Builder;
import net.sf.launch4j.BuilderException;
import net.sf.launch4j.Log;
import net.sf.launch4j.config.Config;
import net.sf.launch4j.config.ConfigPersister;
import net.sf.launch4j.config.ConfigPersisterException;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class Launch4jTask extends Task {
	private File _configFile;
	private AntConfig _config;

	// System properties
	private File tmpdir;		// launch4j.tmpdir
	private File bindir;		// launch4j.bindir

	// Override configFile settings
	private File jar;
	private File outfile;
	private String fileVersion;
	private String txtFileVersion;
	private String productVersion;
	private String txtProductVersion;

	public void execute() throws BuildException {
		try {
			if (tmpdir != null) {
				System.setProperty("launch4j.tmpdir", tmpdir.getPath());
			}
			if (bindir != null) {
				System.setProperty("launch4j.bindir", bindir.getPath());
			}
			if (_configFile != null && _config != null) {
				throw new BuildException(
						Messages.getString("Launch4jTask.specify.config"));
			} else if (_configFile != null) {
				ConfigPersister.getInstance().load(_configFile);
				Config c = ConfigPersister.getInstance().getConfig();
				if (jar != null) {
					c.setJar(jar);
				}
				if (outfile != null) {
					c.setOutfile(outfile);
				}
				if (fileVersion != null) {
					c.getVersionInfo().setFileVersion(fileVersion);
				}
				if (txtFileVersion != null) {
					c.getVersionInfo().setTxtFileVersion(txtFileVersion);
				}
				if (productVersion != null) {
					c.getVersionInfo().setProductVersion(productVersion);
				}
				if (txtProductVersion != null) {
					c.getVersionInfo().setTxtProductVersion(txtProductVersion);
				}
			} else if (_config != null) {
				_config.unwrap();
				ConfigPersister.getInstance().setAntConfig(_config,
						getProject().getBaseDir());
			} else {
				throw new BuildException(
						Messages.getString("Launch4jTask.specify.config"));
			}
			final Builder b = new Builder(Log.getAntLog());
			b.build();
		} catch (ConfigPersisterException e) {
			throw new BuildException(e);
		} catch (BuilderException e) {
			throw new BuildException(e);
		}
	}

	public void setConfigFile(File configFile) {
		_configFile = configFile;
	}

	public void addConfig(AntConfig config) {
		_config = config;
	}

	public void setBindir(File bindir) {
		this.bindir = bindir;
	}

	public void setTmpdir(File tmpdir) {
		this.tmpdir = tmpdir;
	}

	public void setFileVersion(String fileVersion) {
		this.fileVersion = fileVersion;
	}

	public void setJar(File jar) {
		this.jar = jar;
	}

	public void setJarPath(String path) {
		this.jar = new File(path);
	}

	public void setOutfile(File outfile) {
		this.outfile = outfile;
	}

	public void setProductVersion(String productVersion) {
		this.productVersion = productVersion;
	}

	public void setTxtFileVersion(String txtFileVersion) {
		this.txtFileVersion = txtFileVersion;
	}

	public void setTxtProductVersion(String txtProductVersion) {
		this.txtProductVersion = txtProductVersion;
	}
}
