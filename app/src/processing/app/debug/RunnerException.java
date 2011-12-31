/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-08 Ben Fry and Casey Reas
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

package processing.app.debug;


/**
 * An exception with a line number attached that occurs
 * during either compile time or run time.
 */
@SuppressWarnings("serial")
public class RunnerException extends Exception /*RuntimeException*/ {
  protected String message;
  protected int codeIndex;
  protected int codeLine;
  protected int codeColumn;
  protected boolean showStackTrace;

  
  public RunnerException(String message) {
    this(message, true);
  }

  public RunnerException(String message, boolean showStackTrace) {
    this(message, -1, -1, -1, showStackTrace);
  }

  public RunnerException(String message, int file, int line) {
    this(message, file, line, -1, true);
  }

  
  public RunnerException(String message, int file, int line, int column) {
    this(message, file, line, column, true);
  }
  
  
  public RunnerException(String message, int file, int line, int column, 
                         boolean showStackTrace) {
    this.message = message;
    this.codeIndex = file;
    this.codeLine = line;
    this.codeColumn = column;
    this.showStackTrace = showStackTrace;
  }
  
  
  public RunnerException(Exception e) {
    this(e.getMessage(), true);
  }

  /** 
   * Override getMessage() in Throwable, so that I can set 
   * the message text outside the constructor.
   */
  public String getMessage() {
    return message;
  }
  
  
  public void setMessage(String message) {
    this.message = message;
  }
  
  
  public int getCodeIndex() {
    return codeIndex;
  }
  
  
  public void setCodeIndex(int index) {
    codeIndex = index;
  }
  
  
  public boolean hasCodeIndex() {
    return codeIndex != -1;
  }
  
  
  public int getCodeLine() {
    return codeLine;
  }
  
  
  public void setCodeLine(int line) {
    this.codeLine = line;
  }
  
  
  public boolean hasCodeLine() {
    return codeLine != -1;
  }
  
  
  public void setCodeColumn(int column) {
    this.codeColumn = column;
  }
  
  
  public int getCodeColumn() {
    return codeColumn;
  }

  
  public void showStackTrace() {
    showStackTrace = true;
  }
  
  
  public void hideStackTrace() {
    showStackTrace = false;
  }
  

  /**
   * Nix the java.lang crap out of an exception message
   * because it scares the children.
   * <P>
   * This function must be static to be used with super()
   * in each of the constructors above.
   */
  /*
  static public final String massage(String msg) {
    if (msg.indexOf("java.lang.") == 0) {
      //int dot = msg.lastIndexOf('.');
      msg = msg.substring("java.lang.".length());
    }
    return msg;
    //return (dot == -1) ? msg : msg.substring(dot+1);
  }
  */


  public void printStackTrace() {
    if (showStackTrace) {
      super.printStackTrace();
    }
  }
}
