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
 * Created on 2005-04-24
 */
package net.sf.launch4j;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.Reader;
import java.io.Writer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class Util {
	public static final boolean WINDOWS_OS = System.getProperty("os.name")
			.toLowerCase().startsWith("windows");

	private Util() {}

	public static File createTempFile(String suffix) throws IOException {
		String tmpdir = System.getProperty("launch4j.tmpdir");
		if (tmpdir != null) {
			if (tmpdir.indexOf(' ') != -1) {
				throw new IOException(Messages.getString("Util.tmpdir"));
			}
			return File.createTempFile("launch4j", suffix, new File(tmpdir));
		} else {
			return File.createTempFile("launch4j", suffix);
		}
	}

	/**
	 * Returns the base directory of a jar file or null if the class is a standalone file. 
	 * @return System specific path
	 * 
	 * Based on a patch submitted by Josh Elsasser
	 */
	public static File getJarBasedir() {
		String url = Util.class.getClassLoader()
				.getResource(Util.class.getName().replace('.', '/') + ".class")
				.getFile()
				.replaceAll("%20", " ");
		if (url.startsWith("file:")) {
			String jar = url.substring(5, url.lastIndexOf('!'));
			int x = jar.lastIndexOf('/');
			if (x == -1) {
				x = jar.lastIndexOf('\\');	
			}
			String basedir = jar.substring(0, x + 1);
			return new File(basedir);
		} else {
			return new File(".");
		}
	}

	public static File getAbsoluteFile(File basepath, File f) {
		return f.isAbsolute() ? f : new File(basepath, f.getPath());
	}

	public static String getExtension(File f) {
		String name = f.getName();
		int x = name.lastIndexOf('.');
		if (x != -1) {
			return name.substring(x);
		} else {
			return "";
		}
	}

	public static void exec(String[] cmd, Log log) throws ExecException {
		BufferedReader is = null;
		try {
			if (WINDOWS_OS) {
				for (int i = 0; i < cmd.length; i++) {
					cmd[i] = cmd[i].replaceAll("/", "\\\\");
				}
			}
			Process p = Runtime.getRuntime().exec(cmd);
			is = new BufferedReader(new InputStreamReader(p.getErrorStream()));
			String line;
			int errLine = -1;
			Pattern pattern = Pattern.compile(":\\d+:");
			while ((line = is.readLine()) != null) {
				log.append(line);
				Matcher matcher = pattern.matcher(line);
				if (matcher.find()) {
					errLine = Integer.valueOf(
							line.substring(matcher.start() + 1, matcher.end() - 1))
							.intValue();
					if (line.matches("(?i).*unrecognized escape sequence")) {
						log.append(Messages.getString("Util.use.double.backslash"));
					}
					break;
				}
			}
			is.close();
			p.waitFor();
			if (errLine != -1) {
				throw new ExecException(Messages.getString("Util.exec.failed")
						+ ": " + cmd, errLine);
			}
			if (p.exitValue() != 0) {
				throw new ExecException(Messages.getString("Util.exec.failed")
						+ "(" + p.exitValue() + "): " + cmd);
			}
		} catch (IOException e) {
			close(is);
			throw new ExecException(e);
		} catch (InterruptedException e) {
			close(is);
			throw new ExecException(e);
		}
	}

	public static void close(final InputStream o) {
		if (o != null) {
			try {
				o.close();
			} catch (IOException e) {
				System.err.println(e); // XXX log
			}
		}
	}

	public static void close(final OutputStream o) {
		if (o != null) {
			try {
				o.close();
			} catch (IOException e) {
				System.err.println(e); // XXX log
			}
		}
	}

	public static void close(final Reader o) {
		if (o != null) {
			try {
				o.close();
			} catch (IOException e) {
				System.err.println(e); // XXX log
			}
		}
	}

	public static void close(final Writer o) {
		if (o != null) {
			try {
				o.close();
			} catch (IOException e) {
				System.err.println(e); // XXX log
			}
		}
	}

	public static boolean delete(File f) {
		return (f != null) ? f.delete() : false;
	}
}
