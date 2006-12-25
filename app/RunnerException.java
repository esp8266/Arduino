/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-05 Ben Fry and Casey Reas
  Copyright (c) 2001-04 Massachusetts Institute of Technology

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

package processing.app;

/**
 * An exception with a line number attached that occurs
 * during either compile time or run time.
 */
public class RunnerException extends Exception {
  public int file = -1;
  public int line = -1;
  public int column = -1;
  public boolean hideStackTrace;

  public RunnerException() { }

  public RunnerException(String message) {
    super(massage(message));
  }

  public RunnerException(String message, int line) {
    super(massage(message));
    this.line = line;
  }

  public RunnerException(String message, int line, int column) {
    super(massage(message));
    this.line = line;
    this.column = column;
  }

  public RunnerException(String message, int file, int line, int column) {
    super(massage(message));
    this.file = file;
    this.line = line;
    this.column = column;
  }


  /**
   * Nix the java.lang crap out of an exception message
   * because it scares the children.
   * <P>
   * This function must be static to be used with super()
   * in each of the constructors above.
   */
  static public final String massage(String msg) {
    if (msg == null)
      return "";
    if (msg.indexOf("java.lang.") == 0) {
      //int dot = msg.lastIndexOf('.');
      msg = msg.substring("java.lang.".length());
    }
    return msg;
    //return (dot == -1) ? msg : msg.substring(dot+1);
  }


  public void printStackTrace() {
    if (!hideStackTrace) {
      super.printStackTrace();
    }
  }
}
