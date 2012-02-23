/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  AvrdudeUploader - uploader implementation using avrdude
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

import processing.app.Base;
import processing.app.Preferences;
import processing.app.Serial;
import processing.app.SerialException;
import static processing.app.I18n._;

import java.io.*;
import java.util.*;
import java.util.zip.*;
import javax.swing.*;
import gnu.io.*;


public class AvrdudeUploader extends Uploader  {
  public AvrdudeUploader() {
  }

  public boolean uploadUsingPreferences(String buildPath, String className, boolean usingProgrammer)
  throws RunnerException, SerialException {
    this.verbose = verbose;
    Map<String, String> boardPreferences = Base.getBoardPreferences();

    // if no protocol is specified for this board, assume it lacks a 
    // bootloader and upload using the selected programmer.
    if (usingProgrammer || boardPreferences.get("upload.protocol") == null) {
      String programmer = Preferences.get("programmer");
      Target target = Base.getTarget();

      if (programmer.indexOf(":") != -1) {
        target = Base.targetsTable.get(programmer.substring(0, programmer.indexOf(":")));
        programmer = programmer.substring(programmer.indexOf(":") + 1);
      }
      
      Collection params = getProgrammerCommands(target, programmer);
      params.add("-Uflash:w:" + buildPath + File.separator + className + ".hex:i");
      return avrdude(params);
    }

    return uploadViaBootloader(buildPath, className);  
  }
  
  private boolean uploadViaBootloader(String buildPath, String className)
  		throws RunnerException, SerialException {
    Map<String, String> boardPreferences = Base.getBoardPreferences();
    List commandDownloader = new ArrayList();
    String protocol = boardPreferences.get("upload.protocol");
    
    // avrdude wants "stk500v1" to distinguish it from stk500v2
    if (protocol.equals("stk500"))
      protocol = "stk500v1";      
    
	// need to do a little dance for Leonardo and derivatives:
	// open then close the port at the magic baudrate (usually 1200 bps) first to signal to the 
	// sketch that it should reset into bootloader.  after doing this wait a moment for the 
	// bootloader to enumerate.  On Windows, also must deal with the fact that the COM port number
	// changes from bootloader to sketch.
	String leonardoUploadPort = null;
	if (boardPreferences.get("bootloader.path").equals("caterina_LUFA")) {
    	try {    		
			long startTime = System.currentTimeMillis();
			String portsBeforeReenum[] = Serial.list();
	    	Serial.touchPort(Preferences.get("serial.port"), 1200);    		
    		if (Base.isWindows()) {	    		
    			// Windows has a complicated relationship with CDC devices.  Have to deal with 
    			// devices being slow to disconnect, slow to connect, and the fact that COM numbers
    			// change from bootloader to sketch.  To find the bootloader port we scan the list
    			// of reported ports continuously, waiting for the selected port to disappear, then
    			// waiting for a new port to appear.  Have to have long delays between calls to 
    			// Serial.list() on Windows otherwise rxtx causes a bluescreen error.
    			
	    		// Wait for the port to disappear from the list...
				long timeout = 0;
		    	while (true == Arrays.asList(Serial.list()).contains(Preferences.get("serial.port")) && ((timeout = (System.currentTimeMillis() - startTime)) < 4000)) {
	    			Thread.sleep(1000);
	    		}
				System.out.println("first timeout: " + timeout);				
	    		
				Thread.sleep(1000);
				
	    		// ...and wait for a port, any port to appear.
	    		while ((portsBeforeReenum.length != Serial.list().length) && ((timeout = (System.currentTimeMillis() - startTime)) < 8000)) {
	    			Thread.sleep(1000);
	    		}
				System.out.println("second timeout: " + timeout);	    		
	    		
	    		// Figure out which port is new.
				String portsAfterReenum[] = Serial.list();
	    		for (String port : portsAfterReenum) {
	    			if (!Arrays.asList(portsBeforeReenum).contains(port))
	    				leonardoUploadPort = port;
	    		}
				System.out.println("found leo: " + leonardoUploadPort);
    		} else {	
    			// Reenumeration is much faster and less complicated on *nix:
    			// After touching the port wait for it to disappear (timeout on this operation in
    			// case the port is either not a Leonardo, is a Leonardo with no sketch loaded, 
    			// or is a Leonardo that has already been manually reset).  Then wait for the same
    			// port to reappear in the list and proceed.
   	
   				// Wait for the port to disappear from the list...
		    	while (true == Arrays.asList(Serial.list()).contains(Preferences.get("serial.port")) && (System.currentTimeMillis() - startTime < 4000)) {
	    			Thread.sleep(10);
	    		}
	    		// ...and wait for the same-named port to come back.
	    		while (false == Arrays.asList(Serial.list()).contains(Preferences.get("serial.port")) && (System.currentTimeMillis() - startTime < 8000)) {
  			   		Thread.sleep(10);
	  		   	}
	    	}	    	
    	} catch (SerialException ex) { 
    	} catch (InterruptedException ex) { }
    }
    
    commandDownloader.add("-c" + protocol);
    if (null == leonardoUploadPort) {
	    commandDownloader.add(
    	  "-P" + (Base.isWindows() ? "\\\\.\\" : "") + Preferences.get("serial.port"));
    } else {
	    commandDownloader.add(
    	  "-P" + (Base.isWindows() ? "\\\\.\\" : "") + leonardoUploadPort);    
    }
    commandDownloader.add(
      "-b" + Integer.parseInt(boardPreferences.get("upload.speed")));
    commandDownloader.add("-D"); // don't erase
    commandDownloader.add("-Uflash:w:" + buildPath + File.separator + className + ".hex:i");

    if (boardPreferences.get("upload.disable_flushing") == null ||
        boardPreferences.get("upload.disable_flushing").toLowerCase().equals("false")) {
      flushSerialBuffer();
    }

    return avrdude(commandDownloader);
  }
  
