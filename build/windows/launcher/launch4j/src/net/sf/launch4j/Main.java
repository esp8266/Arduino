/*
	Launch4j (http://launch4j.sourceforge.net/)
	Cross-platform Java application wrapper for creating Windows native executables.

	Copyright (c) 2004, 2008 Grzegorz Kowal

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
package net.sf.launch4j;

import java.io.File;
import java.io.InputStream;
import java.util.Properties;

import net.sf.launch4j.config.ConfigPersister;
import net.sf.launch4j.formimpl.MainFrame;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class Main {
	private static String _name; 
	private static String _description;

	public static void main(String[] args) {
		try {
			Properties props = new Properties();
			InputStream in = Main.class.getClassLoader()
					.getResourceAsStream("launch4j.properties");
			props.load(in);
			in.close();
			setDescription(props);

			if (args.length == 0) {
				ConfigPersister.getInstance().createBlank();
				MainFrame.createInstance();
			} else if (args.length == 1 && !args[0].startsWith("-")) {
				ConfigPersister.getInstance().load(new File(args[0]));
				Builder b = new Builder(Log.getConsoleLog());
				b.build();
			} else {
				System.out.println(_description
						+ Messages.getString("Main.usage")
						+ ": launch4j config.xml");
			}
		} catch (Exception e) {
			Log.getConsoleLog().append(e.getMessage());
		} 
	}

	public static String getName() {
		return _name;
	}

	public static String getDescription() {
		return _description;
	}

	private static void setDescription(Properties props) {
		_name = "Launch4j " + props.getProperty("version"); 
		_description = _name + 
				" (http://launch4j.sourceforge.net/)\n" +
				"Cross-platform Java application wrapper" +
						" for creating Windows native executables.\n\n" +
				"Copyright (C) 2004, 2008 Grzegorz Kowal\n\n" +
				"Launch4j comes with ABSOLUTELY NO WARRANTY.\n" +
				"This is free software, licensed under the BSD License.\n" +
				"This product includes software developed by the Apache Software Foundation" +
						" (http://www.apache.org/).";
	}
}
