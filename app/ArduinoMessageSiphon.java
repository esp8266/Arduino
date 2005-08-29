/*
  Part of the Arduino project - http://arduino.berlios.de

  This class reads from an input stream
  and flushes out everything to the System.out
  It can report interrupts back to the parent process
  
  @author	DojoDave
  @www http://www.0j0.org


  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// TODO migrate codebase to arduino.app
package processing.app;

import java.io.*;

class ArduinoMessageSiphon extends Thread
{
	/** Stream to read from */
	InputStream is;
	
	/** Process' parent in execution */
	Thread parent;
	
	/** Output message from System.error */
	public String theMsg ="";

	/** Result value to be sent back */
	public int result = -1;
	
	/** Count errors and warnings to be sent back */
	public int totalErrors = 0;
	
	/** Debug Errors and Warnings */
	public boolean debugWarning = false;
	public boolean debugError = false;
	public boolean logAllOutput = false;
	
	/**
	  * Creates the stream to flush
	  * @param	theIS is the input stream we will monitor
	  *         process is the parent process that called this
	  */
	public ArduinoMessageSiphon(InputStream theIS, Thread process)
	{
		// Assign to member variables
		is = theIS;
		parent = process;

	}

	/**
	  * Creates the stream to flush
	  * @param	theIS is the input stream we will monitor
	  *         process is the parent process that called this
	  *         warnings will debug warnings when calling commands
	  *         errors will debug errors when calling commands
	  */
	  
	/**
	  * The errors it will report are in the ranges
	  * 	49 - 0  for the programmer's errors
	  * 	99 - 50 for the compiler's errors
	  */
	public ArduinoMessageSiphon(InputStream theIS, Thread process, boolean warnings, boolean errors)
	{
		// Assign to member variables
		is = theIS;
		parent = process;
		debugWarning = warnings;
		debugError = errors;

	}

	/**
	  * The errors it will report are in the ranges
	  * 	49 - 0  for the programmer's errors
	  * 	99 - 50 for the compiler's errors
	  */
	public ArduinoMessageSiphon(InputStream theIS, Thread process, boolean outlog)
	{
		// Assign to member variables
		is = theIS;
		parent = process;
		logAllOutput = outlog;

	}

	/** Performs system call specific code */
	public void run()
	{
        BufferedReader br = new BufferedReader(new InputStreamReader(is));
        
		try {
			String theLine;
			while ((theLine = br.readLine()) != null) {
				if (logAllOutput) {
					theMsg += theLine + "\r\n";
					//System.out.println(theMsg);
				}
				if ((theLine.indexOf("warning:") != -1) && (theLine.indexOf("prog.c:") != -1) && (debugWarning)) {
					theMsg += theLine + "\r\n";
					totalErrors++;
				}
				if ((theLine.indexOf("error:") != -1) && (theLine.indexOf("prog.c:") != -1) && (debugError)) {
					result = 97;
					theMsg += theLine + "\r\n";
					totalErrors++;
				}
				if (theLine.indexOf("Errors: none") != -1) {
					result = 0;
				}
				if (theLine.indexOf("bytes transferred") != -1) {
					result = 0;
				}
				if (theLine.toLowerCase().indexOf("not responding") != -1) {
					result = 49;
				}
			}
			br.close();

			// waits 20 secs until being killed by the 
			// parent process or just dies, no zombies wanted!!
			try {
				Thread.currentThread().sleep(20000); 
			} catch (InterruptedException ie) {
			}
		} catch (Exception err) {
			err.printStackTrace();
			result = 96;
		}
	}
}

