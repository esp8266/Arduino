/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Arduino project - http://arduino.berlios.de

  

  Based on processing http://www.processing.org
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

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.lang.reflect.*;
import java.net.*;
import java.util.*;
import java.util.zip.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.undo.*;

import com.apple.mrj.*;
import com.ice.jni.registry.*;

import processing.core.*;


/**
 * The base class for the main Arduino application.
 * <P>
 * Primary role of this class is for platform identification and
 * general interaction with the system (launching URLs, loading
 * files and images, etc) that comes from that.
 */
public class Base {
  static final int VERSION = 12;
  static final String VERSION_NAME = "0012 Alpha";

  /**
   * Path of filename opened on the command line,
   * or via the MRJ open document handler.
   */
  static String openedAtStartup;

  Editor editor;


  static public void main(String args[]) {

    // make sure that this is running on java 1.4

    if (PApplet.javaVersion < 1.4f) {
      //System.err.println("no way man");
      Base.showError("Need to install Java 1.4",
                     "This version of Processing requires    \n" +
                     "Java 1.4 or later to run properly.\n" +
                     "Please visit java.com to upgrade.", null);
    }


    // grab any opened file from the command line

    if (args.length == 1) {
      Base.openedAtStartup = args[0];
    }


    // register a temporary/early version of the mrj open document handler,
    // because the event may be lost (sometimes, not always) by the time
    // that Editor is properly constructed.

    MRJOpenDocumentHandler startupOpen = new MRJOpenDocumentHandler() {
        public void handleOpenFile(File file) {
          // this will only get set once.. later will be handled
          // by the Editor version of this fella
          if (Base.openedAtStartup == null) {
            //System.out.println("handling outside open file: " + file);
            Base.openedAtStartup = file.getAbsolutePath();
          }
        }
      };
    MRJApplicationUtils.registerOpenDocumentHandler(startupOpen);

    Base app = new Base();
  }


  public Base() {

    // set the look and feel before opening the window

    try {
      if (Base.isMacOS()) {
        // Use the Quaqua L & F on OS X to make JFileChooser less awful
        UIManager.setLookAndFeel("ch.randelshofer.quaqua.QuaquaLookAndFeel");
        // undo quaqua trying to fix the margins, since we've already
        // hacked that in, bit by bit, over the years
        UIManager.put("Component.visualMargin", new Insets(1, 1, 1, 1));

      } else if (Base.isLinux()) {
        // Linux is by default even uglier than metal (Motif?).
        // Actually, i'm using native menus, so they're even uglier
        // and Motif-looking (Lesstif?). Ick. Need to fix this.
        //String lfname = UIManager.getCrossPlatformLookAndFeelClassName();
        //UIManager.setLookAndFeel(lfname);

        // For 0120, trying out the gtk+ look and feel as the default.
        // This is available in Java 1.4.2 and later, and it can't possibly
        // be any worse than Metal. (Ocean might also work, but that's for
        // Java 1.5, and we aren't going there yet)
        UIManager.setLookAndFeel("com.sun.java.swing.plaf.gtk.GTKLookAndFeel");

      } else {
        UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
      }
    //} catch (ClassNotFoundException cnfe) {
      // just default to the native look and feel for this platform
      // i.e. appears that some linux systems don't have the gtk l&f
      //UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());

    } catch (Exception e) {
      e.printStackTrace();
    }

    // use native popups so they don't look so crappy on osx
    JPopupMenu.setDefaultLightWeightPopupEnabled(false);

    // build the editor object
    editor = new Editor();

    // get things rawkin
    editor.pack();

    // has to be here to set window size properly
    editor.restorePreferences();

    // show the window
    editor.show();

    // attempt to build libraries
    editor.prepareLibraries();

    // check for updates
    if (Preferences.getBoolean("update.check")) {
      new UpdateCheck(editor);
    }
  }


  // .................................................................


  /**
   * returns true if the Arduino is running on a Mac OS machine,
   * specifically a Mac OS X machine because it doesn't run on OS 9 anymore.
   */
  static public boolean isMacOS() {
    return PApplet.platform == PConstants.MACOSX;
  }


