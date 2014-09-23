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
*/

package cc.arduino.packages;

import processing.app.I18n;
import processing.app.PreferencesData;
import processing.app.debug.MessageConsumer;
import processing.app.debug.MessageSiphon;
import processing.app.debug.RunnerException;
import processing.app.helpers.ProcessUtils;
import processing.app.helpers.StringUtils;

import java.io.File;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import static processing.app.I18n._;

public abstract class Uploader implements MessageConsumer {

  private static final List<String> STRINGS_TO_SUPPRESS;
  private static final List<String> AVRDUDE_PROBLEMS;

  static {
    STRINGS_TO_SUPPRESS = Arrays.asList("Connecting to programmer:",
            "Found programmer: Id = \"CATERIN\"; type = S",
            "Software Version = 1.0; No Hardware Version given.",
            "Programmer supports auto addr increment.",
            "Programmer supports buffered memory access with buffersize=128 bytes.",
            "Programmer supports the following devices:", "Device code: 0x44");

    AVRDUDE_PROBLEMS = Arrays.asList("Programmer is not responding",
            "programmer is not responding",
            "protocol error", "avrdude: ser_open(): can't open device",
            "avrdude: ser_drain(): read error",
            "avrdude: ser_send(): write error",
            "avrdude: error: buffered memory access not supported.");
  }

  protected final boolean verbose;

  private String error;
  protected boolean notFoundError;
  protected boolean noUploadPort;

  protected Uploader() {
    this.verbose = PreferencesData.getBoolean("upload.verbose");
    init(false);
  }

  protected Uploader(boolean nup) {
    this.verbose = PreferencesData.getBoolean("upload.verbose");
    init(nup);
  }

  private void init(boolean nup) {
    this.error = null;
    this.notFoundError = false;
    this.noUploadPort = nup;
  }

  public abstract boolean uploadUsingPreferences(File sourcePath, String buildPath, String className, boolean usingProgrammer, List<String> warningsAccumulator) throws Exception;

  public abstract boolean burnBootloader() throws Exception;

  public boolean requiresAuthorization() {
    return false;
  }

  public String getAuthorizationKey() {
    return null;
  }

  protected boolean executeUploadCommand(Collection<String> command) throws Exception {
    return executeUploadCommand(command.toArray(new String[command.size()]));
  }

  protected boolean executeUploadCommand(String command[]) throws Exception {
    // Skip empty commands
    if (command == null || command.length == 0)
      return true;

    notFoundError = false;
    int result = -1;

    try {
      if (verbose) {
        for (String c : command)
          System.out.print(c + " ");
        System.out.println();
      }
      Process process = ProcessUtils.exec(command);
      new MessageSiphon(process.getInputStream(), this, 100);
      new MessageSiphon(process.getErrorStream(), this, 100);

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

  public void message(String s) {
    // selectively suppress a bunch of avrdude output for AVR109/Caterina that should already be quelled but isn't
    if (!verbose && StringUtils.stringContainsOneOf(s, STRINGS_TO_SUPPRESS)) {
      s = "";
    }

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
    if (StringUtils.stringContainsOneOf(s, AVRDUDE_PROBLEMS)) {
      error = _("Problem uploading to board.  See http://www.arduino.cc/en/Guide/Troubleshooting#upload for suggestions.");
      return;
    }
    if (s.contains("Expected signature")) {
      error = _("Wrong microcontroller found.  Did you select the right board from the Tools > Board menu?");
      return;
    }
  }
}
