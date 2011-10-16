/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 AvrdudeUploader - uploader implementation using avrdude
 Part of the Arduino project - http://www.arduino.cc/

 Copyright (c) 2011 Cristian Maglie

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

public class BossaCUploader extends Uploader {

  public boolean uploadUsingPreferences(String buildPath, String className,
                                        boolean usingProgrammer)
      throws RunnerException, SerialException {
    List<String> commandDownloader = new ArrayList<String>();
    String port = Preferences.get("serial.port");
    if (port.startsWith("/dev/"))
      port = port.substring(5);
    commandDownloader.add("--port=" + port);
    commandDownloader.add("-e");
    commandDownloader.add("-w");
    commandDownloader.add("-s");
    commandDownloader.add("-l");
    commandDownloader.add(buildPath + File.separator + className + ".bin");

    return bossac(commandDownloader);
  }

  public boolean burnBootloader() {
    return true;
  }

  public boolean bossac(Collection params) throws RunnerException {
    List commandDownloader = new ArrayList();
    commandDownloader.add("bossac");

    if (verbose || Preferences.getBoolean("upload.verbose")) {
      commandDownloader.add("-i");
      commandDownloader.add("-d");
    }
    commandDownloader.addAll(params);

    return executeUploadCommand(commandDownloader);
  }

  protected boolean executeUploadCommand(List<String> cmdParams)
      throws RunnerException {

    try {
      String avrBasePath = Base.getHardwarePath() + "/tools/";

      String[] cmdArray = cmdParams.toArray(new String[0]);
      cmdArray[0] = avrBasePath + cmdArray[0];

      if (verbose || Preferences.getBoolean("upload.verbose")) {
        for (String cmd : cmdArray)
          System.out.print(cmd + " ");
        System.out.println();
      }

      Process bossac = Runtime.getRuntime().exec(cmdArray);
      new MessageSiphon(bossac.getInputStream(), this);
      new MessageSiphon(bossac.getErrorStream(), this);

      // wait for the process to finish. if interrupted
      // before waitFor returns, continue waiting
      int result = bossac.waitFor();
      if (result != 0)
        return false;
      return true;
    } catch (Exception e) {
      e.printStackTrace();
      return false;
    }
  }

}
