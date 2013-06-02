/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Uploader - abstract uploading baseclass (common to both uisp and avrdude)
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

package cc.arduino.packages;

import static processing.app.I18n._;

import java.util.Collection;

import processing.app.I18n;
import processing.app.Preferences;
import processing.app.debug.MessageConsumer;
import processing.app.debug.MessageSiphon;
import processing.app.debug.RunnerException;

public abstract class Uploader implements MessageConsumer  {

  private String error = null;

  protected boolean verbose = Preferences.getBoolean("upload.verbose");

  public abstract boolean uploadUsingPreferences(String buildPath, String className, boolean usingProgrammer)
    throws RunnerException;

  public abstract boolean burnBootloader() throws RunnerException;

  public boolean requiresAuthorization() {
    return false;
  }

  public String getAuthorizationKey() {
    return null;
  }

  protected boolean executeUploadCommand(Collection<String> command)
      throws RunnerException {
    return executeUploadCommand(command.toArray(new String[0]));
  }

  protected boolean executeUploadCommand(String command[])
      throws RunnerException {
    notFoundError = false;
    int result = -1;

    try {
      if (verbose) {
        for (String c : command)
          System.out.print(c + " ");
        System.out.println();
      }
      Process process = Runtime.getRuntime().exec(command);
      new MessageSiphon(process.getInputStream(), this);
      new MessageSiphon(process.getErrorStream(), this);

      // wait for the process to finish.
      result = process.waitFor();
    } catch (Exception e) {
      e.printStackTrace();
    }
      
    if (error != null) {
      RunnerException exception = new RunnerException(error);
      exception.hideStackTrace();
      throw exception;
    }

    return result == 0;      
  }

  boolean notFoundError;

  public void message(String s) {
    // selectively suppress a bunch of avrdude output for AVR109/Caterina that should already be quelled but isn't
    if (!verbose && (
      s.indexOf("Connecting to programmer:") != -1 ||
      s.indexOf("Found programmer: Id = \"CATERIN\"; type = S") != -1 ||
      s.indexOf("Software Version = 1.0; No Hardware Version given.") != -1 ||
      s.indexOf("Programmer supports auto addr increment.") != -1 ||
      s.indexOf("Programmer supports buffered memory access with buffersize=128 bytes.") != -1 ||
      s.indexOf("Programmer supports the following devices:") != -1 ||
      s.indexOf("Device code: 0x44") != -1))
      s = "";

    System.err.print(s);

    // ignore cautions
    if (s.contains("Error")) {
      notFoundError = true;
      return;
    }
    if (notFoundError) {
      error = I18n.format(_("the selected serial port {0} does not exist or your board is not connected"), s);
      return;
    }
    if (s.contains("Device is not responding")) {
      error = _("Device is not responding, check the right serial port is selected or RESET the board right before exporting");
      return;
    }
    if (s.contains("Programmer is not responding") ||
        s.contains("programmer is not responding") ||
        s.contains("protocol error") ||
        s.contains("avrdude: ser_open(): can't open device") ||
        s.contains("avrdude: ser_drain(): read error") ||
        s.contains("avrdude: ser_send(): write error") ||
        s.contains("avrdude: error: buffered memory access not supported.")) {
      error = _("Problem uploading to board.  See http://www.arduino.cc/en/Guide/Troubleshooting#upload for suggestions.");
      return;
    }
    if (s.contains("Expected signature")) {
      error = _("Wrong microcontroller found.  Did you select the right board from the Tools > Board menu?");
      return;
    }
  }
}
