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

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;

import net.sf.launch4j.binding.InvariantViolationException;
import net.sf.launch4j.config.Config;
import net.sf.launch4j.config.ConfigPersister;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class Builder {
	private final Log _log;
	private final File _basedir;

	public Builder(Log log) {
		_log = log;
		_basedir = Util.getJarBasedir();
	}

	public Builder(Log log, File basedir) {
		_log = log;
		_basedir = basedir;
	}

	/**
	 * @return Output file path.
	 */
	public File build() throws BuilderException {
		final Config c = ConfigPersister.getInstance().getConfig();
		try {
			c.validate();
		} catch (InvariantViolationException e) {
			throw new BuilderException(e.getMessage());
		}
		File rc = null;
		File ro = null;
		File outfile = null;
		FileInputStream is = null;
		FileOutputStream os = null;
		final RcBuilder rcb = new RcBuilder();
		try {
			rc = rcb.build(c);
			ro = Util.createTempFile("o");
			outfile = ConfigPersister.getInstance().getOutputFile();

			Cmd resCmd = new Cmd(_basedir);
			resCmd.addExe("windres")
					.add(Util.WINDOWS_OS ? "--preprocessor=type" : "--preprocessor=cat")
					.add("-J rc -O coff -F pe-i386")
					.addAbsFile(rc)
					.addAbsFile(ro);
			_log.append(Messages.getString("Builder.compiling.resources"));
			resCmd.exec(_log);

			Cmd ldCmd = new Cmd(_basedir);
			ldCmd.addExe("ld")
					.add("-mi386pe")
					.add("--oformat pei-i386")
					.add((c.getHeaderType().equals(Config.GUI_HEADER))
							? "--subsystem windows" : "--subsystem console")
					.add("-s")		// strip symbols
					.addFiles(c.getHeaderObjects())
					.addAbsFile(ro)
					.addFiles(c.getLibs())
					.add("-o")
					.addAbsFile(outfile);
			_log.append(Messages.getString("Builder.linking"));
			ldCmd.exec(_log);

			if (!c.isDontWrapJar()) {
				_log.append(Messages.getString("Builder.wrapping"));
				int len;
				byte[] buffer = new byte[1024];
				is = new FileInputStream(Util.getAbsoluteFile(
						ConfigPersister.getInstance().getConfigPath(),	c.getJar()));
				os = new FileOutputStream(outfile, true);
				while ((len = is.read(buffer)) > 0) {
					os.write(buffer, 0, len);
				}
			}
			_log.append(Messages.getString("Builder.success") + outfile.getPath());
			return outfile;
		} catch (IOException e) {
			Util.delete(outfile);
			_log.append(e.getMessage());
			throw new BuilderException(e);
		} catch (ExecException e) {
			Util.delete(outfile);
			String msg = e.getMessage(); 
			if (msg != null && msg.indexOf("windres") != -1) {
				if (e.getErrLine() != -1) {
					_log.append(Messages.getString("Builder.line.has.errors",
							String.valueOf(e.getErrLine())));
					_log.append(rcb.getLine(e.getErrLine()));
				} else {
					_log.append(Messages.getString("Builder.generated.resource.file"));
					_log.append(rcb.getContent());
				}
			}
			throw new BuilderException(e);
		} finally {
			Util.close(is);
			Util.close(os);
			Util.delete(rc);
			Util.delete(ro);
		}
	}
}

class Cmd {
	private final List _cmd = new ArrayList();
	private final File _basedir;
	private final File _bindir;

	public Cmd(File basedir) {
		_basedir = basedir;
		String path = System.getProperty("launch4j.bindir");
		if (path == null) {
			_bindir = new File(basedir, "bin");
		} else {
			File bindir = new File(path);
			_bindir = bindir.isAbsolute() ? bindir : new File(basedir, path);
		}
	}

	public Cmd add(String s) {
		StringTokenizer st = new StringTokenizer(s);
		while (st.hasMoreTokens()) {
			_cmd.add(st.nextToken());
		}
		return this;
	}

	public Cmd addAbsFile(File file) {
		_cmd.add(file.getPath());
		return this;
	}

	public Cmd addFile(String pathname) {
		_cmd.add(new File(_basedir, pathname).getPath());
		return this;
	}

	public Cmd addExe(String pathname) {
		if (Util.WINDOWS_OS) {
			pathname += ".exe";
		}
		_cmd.add(new File(_bindir, pathname).getPath());
		return this;
	}

	public Cmd addFiles(List files) {
		for (Iterator iter = files.iterator(); iter.hasNext();) {
			addFile((String) iter.next());
		}
		return this;
	}

	public void exec(Log log) throws ExecException {
		String[] cmd = (String[]) _cmd.toArray(new String[_cmd.size()]);
		Util.exec(cmd, log);
	}
}
