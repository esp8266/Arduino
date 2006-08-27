/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Uploader - default downloader class that connects to uisp
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

  RunnerException exception;
  //PdePreferences preferences;

  //Serial serialPort;
  static InputStream serialInput;
  static OutputStream serialOutput;
  //int serial; // last byte of data received

  public Uploader() {
  }

  public boolean uploadUsingPreferences(String buildPath, String className)
    throws RunnerException {
    List commandDownloader = new ArrayList();
    commandDownloader.add("-dprog=" + Preferences.get("upload.programmer"));
    if (Preferences.get("upload.programmer").equals("dapa"))
      commandDownloader.add("-dlpt=" + Preferences.get("parallel.port"));
    else {
      commandDownloader.add(
        "-dserial=" + (Base.isWindows() ?
          "/dev/" + Preferences.get("serial.port").toLowerCase() :
          Preferences.get("serial.port")));
      commandDownloader.add(
        "-dspeed=" + Preferences.getInteger("serial.download_rate"));
    }
    if (Preferences.getBoolean("upload.erase"))
      commandDownloader.add("--erase");
    commandDownloader.add("--upload");
    if (Preferences.getBoolean("upload.verify"))
      commandDownloader.add("--verify");
    commandDownloader.add("if=" + buildPath + File.separator + className + ".hex");
    return uisp(commandDownloader);
  }

  public boolean burnBootloaderAVRISP() throws RunnerException {
    List commandDownloader = new ArrayList();
    commandDownloader.add("-dprog=" + Preferences.get("bootloader.programmer"));
    commandDownloader.add(
      "-dserial=" + (Base.isWindows() ?
        "/dev/" + Preferences.get("serial.port").toLowerCase() :
        Preferences.get("serial.port")));
    commandDownloader.add("-dspeed=" + Preferences.get("serial.burn_rate"));
    return burnBootloader(commandDownloader);
  }
  
  public boolean burnBootloaderParallel() throws RunnerException {
    List commandDownloader = new ArrayList();
    commandDownloader.add("-dprog=dapa");
    commandDownloader.add("-dlpt=" + Preferences.get("parallel.port"));
    return burnBootloader(commandDownloader);
  }
  
  protected boolean burnBootloader(Collection params) throws RunnerException {
    // I know this is ugly; apologies - that's what happens when you try to
    // write Lisp-style code in Java.
    return
      // unlock bootloader segment of flash memory
      uisp(params, Arrays.asList(new String[] {
        "--wr_lock=" + Preferences.get("bootloader.unlock_bits") })) &&
      // write fuses:
      // bootloader size of 512 words; from 0xE00-0xFFF
      // clock speed of 16 MHz, external quartz
      uisp(params, Arrays.asList(new String[] {
          "--wr_fuse_l=" + Preferences.get("bootloader.low_fuses"),
          "--wr_fuse_h=" + Preferences.get("bootloader.high_fuses") })) &&
      // upload bootloader
      uisp(params, Arrays.asList(new String[] {
          "--erase", "--upload", "--verify",
          "if=" + Preferences.get("bootloader.path") + File.separator +
          Preferences.get("bootloader.file") })) &&
      // lock bootloader segment
      uisp(params, Arrays.asList(new String[] {
        "--wr_lock=" + Preferences.get("bootloader.lock_bits") }));
  }
  
  public boolean uisp(Collection p1, Collection p2) throws RunnerException {
    ArrayList p = new ArrayList(p1);
    p.addAll(p2);
    return uisp(p);
  }
  
  public boolean uisp(Collection params) throws RunnerException {
    String userdir = System.getProperty("user.dir") + File.separator;

    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;
    notFoundError = false;
    int result=0; // pre-initialized to quiet a bogus warning from jikes
    try {
      List commandDownloader = new ArrayList();
      String avrBasePath;
      if(Base.isMacOS()) {
        avrBasePath = new String("tools/avr/bin/"); 
      }
      else if(Base.isLinux()) {
        avrBasePath = new String("");     	
      }
      else {
        avrBasePath = new String(userdir + "tools/avr/bin/"); 
      }
      commandDownloader.add(avrBasePath + "uisp");
      //commandDownloader.add((!Base.isMacOS() ? "" : userdir) + "tools/avr/bin/uisp");
      if (Preferences.getBoolean("upload.verbose"))
        commandDownloader.add("-v=4");
      commandDownloader.add("-dpart=" + Preferences.get("build.mcu"));
      commandDownloader.addAll(params);
  	  //commandDownloader.add("-v=4"); // extra verbosity for help debugging.

      // Cleanup the serial buffer
      Serial serialPort = new Serial();
      byte[] readBuffer;
      while(serialPort.available() > 0) {
        readBuffer = serialPort.readBytes();
        Thread.sleep(100);
      }
      serialPort.dispose();

      String[] commandArray = new String[commandDownloader.size()];
      commandDownloader.toArray(commandArray);
      if (Preferences.getBoolean("upload.verbose")) {
        for(int i = 0; i < commandArray.length; i++) {
          System.out.print(commandArray[i] + " ");
        }
        System.out.println();
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
