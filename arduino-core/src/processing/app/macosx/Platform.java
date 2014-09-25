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

package processing.app.macosx;

import com.apple.eio.FileManager;
import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.Executor;
import processing.app.debug.TargetPackage;
import processing.app.tools.ExternalProcessExecutor;
import processing.app.legacy.PApplet;
import processing.app.legacy.PConstants;

import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.lang.reflect.Method;
import java.net.URI;
import java.util.Map;


/**
 * Platform handler for Mac OS X.
 */
public class Platform extends processing.app.Platform {

  public void setLookAndFeel() throws Exception {
    // Use the Quaqua L & F on OS X to make JFileChooser less awful
    UIManager.setLookAndFeel("ch.randelshofer.quaqua.QuaquaLookAndFeel");
    // undo quaqua trying to fix the margins, since we've already
    // hacked that in, bit by bit, over the years
    UIManager.put("Component.visualMargin", new Insets(1, 1, 1, 1));
  }

  public Platform() {
    // For more information see:
    // http://mail.openjdk.java.net/pipermail/macosx-port-dev/2013-January/005261.html
    Toolkit.getDefaultToolkit();
  }

  public void init() {
    System.setProperty("apple.laf.useScreenMenuBar", "true");
    /*
    try {
      String name = "processing.app.macosx.ThinkDifferent";
      Class osxAdapter = ClassLoader.getSystemClassLoader().loadClass(name);

      Class[] defArgs = { Base.class };
      Method registerMethod = osxAdapter.getDeclaredMethod("register", defArgs);
      if (registerMethod != null) {
        Object[] args = { this };
        registerMethod.invoke(osxAdapter, args);
      }
    } catch (NoClassDefFoundError e) {
      // This will be thrown first if the OSXAdapter is loaded on a system without the EAWT
      // because OSXAdapter extends ApplicationAdapter in its def
      System.err.println("This version of Mac OS X does not support the Apple EAWT." +
                         "Application Menu handling has been disabled (" + e + ")");

    } catch (ClassNotFoundException e) {
      // This shouldn't be reached; if there's a problem with the OSXAdapter
      // we should get the above NoClassDefFoundError first.
      System.err.println("This version of Mac OS X does not support the Apple EAWT. " +
                         "Application Menu handling has been disabled (" + e + ")");
    } catch (Exception e) {
      System.err.println("Exception while loading BaseOSX:");
      e.printStackTrace();
    }
    */
  }


  public File getSettingsFolder() throws Exception {
    return new File(getLibraryFolder(), "Arduino15");
  }


  public File getDefaultSketchbookFolder() throws Exception {
    return new File(getDocumentsFolder(), "Arduino");
    /*
    // looking for /Users/blah/Documents/Processing
    try {
      Class clazz = Class.forName("processing.app.BaseMacOS");
      Method m = clazz.getMethod("getDocumentsFolder", new Class[] { });
      String documentsPath = (String) m.invoke(null, new Object[] { });
      sketchbookFolder = new File(documentsPath, "Arduino");

    } catch (Exception e) {
      sketchbookFolder = promptSketchbookLocation();
    }
    */
  }


  public void openURL(String url) throws Exception {
    if (PApplet.javaVersion < 1.6f) {
      if (url.startsWith("http://")) {
        // formerly com.apple.eio.FileManager.openURL(url);
        // but due to deprecation, instead loading dynamically
        try {
          Class<?> eieio = Class.forName("com.apple.eio.FileManager");
          Method openMethod =
            eieio.getMethod("openURL", new Class[] { String.class });
          openMethod.invoke(null, new Object[] { url });
        } catch (Exception e) {
          e.printStackTrace();
        }
      } else {
      // Assume this is a file instead, and just open it.
      // Extension of http://dev.processing.org/bugs/show_bug.cgi?id=1010
      PApplet.open(url);
      }
    } else {
      try {
        Class<?> desktopClass = Class.forName("java.awt.Desktop");
        Method getMethod = desktopClass.getMethod("getDesktop");
        Object desktop = getMethod.invoke(null, new Object[] { });

        // for Java 1.6, replacing with java.awt.Desktop.browse() 
        // and java.awt.Desktop.open()
        if (url.startsWith("http://")) {  // browse to a location
          Method browseMethod =
            desktopClass.getMethod("browse", new Class[] { URI.class });
          browseMethod.invoke(desktop, new Object[] { new URI(url) });
        } else {  // open a file
          Method openMethod =
            desktopClass.getMethod("open", new Class[] { File.class });
          openMethod.invoke(desktop, new Object[] { new File(url) });
          }
      } catch (Exception e) {
        e.printStackTrace();
        }
      }
    }


  public boolean openFolderAvailable() {
    return true;
  }


  public void openFolder(File file) throws Exception {
    //openURL(file.getAbsolutePath());  // handles char replacement, etc
    PApplet.open(file.getAbsolutePath());
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  // Some of these are supposedly constants in com.apple.eio.FileManager,
  // however they don't seem to link properly from Eclipse.

  static final int kDocumentsFolderType =
    ('d' << 24) | ('o' << 16) | ('c' << 8) | 's';
  //static final int kPreferencesFolderType =
  //  ('p' << 24) | ('r' << 16) | ('e' << 8) | 'f';
  static final int kDomainLibraryFolderType =
    ('d' << 24) | ('l' << 16) | ('i' << 8) | 'b';
  static final short kUserDomain = -32763;


  // apple java extensions documentation
  // http://developer.apple.com/documentation/Java/Reference/1.5.0
  //   /appledoc/api/com/apple/eio/FileManager.html

  // carbon folder constants
  // http://developer.apple.com/documentation/Carbon/Reference
  //   /Folder_Manager/folder_manager_ref/constant_6.html#/
  //   /apple_ref/doc/uid/TP30000238/C006889

  // additional information found int the local file:
  // /System/Library/Frameworks/CoreServices.framework
  //   /Versions/Current/Frameworks/CarbonCore.framework/Headers/


  protected String getLibraryFolder() throws FileNotFoundException {
    return FileManager.findFolder(kUserDomain, kDomainLibraryFolderType);
  }


  protected String getDocumentsFolder() throws FileNotFoundException {
    return FileManager.findFolder(kUserDomain, kDocumentsFolderType);
  }

  @Override
  public String getName() {
    return PConstants.platformNames[PConstants.MACOSX];
  }

  @Override
  public String resolveDeviceAttachedTo(String serial, Map<String, TargetPackage> packages, String devicesListOutput) {
    if (devicesListOutput == null) {
      return super.resolveDeviceAttachedTo(serial, packages, devicesListOutput);
    }

    try {
      String vidPid = new SystemProfilerParser().extractVIDAndPID(devicesListOutput, serial);

      if (vidPid == null) {
        return super.resolveDeviceAttachedTo(serial, packages, devicesListOutput);
      }

      return super.resolveDeviceByVendorIdProductId(packages, vidPid);
    } catch (IOException e) {
      return super.resolveDeviceAttachedTo(serial, packages, devicesListOutput);
    }
  }

  @Override
  public String preListAllCandidateDevices() {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    Executor executor = new ExternalProcessExecutor(baos);

    try {
      CommandLine toDevicePath = CommandLine.parse("/usr/sbin/system_profiler SPUSBDataType");
      executor.execute(toDevicePath);
      return new String(baos.toByteArray());
    } catch (Throwable e) {
      return super.preListAllCandidateDevices();
    }
  }
}
