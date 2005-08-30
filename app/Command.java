/*
  Part of the Arduino project - http://arduino.berlios.de

  This class creates a compilation thread
  this is needed for detecting time-outs
  in system calls. It may happen in
  windows whenever there is a syntax error
  that the compilation may hang the IDE
  
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

class Command extends Thread
{
	/** Command to execute on command line */
	String command;
	
	/** Process in execution */
	Process process;
	
	/** Output message from System.out */
	public String outMsg ="";

	/** Error message from System.error */
	public String errorMsg ="";

	/** Result value to be sent back */
	public int outResult = -1;
	public int errorResult = -1;
	
	/** Total errors and warnings to be sent back */
	public int totalErrors = 0;
	
	/** Waiting value */
	public int waitResult = -1;
	
	/** Debug Errors and Warnings */
	public boolean debugWarning = false;
	public boolean debugError = false;
	public boolean logAllOutput = false;
	
	/**
	  * Creates the compiler object with the 
	  * gnumake line to call
	  * @param	command	name of the command line to call
	  */
	public Command (String co)
	{
		// Assign to member variable
		command = co;

	}

	/**
	  * Creates the compiler object with the 
	  * gnumake line to call
	  * @param	command	name of the command line to call
	  *         outlog will debug warnings when calling commands
	  */
	public Command (String co, boolean outlog)
	{
		// Assign to member variable
		command = co;
		logAllOutput = outlog;
	}

	/**
	  * Creates the compiler object with the 
	  * gnumake line to call
	  * @param	command	name of the command line to call
	  *         warnings will debug warnings when calling commands
	  *         errors will debug errors when calling commands
	  */
	public Command (String co, boolean warnings, boolean errors)
	{
		// Assign to member variable
		command = co;
		debugWarning = warnings;
		debugError = errors;
	}

	/** Performs system call specific code */
	public void run()
	{
		try {
	        process = Runtime.getRuntime().exec(command);
	        ArduinoMessageSiphon input;
	        ArduinoMessageSiphon error;
	        if (logAllOutput) {
		        input = new ArduinoMessageSiphon(process.getInputStream(), this, logAllOutput);
		        error = new ArduinoMessageSiphon(process.getErrorStream(), this, logAllOutput);
	        } else {
		        input = new ArduinoMessageSiphon(process.getInputStream(), this, debugWarning, debugError);
		        error = new ArduinoMessageSiphon(process.getErrorStream(), this, debugWarning, debugError);
	        }
	        input.start();
	        error.start();
	        
	        // wait for the process to finish.  if interrupted
	        // before waitFor returns, continue waiting
	        //
	        boolean compiling = true;
	        while (compiling) {
	          try {
	            waitResult = process.waitFor();
	            outResult = input.result;
	            errorResult = error.result;
	            compiling = false;
	          } catch (InterruptedException ignored) { }
	        }

			try {
				waitResult = process.exitValue();
			} catch (IllegalThreadStateException ignored) { }
	        
	        // trace the error and outout
	        outMsg = input.theMsg;
	        errorMsg = error.theMsg;
	        totalErrors = error.totalErrors;
			
			// waits 20 secs until being killed by the 
			// parent process or just dies, no zombies wanted!!
			try {
				Thread.currentThread().sleep(20000); 
			} catch (InterruptedException ie) {
			}
			
		} catch (IOException ioe) {
			outResult = 99;
		}

	}
}