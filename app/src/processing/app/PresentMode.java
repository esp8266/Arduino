/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2005-09 Ben Fry and Casey Reas

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

import java.awt.*;
import java.awt.event.*;
import java.util.Vector;
import javax.swing.*;


/**
 * Helper class for full-screen presentation mode.
 */
public class PresentMode {

  static GraphicsDevice devices[];

  /**
   * Index of the default display device, probably the one that p5 was
   * started from.
   */
  static int defaultIndex;

  /**
   * Menu object for preferences window
   */
  //JMenu preferencesMenu;
  static JComboBox selector;

  /**
   * Index of the currently selected display to be used for present mode.
   */
  static GraphicsDevice device;


  static {
    GraphicsEnvironment environment =
      GraphicsEnvironment.getLocalGraphicsEnvironment();
    devices = environment.getScreenDevices();
    GraphicsDevice defaultDevice = environment.getDefaultScreenDevice();

    Vector<String> names = new Vector<String>();
    for (int i = 0; i < devices.length; i++) {
      String name = String.valueOf(i + 1);
      if (devices[i] == defaultDevice) {
        defaultIndex = i;
        name += " (default)";
      }
      names.add(name);
    }

    selector = new JComboBox(names);
    selector.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          int index = selector.getSelectedIndex();
          //device = devices[index];
          PreferencesData.setInteger("run.present.display", index + 1);
        }
      });
  }


  static public JComboBox getSelector() {
    int deviceIndex = PreferencesData.getInteger("run.present.display") - 1;
    selector.setSelectedIndex(deviceIndex);
    return selector;
  }


  /*
  static public JFrame create() {
    int deviceIndex = PrePreferences.getInteger("run.present.display") - 1;
    if ((deviceIndex < 0) || (deviceIndex >= devices.length)) {
      Base.showWarning("Display doesn't exist",
                          "Present Mode is set to use display " +
                          (deviceIndex+1) +
                          " but that doesn't seem to exist.    \n" +
                          "This preference will be reset to " +
                          "use the default display.", null);
      deviceIndex = defaultIndex;
    }
    //GraphicsDevice device = devices[
    //JFrame frame = new JFrame(devices[deviceIndex]);
    PresentFrame frame = new PresentFrame(devices[deviceIndex]);
  }


  public void show() {
    setUndecorated(true);
    setResizable(false);
    device.setFullScreenWindow(this);
  }


  public Window getWindow() {
    return device.getFullScreenWindow();  // isn't this just me?
  }


  public void dispose() {
    Window window = device.getFullScreenWindow();
    if (window != null) {
      window.dispose();
    }
    device.setFullScreenWindow(null);
  }
  */
}
