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

package processing.app;
import java.io.*;
import java.util.*;
import java.util.zip.*;
import javax.swing.*;
import gnu.io.*;


public class AvrdudeUploader extends Uploader  {
  public AvrdudeUploader() {
  }

  // XXX: add support for uploading sketches using a programmer
  public boolean uploadUsingPreferences(String buildPath, String className)
  throws RunnerException {
    if (Preferences.get("upload.using").equals("bootloader")) {
      return uploadViaBootloader(buildPath, className);
    } else {
      Collection params = getProgrammerCommands(Preferences.get("upload.using"));
      params.add("-Uflash:w:" + buildPath + File.separator + className + ".hex:i");
      return avrdude(params);
    }
  }
  
  private boolean uploadViaBootloader(String buildPath, String className)
  throws RunnerException {
    List commandDownloader = new ArrayList();
    String protocol = Preferences.get("boards." + Preferences.get("board") + ".upload.protocol");
    
    // avrdude wants "stk500v1" to distinguish it from stk500v2
    if (protocol.equals("stk500"))
      protocol = "stk500v1";
    commandDownloader.add("-c" + protocol);
    commandDownloader.add("-P" + (Base.isWindows() ? "\\\\.\\" : "") + Preferences.get("serial.port"));
    commandDownloader.add(
      "-b" + Preferences.getInteger("boards." + Preferences.get("board") + ".upload.speed"));
    commandDownloader.add("-D"); // don't erase
    commandDownloader.add("-Uflash:w:" + buildPath + File.separator + className + ".hex:i");

    flushSerialBuffer();

    return avrdude(commandDownloader);
  }
  
  public boolean burnBootloader(String programmer) throws RunnerException {
    return burnBootloader(getProgrammerCommands(programmer));
  }
  
  private Collection getProgrammerCommands(String programmer) {
    List params = new ArrayList();
    params.add("-c" + Preferences.get("programmers." + programmer + ".protocol"));
    
    if ("usb".equals(Preferences.get("programmers." + programmer + ".communication"))) {
      params.add("-Pusb");
    } else if ("serial".equals(Preferences.get("programmers." + programmer + ".communication"))) {
      params.add("-P" + (Base.isWindows() ? "\\\\.\\" : "") + Preferences.get("serial.port"));
      // XXX: add support for specifying the baud rate for serial programmers.
    }
    // XXX: add support for specifying the port address for parallel
    // programmers, although avrdude has a default that works in most cases.
    
    if (Preferences.get("programmers." + programmer + ".delay") != null)
      params.add("-i" + Preferences.get("programmers." + programmer + ".delay"));
    
    return params;
  }
  
  protected boolean burnBootloader(Collection params)
  throws RunnerException {
    List fuses = new ArrayList();
    fuses.add("-e"); // erase the chip
    fuses.add("-Ulock:w:" + Preferences.get("boards." + Preferences.get("board") + ".bootloader.unlock_bits") + ":m");
    if (Preferences.get("boards." + Preferences.get("board") + ".bootloader.extended_fuses") != null)
      fuses.add("-Uefuse:w:" + Preferences.get("boards." + Preferences.get("board") + ".bootloader.extended_fuses") + ":m");
    fuses.add("-Uhfuse:w:" + Preferences.get("boards." + Preferences.get("board") + ".bootloader.high_fuses") + ":m");
    fuses.add("-Ulfuse:w:" + Preferences.get("boards." + Preferences.get("board") + ".bootloader.low_fuses") + ":m");

    if (!avrdude(params, fuses))
      return false;
      
    List bootloader = new ArrayList();
    bootloader.add("-Uflash:w:" + "hardware" + File.separator + "bootloaders" + File.separator +
            Preferences.get("boards." + Preferences.get("board") + ".bootloader.path") +
            File.separator + Preferences.get("boards." + Preferences.get("board") + ".bootloader.file") + ":i");
    bootloader.add("-Ulock:w:" + Preferences.get("boards." + Preferences.get("board") + ".bootloader.lock_bits") + ":m");

    return avrdude(params, bootloader);
  }
  
  public boolean avrdude(Collection p1, Collection p2) throws RunnerException {
    ArrayList p = new ArrayList(p1);
    p.addAll(p2);
    return avrdude(p);
  }
  
  public boolean avrdude(Collection params) throws RunnerException {
    List commandDownloader = new ArrayList();
    commandDownloader.add("avrdude");

    // Point avrdude at its config file since it's in a non-standard location.
    if(Base.isMacOS()) {
      commandDownloader.add("-C" + "hardware/tools/avr/etc/avrdude.conf");
    }
    else if(Base.isWindows()) {
      String userdir = System.getProperty("user.dir") + File.separator;
      commandDownloader.add("-C" + userdir + "hardware/tools/avr/etc/avrdude.conf");
    } else {
      // ???: is it better to have Linux users install avrdude themselves, in
      // a way that it can find its own configuration file?
      commandDownloader.add("-C" + "hardware/tools/avrdude.conf");
    }

    if (Preferences.getBoolean("upload.verbose")) {
      commandDownloader.add("-v");
      commandDownloader.add("-v");
      commandDownloader.add("-v");
      commandDownloader.add("-v");
    } else {
      commandDownloader.add("-q");
      commandDownloader.add("-q");
    }
    // XXX: quick hack to chop the "atmega" off of "atmega8" and "atmega168",
    // then shove an "m" at the beginning.  won't work for attiny's, etc.
    commandDownloader.add("-pm" + 
      Preferences.get("boards." + Preferences.get("board") + ".build.mcu").substring(6));
    commandDownloader.addAll(params);

    return executeUploadCommand(commandDownloader);
  }
}
