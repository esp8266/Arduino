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
 * Created on 2004-01-30
 */
package net.sf.launch4j.binding;

import java.io.File;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;

import net.sf.launch4j.Util;
import net.sf.launch4j.config.ConfigPersister;

/**
 * @author Copyright (C) 2004 Grzegorz Kowal
 */
public class Validator {
	public static final String ALPHANUMERIC_PATTERN = "[\\w]*?";
	public static final String ALPHA_PATTERN = "[\\w&&\\D]*?";
	public static final String NUMERIC_PATTERN = "[\\d]*?";
	public static final String PATH_PATTERN = "[\\w|[ .,:\\-/\\\\]]*?";

	public static final int MAX_STR = 128;
	public static final int MAX_PATH = 260;
	public static final int MAX_BIG_STR = 8192;	// or 16384;
	public static final int MAX_ARGS = 32767 - 2048;

	private Validator() {}

	public static boolean isEmpty(String s) {
		return s == null || s.equals("");
	}

	public static void checkNotNull(Object o, String property, String name) {
		if (o == null) {
			signalViolation(property,
					Messages.getString("Validator.empty.field", name));
		}
	}

	public static void checkString(String s, int maxLength, String property,
			String name) {
		if (s == null || s.length() == 0) {
			signalViolation(property,
					Messages.getString("Validator.empty.field", name));
		}
		if (s.length() > maxLength) {
			signalLengthViolation(property, name, maxLength);
		}
	}

	public static void checkOptStrings(List strings, int maxLength, int totalMaxLength,
			String property, String name) {
		if (strings == null) {
			return;
		}
		int totalLength = 0;
		for (Iterator iter = strings.iterator(); iter.hasNext();) {
			String s = (String) iter.next();
			checkString(s, maxLength, property, name);
			totalLength += s.length();
			if (totalLength > totalMaxLength) {
				signalLengthViolation(property, name, totalMaxLength);
			}
		}
	}

	public static void checkString(String s, int maxLength, String pattern,
			String property, String name) {
		checkString(s, maxLength, property, name);
		if (!s.matches(pattern)) {
			signalViolation(property,
					Messages.getString("Validator.invalid.data", name));
		}
	}

	public static void checkOptStrings(List strings, int maxLength, int totalMaxLength,
			String pattern, String property, String name, String msg) {
		if (strings == null) {
			return;
		}
		int totalLength = 0;
		for (Iterator iter = strings.iterator(); iter.hasNext();) {
			String s = (String) iter.next();
			checkString(s, maxLength, property, name);
			if (!s.matches(pattern)) {
				signalViolation(property, msg != null
						? msg 
						: Messages.getString("Validator.invalid.data", name));
			}
			totalLength += s.length();
			if (totalLength > totalMaxLength) {
				signalLengthViolation(property, name, totalMaxLength);
			}
		}
	}

	public static void checkOptString(String s, int maxLength, String property,
			String name) {
		if (s == null || s.length() == 0) {
			return;
		}
		if (s.length() > maxLength) {
			signalLengthViolation(property, name, maxLength);
		}
	}

	public static void checkOptString(String s, int maxLength, String pattern,
			String property, String name) {
		if (s == null || s.length() == 0) {
			return;
		}
		if (s.length() > maxLength) {
			signalLengthViolation(property, name, maxLength);
		}
		if (!s.matches(pattern)) {
			signalViolation(property,
					Messages.getString("Validator.invalid.data", name));
		}
	}

	public static void checkRange(int value, int min, int max,
			String property, String name) {
		if (value < min || value > max) {
			signalViolation(property,
					Messages.getString("Validator.must.be.in.range", name,
							String.valueOf(min), String.valueOf(max)));
		}
	}

	public static void checkRange(char value, char min, char max,
			String property, String name) {
		if (value < min || value > max) {
			signalViolation(property, Messages.getString("Validator.must.be.in.range",
					name, String.valueOf(min), String.valueOf(max)));
		}
	}

	public static void checkMin(int value, int min, String property, String name) {
		if (value < min) {
			signalViolation(property,
					Messages.getString("Validator.must.be.at.least", name,
							String.valueOf(min)));
		}
	}

	public static void checkIn(String s, String[] strings, String property,
			String name) {
		if (isEmpty(s)) {
			signalViolation(property,
					Messages.getString("Validator.empty.field", name));
		}
		List list = Arrays.asList(strings);
		if (!list.contains(s)) {
			signalViolation(property,
					Messages.getString("Validator.invalid.option", name, list.toString())); 
		}
	}

	public static void checkTrue(boolean condition, String property, String msg) {
		if (!condition) {
			signalViolation(property, msg);
		}
	}
	
	public static void checkFalse(boolean condition, String property, String msg) {
		if (condition) {
			signalViolation(property, msg);
		}
	}
	
	public static void checkElementsNotNullUnique(Collection c, String property,
			String msg) {
		if (c.contains(null)
				|| new HashSet(c).size() != c.size()) {
			signalViolation(property,
					Messages.getString("Validator.already.exists", msg)); 
		}
	}

	public static void checkElementsUnique(Collection c, String property, String msg) {
		if (new HashSet(c).size() != c.size()) {
			signalViolation(property,
					Messages.getString("Validator.already.exists", msg));
		}
	}

	public static void checkFile(File f, String property, String fileDescription) {
		File cfgPath = ConfigPersister.getInstance().getConfigPath();
		if (f == null
				|| f.getPath().equals("")
				|| (!f.exists() && !Util.getAbsoluteFile(cfgPath, f).exists())) {
			signalViolation(property,
					Messages.getString("Validator.doesnt.exist", fileDescription));
		}
	}

	public static void checkOptFile(File f, String property, String fileDescription) {
		if (f != null && f.getPath().length() > 0) {
			checkFile(f, property, fileDescription);
		}
	}

	public static void checkRelativeWinPath(String path, String property, String msg) {
		if (path == null
				|| path.equals("")
				|| path.startsWith("/")
				|| path.startsWith("\\")
				|| path.indexOf(':') != -1) {
			signalViolation(property, msg);
		}
	}

	public static void signalLengthViolation(String property, String name,
			int maxLength) {
		signalViolation(property,
				Messages.getString("Validator.exceeds.max.length", name,
						String.valueOf(maxLength)));
	}

	public static void signalViolation(String property, String msg)	{
		throw new InvariantViolationException(property, msg);
	}
}