  /**
   * returns true if running on windows.
   */
  static public boolean isWindows() {
    return PApplet.platform == PConstants.WINDOWS;
  }


  /**
   * true if running on linux.
   */
  static public boolean isLinux() {
    return PApplet.platform == PConstants.LINUX;
  }


  // .................................................................


  static final int kDocumentsFolderType =
    ('d' << 24) | ('o' << 16) | ('c' << 8) | 's';
  static final int kPreferencesFolderType =
    ('p' << 24) | ('r' << 16) | ('e' << 8) | 'f';
  static final int kDomainLibraryFolderType =
    ('d' << 24) | ('l' << 16) | ('i' << 8) | 'b';
  static final short kUserDomain = -32763;


  static public File getSettingsFolder() {
    File dataFolder = null;

    String pref = Preferences.get("settings.path");
    if (pref != null) {
      dataFolder = new File(pref);

    } else if (Base.isMacOS()) {
      // carbon folder constants
      // http://developer.apple.com/documentation/Carbon/Reference
      //   /Folder_Manager/folder_manager_ref/constant_6.html#/
      //   /apple_ref/doc/uid/TP30000238/C006889

      // additional information found int the local file:
      // /System/Library/Frameworks/CoreServices.framework
      //   /Versions/Current/Frameworks/CarbonCore.framework/Headers/

      // this is the 1.4 version.. but using 1.3 since i have the stubs
      // import com.apple.eio.*
      //println(FileManager.findFolder(kUserDomain,
      //        kDomainLibraryFolderType));

      // not clear if i can write to this folder tho..
      try {
        /*
        if (false) {
          // this is because the mrjtoolkit stubs don't have the
          // thows exception around them
          new FileInputStream("ignored");
        }
        */

        // this method has to be dynamically loaded, because
        MRJOSType domainLibrary = new MRJOSType("dlib");
        Method findFolderMethod =
          MRJFileUtils.class.getMethod("findFolder",
                                       new Class[] { Short.TYPE,
                                                     MRJOSType.class });
        File libraryFolder = (File)
          findFolderMethod.invoke(null, new Object[] { new Short(kUserDomain),
                                                       domainLibrary });

        dataFolder = new File(libraryFolder, "Arduino");

      } catch (Exception e) {
        // this could be FileNotFound or NoSuchMethod
        //} catch (FileNotFoundException e) {
        //e.printStackTrace();
        //System.exit(1);
        showError("Problem getting data folder",
                  "Error getting the Arduino data folder.", e);
      }

    } else if (Base.isWindows()) {
      // looking for Documents and Settings/blah/Application Data/Arduino

      // this is just based on the other documentation, and eyeballing
      // that part of the registry.. not confirmed by any msft/msdn docs.
      // HKEY_CURRENT_USER\Software\Microsoft
      //   \Windows\CurrentVersion\Explorer\Shell Folders
      // Value Name: AppData
      // Value Type: REG_SZ
      // Value Data: path

      try {
        //RegistryKey topKey = Registry.getTopLevelKey("HKCU");
        RegistryKey topKey = Registry.HKEY_CURRENT_USER;

        String localKeyPath =
          "Software\\Microsoft\\Windows\\CurrentVersion" +
          "\\Explorer\\Shell Folders";
        RegistryKey localKey = topKey.openSubKey(localKeyPath);
        String appDataPath = cleanKey(localKey.getStringValue("AppData"));
        //System.out.println("app data path is " + appDataPath);
        //System.exit(0);
        //topKey.closeKey();  // necessary?
        //localKey.closeKey();

        dataFolder = new File(appDataPath, "Arduino");

      } catch (Exception e) {
        showError("Problem getting data folder",
                  "Error getting the Arduino data folder.", e);
      }
      //return null;

    } else {
      // otherwise make a .arduino directory int the user's home dir
      File home = new File(System.getProperty("user.home"));
      dataFolder = new File(home, ".arduino");
    }

    // create the folder if it doesn't exist already
    boolean result = true;
    if (!dataFolder.exists()) {
      result = dataFolder.mkdirs();
    }

    if (!result) {
      // try the fallback location
      System.out.println("Using fallback path for settings.");
      String fallback = Preferences.get("settings.path.fallback");
      dataFolder = new File(fallback);
      if (!dataFolder.exists()) {
        result = dataFolder.mkdirs();
      }
    }

    if (!result) {
      showError("Settings issues",
                "Arduino cannot run because it could not\n" +
                "create a folder to store your settings.", null);
    }

    return dataFolder;
  }


