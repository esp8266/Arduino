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
import processing.app.helpers.StringReplacer;

public class AvrdudeUploader extends Uploader  {

  public boolean uploadUsingPreferences(String buildPath, String className, boolean usingProgrammer)
  throws RunnerException, SerialException {
    // FIXME: Preferences should be reorganized
    PreferencesMap prefs = Preferences.getMap();
    prefs.putAll(Base.getBoardPreferences());

    // if no protocol is specified for this board, assume it lacks a 
    // bootloader and upload using the selected programmer.
    if (usingProgrammer || prefs.get("upload.protocol") == null) {
      return uploadUsingProgrammer(buildPath, className);
    }

    prefs.put("build.path", buildPath);
    prefs.put("build.project_name", className);

    TargetPlatform targetPlatform = Base.getTargetPlatform();

    prefs.putAll(targetPlatform.getTool(prefs.get("upload.tool")));
    if (verbose)
      prefs.put("upload.verbose", prefs.get("upload.params.verbose"));
    else
      prefs.put("upload.verbose", prefs.get("upload.params.quiet"));

    String pattern = prefs.get("upload.pattern");
    try {
      if (prefs.get("upload.disable_flushing") == null
          || prefs.get("upload.disable_flushing").toLowerCase().equals("false")) {
        flushSerialBuffer();
      }

      String[] cmd = StringReplacer.formatAndSplit(pattern, prefs, true);
      return executeUploadCommand(cmd);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
  }

  public boolean uploadUsingProgrammer(String buildPath, String className)
      throws RunnerException {
    
    String programmer = Preferences.get("programmer");
    TargetPlatform targetPlatform = Base.getTargetPlatform();
    if (programmer.contains(":")) {
      String[] split = programmer.split(":", 2);
      targetPlatform = Base.getTargetPlatform(split[0], Preferences
          .get("target_platform"));
      programmer = split[1];
    }

    PreferencesMap prefs = Preferences.getMap();
    prefs.putAll(Base.getBoardPreferences());
    prefs.putAll(targetPlatform.getProgrammers().get(programmer));
    
    prefs.put("build.path", buildPath);
    prefs.put("build.project_name", className);

    PreferencesMap programmers = targetPlatform.getPreferences()
        .createSubTree("programmers");
    prefs.putAll(programmers.createSubTree(prefs.get("program.tool")));

    if (verbose)
      prefs.put("program.verbose", prefs.get("program.params.verbose"));
    else
      prefs.put("program.verbose", prefs.get("program.params.quiet"));

    String pattern = prefs.get("program.pattern");
    try {
      if (prefs.get("program.disable_flushing") == null
          || prefs.get("program.disable_flushing").toLowerCase().equals("false")) {
        flushSerialBuffer();
      }

      String[] cmd = StringReplacer.formatAndSplit(pattern, prefs, true);
      return executeUploadCommand(cmd);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
  }
  
  public boolean burnBootloader() throws RunnerException {
    String programmer = Preferences.get("programmer");
    TargetPlatform targetPlatform = Base.getTargetPlatform();
    if (programmer.contains(":")) {
      String[] split = programmer.split(":", 2);
      targetPlatform = Base.getTargetPlatform(split[0], Preferences
          .get("target_platform"));
      programmer = split[1];
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
      params.add("-P" + (Base.isWindows() ? "\\\\.\\" : "")
                 + Preferences.get("serial.port"));
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
    List<String> fuses = new ArrayList<String>(params);
    fuses.add("-e"); // erase the chip
    if (boardPreferences.get("bootloader.unlock_bits") != null)
      fuses.add("-Ulock:w:" + boardPreferences.get("bootloader.unlock_bits") + ":m");
    if (boardPreferences.get("bootloader.extended_fuses") != null)
      fuses.add("-Uefuse:w:" + boardPreferences.get("bootloader.extended_fuses") + ":m");
    fuses.add("-Uhfuse:w:" + boardPreferences.get("bootloader.high_fuses") + ":m");
    fuses.add("-Ulfuse:w:" + boardPreferences.get("bootloader.low_fuses") + ":m");

    if (!avrdude(fuses))
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
        targetPlatform = Base.getTargetPlatform();
      } else {
        String[] split = bootloaderPath.split(":", 2);
        targetPlatform = Base.getTargetPlatform(split[0], Preferences
            .get("target_platform"));
        bootloaderPath = split[1];
      }

      File bootloadersFile = new File(targetPlatform.getFolder(), "bootloaders");
      File bootloaderFile = new File(bootloadersFile, bootloaderPath);
      bootloaderPath = bootloaderFile.getAbsolutePath();

      bootloader.add("-Uflash:w:" + bootloaderPath + File.separator +
          boardPreferences.get("bootloader.file") + ":i");
    }
    if (boardPreferences.get("bootloader.lock_bits") != null)
      bootloader.add("-Ulock:w:" + boardPreferences.get("bootloader.lock_bits") + ":m");

    if (bootloader.size() > 0) {
      params.addAll(bootloader);
      return avrdude(params);
    }
    
    return true;
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
