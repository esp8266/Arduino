/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Sizer - computes the size of a .hex file
  Part of the Arduino project - http://www.arduino.cc/

  Copyright (c) 2006 David A. Mellis
  Copyright (c) 2011 Cristian Maglie <c.maglie@bug.st>

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

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import processing.app.helpers.PreferencesMap;
import processing.app.helpers.StringReplacer;

public class Sizer implements MessageConsumer {
  private long size;
  private RunnerException exception;
  private PreferencesMap prefs;
  private String firstLine;
  private Pattern pattern;
  
  public Sizer(PreferencesMap _prefs) {
    prefs = _prefs;
    pattern = Pattern.compile(prefs.get("recipe.size.regex"));
  }
  
  public long computeSize() throws RunnerException {

    int r = 0;
    try {
      String pattern = prefs.get("recipe.size.pattern");
      String cmd[] = StringReplacer.formatAndSplit(pattern, prefs, true);

      exception = null;
      size = -1;
      Process process = Runtime.getRuntime().exec(cmd);
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
      
    if (size == -1)
      throw new RunnerException(firstLine);
      
    return size;
  }
  
  public void message(String s) {
    if (firstLine == null)
      firstLine = s;
    Matcher matcher = pattern.matcher(s.trim());
    if (matcher.matches())
      size = Long.parseLong(matcher.group(1));
  }
}