  static public File getSettingsFile(String filename) {
    return new File(getSettingsFolder(), filename);
  }


  static File buildFolder;

  static public File getBuildFolder() {
    if (buildFolder == null) {
      String buildPath = Preferences.get("build.path");
      if (buildPath != null) {
        buildFolder = new File(buildPath);

      } else {
        //File folder = new File(getTempFolder(), "build");
        //if (!folder.exists()) folder.mkdirs();
        buildFolder = createTempFolder("build");
        buildFolder.deleteOnExit();
      }
    }
    return buildFolder;
  }


  /**
   * Get the path to the platform's temporary folder, by creating
   * a temporary temporary file and getting its parent folder.
   * <br/>
   * Modified for revision 0094 to actually make the folder randomized
   * to avoid conflicts in multi-user environments. (Bug 177)
   */
  static public File createTempFolder(String name) {
    try {
      File folder = File.createTempFile(name, null);
      //String tempPath = ignored.getParent();
      //return new File(tempPath);
      folder.delete();
      folder.mkdirs();
      return folder;

    } catch (Exception e) {
      e.printStackTrace();
    }
    return null;  // TODO could we *really* ever reach this?
  }


  /*
  static public void addBuildFolderToClassPath() {
    String path = getBuildFolder().getAbsolutePath();
    String jcp = System.getProperty("java.class.path");
    if (jcp.indexOf(path) == -1) {
      System.setProperty("java.class.path", path + File.pathSeparator + jcp);
      //return new File(getProcessingDataFolder(), "build");
      System.out.println("jcp is now " +
                         System.getProperty("java.class.path"));
    }
  }
  */


