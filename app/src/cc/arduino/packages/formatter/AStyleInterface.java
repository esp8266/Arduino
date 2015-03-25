/*
 * This file is part of Arduino.
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
 */

package cc.arduino.packages.formatter;

import processing.app.Base;

import java.io.File;

public class AStyleInterface {

  static {
    File astyleLib = new File(Base.getContentFile("lib"), System.mapLibraryName("astylej"));
    String astylePath = astyleLib.getAbsolutePath();
    try {
      System.load(astylePath);
    } catch (UnsatisfiedLinkError e) {
      e.printStackTrace();
      System.out.println(e.getMessage());
      System.out.println("Cannot load native library " + astylePath);
      System.out.println("The program has terminated!");
      System.exit(1);
    }
  }

  /**
   * Calls the AStyleMain function in Artistic Style.
   *
   * @param textIn  A string containing the source code to be formatted.
   * @param options A string of options to Artistic Style.
   * @return A String containing the formatted source from Artistic Style.
   */
  public native String AStyleMain(String textIn, String options);

  /**
   * Calls the AStyleGetVersion function in Artistic Style.
   *
   * @return A String containing the version number of Artistic Style.
   */
  public native String AStyleGetVersion();

  /**
   * Error handler for messages from Artistic Style.
   * This method is called only if there are errors when AStyleMain is called.
   * This is for debugging and there should be no errors when the calling
   * parameters are correct.
   * Changing the method name requires changing Artistic Style.
   * Signature: (ILjava/lang/String;)V.
   *
   * @param errorNumber  The error number from Artistic Style.
   * @param errorMessage The error message from Artistic Style.
   */
  private void ErrorHandler(int errorNumber, String errorMessage) {
    System.out.println("AStyle error " + String.valueOf(errorNumber) + " - " + errorMessage);
  }

}
