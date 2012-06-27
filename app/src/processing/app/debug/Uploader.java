/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Uploader - abstract uploading baseclass (common to both uisp and avrdude)
  Part of the Arduino project - http://www.arduino.cc/

  Copyright (c) 2004-05
  Hernando Barragan

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
  
  $Id$
*/

package processing.app.debug;

import static processing.app.I18n._;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.Collection;

import processing.app.I18n;
import processing.app.Preferences;
import processing.app.Serial;
import processing.app.SerialException;
import processing.app.SerialNotFoundException;

public abstract class Uploader implements MessageConsumer  {
  static final String BUGS_URL =
    _("https://developer.berlios.de/bugs/?group_id=3590");
  static final String SUPER_BADNESS =
    I18n.format(_("Compiler error, please submit this code to {0}"), BUGS_URL);

  RunnerException exception;

  static InputStream serialInput;
  static OutputStream serialOutput;
  
  boolean verbose;

  public abstract boolean uploadUsingPreferences(String buildPath, String className, boolean usingProgrammer)
    throws RunnerException, SerialException;
  
  public abstract boolean burnBootloader() throws RunnerException;
  
  protected void flushSerialBuffer() throws RunnerException, SerialException {
    // Cleanup the serial buffer
    try {
      Serial serialPort = new Serial();
      while(serialPort.available() > 0) {
        serialPort.readBytes();
        try {
          Thread.sleep(100);
        } catch (InterruptedException e) {}
      }

      serialPort.setDTR(false);
      serialPort.setRTS(false);

      try {
        Thread.sleep(100);
      } catch (InterruptedException e) {}

      serialPort.setDTR(true);
      serialPort.setRTS(true);
      
      serialPort.dispose();
    } catch (SerialNotFoundException e) {
      throw e;
    } catch(Exception e) {
      e.printStackTrace();
      throw new RunnerException(e.getMessage());
    }
  }

  protected boolean executeUploadCommand(Collection<String> commandDownloader)
      throws RunnerException {
    String[] commandArray = new String[commandDownloader.size()];
    commandDownloader.toArray(commandArray);
    return executeUploadCommand(commandArray);
  }

  protected boolean executeUploadCommand(String commandArray[]) 
    throws RunnerException
  {
    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;
    notFoundError = false;
    int result=0; // pre-initialized to quiet a bogus warning from jikes
    
    try {
        for(int i = 0; i < commandArray.length; i++) {
          System.out.print(commandArray[i] + " ");
        }
        System.out.println();
        if (verbose || Preferences.getBoolean("upload.verbose")) {
      }
      Process process = Runtime.getRuntime().exec(commandArray);
      new MessageSiphon(process.getInputStream(), this);
      new MessageSiphon(process.getErrorStream(), this);

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
      if(exception!=null) {
        exception.hideStackTrace();
        throw exception;   
      }
      if(result!=0)
        return false;
    } catch (Exception e) {
      String msg = e.getMessage();
      if ((msg != null) && (msg.indexOf("uisp: not found") != -1) && (msg.indexOf("avrdude: not found") != -1)) {
        //System.err.println("uisp is missing");
        //JOptionPane.showMessageDialog(editor.base,
        //                              "Could not find the compiler.\n" +
        //                              "uisp is missing from your PATH,\n" +
        //                              "see readme.txt for help.",
        //                              "Compiler error",
        //                              JOptionPane.ERROR_MESSAGE);
        return false;
      } else {
        e.printStackTrace();
        result = -1;
      }
    }
    //System.out.println("result2 is "+result);
    // if the result isn't a known, expected value it means that something
    // is fairly wrong, one possibility is that jikes has crashed.
    //
    if (exception != null) throw exception;

    if ((result != 0) && (result != 1 )) {
      exception = new RunnerException(SUPER_BADNESS);
      //editor.error(exception);
      //PdeBase.openURL(BUGS_URL);
      //throw new PdeException(SUPER_BADNESS);
    }

    return (result == 0); // ? true : false;      

  }

  boolean firstErrorFound;
  boolean secondErrorFound;

  // part of the PdeMessageConsumer interface
  //
  boolean notFoundError;

  public void message(String s) {
  	// selectively suppress a bunch of avrdude output for AVR109/Caterina that should already be quelled but isn't
	if (!Preferences.getBoolean("upload.verbose") && ( 
		s.indexOf("Connecting to programmer:") != -1 ||
		s.indexOf("Found programmer: Id = \"CATERIN\"; type = S") != -1 ||
		s.indexOf("Software Version = 1.0; No Hardware Version given.") != -1 ||
		s.indexOf("Programmer supports auto addr increment.") != -1 ||
		s.indexOf("Programmer supports buffered memory access with buffersize=128 bytes.") != -1 || 
		s.indexOf("Programmer supports the following devices:") != -1 || 
		s.indexOf("Device code: 0x44") != -1))
		s = "";	
    
    System.err.print(s);

    // ignore cautions
    if (s.indexOf("Error") != -1) {
      //exception = new RunnerException(s+" Check the serial port selected or your Board is connected");
      //System.out.println(s);
      notFoundError = true;
      return;
    }
    if(notFoundError) {
      //System.out.println("throwing something");
      exception = new RunnerException(I18n.format(_("the selected serial port {0} does not exist or your board is not connected"), s));
      return;
    }
    if (s.indexOf("Device is not responding") != -1 ) {
      exception =  new RunnerException(_("Device is not responding, check the right serial port is selected or RESET the board right before exporting"));
      return;
    }
    if (s.indexOf("Programmer is not responding") != -1 ||
        s.indexOf("programmer is not responding") != -1 ||
        s.indexOf("protocol error") != -1 ||
        s.indexOf("avrdude: ser_open(): can't open device") != -1 ||
        s.indexOf("avrdude: ser_drain(): read error") != -1 ||
        s.indexOf("avrdude: ser_send(): write error") != -1 ||
        s.indexOf("avrdude: error: buffered memory access not supported.") != -1) {
      exception = new RunnerException(_("Problem uploading to board.  See http://www.arduino.cc/en/Guide/Troubleshooting#upload for suggestions."));
      return;
    }
    if (s.indexOf("Expected signature") != -1) {
      exception = new RunnerException(_("Wrong microcontroller found.  Did you select the right board from the Tools > Board menu?"));
      return;
    }
  }


}