  static public File getDefaultSketchbookFolder() {
    File sketchbookFolder = null;

    if (Base.isMacOS()) {
      // looking for /Users/blah/Documents/Arduino

      // carbon folder constants
      // http://developer.apple.com/documentation/Carbon/Reference/Folder_Manager/folder_manager_ref/constant_6.html#//apple_ref/doc/uid/TP30000238/C006889

      // additional information found int the local file:
      // /System/Library/Frameworks/CoreServices.framework/Versions/Current/Frameworks/CarbonCore.framework/Headers/

      // this is the 1.4 version.. but using 1.3 since i have the stubs
      // import com.apple.eio.*
      //println(FileManager.findFolder(kUserDomain,
      //        kDomainLibraryFolderType));

      // not clear if i can write to this folder tho..
      try {
        MRJOSType domainDocuments = new MRJOSType("docs");
        //File libraryFolder = MRJFileUtils.findFolder(domainDocuments);

        // for 77, try switching this to the user domain, just to be sure
        Method findFolderMethod =
          MRJFileUtils.class.getMethod("findFolder",
                                       new Class[] { Short.TYPE,
                                                     MRJOSType.class });
        File documentsFolder = (File)
          findFolderMethod.invoke(null, new Object[] { new Short(kUserDomain),
                                                       domainDocuments });
        sketchbookFolder = new File(documentsFolder, "Arduino");

      } catch (Exception e) {
        //showError("Could not find folder",
        //          "Could not locate the Documents folder.", e);
        sketchbookFolder = promptSketchbookLocation();
      }

    } else if (isWindows()) {
      // looking for Documents and Settings/blah/My Documents/Arduino
      // (though using a reg key since it's different on other platforms)

      // http://support.microsoft.com/?kbid=221837&sd=RMVP
      // The path to the My Documents folder is stored in the
      // following registry key, where path is the complete path
      // to your storage location:
      // HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders
      // Value Name: Personal
      // Value Type: REG_SZ
      // Value Data: path

      try {
        RegistryKey topKey = Registry.HKEY_CURRENT_USER;

        String localKeyPath =
          "Software\\Microsoft\\Windows\\CurrentVersion" +
          "\\Explorer\\Shell Folders";
        RegistryKey localKey = topKey.openSubKey(localKeyPath);
        String personalPath = cleanKey(localKey.getStringValue("Personal"));
        //topKey.closeKey();  // necessary?
        //localKey.closeKey();
        sketchbookFolder = new File(personalPath, "Arduino");

      } catch (Exception e) {
        //showError("Problem getting folder",
        //          "Could not locate the Documents folder.", e);
        sketchbookFolder = promptSketchbookLocation();
      }

    } else {
      sketchbookFolder = promptSketchbookLocation();

      /*
      // on linux (or elsewhere?) prompt the user for the location
      JFileChooser fc = new JFileChooser();
      fc.setDialogTitle("Select the folder where " +
                        "Arduino programs should be stored...");
      //fc.setSelectedFile(new File(sketchbookLocationField.getText()));
      fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

      int returned = fc.showOpenDialog(new JDialog());
      if (returned == JFileChooser.APPROVE_OPTION) {
        //File file = fc.getSelectedFile();
        //sketchbookLocationField.setText(file.getAbsolutePath());
        sketchbookFolder = fc.getSelectedFile();

      } else {
        System.exit(0);
      }
      */
    }

    // create the folder if it doesn't exist already
    boolean result = true;
    if (!sketchbookFolder.exists()) {
      result = sketchbookFolder.mkdirs();
    }

    if (!result) {
      // try the fallback location
      System.out.println("Using fallback path for sketchbook.");
      String fallback = Preferences.get("sketchbook.path.fallback");
      sketchbookFolder = new File(fallback);
      if (!sketchbookFolder.exists()) {
        result = sketchbookFolder.mkdirs();
      }
    }

    if (!result) {
      showError("error",
                "Arduino cannot run because it could not\n" +
                "create a folder to store your sketchbook.", null);
    }

    return sketchbookFolder;
  }


  /**
   * Check for a new sketchbook location.
   */
  static protected File promptSketchbookLocation() {
    File folder = null;

    folder = new File(System.getProperty("user.home"), "sketchbook");
    if (!folder.exists()) {
      folder.mkdirs();
      return folder;
    }

    folder = Base.selectFolder("Select (or create new) folder for sketches...",
                               null, null);
    if (folder == null) {
      System.exit(0);
    }
    return folder;
  }


  /**
   * Implementation for choosing directories that handles both the
   * Mac OS X hack to allow the native AWT file dialog, or uses
   * the JFileChooser on other platforms. Mac AWT trick obtained from
   * <A HREF="http://lists.apple.com/archives/java-dev/2003/Jul/msg00243.html">this post</A>
   * on the OS X Java dev archive which explains the cryptic note in
   * Apple's Java 1.4 release docs about the special System property.
   */
  static public File selectFolder(String prompt, File folder, Frame frame) {
    if (Base.isMacOS()) {
      if (frame == null) frame = new Frame(); //.pack();
      FileDialog fd = new FileDialog(frame, prompt, FileDialog.LOAD);
      if (folder != null) {
        fd.setDirectory(folder.getParent());
        //fd.setFile(folder.getName());
      }
      System.setProperty("apple.awt.fileDialogForDirectories", "true");
      fd.show();
      System.setProperty("apple.awt.fileDialogForDirectories", "false");
      if (fd.getFile() == null) {
        return null;
      }
      return new File(fd.getDirectory(), fd.getFile());

    } else {
      JFileChooser fc = new JFileChooser();
      fc.setDialogTitle(prompt);
      if (folder != null) {
        fc.setSelectedFile(folder);
      }
      fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

      int returned = fc.showOpenDialog(new JDialog());
      if (returned == JFileChooser.APPROVE_OPTION) {
        return fc.getSelectedFile();
      }
    }
    return null;
  }


