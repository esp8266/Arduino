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
 * Created on May 12, 2005
 */
package net.sf.launch4j;

import javax.swing.JTextArea;
import javax.swing.SwingUtilities;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public abstract class Log {
	private static final Log _consoleLog = new ConsoleLog();
	private static final Log _antLog = new AntLog();

	public abstract void clear();
	public abstract void append(String line);

	public static Log getConsoleLog() {
		return _consoleLog;
	}
	
	public static Log getAntLog() {
		return _antLog;
	}

	public static Log getSwingLog(JTextArea textArea) {
		return new SwingLog(textArea);
	}
}

class ConsoleLog extends Log {
	public void clear() {
		System.out.println("\n");
	}

	public void append(String line) {
		System.out.println("launch4j: " + line);
	}
}

class AntLog extends Log {
	public void clear() {
		System.out.println("\n");
	}

	public void append(String line) {
		System.out.println(line);
	}
}

class SwingLog extends Log {
	private final JTextArea _textArea;

	public SwingLog(JTextArea textArea) {
		_textArea = textArea;
	}

	public void clear() {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				_textArea.setText("");
		}});
	}

	public void append(final String line) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				_textArea.append(line + "\n");
		}});
	}
}
