/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Uploader - default downloader class that connects to uisp
  Part of the Arduino project - http://arduino.berlios.de/

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
  
  $Id:$
*/

package processing.app;
import java.io.*;
import java.util.*;
import java.util.zip.*;
import javax.swing.*;
//#ifndef RXTX
//import javax.comm.*;
//#else
// rxtx uses package gnu.io, but all the class names
// are the same as those used by javax.comm
import gnu.io.*;
//#endif


public class Uploader implements MessageConsumer  {
  static final String BUGS_URL =
    "https://developer.berlios.de/bugs/?group_id=3590";
  static final String SUPER_BADNESS =
    "Compiler error, please submit this code to " + BUGS_URL;

  String buildPath;
  String className;
  File includeFolder;
  RunnerException exception;
  Sketch sketch;
  //PdePreferences preferences;

  //Serial serialPort;
  static InputStream serialInput;
  static OutputStream serialOutput;
  //int serial; // last byte of data received

  private String serial_port = "COM1";
  private int serial_rate = 9600;
  private char serial_parity = 'N';
  private int serial_databits = 8;
  private float serial_stopbits = 1;

  public void serialPreferences() {
    //System.out.println("setting serial properties");
    serial_port = Preferences.get("serial.port");
    serial_rate = Preferences.getInteger("serial.download_rate");
    serial_parity = Preferences.get("serial.parity").charAt(0);
    serial_databits = Preferences.getInteger("serial.databits");
    serial_stopbits = new Float(Preferences.get("serial.stopbits")).floatValue();
  }

  public Uploader(String buildPath, String className,
                      Sketch sketch) {
    this.buildPath = buildPath;
    this.includeFolder = includeFolder;
    this.className = className;
    this.sketch = sketch;
  }

  public boolean downloadJava(PrintStream leechErr) throws RunnerException {
    String userdir = System.getProperty("user.dir") + File.separator;
//    String commandDownloader[] = new String[] {
//    ((!Base.isMacOS()) ?  "tools/avr/bin/uisp" :
//      userdir + "tools/avr/bin/uisp"),
//    //[2] Serial port
//    //[3] Serial download rate
//    //[6] hex class file
//     "-dprog=stk500",
//     " ",
//     " ",
//     "-dpart=" + Preferences.get("build.mcu"),
//     "--upload",
//     " "
//    };

    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;
    notFoundError = false;
    int result=0; // pre-initialized to quiet a bogus warning from jikes
    try {
      serialPreferences();
      List commandDownloader = new ArrayList();
      commandDownloader.add((!Base.isMacOS() ? "" : userdir) + "tools/avr/bin/uisp");
      commandDownloader.add("-dprog=" + Preferences.get("upload.programmer"));
      commandDownloader.add("-dpart=" + Preferences.get("build.mcu"));
	  //commandDownloader.add("-v=4"); // extra verbosity for help debugging.
      if (Preferences.get("upload.programmer").equals("dapa"))
        commandDownloader.add("-dlpt=" + Preferences.get("parallel.port"));
      else {
        commandDownloader.add("-dserial=" + (Base.isWindows() ? "/dev/" + serial_port.toLowerCase() : serial_port));
        commandDownloader.add("-dspeed=" + serial_rate);
      }
      if (Preferences.getBoolean("upload.erase"))
        commandDownloader.add("--erase");
      commandDownloader.add("--upload");
      if (Preferences.getBoolean("upload.verify"))
        commandDownloader.add("--verify");
      commandDownloader.add("if=" + buildPath + File.separator + className + ".hex");

//      commandDownloader[2] = ((!Base.isMacOS()) ? "-dserial=/dev/" + serial_port.toLowerCase() : "-dserial=" + serial_port );
//      commandDownloader[3] = "-dspeed=" + serial_rate;
//      commandDownloader[6] = "if=" + buildPath + File.separator + className + ".hex";
      /*for(int i = 0; i < commandDownloader.length; i++) {
	System.out.println(commandDownloader[i]);
      }*/

      // Cleanup the serial buffer
      Serial serialPort = new Serial();
      byte[] readBuffer;
      while(serialPort.available() > 0) {
        readBuffer = serialPort.readBytes();
        Thread.sleep(100);
      }
      serialPort.dispose();

      String[] commandArray = new String[commandDownloader.size()];
      Process process = Runtime.getRuntime().exec((String[]) commandDownloader.toArray(commandArray));
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
        exception.hideStackTrace = true;
        throw exception;   
      }
      if(result!=0)
        return false;
    } catch (Exception e) {
      String msg = e.getMessage();
      if ((msg != null) && (msg.indexOf("uisp: not found") != -1)) {
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
    //System.err.println("MSG: " + s);
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
      exception = new RunnerException("the selected serial port "+s+" does not exist or your board is not connected");
      return;
    }
    // jikes always uses a forward slash character as its separator, so
    // we need to replace any platform-specific separator characters before
    // attemping to compare
    //
    if (s.indexOf("Device is not responding") != -1 ) {
      exception =  new RunnerException("Device is not responding, check the right serial port is selected or RESET the board right before exporting");
      return;
    }
    if (s.indexOf("Programmer is not responding") != -1) {
      exception = new RunnerException("Programmer is not responding, RESET the board right before exporting");
      return;
    }
  }


}
