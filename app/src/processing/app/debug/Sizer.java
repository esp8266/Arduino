/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Sizer - computes the size of a .hex file
  Part of the Arduino project - http://www.arduino.cc/

  Copyright (c) 2006 David A. Mellis

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

import java.io.*;
import java.util.*;

public class Sizer implements MessageConsumer {
  private String buildPath, sketchName;
  private String firstLine;
  private long size;
  private RunnerException exception;

  public Sizer(String buildPath, String sketchName) {
    this.buildPath = buildPath;
    this.sketchName = sketchName;
  }
  
  public long computeSize() throws RunnerException {
    String avrBasePath = Base.getAvrBasePath();
    String commandSize[] = new String[] {
      avrBasePath + "avr-size",
      " "
    };
    
    commandSize[1] = buildPath + File.separator + sketchName + ".hex";

    int r = 0;
    try {
      exception = null;
      size = -1;
      firstLine = null;
      Process process = Runtime.getRuntime().exec(commandSize);
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
    else {
      StringTokenizer st = new StringTokenizer(s, " ");
      try {
        st.nextToken();
        st.nextToken();
        st.nextToken();
        size = (new Integer(st.nextToken().trim())).longValue();
      } catch (NoSuchElementException e) {
        exception = new RunnerException(e.toString());
      } catch (NumberFormatException e) {
        exception = new RunnerException(e.toString());
      }
    }
  }
}