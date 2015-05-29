/*
 * This file is part of Arduino.
 *
 * Code inspired by this tutorial http://wiki.netbeans.org/Splash_Screen_Beginner_Tutorial. License says "You may modify and use it as you wish."
 *
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
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
 */

package cc.arduino.view;

import java.awt.*;
import java.awt.geom.Rectangle2D;
import java.util.Map;

public class SplashScreenHelper {

  private final Map desktopHints;
  private final SplashScreen splash;
  private Rectangle2D.Double splashTextArea;
  private Graphics2D splashGraphics;

  public SplashScreenHelper(SplashScreen splash) {
    this.splash = splash;
    Toolkit tk = Toolkit.getDefaultToolkit();
    desktopHints = (Map) tk.getDesktopProperty("awt.font.desktophints");
  }

  public void splashText(String str) {
    if (splash == null) {
      printText(str);
      return;
    }
    if (!splash.isVisible()) {
      return;
    }

    if (splashTextArea == null) {
      // stake out some area for our status information
      splashTextArea = new Rectangle2D.Double(0, 300, 520, 30);

      // create the Graphics environment for drawing status info
      splashGraphics = splash.createGraphics();

      if (desktopHints != null) {
        splashGraphics.addRenderingHints(desktopHints);
      }
    }

    // erase the last status text
    splashGraphics.setPaint(new Color(245, 245, 245));
    splashGraphics.fill(splashTextArea);

    // draw the text
    splashGraphics.setPaint(Color.BLACK);
    FontMetrics metrics = splashGraphics.getFontMetrics();
    splashGraphics.drawString(str, (int) splashTextArea.getX() + 10, (int) splashTextArea.getY() + (30 - metrics.getHeight()) + 4);

    // make sure it's displayed
    splash.update();
  }

  public void close() {
    if (splash == null) {
      return;
    }
    splash.close();
  }

  private void printText(String str) {
    System.err.println(str);
  }

}
