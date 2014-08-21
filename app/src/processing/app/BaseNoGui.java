package processing.app;

import static processing.app.I18n._;

import java.io.File;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import processing.app.debug.TargetBoard;
import processing.app.debug.TargetPackage;
import processing.app.debug.TargetPlatform;
import processing.app.debug.TargetPlatformException;
import processing.app.helpers.OSUtils;
import processing.app.helpers.PreferencesMap;
import processing.app.helpers.filefilters.OnlyDirs;

public class BaseNoGui {

  public static final int REVISION = 158;
  /** This might be replaced by main() if there's a lib/version.txt file. */
  static String VERSION_NAME = "0158";
  /** Set true if this a proper release rather than a numbered revision. */
  static public boolean RELEASE = false;

  // Current directory to use for relative paths specified on the
  // commandline
  static String currentDirectory = System.getProperty("user.dir");

  static public Map<String, TargetPackage> packages;

  static Platform platform;

  static File portableFolder = null;

  // Returns a File object for the given pathname. If the pathname
  // is not absolute, it is interpreted relative to the current
  // directory when starting the IDE (which is not the same as the
  // current working directory!).
  static public File absoluteFile(String path) {
    if (path == null) return null;

    File file = new File(path);
    if (!file.isAbsolute()) {
      file = new File(currentDirectory, path);
    }
    return file;
  }

  static public String getAvrBasePath() {
    String path = getHardwarePath() + File.separator + "tools" +
                  File.separator + "avr" + File.separator + "bin" + File.separator;
    if (OSUtils.isLinux() && !(new File(path)).exists()) {
      return "";  // use distribution provided avr tools if bundled tools missing
    }
    return path;
  }

  static public PreferencesMap getBoardPreferences() {
    TargetBoard board = getTargetBoard();
    
    PreferencesMap prefs = new PreferencesMap(board.getPreferences());
    for (String menuId : board.getMenuIds()) {
      String entry = Preferences.get("custom_" + menuId);
      if (board.hasMenu(menuId) && entry != null &&
          entry.startsWith(board.getId())) {
        String selectionId = entry.substring(entry.indexOf("_") + 1);
        prefs.putAll(board.getMenuPreferences(menuId, selectionId));
        prefs.put("name", prefs.get("name") + ", " +
            board.getMenuLabel(menuId, selectionId));
      }
    }
    return prefs;
  }

  static public File getContentFile(String name) {
    String path = System.getProperty("user.dir");

    // Get a path to somewhere inside the .app folder
    if (OSUtils.isMacOS()) {
//      <key>javaroot</key>
//      <string>$JAVAROOT</string>
      String javaroot = System.getProperty("javaroot");
      if (javaroot != null) {
        path = javaroot;
      }
    }
    File working = new File(path);
    return new File(working, name);
  }

  static public File getHardwareFolder() {
    // calculate on the fly because it's needed by Preferences.init() to find
    // the boards.txt and programmers.txt preferences files (which happens
    // before the other folders / paths get cached).
    return getContentFile("hardware");
  }

  static public String getHardwarePath() {
    return getHardwareFolder().getAbsolutePath();
  }

  static public Platform getPlatform() {
    return platform;
  }

  static public File getPortableFolder() {
    return portableFolder;
  }

  static public File getSketchbookFolder() {
    if (portableFolder != null)
      return new File(portableFolder, Preferences.get("sketchbook.path"));
    return absoluteFile(Preferences.get("sketchbook.path"));
  }

  static public File getSketchbookHardwareFolder() {
    return new File(getSketchbookFolder(), "hardware");
  }

  public static TargetBoard getTargetBoard() {
    String boardId = Preferences.get("board");
    return getTargetPlatform().getBoard(boardId);
  }

  /**
   * Returns the currently selected TargetPlatform.
   *
   * @return
   */
  static public TargetPlatform getTargetPlatform() {
    String packageName = Preferences.get("target_package");
    String platformName = Preferences.get("target_platform");
    return getTargetPlatform(packageName, platformName);
  }

  /**
   * Returns a specific TargetPlatform searching Package/Platform
   *
   * @param packageName
   * @param platformName
   * @return
   */
  static public TargetPlatform getTargetPlatform(String packageName,
                                                 String platformName) {
    TargetPackage p = packages.get(packageName);
    if (p == null)
      return null;
    return p.get(platformName);
  }

  static public void initPackages() {
    packages = new HashMap<String, TargetPackage>();
    loadHardware(getHardwareFolder());
    loadHardware(getSketchbookHardwareFolder());
    if (packages.size() == 0) {
      System.out.println(_("No valid configured cores found! Exiting..."));
      System.exit(3);
    }
  }

  static protected void initPlatform() {
    try {
      Class<?> platformClass = Class.forName("processing.app.Platform");
      if (OSUtils.isMacOS()) {
        platformClass = Class.forName("processing.app.macosx.Platform");
      } else if (OSUtils.isWindows()) {
        platformClass = Class.forName("processing.app.windows.Platform");
      } else if (OSUtils.isLinux()) {
        platformClass = Class.forName("processing.app.linux.Platform");
      }
      platform = (Platform) platformClass.newInstance();
    } catch (Exception e) {
      Base.showError(_("Problem Setting the Platform"),
                     _("An unknown error occurred while trying to load\n" +
                       "platform-specific code for your machine."), e);
    }
  }

  static public void initPortableFolder() {
    // Portable folder
    portableFolder = getContentFile("portable");
    if (!portableFolder.exists())
      portableFolder = null;
  }

  static protected void loadHardware(File folder) {
    if (!folder.isDirectory()) return;

    String list[] = folder.list(new OnlyDirs());

    // if a bad folder or something like that, this might come back null
    if (list == null) return;

    // alphabetize list, since it's not always alpha order
    // replaced hella slow bubble sort with this feller for 0093
    Arrays.sort(list, String.CASE_INSENSITIVE_ORDER);

    for (String target : list) {
      // Skip reserved 'tools' folder.
      if (target.equals("tools"))
        continue;
      File subfolder = new File(folder, target);
      
      try {
        BaseNoGui.packages.put(target, new TargetPackage(target, subfolder));
      } catch (TargetPlatformException e) {
        System.out.println("WARNING: Error loading hardware folder " + target);
        System.out.println("  " + e.getMessage());
      }
    }
  }

}
