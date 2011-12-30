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

import java.io.File;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import processing.app.Base;
import processing.app.Preferences;
import processing.app.SerialException;
import processing.app.helpers.PreferencesMap;

public class AvrdudeUploader extends Uploader  {

  public boolean uploadUsingPreferences(String buildPath, String className, boolean usingProgrammer)
  throws RunnerException, SerialException {
    PreferencesMap boardPreferences = Base.getBoardPreferences();

    // if no protocol is specified for this board, assume it lacks a 
    // bootloader and upload using the selected programmer.
    if (usingProgrammer || boardPreferences.get("upload.protocol") == null) {
      String programmer = Preferences.get("programmer");
      TargetPlatform targetPlatform = Base.getTarget();

      if (programmer.contains(":")) {
      	String[] split = programmer.split(":");
      	targetPlatform = Base.getTargetPlatform(split[0], split[1]);
        programmer = split[2];
      }
      
      Collection<String> params = getProgrammerCommands(targetPlatform, programmer);
      params.add("-Uflash:w:" + buildPath + File.separator + className + ".hex:i");
      return avrdude(params);
    }

    return uploadViaBootloader(buildPath, className);
  }
  
  private boolean uploadViaBootloader(String buildPath, String className)
  throws RunnerException, SerialException {
    PreferencesMap boardPreferences = Base.getBoardPreferences();
    List<String> commandDownloader = new ArrayList<String>();
    String protocol = boardPreferences.get("upload.protocol");
    
    // avrdude wants "stk500v1" to distinguish it from stk500v2
    if (protocol.equals("stk500"))
      protocol = "stk500v1";
    commandDownloader.add("-c" + protocol);
    commandDownloader.add(
      "-P" + (Base.isWindows() ? "\\\\.\\" : "") + Preferences.get("serial.port"));
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
    TargetPlatform targetPlatform = Base.getTarget();
    if (programmer.contains(":")) {
    	String[] split = programmer.split(":");
    	targetPlatform = Base.getTargetPlatform(split[0], split[1]);
      programmer = split[2];
    }
    return burnBootloader(getProgrammerCommands(targetPlatform, programmer));
  }
  
  private Collection<String> getProgrammerCommands(TargetPlatform target, String programmer) {
    PreferencesMap programmerPreferences = target.getProgrammers().get(programmer);
    List<String> params = new ArrayList<String>();
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
  
  protected boolean burnBootloader(Collection<String> params)
  throws RunnerException {
    PreferencesMap boardPreferences = Base.getBoardPreferences();
    List<String> fuses = new ArrayList<String>();
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
    
    List<String> bootloader = new ArrayList<String>();
    String bootloaderPath = boardPreferences.get("bootloader.path");
    
		if (bootloaderPath != null) {
			TargetPlatform targetPlatform;
			if (bootloaderPath.contains(":")) {
				// the current target (associated with the board)
				targetPlatform = Base.getTarget();
			} else {
				String[] split = bootloaderPath.split(":", 3);
				targetPlatform = Base.getTargetPlatform(split[0], split[1]);
				bootloaderPath = split[2];
			}

			File bootloadersFile = new File(targetPlatform.getFolder(), "bootloaders");
			File bootloaderFile = new File(bootloadersFile, bootloaderPath);
			bootloaderPath = bootloaderFile.getAbsolutePath();

			bootloader.add("-Uflash:w:" + bootloaderPath + File.separator
					+ boardPreferences.get("bootloader.file") + ":i");
		}
    if (boardPreferences.get("bootloader.lock_bits") != null)
      bootloader.add("-Ulock:w:" + boardPreferences.get("bootloader.lock_bits") + ":m");

    if (bootloader.size() > 0)
      return avrdude(params, bootloader);
    
    return true;
  }
  
  public boolean avrdude(Collection<String> p1, Collection<String> p2) throws RunnerException {
    List<String> p = new ArrayList<String>(p1);
    p.addAll(p2);
    return avrdude(p);
  }
  
  public boolean avrdude(Collection<String> params) throws RunnerException {
    List<String> commandDownloader = new ArrayList<String>();
      
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
