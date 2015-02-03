package processing.app;

import static processing.app.I18n._;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.net.URISyntaxException;
import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.commons.logging.impl.LogFactoryImpl;
import org.apache.commons.logging.impl.NoOpLog;

import cc.arduino.packages.DiscoveryManager;
import cc.arduino.packages.Uploader;

import processing.app.debug.Compiler;
import processing.app.debug.TargetBoard;
import processing.app.debug.TargetPackage;
import processing.app.debug.TargetPlatform;
import processing.app.debug.TargetPlatformException;
import processing.app.helpers.BasicUserNotifier;
import processing.app.helpers.CommandlineParser;
import processing.app.helpers.OSUtils;
import processing.app.helpers.PreferencesMap;
import processing.app.helpers.UserNotifier;
import processing.app.helpers.filefilters.OnlyDirs;
import processing.app.helpers.filefilters.OnlyFilesWithExtension;
import processing.app.legacy.PApplet;
import processing.app.packages.Library;
import processing.app.packages.LibraryList;

public class BaseNoGui {

  /** Version string to be used for build */
  public static final int REVISION = 10600;
  /** Extended version string displayed on GUI */
  static String VERSION_NAME = "1.6.0";

  static File buildFolder;

  // Current directory to use for relative paths specified on the
  // commandline
  static String currentDirectory = System.getProperty("user.dir");

  private static DiscoveryManager discoveryManager = new DiscoveryManager();
  
  // these are static because they're used by Sketch
  static private File examplesFolder;
  static private File toolsFolder;

  // maps #included files to their library folder
  public static Map<String, Library> importToLibraryTable;

  // maps library name to their library folder
  static private LibraryList libraries;

  static private List<File> librariesFolders;

  static UserNotifier notifier = new BasicUserNotifier();

  static public Map<String, TargetPackage> packages;

  static Platform platform;

  static File portableFolder = null;

  static final String portableSketchbookFolder = "sketchbook";

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

  /**
   * Get the number of lines in a file by counting the number of newline
   * characters inside a String (and adding 1).
   */
  static public int countLines(String what) {
    int count = 1;
    for (char c : what.toCharArray()) {
      if (c == '\n') count++;
    }
    return count;
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
    return null;
  }

  static public String getAvrBasePath() {
    String path = getHardwarePath() + File.separator + "tools" +
                  File.separator + "avr" + File.separator + "bin" + File.separator;
    if (OSUtils.isLinux() && !(new File(path)).exists()) {
      return "";  // use distribution provided avr tools if bundled tools missing
    }
    return path;
  }

  static public File getBuildFolder() {
    if (buildFolder == null) {
      String buildPath = PreferencesData.get("build.path");
      if (buildPath != null) {
        buildFolder = absoluteFile(buildPath);
        if (!buildFolder.exists())
          buildFolder.mkdirs();
      } else {
        //File folder = new File(getTempFolder(), "build");
        //if (!folder.exists()) folder.mkdirs();
        buildFolder = createTempFolder("build");
        buildFolder.deleteOnExit();
      }
    }
    return buildFolder;
  }

