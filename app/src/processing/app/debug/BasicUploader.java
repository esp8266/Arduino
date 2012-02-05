/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  BasicUploader - generic command line uploader implementation
  Part of the Arduino project - http://www.arduino.cc/

  Copyright (c) 2004-05
  Hernando Barragan
  Copyright (c) 2012
  Cristian Maglie <c.maglie@bug.st>

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
import processing.app.SerialException;
import processing.app.helpers.PreferencesMap;
import processing.app.helpers.StringReplacer;

public class BasicUploader extends Uploader  {

  public boolean uploadUsingPreferences(String buildPath, String className,
                                        boolean usingProgrammer)
      throws RunnerException, SerialException {
    // FIXME: Preferences should be reorganized
    TargetPlatform targetPlatform = Base.getTargetPlatform();
    PreferencesMap prefs = Preferences.getMap();
    prefs.putAll(Base.getBoardPreferences());
    prefs.putAll(targetPlatform.getTool(prefs.get("upload.tool")));

    // if no protocol is specified for this board, assume it lacks a 
    // bootloader and upload using the selected programmer.
    if (usingProgrammer || prefs.get("upload.protocol") == null) {
      return uploadUsingProgrammer(buildPath, className);
    }

    prefs.put("build.path", buildPath);
    prefs.put("build.project_name", className);
    if (verbose)
      prefs.put("upload.verbose", prefs.get("upload.params.verbose"));
    else
      prefs.put("upload.verbose", prefs.get("upload.params.quiet"));

    try {
//      if (prefs.get("upload.disable_flushing") == null
//          || prefs.get("upload.disable_flushing").toLowerCase().equals("false")) {
//        flushSerialBuffer();
//      }

      String pattern = prefs.get("upload.pattern");
      String[] cmd = StringReplacer.formatAndSplit(pattern, prefs, true);
      return executeUploadCommand(cmd);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
  }

  public boolean uploadUsingProgrammer(String buildPath, String className)
      throws RunnerException {

    TargetPlatform targetPlatform = Base.getTargetPlatform();
    String programmer = Preferences.get("programmer");
    if (programmer.contains(":")) {
      String[] split = programmer.split(":", 2);
      targetPlatform = Base.getCurrentTargetPlatformFromPackage(split[0]);
      programmer = split[1];
    }

    PreferencesMap prefs = Preferences.getMap();
    prefs.putAll(Base.getBoardPreferences());
    prefs.putAll(targetPlatform.getProgrammer(programmer));
    prefs.putAll(targetPlatform.getTool(prefs.get("program.tool")));

    prefs.put("build.path", buildPath);
    prefs.put("build.project_name", className);

    if (verbose)
      prefs.put("program.verbose", prefs.get("program.params.verbose"));
    else
      prefs.put("program.verbose", prefs.get("program.params.quiet"));

    try {
      // if (prefs.get("program.disable_flushing") == null
      // || prefs.get("program.disable_flushing").toLowerCase().equals("false"))
      // {
      // flushSerialBuffer();
      // }

      String pattern = prefs.get("program.pattern");
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
      targetPlatform = Base.getCurrentTargetPlatformFromPackage(split[0]);
      programmer = split[1];
    }
    
    PreferencesMap prefs = Preferences.getMap();
    prefs.putAll(Base.getBoardPreferences());
    prefs.putAll(targetPlatform.getProgrammer(programmer));
    prefs.putAll(targetPlatform.getTool(prefs.get("bootloader.tool")));
    if (verbose)
      prefs.put("bootloader.verbose", prefs.get("bootloader.params.verbose"));
    else
      prefs.put("bootloader.verbose", prefs.get("bootloader.params.quiet"));

    try {
      // if (prefs.get("program.disable_flushing") == null
      // || prefs.get("program.disable_flushing").toLowerCase().equals("false"))
      // {
      // flushSerialBuffer();
      // }

      prefs.put("bootloader.params", prefs.get("bootloader.erase.params"));
      String pattern = prefs.get("bootloader.pattern");
      String[] cmd = StringReplacer.formatAndSplit(pattern, prefs, true);
      if (!executeUploadCommand(cmd))
        return false;

      prefs.put("bootloader.params", prefs.get("bootloader.write.params"));
      pattern = prefs.get("bootloader.pattern");
      cmd = StringReplacer.formatAndSplit(pattern, prefs, true);
      return executeUploadCommand(cmd);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
  }
}
