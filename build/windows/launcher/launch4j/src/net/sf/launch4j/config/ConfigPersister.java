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
 * Created on Apr 22, 2005
 */
package net.sf.launch4j.config;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

import net.sf.launch4j.Util;

import com.thoughtworks.xstream.XStream;
import com.thoughtworks.xstream.io.xml.DomDriver;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class ConfigPersister {

	private static final ConfigPersister _instance = new ConfigPersister();

	private final XStream _xstream;
	private Config _config;
	private File _configPath;

	private ConfigPersister() {
		_xstream = new XStream(new DomDriver());
    	_xstream.alias("launch4jConfig", Config.class);
    	_xstream.alias("classPath", ClassPath.class);
    	_xstream.alias("jre", Jre.class);
    	_xstream.alias("splash", Splash.class);
    	_xstream.alias("versionInfo", VersionInfo.class);

    	_xstream.addImplicitCollection(Config.class, "headerObjects", "obj",
    			String.class);
    	_xstream.addImplicitCollection(Config.class, "libs", "lib", String.class);
    	_xstream.addImplicitCollection(Config.class, "variables", "var", String.class);
    	_xstream.addImplicitCollection(ClassPath.class, "paths", "cp", String.class);
    	_xstream.addImplicitCollection(Jre.class, "options", "opt", String.class);
	}

	public static ConfigPersister getInstance() {
		return _instance;
	}
	
	public Config getConfig() {
		return _config;
	}

	public File getConfigPath() {
		return _configPath;
	}
	
	public File getOutputPath() throws IOException {
		if (_config.getOutfile().isAbsolute()) {
			return _config.getOutfile().getParentFile();
		}
		File parent = _config.getOutfile().getParentFile();
		return (parent != null) ? new File(_configPath, parent.getPath()) : _configPath;
	}
	
	public File getOutputFile() throws IOException {
		return _config.getOutfile().isAbsolute()
			? _config.getOutfile()
			: new File(getOutputPath(), _config.getOutfile().getName());
	}

	public void createBlank() {
		_config = new Config();
		_config.setJre(new Jre());
		_configPath = null;
	}

	public void setAntConfig(Config c, File basedir) {
		_config = c;
		_configPath = basedir;
	}

	public void load(File f) throws ConfigPersisterException {
		try {
			FileReader r = new FileReader(f);
			char[] buf = new char[(int) f.length()];
			r.read(buf);
	    	r.close();
	    	// Convert 2.x config to 3.x
	    	String s = String.valueOf(buf)
	    			.replaceAll("<headerType>0<", "<headerType>gui<")
	    			.replaceAll("<headerType>1<", "<headerType>console<")
	    			.replaceAll("jarArgs>", "cmdLine>")
	    			.replaceAll("<jarArgs[ ]*/>", "<cmdLine/>")
	    			.replaceAll("args>", "opt>")
	    			.replaceAll("<args[ ]*/>", "<opt/>")
	    			.replaceAll("<dontUsePrivateJres>false</dontUsePrivateJres>",
	    					"<jdkPreference>" + Jre.JDK_PREFERENCE_PREFER_JRE + "</jdkPreference>")
	    			.replaceAll("<dontUsePrivateJres>true</dontUsePrivateJres>",
	    					"<jdkPreference>" + Jre.JDK_PREFERENCE_JRE_ONLY + "</jdkPreference>")
	    			.replaceAll("<initialHeapSize>0</initialHeapSize>", "")
	    			.replaceAll("<maxHeapSize>0</maxHeapSize>", "");
	    	_config = (Config) _xstream.fromXML(s);
	    	setConfigPath(f);
		} catch (Exception e) {
			throw new ConfigPersisterException(e);
		}
	}

	/**
	 * Imports launch4j 1.x.x config file.
	 */
	public void loadVersion1(File f) throws ConfigPersisterException {
		try {
			Props props = new Props(f);
			_config = new Config();
			String header = props.getProperty(Config.HEADER);
			_config.setHeaderType(header == null
					|| header.toLowerCase().equals("guihead.bin") ? Config.GUI_HEADER
															: Config.CONSOLE_HEADER);
			_config.setJar(props.getFile(Config.JAR));
			_config.setOutfile(props.getFile(Config.OUTFILE));
			_config.setJre(new Jre());
			_config.getJre().setPath(props.getProperty(Jre.PATH));
			_config.getJre().setMinVersion(props.getProperty(Jre.MIN_VERSION));
			_config.getJre().setMaxVersion(props.getProperty(Jre.MAX_VERSION));
			String args = props.getProperty(Jre.ARGS);
			if (args != null) {
				List jreOptions = new ArrayList();
				jreOptions.add(args);
				_config.getJre().setOptions(jreOptions);
			}
			_config.setCmdLine(props.getProperty(Config.JAR_ARGS));
			_config.setChdir("true".equals(props.getProperty(Config.CHDIR))
					? "." : null);
			_config.setCustomProcName("true".equals(
					props.getProperty("setProcName")));				// 1.x
			_config.setStayAlive("true".equals(props.getProperty(Config.STAY_ALIVE)));
			_config.setErrTitle(props.getProperty(Config.ERR_TITLE));
			_config.setIcon(props.getFile(Config.ICON));
			File splashFile = props.getFile(Splash.SPLASH_FILE);
			if (splashFile != null) {
				_config.setSplash(new Splash());
				_config.getSplash().setFile(splashFile);
				String waitfor = props.getProperty("waitfor");		// 1.x
				_config.getSplash().setWaitForWindow(waitfor != null
													&& !waitfor.equals(""));
				String splashTimeout = props.getProperty(Splash.TIMEOUT);
				if (splashTimeout != null) {
					_config.getSplash().setTimeout(Integer.parseInt(splashTimeout));	
				}
				_config.getSplash().setTimeoutErr("true".equals(
						props.getProperty(Splash.TIMEOUT_ERR)));
			} else {
				_config.setSplash(null);
			}
			setConfigPath(f);
		} catch (IOException e) {
			throw new ConfigPersisterException(e);
		}
	}

	public void save(File f) throws ConfigPersisterException {
		try {
			BufferedWriter w = new BufferedWriter(new FileWriter(f));
	    	_xstream.toXML(_config, w);
	    	w.close();
	    	setConfigPath(f);
		} catch (Exception e) {
			throw new ConfigPersisterException(e);
		}
	}

	private void setConfigPath(File configFile) {
		_configPath = configFile.getAbsoluteFile().getParentFile();
	}

	private class Props {
		final Properties _properties = new Properties();

		public Props(File f) throws IOException {
			FileInputStream is = null;
			try {
				is = new FileInputStream(f);
				_properties.load(is);
			} finally {
				Util.close(is);
			}
		}

		/**
		 * Get property and remove trailing # comments.
		 */
		public String getProperty(String key) {
			String p = _properties.getProperty(key);
			if (p == null) {
				return null;
			}
			int x = p.indexOf('#');
			if (x == -1) {
				return p;
			}
			do {
				x--;
			} while (x > 0 && (p.charAt(x) == ' ' || p.charAt(x) == '\t'));
			return (x == 0) ? "" : p.substring(0, x + 1);
		}

		public File getFile(String key) {
			String value = getProperty(key);
			return value != null ? new File(value) : null;
		}
	}
}
