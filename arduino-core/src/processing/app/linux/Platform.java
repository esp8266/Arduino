/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2008 Ben Fry and Casey Reas

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

package processing.app.linux;

import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.Executor;
import org.apache.commons.exec.PumpStreamHandler;
import processing.app.PreferencesData;
import processing.app.debug.TargetPackage;
import processing.app.legacy.PConstants;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.util.Map;


/**
 * Used by Base for platform-specific tweaking, for instance finding the
 * sketchbook location using the Windows registry, or OS X event handling.
 */
public class Platform extends processing.app.Platform {

  // TODO Need to be smarter here since KDE people ain't gonna like that GTK.
  //      It may even throw a weird exception at 'em for their trouble.
  public void setLookAndFeel() throws Exception {
    GTKLookAndFeelFixer.installGtkPopupBugWorkaround();
  }


  public File getDefaultSketchbookFolder() throws Exception {
    File home = new File(System.getProperty("user.home"));
    return new File(home, "Arduino");
  }


  public void openURL(String url) throws Exception {
    if (openFolderAvailable()) {
      String launcher = PreferencesData.get("launcher");
      if (launcher != null) {
        Runtime.getRuntime().exec(new String[] { launcher, url });
      }
    }
  }


  public boolean openFolderAvailable() {
    if (PreferencesData.get("launcher") != null) {
      return true;
    }

    // Attempt to use xdg-open
    try {
      Process p = Runtime.getRuntime().exec(new String[] { "xdg-open" });
      p.waitFor();
      PreferencesData.set("launcher", "xdg-open");
      return true;
    } catch (Exception e) { }

    // Attempt to use gnome-open
    try {
      Process p = Runtime.getRuntime().exec(new String[] { "gnome-open" });
      p.waitFor();
      // Not installed will throw an IOException (JDK 1.4.2, Ubuntu 7.04)
      PreferencesData.set("launcher", "gnome-open");
      return true;
    } catch (Exception e) { }

    // Attempt with kde-open
    try {
      Process p = Runtime.getRuntime().exec(new String[] { "kde-open" });
      p.waitFor();
      PreferencesData.set("launcher", "kde-open");
      return true;
    } catch (Exception e) { }

    return false;
  }


  public void openFolder(File file) throws Exception {
    if (openFolderAvailable()) {
      String launcher = PreferencesData.get("launcher");
      try {
        String[] params = new String[] { launcher, file.getAbsolutePath() };
        //processing.core.PApplet.println(params);
        /*Process p =*/ Runtime.getRuntime().exec(params);
        /*int result =*/ //p.waitFor();
      } catch (Exception e) {
        e.printStackTrace();
      }
    } else {
      System.out.println("No launcher set, cannot open " +
                         file.getAbsolutePath());
    }
  }

  @Override
  public String getName() {
    return PConstants.platformNames[PConstants.LINUX];
  }

  @Override
  public Map<String, Object> resolveDeviceAttachedTo(String serial, Map<String, TargetPackage> packages, String devicesListOutput) {
    assert packages != null;
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    Executor executor = new DefaultExecutor();
    executor.setStreamHandler(new PumpStreamHandler(baos, null));

    try {
      CommandLine toDevicePath = CommandLine.parse("udevadm info -q path -n " + serial);
      executor.execute(toDevicePath);
      String devicePath = new String(baos.toByteArray());
      baos.reset();
      CommandLine commandLine = CommandLine.parse("udevadm info --query=property -p " + devicePath);
      executor.execute(commandLine);
      String vidPid = new UDevAdmParser().extractVIDAndPID(new String(baos.toByteArray()));

      if (vidPid == null) {
        return super.resolveDeviceAttachedTo(serial, packages, devicesListOutput);
      }

      return super.resolveDeviceByVendorIdProductId(packages, vidPid);
    } catch (IOException e) {
      return super.resolveDeviceAttachedTo(serial, packages, devicesListOutput);
    }
  }
}
