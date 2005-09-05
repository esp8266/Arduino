/*
  Downloader - default downloader class that connects to uisp

  Part of the Arduino project http://arduino.berlios.de

  Based on PdeDownloader by
  Copyright (c) 2005
  Hernando Barragan, Interaction Design Institute Ivrea

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


package processing.app;



import java.io.*;
import java.util.*;
import java.util.zip.*;
import javax.swing.*;

import gnu.io.*;



public class Downloader implements MessageConsumer {
  static final String BUGS_URL =
    "http://arduino.berlios.de";
  static final String SUPER_BADNESS =
    "Compiler error, please submit this code to " + BUGS_URL;

  String buildPath;
  String className;
  File includeFolder;
  RunnerException exception;
  Sketch sketch;
  //Preferences preferences;

  //static SerialPort serialPort;
  static InputStream serialInput;
  static OutputStream serialOutput;
  //int serial; // last byte of data received

  private String serial_port = "COM1";
  private int serial_rate = 9600;
  private char serial_parity = 'N';
  private int serial_databits = 8;
  private float serial_stopbits = 1;





  public Downloader() {
  	serial_port = Preferences.get("serial.port");
  	
  }

  // Calls the makefile with the "program" option
  // TODO Windows paths!
  private boolean downloadMake(String userdir) {
    System.out.println("Downloading - makefile");
    Process process;
    int result = 0;
    String command = "";
    try {
      serial_port = Preferences.get("serial.port");
    //TODO test this in windows
    // FIXME: this is really nasty, it seems that MACOS is making the
    //        compilation inside the lib folder, while windows is doing it
    //        inside the work folder ... why why why  --DojoDave 
    if (Base.isWindows()) {
	    command = userdir + "tools\\gnumake.exe  SERIAL=" + serial_port + " -C " + userdir + ". program";
    } else if (Base.isMacOS()) {
	    command = userdir + "tools/gnumake SERIAL=" + serial_port + "  -C " + "lib program";
    }
//      String command = userdir + "lib/wiringlite/bin/gnumake SERIAL=" + serial_port + "  -C " + userdir + "lib/wiringlite program";
      System.out.println(command);
      process = Runtime.getRuntime().exec(command);
      new MessageSiphon(process.getInputStream(), this);
      new MessageSiphon(process.getErrorStream(), this);
      
      
      boolean compiling = true;
      while (compiling) {
        try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException ignored) { }
      }
    } catch (Exception e) {
      e.printStackTrace();
      System.out.println("Error: GNUMake probably couldn't be found");
      result = 99;
    }
    if(0 == result){
      System.out.println("Arduino Download Successful");
    }else{
      System.out.println("Arduino Download Unsuccessful (error: " + result + ")");
    }
    return (result == 0);
  }

  public boolean downloadJava() {
      String userdir = System.getProperty("user.dir") + File.separator;
    
	
      //return downloadNative(userdir);
      return downloadNew(userdir);
    
  }
  
  
  // Calls UISP directly, skipping the makefile
  //
  // TODO Windows paths!!
  public boolean downloadNative(String userdir) {
	System.out.println("Downloading code");
    
    
    // FIXME: this is really nasty, it seems that MACOS is making the
    //        compilation inside the lib folder, while windows is doing it
    //        inside the work folder ... why why why  --DojoDave 
    if (Base.isWindows()) {
		buildPath = userdir + "tmp";
    } else if (Base.isMacOS()) {
		buildPath = userdir + "lib/tmp";
    }
     
    String commandDownloader[] = new String[] {
     userdir + "tools/avr/bin/uisp",
    //[2] Serial port
    //[6] hex class file
     "-dprog=stk500",
     " ",
     "-dspeed=9600",
     "-dpart=atmega8",
     "--upload",
     " "
    };

    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;

    int result=0; // pre-initialized to quiet a bogus warning from gcc
    try {

      serial_port = Preferences.get("serial.port");
      commandDownloader[2] = "-dserial=" + serial_port;
      commandDownloader[6] = "if=" + buildPath + File.separator + "prog.hex";

      // for(int i = 0; i < commandDownloader.length; i++) {
	  // System.out.print(commandDownloader[i] + " "); 
      //}
      
      Process process = Runtime.getRuntime().exec(commandDownloader);
      new processing.app.MessageSiphon(process.getInputStream(), this);
      new processing.app.MessageSiphon(process.getErrorStream(), this);

	  
      // wait for the process to finish.  if interrupted
      // before waitFor returns, continue waiting
      //
      boolean compiling = true;
      while (compiling) {
        try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException intExc) {
        }
      }    
    } catch (Exception e) {
      String msg = e.getMessage();
      if ((msg != null) && (msg.indexOf("uisp: not found") != -1)) {
        //System.err.println("gcc is missing");
        //JOptionPane.showMessageDialog(editor.base,
        //                              "Could not find the downloader.\n" +
        //                              "uisp is missing from your PATH,\n" +
        //                              "see readme.txt for help.",
        //                              "Downloade error",
        //                              JOptionPane.ERROR_MESSAGE);
        return false;
      }
      e.printStackTrace();
      result = -1;
    }

    // if the result isn't a known, expected value it means that something
    // is fairly wrong, one possibility is that gcc has crashed.
    //
    if (result != 0 && result != 1 ) {
      exception = new RunnerException(SUPER_BADNESS);
      //editor.error(exception);
      //Base.openURL(BUGS_URL);
    }

    return (result == 0) ? true : false;      

  }

  boolean firstErrorFound;
  boolean secondErrorFound;

  // part of the MessageConsumer interface
  //
  // at the moment we are pretty happy just to see the error messages coming back
  public void message(String s) {
    //System.err.println("MSG: " + s);
    System.err.print(s);

 
  }
  
  
   public boolean downloadNew(String userdir) {
	
	serial_port = Preferences.get("serial.port");
	
	// TODO must manage this flag from the editor
	boolean alreadyCompiled = true;
	
	// TODO must manage this flag from global prefs
	boolean debug = false;
	
	String verbose = " -s ";
	
  	if (alreadyCompiled) {
		if (serial_port == "") {
			System.out.println("The serial port is not set!");
			System.out.println("Use the Options -> Serial menu to select");
			System.out.println("where you have connected your arduino board");
			return false;
		};
		
		String commandLine = "";
	    

		if (debug) System.out.println("userdir is "+userdir);
	
		int result = -1;
		
		
		// TODO make this code more portable using File.separator
	    if (Base.isMacOS()) {          
		   commandLine = userdir + "tools/avr/bin/uisp ";
		   commandLine += " -dprog=stk500 -dspeed=9600 ";
		   commandLine += " -dserial=" + serial_port; 
		   commandLine += " -dpart=ATmega8";
		   commandLine += " if=" +userdir + "lib/wiringlite/tmp/prog.hex --upload";
	    } else {          
			commandLine = userdir + "tools\\gnumake.exe" + " SERIAL=" + serial_port + verbose + " -C " + userdir + ". program";
		}
	    if (debug) System.out.println(commandLine);
	    
		// Launch the command as a thread (this way we can kill it
		// in the case it times out)
		Command command = new Command(commandLine, true);
		command.setName("theDownloader");
		command.start();
		
		// TODO move this to Preferences
		// Default timeout when calling a command (in seconds)
    	final int maxTimeOut = 30;  // 10 secs
		
		
	    // Start timer to monitor buffer timeout ==> deadlock in process
	int timeCount = 0;
	
	while ((timeCount <= maxTimeOut) && (result == -1) && command.isAlive()) {
		try {
			result = command.waitResult;
			Thread.currentThread().sleep(1000);
		} catch (InterruptedException ie) {
		}
		timeCount++;
	}
	result = command.waitResult;
	
		if ((result != 0) && (command.errorResult == -1)) {
//			result = 94;
			System.out.println("Time out error when trying to upload the program");
			System.out.println("Board not present, bootloader not installed or processor's failure");
			System.out.println("Arduino download unsuccessful (error: " + result + ")");
		} else if (result == 0) {
			System.out.println(command.errorMsg);
			System.out.println("OK - Arduino download successful");
		} else if ((result != 0) && (49 == command.errorResult)) {
			System.out.println(command.errorMsg);
			System.out.println("Bootloader not responding");
			System.out.println("Arduino download unsuccessful (error: " + result + ")");
	    } else {
			System.out.println(command.errorMsg);
			System.out.println("Arduino download unsuccessful (error: " + result + ")");
	    }
		if (command.isAlive()) command.process.destroy();
  	} else {
		System.out.println("You have to compile the code first");
		System.out.println("Arduino download unsuccessful");
  	}
		
	return true;
  }  
  
  
  
  


}

