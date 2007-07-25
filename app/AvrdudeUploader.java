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

  public boolean uploadUsingPreferences(String buildPath, String className)
    throws RunnerException {
    List commandDownloader = new ArrayList();
      
    // avrdude doesn't want to read device signatures (it always gets
    // 0x000000); force it to continue uploading anyway
    commandDownloader.add("-F");

    String programmer = Preferences.get("upload.programmer");
    
    // avrdude wants "stk500v1" to distinguish it from stk500v2
    if (programmer.equals("stk500"))
      programmer = "stk500v1";
    commandDownloader.add("-c" + programmer);
    if (Preferences.get("upload.programmer").equals("dapa")) {
      // avrdude doesn't need to be told the address of the parallel port
      //commandDownloader.add("-dlpt=" + Preferences.get("parallel.port"));
    } else {
      commandDownloader.add("-P" + Preferences.get("serial.port"));
      commandDownloader.add(
        "-b" + Preferences.getInteger("serial.download_rate"));
    }
    if (Preferences.getBoolean("upload.erase"))
      commandDownloader.add("-e");
    else
      commandDownloader.add("-D");
    if (!Preferences.getBoolean("upload.verify"))
      commandDownloader.add("-V");
    commandDownloader.add("-Uflash:w:" + buildPath + File.separator + className + ".hex:i");

    flushSerialBuffer();

    return uisp(commandDownloader);
  }

  public boolean burnBootloaderAVRISP(String target) throws RunnerException {
    List commandDownloader = new ArrayList();
    commandDownloader.add("-c" +
      Preferences.get("bootloader." + target + ".programmer"));

    if (Preferences.get("bootloader." + target + ".communication").equals("usb")) {
      commandDownloader.add("-Pusb");
    } else {
      commandDownloader.add(
        "-P" + (Base.isWindows() ?
          "/dev/" + Preferences.get("serial.port").toLowerCase() :
          Preferences.get("serial.port")));
    }
    commandDownloader.add("-b" + Preferences.get("serial.burn_rate"));
    return burnBootloader(target, commandDownloader);
  }
  
  public boolean burnBootloaderParallel(String target) throws RunnerException {
    List commandDownloader = new ArrayList();
    commandDownloader.add("-dprog=dapa");
    commandDownloader.add("-dlpt=" + Preferences.get("parallel.port"));
    return burnBootloader(target, commandDownloader);
  }
  
  protected boolean burnBootloader(String target, Collection params)
    throws RunnerException
  {
    return
      // unlock bootloader segment of flash memory and write fuses
      uisp(params, Arrays.asList(new String[] {
        "-e",
        "-Ulock:w:" + Preferences.get("bootloader." + target + ".unlock_bits") + ":m",
        "-Uefuse:w:" + Preferences.get("bootloader." + target + ".extended_fuses") + ":m",
        "-Uhfuse:w:" + Preferences.get("bootloader." + target + ".high_fuses") + ":m",
        "-Ulfuse:w:" + Preferences.get("bootloader." + target + ".low_fuses") + ":m",
      })) &&
      // upload bootloader and lock bootloader segment
      uisp(params, Arrays.asList(new String[] {
          "-Uflash:w:" + Preferences.get("bootloader." + target + ".path") +
            File.separator + Preferences.get("bootloader." + target + ".file") + ":i",
          "-Ulock:w:" + Preferences.get("bootloader." + target + ".lock_bits") + ":m"
      }));
  }
  
  public boolean uisp(Collection p1, Collection p2) throws RunnerException {
    ArrayList p = new ArrayList(p1);
    p.addAll(p2);
    return uisp(p);
  }
  
  public boolean uisp(Collection params) throws RunnerException {
    List commandDownloader = new ArrayList();
    commandDownloader.add("avrdude");

    // On Windows and the Mac, we need to point avrdude at its config file
    // since it's getting installed in an unexpected location (i.e. a
    // sub-directory of wherever the user happens to stick Arduino).  On Linux,
    // avrdude will have been properly installed by the distribution's package
    // manager and should be able to find its config file.
    if(Base.isMacOS()) {
      commandDownloader.add("-C" + "tools/avr/etc/avrdude.conf");
    }
    else if(Base.isWindows()) {
      String userdir = System.getProperty("user.dir") + File.separator;
      commandDownloader.add("-C" + userdir + "tools/avr/etc/avrdude.conf");
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
    commandDownloader.add("-pm" + Preferences.get("build.mcu").substring(6));
    commandDownloader.addAll(params);

    return executeUploadCommand(commandDownloader);
  }
}