  static public PreferencesMap getBoardPreferences() {
    TargetBoard board = getTargetBoard();
    if (board == null)
      return null;
    
    PreferencesMap prefs = new PreferencesMap(board.getPreferences());
    for (String menuId : board.getMenuIds()) {
      String entry = PreferencesData.get("custom_" + menuId);
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
    File path = new File(System.getProperty("user.dir"));

    if (OSUtils.isMacOS()) {
      if (System.getProperty("WORK_DIR") != null) {
        path = new File(System.getProperty("WORK_DIR"));
      } else {
        try {
          path = new File(BaseNoGui.class.getProtectionDomain().getCodeSource().getLocation().toURI().getPath()).getParentFile();
        } catch (URISyntaxException e) {
          throw new RuntimeException(e);
        }
      }
    }

    return new File(path, name);
  }

  static public TargetPlatform getCurrentTargetPlatformFromPackage(String pack) {
    return getTargetPlatform(pack, PreferencesData.get("target_platform"));
  }

  static public  File getDefaultSketchbookFolder() {
    if (getPortableFolder() != null)
      return new File(getPortableFolder(), getPortableSketchbookFolder());

    File sketchbookFolder = null;
    try {
      sketchbookFolder = getPlatform().getDefaultSketchbookFolder();
    } catch (Exception e) { }

    return sketchbookFolder;
  }

  public static DiscoveryManager getDiscoveryManager() {
    return discoveryManager;
  }

  static public File getExamplesFolder() {
    return examplesFolder;
  }

  static public String getExamplesPath() {
    return examplesFolder.getAbsolutePath();
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

  static public LibraryList getLibraries() {
    return libraries;
  }

  static public List<File> getLibrariesPath() {
    return librariesFolders;
  }

  /**
   * Return an InputStream for a file inside the Processing lib folder.
   */
  static public InputStream getLibStream(String filename) throws IOException {
    return new FileInputStream(new File(getContentFile("lib"), filename));
  }

  static public Platform getPlatform() {
    return platform;
  }

  static public File getPortableFolder() {
    return portableFolder;
  }

  static public String getPortableSketchbookFolder() {
    return portableSketchbookFolder;
  }

  /**
   * Convenience method to get a File object for the specified filename inside
   * the settings folder.
   * For now, only used by Preferences to get the preferences.txt file.
   * @param filename A file inside the settings folder.
   * @return filename wrapped as a File object inside the settings folder
   */
  static public File getSettingsFile(String filename) {
    return new File(getSettingsFolder(), filename);
  }

  static public File getSettingsFolder() {
    if (getPortableFolder() != null)
      return getPortableFolder();

    File settingsFolder = null;

    String preferencesPath = PreferencesData.get("settings.path");
    if (preferencesPath != null) {
      settingsFolder = absoluteFile(preferencesPath);

    } else {
      try {
        settingsFolder = getPlatform().getSettingsFolder();
      } catch (Exception e) {
        showError(_("Problem getting data folder"),
                  _("Error getting the Arduino data folder."), e);
      }
    }

    // create the folder if it doesn't exist already
    if (!settingsFolder.exists()) {
      if (!settingsFolder.mkdirs()) {
        showError(_("Settings issues"),
                _("Arduino cannot run because it could not\n" +
                        "create a folder to store your settings."), null);
      }
    }
    return settingsFolder;
  }

  static public File getSketchbookFolder() {
    if (portableFolder != null)
      return new File(portableFolder, PreferencesData.get("sketchbook.path"));
    return absoluteFile(PreferencesData.get("sketchbook.path"));
  }

  static public File getSketchbookHardwareFolder() {
    return new File(getSketchbookFolder(), "hardware");
  }

  static public File getSketchbookLibrariesFolder() {
    File libdir = new File(getSketchbookFolder(), "libraries");
    if (!libdir.exists()) {
      try {
        libdir.mkdirs();
        File readme = new File(libdir, "readme.txt");
        FileWriter freadme = new FileWriter(readme);
        freadme.write(_("For information on installing libraries, see: " +
                        "http://arduino.cc/en/Guide/Libraries\n"));
        freadme.close();
      } catch (Exception e) {
      }
    }
    return libdir;
  }

  static public String getSketchbookPath() {
    // Get the sketchbook path, and make sure it's set properly
    String sketchbookPath = PreferencesData.get("sketchbook.path");

    // If a value is at least set, first check to see if the folder exists.
    // If it doesn't, warn the user that the sketchbook folder is being reset.
    if (sketchbookPath != null) {
      File sketchbookFolder;
      if (getPortableFolder() != null)
        sketchbookFolder = new File(getPortableFolder(), sketchbookPath);
      else
        sketchbookFolder = absoluteFile(sketchbookPath);
      if (!sketchbookFolder.exists()) {
        showWarning(_("Sketchbook folder disappeared"),
                    _("The sketchbook folder no longer exists.\n" +
                      "Arduino will switch to the default sketchbook\n" +
                      "location, and create a new sketchbook folder if\n" +
                      "necessary. Arduino will then stop talking about\n" +
                      "himself in the third person."), null);
        sketchbookPath = null;
      }
    }

    return sketchbookPath;
  }

  public static TargetBoard getTargetBoard() {
    TargetPlatform targetPlatform = getTargetPlatform();
    if (targetPlatform == null)
      return null;
    String boardId = PreferencesData.get("board");
    return targetPlatform.getBoard(boardId);
  }

  /**
   * Returns a specific TargetPackage
   *
   * @param packageName
   * @return
   */
  static public TargetPackage getTargetPackage(String packageName) {
    return packages.get(packageName);
  }

  /**
   * Returns the currently selected TargetPlatform.
   *
   * @return
   */
  static public TargetPlatform getTargetPlatform() {
    String packageName = PreferencesData.get("target_package");
    String platformName = PreferencesData.get("target_platform");
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

  static public File getToolsFolder() {
    return toolsFolder;
  }

  static public String getToolsPath() {
    return toolsFolder.getAbsolutePath();
  }

  static public LibraryList getUserLibs() {
    if (libraries == null)
      return new LibraryList();
    return libraries.filterLibrariesInSubfolder(getSketchbookFolder());
  }

  /**
   * Given a folder, return a list of the header files in that folder (but not
   * the header files in its sub-folders, as those should be included from
   * within the header files at the top-level).
   */
  static public String[] headerListFromIncludePath(File path) throws IOException {
    String[] list = path.list(new OnlyFilesWithExtension(".h"));
    if (list == null) {
      throw new IOException();
    }
    return list;
  }

  static public void init(String[] args) {
    getPlatform().init();
  
    String sketchbookPath = getSketchbookPath();
  
    // If no path is set, get the default sketchbook folder for this platform
    if (sketchbookPath == null) {
      if (BaseNoGui.getPortableFolder() != null)
        PreferencesData.set("sketchbook.path", getPortableSketchbookFolder());
      else
        showError(_("No sketchbook"), _("Sketchbook path not defined"), null);
    }
  
    BaseNoGui.initPackages();
    
    // Setup board-dependent variables.
    onBoardOrPortChange();
  
    CommandlineParser parser = CommandlineParser.newCommandlineParser(args);

    for (String path: parser.getFilenames()) {
      // Correctly resolve relative paths
      File file = absoluteFile(path);
  
      // Fix a problem with systems that use a non-ASCII languages. Paths are
      // being passed in with 8.3 syntax, which makes the sketch loader code
      // unhappy, since the sketch folder naming doesn't match up correctly.
      // http://dev.processing.org/bugs/show_bug.cgi?id=1089
      if (OSUtils.isWindows()) {
        try {
          file = file.getCanonicalFile();
        } catch (IOException e) {
          e.printStackTrace();
        }
      }

      if (!parser.isVerifyOrUploadMode() && !parser.isGetPrefMode())
        showError(_("Mode not supported"), _("Only --verify, --upload or --get-pref are supported"), null);

      if (!parser.isForceSavePrefs())
        PreferencesData.setDoSave(false);
      if (!file.exists()) {
        String mess = I18n.format(_("Failed to open sketch: \"{0}\""), path);
        // Open failure is fatal in upload/verify mode
        showError(null, mess, 2);
      }
    }
  
    // Save the preferences. For GUI mode, this happens in the quit
    // handler, but for other modes we should also make sure to save
    // them.
    PreferencesData.save();

    if (parser.isVerifyOrUploadMode()) {
      // Set verbosity for command line build
      PreferencesData.set("build.verbose", "" + parser.isDoVerboseBuild());
      PreferencesData.set("upload.verbose", "" + parser.isDoVerboseUpload());

      // Make sure these verbosity preferences are only for the
      // current session
      PreferencesData.setDoSave(false);

      if (parser.isUploadMode()) {

        if (parser.getFilenames().size() != 1)
        {
          showError(_("Multiple files not supported"), _("The --upload option supports only one file at a time"), null);
        }

        List<String> warningsAccumulator = new LinkedList<String>();
        boolean success = false;
        try {
          // Editor constructor loads the sketch with handleOpenInternal() that
          // creates a new Sketch that, in trun, calls load() inside its constructor
          // This translates here as:
          //   SketchData data = new SketchData(file);
          //   File tempBuildFolder = getBuildFolder();
          //   data.load();
          SketchData data = new SketchData(absoluteFile(parser.getFilenames().get(0)));
          File tempBuildFolder = getBuildFolder();
          data.load();

          // Sketch.exportApplet()
          //  - calls Sketch.prepare() that calls Sketch.ensureExistence()
          //  - calls Sketch.build(verbose=false) that calls Sketch.ensureExistence(), set progressListener and calls Compiler.build()
          //  - calls Sketch.upload() (see later...)
          if (!data.getFolder().exists()) showError(_("No sketch"), _("Can't find the sketch in the specified path"), null);
          String suggestedClassName = Compiler.build(data, tempBuildFolder.getAbsolutePath(), tempBuildFolder, null, parser.isDoVerboseBuild());
          if (suggestedClassName == null) showError(_("Error while verifying"), _("An error occurred while verifying the sketch"), null);
          showMessage(_("Done compiling"), _("Done compiling"));

          //  - chiama Sketch.upload() ... to be continued ...        
          Uploader uploader = Compiler.getUploaderByPreferences(parser.isNoUploadPort());
          if (uploader.requiresAuthorization() && !PreferencesData.has(uploader.getAuthorizationKey())) showError("...", "...", null);
          try {
            success = Compiler.upload(data, uploader, tempBuildFolder.getAbsolutePath(), suggestedClassName, parser.isDoUseProgrammer(), parser.isNoUploadPort(), warningsAccumulator);
            showMessage(_("Done uploading"), _("Done uploading"));
          } finally {
            if (uploader.requiresAuthorization() && !success) {
              PreferencesData.remove(uploader.getAuthorizationKey());
            }
          }
        } catch (Exception e) {
          showError(_("Error while verifying/uploading"), _("An error occurred while verifying/uploading the sketch"), e);
        }
        for (String warning : warningsAccumulator) {
          System.out.print(_("Warning"));
          System.out.print(": ");
          System.out.println(warning);
        }
        if (!success) showError(_("Error while uploading"), _("An error occurred while uploading the sketch"), null);
      } else {

        for (String path : parser.getFilenames())
        {
          try {
            // Editor constructor loads sketch with handleOpenInternal() that
            // creates a new Sketch that calls load() in its constructor
            // This translates here as:
            //   SketchData data = new SketchData(file);
            //   File tempBuildFolder = getBuildFolder();
            //   data.load();
            SketchData data = new SketchData(absoluteFile(path));
            File tempBuildFolder = getBuildFolder();
            data.load();

            // Sketch.prepare() calls Sketch.ensureExistence()
            // Sketch.build(verbose) calls Sketch.ensureExistence() and set progressListener and, finally, calls Compiler.build()
            // This translates here as:
            //    if (!data.getFolder().exists()) showError(...);
            //    String ... = Compiler.build(data, tempBuildFolder.getAbsolutePath(), tempBuildFolder, null, verbose);
            if (!data.getFolder().exists()) showError(_("No sketch"), _("Can't find the sketch in the specified path"), null);
            String suggestedClassName = Compiler.build(data, tempBuildFolder.getAbsolutePath(), tempBuildFolder, null, parser.isDoVerboseBuild());
            if (suggestedClassName == null) showError(_("Error while verifying"), _("An error occurred while verifying the sketch"), null);
            showMessage(_("Done compiling"), _("Done compiling"));
          } catch (Exception e) {
            showError(_("Error while verifying"), _("An error occurred while verifying the sketch"), e);
          }
        }

      }

      // No errors exit gracefully
      System.exit(0);
    }
    else if (parser.isGetPrefMode()) {
      String value = PreferencesData.get(parser.getGetPref(), null);
      if (value != null) {
        System.out.println(value);
        System.exit(0);
      } else {
        System.exit(4);
      }
    }
  }

  static public void initLogger() {
    System.setProperty(LogFactoryImpl.LOG_PROPERTY, NoOpLog.class.getCanonicalName());
    Logger.getLogger("javax.jmdns").setLevel(Level.OFF);
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
      showError(_("Problem Setting the Platform"),
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

  static public void initVersion() {
    // help 3rd party installers find the correct hardware path
    PreferencesData.set("last.ide." + VERSION_NAME + ".hardwarepath", getHardwarePath());
    PreferencesData.set("last.ide." + VERSION_NAME + ".daterun", "" + (new Date()).getTime() / 1000);
  }

  /**
   * Return true if the name is valid for a Processing sketch.
   */
  static public boolean isSanitaryName(String name) {
    return sanitizeName(name).equals(name);
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
        packages.put(target, new TargetPackage(target, subfolder));
      } catch (TargetPlatformException e) {
        System.out.println("WARNING: Error loading hardware folder " + target);
        System.out.println("  " + e.getMessage());
      }
    }
  }

  /**
   * Grab the contents of a file as a string.
   */
  static public String loadFile(File file) throws IOException {
    String[] contents = PApplet.loadStrings(file);
    if (contents == null) return null;
    return PApplet.join(contents, "\n");
  }

  static public void main(String args[]) throws Exception {
    if (args.length == 0)
      showError(_("No parameters"), _("No command line parameters found"), null);

    initPlatform();
    
    initPortableFolder();
    
    initParameters(args);
    
    init(args);
  }

  static public void onBoardOrPortChange() {
    examplesFolder = getContentFile("examples");
    toolsFolder = getContentFile("tools");
    librariesFolders = new ArrayList<File>();
    librariesFolders.add(getContentFile("libraries"));

    // Add library folder for the current selected platform
    TargetPlatform targetPlatform = getTargetPlatform();
    if (targetPlatform != null) {
      String core = getBoardPreferences().get("build.core");
      if (core.contains(":")) {
        String referencedCore = core.split(":")[0];
        TargetPlatform referencedPlatform = getTargetPlatform(referencedCore, targetPlatform.getId());
        if (referencedPlatform != null) {
          File referencedPlatformFolder = referencedPlatform.getFolder();
          librariesFolders.add(new File(referencedPlatformFolder, "libraries"));
        }
      }
      File platformFolder = targetPlatform.getFolder();
      librariesFolders.add(new File(platformFolder, "libraries"));
      librariesFolders.add(getSketchbookLibrariesFolder());
    }

    // Scan for libraries in each library folder.
    // Libraries located in the latest folders on the list can override
    // other libraries with the same name.
    try {
      scanAndUpdateLibraries(librariesFolders);
    } catch (IOException e) {
      showWarning(_("Error"), _("Error loading libraries"), e);
    }

    populateImportToLibraryTable();
  }

  static public void populateImportToLibraryTable() {
    // Populate importToLibraryTable
    importToLibraryTable = new HashMap<String, Library>();
    for (Library lib : getLibraries()) {
      try {
        String headers[] = headerListFromIncludePath(lib.getSrcFolder());
        for (String header : headers) {
          Library old = importToLibraryTable.get(header);
          if (old != null) {
            // If a library was already found with this header, keep
            // it if the library's name matches the header name.
            String name = header.substring(0, header.length() - 2);
            if (old.getFolder().getPath().endsWith(name))
              continue;
          }
          importToLibraryTable.put(header, lib);
        }
      } catch (IOException e) {
        showWarning(_("Error"), I18n
            .format("Unable to list header files in {0}", lib.getSrcFolder()), e);
      }
    }
  }

  static public void initParameters(String args[]) {
    String preferencesFile = null;

    // Do a first pass over the commandline arguments, the rest of them
    // will be processed by the Base constructor. Note that this loop
    // does not look at the last element of args, to prevent crashing
    // when no parameter was specified to an option. Later, Base() will
    // then show an error for these.
    for (int i = 0; i < args.length - 1; i++) {
      if (args[i].equals("--preferences-file")) {
        ++i;
        preferencesFile = args[i];
        continue;
      }
      if (args[i].equals("--curdir")) {
        i++;
        currentDirectory = args[i];
        continue;
      }
    }

    // run static initialization that grabs all the prefs
    PreferencesData.init(absoluteFile(preferencesFile));
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
        if (!PreferencesData.getBoolean("compiler.save_build_files")) {
          if (!dead.delete()) {
            // temporarily disabled
        System.err.println(I18n.format(_("Could not delete {0}"), dead));
          }
        }
      } else {
        removeDir(dead);
        //dead.delete();
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
        System.err.println(I18n.format(_("Could not delete {0}"), dir));
      }
    }
  }

  /**
   * Produce a sanitized name that fits our standards for likely to work.
   * <p/>
   * Java classes have a wider range of names that are technically allowed
   * (supposedly any Unicode name) than what we support. The reason for
   * going more narrow is to avoid situations with text encodings and
   * converting during the process of moving files between operating
   * systems, i.e. uploading from a Windows machine to a Linux server,
   * or reading a FAT32 partition in OS X and using a thumb drive.
   * <p/>
   * This helper function replaces everything but A-Z, a-z, and 0-9 with
   * underscores. Also disallows starting the sketch name with a digit.
   */
  static public String sanitizeName(String origName) {
    char c[] = origName.toCharArray();
    StringBuffer buffer = new StringBuffer();

    // can't lead with a digit, so start with an underscore
    if ((c[0] >= '0') && (c[0] <= '9')) {
      buffer.append('_');
    }
    for (int i = 0; i < c.length; i++) {
      if (((c[i] >= '0') && (c[i] <= '9')) ||
          ((c[i] >= 'a') && (c[i] <= 'z')) ||
          ((c[i] >= 'A') && (c[i] <= 'Z')) ||
          ((i > 0) && (c[i] == '-')) ||
          ((i > 0) && (c[i] == '.'))) {
        buffer.append(c[i]);
      } else {
        buffer.append('_');
      }
    }
    // let's not be ridiculous about the length of filenames.
    // in fact, Mac OS 9 can handle 255 chars, though it can't really
    // deal with filenames longer than 31 chars in the Finder.
    // but limiting to that for sketches would mean setting the
    // upper-bound on the character limit here to 25 characters
    // (to handle the base name + ".class")
    if (buffer.length() > 63) {
      buffer.setLength(63);
    }
    return buffer.toString();
  }

  /**
   * Spew the contents of a String object out to a file.
   */
  static public void saveFile(String str, File file) throws IOException {
    File temp = File.createTempFile(file.getName(), null, file.getParentFile());
    PApplet.saveStrings(temp, new String[] { str });
    if (file.exists()) {
      boolean result = file.delete();
      if (!result) {
        throw new IOException(
      I18n.format(
        _("Could not remove old version of {0}"),
        file.getAbsolutePath()));
      }
    }
    boolean result = temp.renameTo(file);
    if (!result) {
      throw new IOException(
    I18n.format(
      _("Could not replace {0}"),
      file.getAbsolutePath()));
    }
  }

  static public void scanAndUpdateLibraries(List<File> folders) throws IOException {
    libraries = scanLibraries(folders);
  }

  static public LibraryList scanLibraries(List<File> folders) throws IOException {
    LibraryList res = new LibraryList();
    for (File folder : folders)
      res.addOrReplaceAll(scanLibraries(folder));
    return res;
  }

  static public LibraryList scanLibraries(File folder) throws IOException {
    LibraryList res = new LibraryList();

    String list[] = folder.list(new OnlyDirs());
    // if a bad folder or something like that, this might come back null
    if (list == null)
      return res;

    for (String libName : list) {
      File subfolder = new File(folder, libName);
      if (!isSanitaryName(libName)) {
        String mess = I18n.format(_("The library \"{0}\" cannot be used.\n"
            + "Library names must contain only basic letters and numbers.\n"
            + "(ASCII only and no spaces, and it cannot start with a number)"),
                                  libName);
        showMessage(_("Ignoring bad library name"), mess);
        continue;
      }

      try {
        Library lib = Library.create(subfolder);
        // (also replace previously found libs with the same name)
        if (lib != null)
          res.addOrReplace(lib);
      } catch (IOException e) {
        System.out.println(I18n.format(_("Invalid library found in {0}: {1}"),
                                       subfolder, e.getMessage()));
      }
    }
    return res;
  }

  static public void selectBoard(TargetBoard targetBoard) {
    TargetPlatform targetPlatform = targetBoard.getContainerPlatform();
    TargetPackage targetPackage = targetPlatform.getContainerPackage();

    PreferencesData.set("target_package", targetPackage.getId());
    PreferencesData.set("target_platform", targetPlatform.getId());
    PreferencesData.set("board", targetBoard.getId());

    File platformFolder = targetPlatform.getFolder();
    PreferencesData.set("runtime.platform.path", platformFolder.getAbsolutePath());
    PreferencesData.set("runtime.hardware.path", platformFolder.getParentFile().getAbsolutePath());
  }

  public static void selectSerialPort(String port) {
    PreferencesData.set("serial.port", port);
    if (port.startsWith("/dev/"))
      PreferencesData.set("serial.port.file", port.substring(5));
    else
      PreferencesData.set("serial.port.file", port);
  }

  public static void setBuildFolder(File newBuildFolder) {
    buildFolder = newBuildFolder;
  }

  static public void showError(String title, String message, int exit_code) {
    showError(title, message, null, exit_code);
  }

  static public void showError(String title, String message, Throwable e) {
    notifier.showError(title, message, e, 1);
  }

  /**
   * Show an error message that's actually fatal to the program.
   * This is an error that can't be recovered. Use showWarning()
   * for errors that allow P5 to continue running.
   */
  static public void showError(String title, String message, Throwable e, int exit_code) {
    notifier.showError(title, message, e, exit_code);
  }

  /**
   * "No cookie for you" type messages. Nothing fatal or all that
   * much of a bummer, but something to notify the user about.
   */
  static public void showMessage(String title, String message) {
    notifier.showMessage(title, message);
  }

  /**
   * Non-fatal error message with optional stack trace side dish.
   */
  static public void showWarning(String title, String message, Exception e) {
    notifier.showWarning(title, message, e);
  }

}
