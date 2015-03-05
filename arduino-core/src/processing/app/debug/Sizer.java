/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Sizer - computes the size of a .hex file
  Part of the Arduino project - http://www.arduino.cc/

  Copyright (c) 2006 David A. Mellis
  Copyright (c) 2011 Cristian Maglie <c.maglie@arduino.cc>

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

package processing.app.debug;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import processing.app.helpers.PreferencesMap;
import processing.app.helpers.ProcessUtils;
import processing.app.helpers.StringReplacer;

public class Sizer implements MessageConsumer {
  private long textSize;
  private long dataSize;
  private long eepromSize;
  private RunnerException exception;
  private PreferencesMap prefs;
  private String firstLine;
  private Pattern textPattern;
  private Pattern dataPattern;
  private Pattern eepromPattern;
  
  public Sizer(PreferencesMap _prefs) {
    prefs = _prefs;
    textPattern = Pattern.compile(prefs.get("recipe.size.regex"));
    dataPattern = null;
    String pref = prefs.get("recipe.size.regex.data");
    if (pref != null)
      dataPattern = Pattern.compile(pref);
    eepromPattern = null;
    pref = prefs.get("recipe.size.regex.eeprom");
    if (pref != null)
      eepromPattern = Pattern.compile(pref);
  }
  
  public long[] computeSize() throws RunnerException {

    int r = 0;
    try {
      String pattern = prefs.get("recipe.size.pattern");
      String cmd[] = StringReplacer.formatAndSplit(pattern, prefs, true);

      exception = null;
      textSize = -1;
      dataSize = -1;
      eepromSize = -1;
      Process process = ProcessUtils.exec(cmd);
      MessageSiphon in = new MessageSiphon(process.getInputStream(), this);
      MessageSiphon err = new MessageSiphon(process.getErrorStream(), this);

      boolean running = true;
      while(running) {
        try {
          in.join();
          err.join();
          r = process.waitFor();
          running = false;
        } catch (InterruptedException intExc) { }
      }
    } catch (Exception e) {
      // The default Throwable.toString() never returns null, but apparently
      // some sub-class has overridden it to do so, thus we need to check for
      // it.  See: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1166589459
      exception = new RunnerException(
        (e.toString() == null) ? e.getClass().getName() + r : e.toString() + r);
    }
    
    if (exception != null)
      throw exception;
      
    if (textSize == -1)
      throw new RunnerException(firstLine);
      
    return new long[] { textSize, dataSize, eepromSize };
  }
  
  public void message(String s) {
    if (firstLine == null)
      firstLine = s;
    Matcher textMatcher = textPattern.matcher(s.trim());
    if (textMatcher.matches()) {
      if (textSize < 0)
        textSize = 0;
      textSize += Long.parseLong(textMatcher.group(1));
    }
    if(dataPattern != null) {
      Matcher dataMatcher = dataPattern.matcher(s.trim());
      if (dataMatcher.matches()) {
        if (dataSize < 0)
          dataSize = 0;
        dataSize += Long.parseLong(dataMatcher.group(1));
      }
    }
    if(eepromPattern != null) {
      Matcher eepromMatcher = eepromPattern.matcher(s.trim());
      if (eepromMatcher.matches()) {
        if (eepromSize < 0)
          eepromSize = 0;
        eepromSize += Long.parseLong(eepromMatcher.group(1));
      }
    }
  }
}