  public boolean burnBootloader() throws RunnerException {
    String programmer = Preferences.get("programmer");
    Target target = Base.getTarget();
    if (programmer.indexOf(":") != -1) {
      target = Base.targetsTable.get(programmer.substring(0, programmer.indexOf(":")));
      programmer = programmer.substring(programmer.indexOf(":") + 1);
    }
    return burnBootloader(getProgrammerCommands(target, programmer));
  }
  
  private Collection getProgrammerCommands(Target target, String programmer) {
    Map<String, String> programmerPreferences = target.getProgrammers().get(programmer);
    List params = new ArrayList();
    params.add("-c" + programmerPreferences.get("protocol"));
    
    if ("usb".equals(programmerPreferences.get("communication"))) {
      params.add("-Pusb");
    } else if ("serial".equals(programmerPreferences.get("communication"))) {
      params.add("-P" + (Base.isWindows() ? "\\\\.\\" : "") + Preferences.get("serial.port"));
      if (programmerPreferences.get("speed") != null) {
	params.add("-b" + Integer.parseInt(programmerPreferences.get("speed")));
      }
    }
    // XXX: add support for specifying the port address for parallel
    // programmers, although avrdude has a default that works in most cases.
    
    if (programmerPreferences.get("force") != null &&
        programmerPreferences.get("force").toLowerCase().equals("true"))
      params.add("-F");
    
    if (programmerPreferences.get("delay") != null)
      params.add("-i" + programmerPreferences.get("delay"));
    
    return params;
  }
  
  protected boolean burnBootloader(Collection params)
  throws RunnerException {
    Map<String, String> boardPreferences = Base.getBoardPreferences();
    List fuses = new ArrayList();
    fuses.add("-e"); // erase the chip
    if (boardPreferences.get("bootloader.unlock_bits") != null)
      fuses.add("-Ulock:w:" + boardPreferences.get("bootloader.unlock_bits") + ":m");
    if (boardPreferences.get("bootloader.extended_fuses") != null)
      fuses.add("-Uefuse:w:" + boardPreferences.get("bootloader.extended_fuses") + ":m");
    fuses.add("-Uhfuse:w:" + boardPreferences.get("bootloader.high_fuses") + ":m");
    fuses.add("-Ulfuse:w:" + boardPreferences.get("bootloader.low_fuses") + ":m");

    if (!avrdude(params, fuses))
      return false;

    try {
      Thread.sleep(1000);
    } catch (InterruptedException e) {}
    
    Target t;
    List bootloader = new ArrayList();
    String bootloaderPath = boardPreferences.get("bootloader.path");
    
    if (bootloaderPath != null) {
      if (bootloaderPath.indexOf(':') == -1) {
        t = Base.getTarget(); // the current target (associated with the board)
      } else {
        String targetName = bootloaderPath.substring(0, bootloaderPath.indexOf(':'));
        t = Base.targetsTable.get(targetName);
        bootloaderPath = bootloaderPath.substring(bootloaderPath.indexOf(':') + 1);
      }
      
      File bootloadersFile = new File(t.getFolder(), "bootloaders");
      File bootloaderFile = new File(bootloadersFile, bootloaderPath);
      bootloaderPath = bootloaderFile.getAbsolutePath();
      
      bootloader.add("-Uflash:w:" + bootloaderPath + File.separator +
                     boardPreferences.get("bootloader.file") + ":i");
    }
    if (boardPreferences.get("bootloader.lock_bits") != null)
      bootloader.add("-Ulock:w:" + boardPreferences.get("bootloader.lock_bits") + ":m");

    if (bootloader.size() > 0)
      return avrdude(params, bootloader);
    
    return true;
  }
  
  public boolean avrdude(Collection p1, Collection p2) throws RunnerException {
    ArrayList p = new ArrayList(p1);
    p.addAll(p2);
    return avrdude(p);
  }
  
  public boolean avrdude(Collection params) throws RunnerException {
    List commandDownloader = new ArrayList();
      
    if(Base.isLinux()) {
      if ((new File(Base.getHardwarePath() + "/tools/" + "avrdude")).exists()) {
        commandDownloader.add(Base.getHardwarePath() + "/tools/" + "avrdude");
        commandDownloader.add("-C" + Base.getHardwarePath() + "/tools/avrdude.conf");
      } else {
        commandDownloader.add("avrdude");
      }
    }
    else {
      commandDownloader.add(Base.getHardwarePath() + "/tools/avr/bin/" + "avrdude");
      commandDownloader.add("-C" + Base.getHardwarePath() + "/tools/avr/etc/avrdude.conf");
    }

    if (verbose || Preferences.getBoolean("upload.verbose")) {
      commandDownloader.add("-v");
      commandDownloader.add("-v");
      commandDownloader.add("-v");
      commandDownloader.add("-v");
    } else {
      commandDownloader.add("-q");
      commandDownloader.add("-q");
    }
    commandDownloader.add("-p" + Base.getBoardPreferences().get("build.mcu"));
    commandDownloader.addAll(params);

    return executeUploadCommand(commandDownloader);
  }
}
