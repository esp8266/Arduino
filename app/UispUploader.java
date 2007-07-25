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


public class UispUploader extends Uploader  {
  //PdePreferences preferences;

  //Serial serialPort;
  static InputStream serialInput;
  static OutputStream serialOutput;
  //int serial; // last byte of data received

  public UispUploader() {
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
    
    flushSerialBuffer();

    return uisp(commandDownloader);
  }

  public boolean burnBootloaderAVRISP(String target) throws RunnerException {
    List commandDownloader = new ArrayList();
    commandDownloader.add("-dprog=" + Preferences.get("bootloader.programmer"));
    commandDownloader.add(
      "-dserial=" + (Base.isWindows() ?
        "/dev/" + Preferences.get("serial.port").toLowerCase() :
        Preferences.get("serial.port")));
    commandDownloader.add("-dspeed=" + Preferences.get("serial.burn_rate"));
    return burnBootloader(commandDownloader);
  }
  
  public boolean burnBootloaderParallel(String target) throws RunnerException {
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
      List commandDownloader = new ArrayList();
      commandDownloader.add("uisp");
      if (Preferences.getBoolean("upload.verbose"))
        commandDownloader.add("-v=4");
      commandDownloader.add("-dpart=" + Preferences.get("build.mcu"));
      commandDownloader.addAll(params);

      return executeUploadCommand(commandDownloader);
  }
}