  static public String cleanKey(String what) {
    // jnireg seems to be reading the chars as bytes
    // so maybe be as simple as & 0xff and then running through decoder

    char c[] = what.toCharArray();

    // if chars are in the tooHigh range, it's prolly because
    // a byte from the jni registry was turned into a char
    // and there was a sign extension.
    // e.g. 0xFC (252, umlaut u) became 0xFFFC (65532).
    // but on a japanese system, maybe this is two-byte and ok?
    int tooHigh = 65536 - 128;
    for (int i = 0; i < c.length; i++) {
      if (c[i] >= tooHigh) c[i] &= 0xff;

      /*
      if ((c[i] >= 32) && (c[i] < 128)) {
        System.out.print(c[i]);
      } else {
        System.out.print("[" + PApplet.hex(c[i]) + "]");
      }
      */
    }
    //System.out.println();
    return new String(c);
  }


  // .................................................................


  // someone needs to be slapped
  //static KeyStroke closeWindowKeyStroke;

  /**
   * Return true if the key event was a Ctrl-W or an ESC,
   * both indicators to close the window.
   * Use as part of a keyPressed() event handler for frames.
   */
  /*
  static public boolean isCloseWindowEvent(KeyEvent e) {
    if (closeWindowKeyStroke == null) {
      int modifiers = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
      closeWindowKeyStroke = KeyStroke.getKeyStroke('W', modifiers);
    }
    return ((e.getKeyCode() == KeyEvent.VK_ESCAPE) ||
            KeyStroke.getKeyStrokeForEvent(e).equals(closeWindowKeyStroke));
  }
  */


  /**
   * Registers key events for a Ctrl-W and ESC with an ActionListener
   * that will take care of disposing the window.
   */
  static public void registerWindowCloseKeys(JRootPane root, //Window window,
                                             ActionListener disposer) {
    /*
    JRootPane root = null;
    if (window instanceof JFrame) {
      root = ((JFrame)window).getRootPane();
    } else if (window instanceof JDialog) {
      root = ((JDialog)window).getRootPane();
    }
    */

    KeyStroke stroke = KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0);
    root.registerKeyboardAction(disposer, stroke,
                                JComponent.WHEN_IN_FOCUSED_WINDOW);

    int modifiers = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
    stroke = KeyStroke.getKeyStroke('W', modifiers);
    root.registerKeyboardAction(disposer, stroke,
                                JComponent.WHEN_IN_FOCUSED_WINDOW);
  }


  // .................................................................


  static public void showReference(String referenceFile) {
    openURL(Base.getContents("reference" + File.separator + referenceFile));
  }


  static public void showReference() {
    showReference("index.html");
  }


  static public void showEnvironment() {
    showReference("Guide_Environment.html");
  }


  static public void showTroubleshooting() {
    showReference("Guide_Troubleshooting.html");
  }


  /**
   * Opens the local copy of the FAQ that's included
   * with the Processing download.
   */
  static public void showFAQ() {
    showReference("faq.html");
  }


  // .................................................................


  /**
   * Implements the cross-platform headache of opening URLs
   * TODO This code should be replaced by PApplet.link(),
   * however that's not a static method (because it requires
   * an AppletContext when used as an applet), so it's mildly
   * trickier than just removing this method.
   */
  static public void openURL(String url) {
    //System.out.println("opening url " + url);
    try {
      if (Base.isWindows()) {
        // this is not guaranteed to work, because who knows if the
        // path will always be c:\progra~1 et al. also if the user has
        // a different browser set as their default (which would
        // include me) it'd be annoying to be dropped into ie.
        //Runtime.getRuntime().exec("c:\\progra~1\\intern~1\\iexplore "
        // + currentDir

        // the following uses a shell execute to launch the .html file
        // note that under cygwin, the .html files have to be chmodded +x
        // after they're unpacked from the zip file. i don't know why,
        // and don't understand what this does in terms of windows
        // permissions. without the chmod, the command prompt says
        // "Access is denied" in both cygwin and the "dos" prompt.
        //Runtime.getRuntime().exec("cmd /c " + currentDir + "\\reference\\" +
        //                    referenceFile + ".html");
        if (url.startsWith("http://")) {
          // open dos prompt, give it 'start' command, which will
          // open the url properly. start by itself won't work since
          // it appears to need cmd
          Runtime.getRuntime().exec("cmd /c start " + url);
        } else {
          // just launching the .html file via the shell works
          // but make sure to chmod +x the .html files first
          // also place quotes around it in case there's a space
          // in the user.dir part of the url
          Runtime.getRuntime().exec("cmd /c \"" + url + "\"");
        }

      } else if (Base.isMacOS()) {
        //com.apple.eio.FileManager.openURL(url);

        if (!url.startsWith("http://")) {
          // prepend file:// on this guy since it's a file
          url = "file://" + url;

          // replace spaces with %20 for the file url
          // otherwise the mac doesn't like to open it
          // can't just use URLEncoder, since that makes slashes into
          // %2F characters, which is no good. some might say "useless"
          if (url.indexOf(' ') != -1) {
            StringBuffer sb = new StringBuffer();
            char c[] = url.toCharArray();
            for (int i = 0; i < c.length; i++) {
              if (c[i] == ' ') {
                sb.append("%20");
              } else {
                sb.append(c[i]);
              }
            }
            url = sb.toString();
          }
        }
        com.apple.mrj.MRJFileUtils.openURL(url);

      } else if (Base.isLinux()) {
        // how's mozilla sound to ya, laddie?
        //Runtime.getRuntime().exec(new String[] { "mozilla", url });
        //String browser = Preferences.get("browser");
        //Runtime.getRuntime().exec(new String[] { browser, url });
        String launcher = Preferences.get("launcher.linux");
        if (launcher != null) {
          Runtime.getRuntime().exec(new String[] { launcher, url });
        }
      } else {
        String launcher = Preferences.get("launcher");
        if (launcher != null) {
          Runtime.getRuntime().exec(new String[] { launcher, url });
      } else {
          System.err.println("Unspecified platform, no launcher available.");
        }
      }

    } catch (IOException e) {
      Base.showWarning("Could not open URL",
                          "An error occurred while trying to open\n" + url, e);
    }
  }


  static boolean openFolderAvailable() {
    if (Base.isWindows() || Base.isMacOS()) return true;

    if (Base.isLinux()) {
      // Assume that this is set to something valid
      if (Preferences.get("launcher.linux") != null) {
        return true;
      }

      // Attempt to use gnome-open
      try {
        Process p = Runtime.getRuntime().exec(new String[] { "gnome-open" });
        int result = p.waitFor();
        // Not installed will throw an IOException (JDK 1.4.2, Ubuntu 7.04)
        Preferences.set("launcher.linux", "gnome-open");
        return true;
      } catch (Exception e) { }

      // Attempt with kde-open
      try {
        Process p = Runtime.getRuntime().exec(new String[] { "kde-open" });
        int result = p.waitFor();
        Preferences.set("launcher.linux", "kde-open");
        return true;
      } catch (Exception e) { }
    }
    return false;
  }


  /**
   * Implements the other cross-platform headache of opening
   * a folder in the machine's native file browser.
   */
  static public void openFolder(File file) {
    try {
      String folder = file.getAbsolutePath();

      if (Base.isWindows()) {
        // doesn't work
        //Runtime.getRuntime().exec("cmd /c \"" + folder + "\"");

        // works fine on winxp, prolly win2k as well
        Runtime.getRuntime().exec("explorer \"" + folder + "\"");

        // not tested
        //Runtime.getRuntime().exec("start explorer \"" + folder + "\"");

      } else if (Base.isMacOS()) {
        openURL(folder);  // handles char replacement, etc

      } else if (Base.isLinux()) {
        String launcher = Preferences.get("launcher.linux");
        if (launcher != null) {
          Runtime.getRuntime().exec(new String[] { launcher, folder });
        }
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
  }


  /**
   * "No cookie for you" type messages. Nothing fatal or all that
   * much of a bummer, but something to notify the user about.
   */
  static public void showMessage(String title, String message) {
    if (title == null) title = "Message";
    JOptionPane.showMessageDialog(new Frame(), message, title,
                                  JOptionPane.INFORMATION_MESSAGE);
  }


  /**
   * Non-fatal error message with optional stack trace side dish.
   */
  static public void showWarning(String title, String message,
                                 Exception e) {
    if (title == null) title = "Warning";
    JOptionPane.showMessageDialog(new Frame(), message, title,
                                  JOptionPane.WARNING_MESSAGE);

    //System.err.println(e.toString());
    if (e != null) e.printStackTrace();
  }


  /**
   * Show an error message that's actually fatal to the program.
   * This is an error that can't be recovered. Use showWarning()
   * for errors that allow P5 to continue running.
   */
  static public void showError(String title, String message,
                               Throwable e) {
    if (title == null) title = "Error";
    JOptionPane.showMessageDialog(new Frame(), message, title,
                                  JOptionPane.ERROR_MESSAGE);

    if (e != null) e.printStackTrace();
    System.exit(1);
  }


  // ...................................................................


  static public String getContents(String what) {
    String basePath = System.getProperty("user.dir");
    return basePath + File.separator + what;
  }

  static public String getLibContents(String what) {
      /* On MacOSX, the arduino.app-resources property points to the
       * resources directory inside the app bundle. On other platforms
       * it's not set.
       */
      String appResources = System.getProperty("arduino.app-resources");
      if (appResources != null) {
          return appResources + File.separator + what;
      } else {
          return getContents("lib" + File.separator + what);
      }
  }


  static public Image getImage(String name, Component who) {
    Image image = null;
    Toolkit tk = Toolkit.getDefaultToolkit();

    image = tk.getImage(getLibContents(name));
    MediaTracker tracker = new MediaTracker(who);
    tracker.addImage(image, 0);
    try {
      tracker.waitForAll();
    } catch (InterruptedException e) { }
    return image;
  }


  static public InputStream getStream(String filename) throws IOException {
    return new FileInputStream(getLibContents(filename));
  }


  // ...................................................................


  static public byte[] grabFile(File file) throws IOException {
    int size = (int) file.length();
    FileInputStream input = new FileInputStream(file);
    byte buffer[] = new byte[size];
    int offset = 0;
    int bytesRead;
    while ((bytesRead = input.read(buffer, offset, size-offset)) != -1) {
      offset += bytesRead;
      if (bytesRead == 0) break;
    }
    input.close();  // weren't properly being closed
    input = null;
    return buffer;
  }


  static public void copyFile(File afile, File bfile) throws IOException {
    InputStream from = new BufferedInputStream(new FileInputStream(afile));
    OutputStream to = new BufferedOutputStream(new FileOutputStream(bfile));
    byte[] buffer = new byte[16 * 1024];
    int bytesRead;
    while ((bytesRead = from.read(buffer)) != -1) {
      to.write(buffer, 0, bytesRead);
    }
    to.flush();
    from.close(); // ??
    from = null;
    to.close(); // ??
    to = null;

    bfile.setLastModified(afile.lastModified());  // jdk13+ required
  //} catch (IOException e) {
  //  e.printStackTrace();
  //}
  }


  /**
   * Grab the contents of a file as a string.
   */
  static public String loadFile(File file) throws IOException {
    // empty code file.. no worries, might be getting filled up later
    if (file.length() == 0) return "";

    InputStreamReader isr = new InputStreamReader(new FileInputStream(file));
    BufferedReader reader = new BufferedReader(isr);

    StringBuffer buffer = new StringBuffer();
    String line = null;
    while ((line = reader.readLine()) != null) {
      buffer.append(line);
      buffer.append('\n');
    }
    reader.close();
    return buffer.toString();
  }


  /**
   * Spew the contents of a String object out to a file.
   */
  static public void saveFile(String str,
                              File file) throws IOException {

    ByteArrayInputStream bis = new ByteArrayInputStream(str.getBytes());
    InputStreamReader isr = new InputStreamReader(bis);
    BufferedReader reader = new BufferedReader(isr);

    FileWriter fw = new FileWriter(file);
    PrintWriter writer = new PrintWriter(new BufferedWriter(fw));

    String line = null;
    while ((line = reader.readLine()) != null) {
      writer.println(line);
    }
    writer.flush();
    writer.close();
  }


  static public void copyDir(File sourceDir,
                             File targetDir) throws IOException {
    targetDir.mkdirs();
    String files[] = sourceDir.list();
    for (int i = 0; i < files.length; i++) {
      if (files[i].equals(".") || files[i].equals("..")) continue;
      File source = new File(sourceDir, files[i]);
      File target = new File(targetDir, files[i]);
      if (source.isDirectory()) {
        //target.mkdirs();
        copyDir(source, target);
        target.setLastModified(source.lastModified());
      } else {
        copyFile(source, target);
      }
    }
  }


  /**
   * Remove all files in a directory and the directory itself.
   */
  static public void removeDir(File dir) {
    if (dir.exists()) {
      removeDescendants(dir);
      if (!dir.delete()) {
        System.err.println("Could not delete " + dir);
      }
    }
  }


  /**
   * Recursively remove all files within a directory,
   * used with removeDir(), or when the contents of a dir
   * should be removed, but not the directory itself.
   * (i.e. when cleaning temp files from lib/build)
   */
  static public void removeDescendants(File dir) {
    if (!dir.exists()) return;

    String files[] = dir.list();
    for (int i = 0; i < files.length; i++) {
      if (files[i].equals(".") || files[i].equals("..")) continue;
      File dead = new File(dir, files[i]);
      if (!dead.isDirectory()) {
        if (!Preferences.getBoolean("compiler.save_build_files")) {
          if (!dead.delete()) {
            // temporarily disabled
            System.err.println("Could not delete " + dead);
          }
        }
      } else {
        removeDir(dead);
        //dead.delete();
      }
    }
  }


  /**
   * Calculate the size of the contents of a folder.
   * Used to determine whether sketches are empty or not.
   * Note that the function calls itself recursively.
   */
  static public int calcFolderSize(File folder) {
    int size = 0;

    String files[] = folder.list();
    // null if folder doesn't exist, happens when deleting sketch
    if (files == null) return -1;

    for (int i = 0; i < files.length; i++) {
      if (files[i].equals(".") || (files[i].equals("..")) ||
          files[i].equals(".DS_Store")) continue;
      File fella = new File(folder, files[i]);
      if (fella.isDirectory()) {
        size += calcFolderSize(fella);
      } else {
        size += (int) fella.length();
      }
    }
    return size;
  }


  /**
   * Gets a list of all files within the specified folder,
   * and returns a list of their relative paths.
   * Ignores any files/folders prefixed with a dot.
   */
  static public String[] listFiles(String path, boolean relative) {
    return listFiles(new File(path), relative);
  }

  static public String[] listFiles(File folder, boolean relative) {
    String path = folder.getAbsolutePath();
    Vector vector = new Vector();
    listFiles(relative ? (path + File.separator) : "", path, vector);
    String outgoing[] = new String[vector.size()];
    vector.copyInto(outgoing);
    return outgoing;
  }

  static protected void listFiles(String basePath,
                                  String path, Vector vector) {
    File folder = new File(path);
    String list[] = folder.list();
    if (list == null) return;

    for (int i = 0; i < list.length; i++) {
      if (list[i].charAt(0) == '.') continue;

      File file = new File(path, list[i]);
      String newPath = file.getAbsolutePath();
      if (newPath.startsWith(basePath)) {
        newPath = newPath.substring(basePath.length());
      }
      vector.add(newPath);
      if (file.isDirectory()) {
        listFiles(basePath, newPath, vector);
      }
    }
  }
}
