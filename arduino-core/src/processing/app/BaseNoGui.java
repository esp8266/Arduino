package processing.app;

import cc.arduino.contributions.SignatureVerificationFailedException;
import cc.arduino.contributions.libraries.LibrariesIndexer;
import cc.arduino.contributions.packages.ContributedTool;
import cc.arduino.contributions.packages.ContributionsIndexer;
import cc.arduino.files.DeleteFilesOnShutdown;
import cc.arduino.packages.DiscoveryManager;
import cc.arduino.packages.Uploader;
import com.fasterxml.jackson.core.JsonProcessingException;
import org.apache.commons.compress.utils.IOUtils;
import org.apache.commons.logging.impl.LogFactoryImpl;
import org.apache.commons.logging.impl.NoOpLog;
import processing.app.debug.Compiler;
import processing.app.debug.*;
import processing.app.helpers.*;
import processing.app.helpers.filefilters.OnlyDirs;
import processing.app.helpers.filefilters.OnlyFilesWithExtension;
import processing.app.legacy.PApplet;
import processing.app.packages.LibraryList;
import processing.app.packages.UserLibrary;

import java.io.*;
import java.net.URISyntaxException;
import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;

import static processing.app.I18n._;

public class BaseNoGui {

  /** Version string to be used for build */
  public static final int REVISION = 10605;
  /** Extended version string displayed on GUI */
  public static final String VERSION_NAME = "1.6.5";
  public static final String VERSION_NAME_LONG;

  static {
    String versionNameLong = VERSION_NAME;
    File hourlyBuildTxt = new File(getContentFile("lib"), "hourlyBuild.txt");
    if (hourlyBuildTxt.exists() && hourlyBuildTxt.canRead()) {
      versionNameLong += " Hourly Build";
      try {
        versionNameLong += " " + FileUtils.readFileToString(hourlyBuildTxt).trim();
      } catch (IOException e) {
        //noop
      }
    }
    VERSION_NAME_LONG = versionNameLong;
  }

  static File buildFolder;

  // Current directory to use for relative paths specified on the
  // commandline
  static String currentDirectory = System.getProperty("user.dir");

  private static DiscoveryManager discoveryManager = new DiscoveryManager();
  
  // these are static because they're used by Sketch
  static private File examplesFolder;
  static private File toolsFolder;

  // maps #included files to their library folder
  public static Map<String, LibraryList> importToLibraryTable;

  // maps library name to their library folder
  static private LibraryList libraries;

  // XXX: Remove this field
  static private List<File> librariesFolders;

  static UserNotifier notifier = new BasicUserNotifier();

  static public Map<String, TargetPackage> packages;

  static Platform platform;

  static File portableFolder = null;
  static final String portableSketchbookFolder = "sketchbook";

  public static ContributionsIndexer indexer;
  static LibrariesIndexer librariesIndexer;

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
        DeleteFilesOnShutdown.add(buildFolder);
      }
    }
    return buildFolder;
  }

  static public PreferencesMap getBoardPreferences() {
    TargetBoard board = getTargetBoard();
    if (board == null)
      return null;
    String boardId = board.getId();
    
    PreferencesMap prefs = new PreferencesMap(board.getPreferences());

    String extendedName = prefs.get("name");
    for (String menuId : board.getMenuIds()) {
      if (!board.hasMenu(menuId))
        continue;

      // Get "custom_[MENU_ID]" preference (for example "custom_cpu")
      String entry = PreferencesData.get("custom_" + menuId);
      if (entry != null && entry.startsWith(boardId)) {

        String selectionId = entry.substring(boardId.length() + 1);
        prefs.putAll(board.getMenuPreferences(menuId, selectionId));

        // Update the name with the extended configuration
        extendedName += ", " + board.getMenuLabel(menuId, selectionId);
      }
    }
    prefs.put("name", extendedName);
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
      FileWriter freadme = null;
      try {
        libdir.mkdirs();
        freadme = new FileWriter(new File(libdir, "readme.txt"));
        freadme.write(_("For information on installing libraries, see: " +
                        "http://www.arduino.cc/en/Guide/Libraries\n"));
      } catch (Exception e) {
      } finally {
        IOUtils.closeQuietly(freadme);
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
    LibraryList libs = BaseNoGui.librariesIndexer.getInstalledLibraries();
    return libs.filterLibrariesInSubfolder(getSketchbookFolder());
  }

  /**
   * Given a folder, return a list of the header files in that folder (but not
   * the header files in its sub-folders, as those should be included from
   * within the header files at the top-level).
   */
  static public String[] headerListFromIncludePath(File path) throws IOException {
    String[] list = path.list(new OnlyFilesWithExtension(".h", ".hh", ".hpp"));
    if (list == null) {
      throw new IOException();
    }
    return list;
  }

  static public void init(String[] args) throws Exception {
    CommandlineParser parser = new CommandlineParser(args);
    parser.parseArgumentsPhase1();

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

    parser.parseArgumentsPhase2();

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
          String suggestedClassName = Compiler.build(data, tempBuildFolder.getAbsolutePath(), tempBuildFolder, null, parser.isDoVerboseBuild(), false);
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
            String suggestedClassName = Compiler.build(data, tempBuildFolder.getAbsolutePath(), tempBuildFolder, null, parser.isDoVerboseBuild(), false);
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
      dumpPrefs(parser);
    }
  }

  protected static void dumpPrefs(CommandlineParser parser) {
    if (parser.getGetPref() != null) {
      String value = PreferencesData.get(parser.getGetPref(), null);
      if (value != null) {
        System.out.println(value);
        System.exit(0);
      } else {
        System.exit(4);
      }
    } else {
      System.out.println("#PREFDUMP#");
      PreferencesMap prefs = PreferencesData.getMap();
      for (Map.Entry<String, String> entry : prefs.entrySet()) {
        System.out.println(entry.getKey() + "=" + entry.getValue());
      }
      System.exit(0);
    }
  }

  static public void initLogger() {
    System.setProperty(LogFactoryImpl.LOG_PROPERTY, NoOpLog.class.getCanonicalName());
    Logger.getLogger("javax.jmdns").setLevel(Level.OFF);
  }

  static public void initPackages() throws Exception {
    indexer = new ContributionsIndexer(BaseNoGui.getSettingsFolder(), BaseNoGui.getPlatform());
    File indexFile = indexer.getIndexFile("package_index.json");
    File defaultPackageJsonFile = new File(getContentFile("dist"), "package_index.json");
    if (!indexFile.isFile() || (defaultPackageJsonFile.isFile() && defaultPackageJsonFile.lastModified() > indexFile.lastModified())) {
      FileUtils.copyFile(defaultPackageJsonFile, indexFile);
    } else if (!indexFile.isFile()) {
      // Otherwise create an empty packages index
      FileOutputStream out = null;
      try {
        out = new FileOutputStream(indexFile);
        out.write("{ \"packages\" : [ ] }".getBytes());
      } finally {
        IOUtils.closeQuietly(out);
      }
    }

    File indexSignatureFile = indexer.getIndexFile("package_index.json.sig");
    File defaultPackageJsonSignatureFile = new File(getContentFile("dist"), "package_index.json.sig");
    if (!indexSignatureFile.isFile() || (defaultPackageJsonSignatureFile.isFile() && defaultPackageJsonSignatureFile.lastModified() > indexSignatureFile.lastModified())) {
      FileUtils.copyFile(defaultPackageJsonSignatureFile, indexSignatureFile);
    }

    try {
      indexer.parseIndex();
    } catch (JsonProcessingException e) {
      FileUtils.deleteIfExists(indexFile);
      FileUtils.deleteIfExists(indexSignatureFile);
      throw e;
    } catch (SignatureVerificationFailedException e) {
      FileUtils.deleteIfExists(indexFile);
      FileUtils.deleteIfExists(indexSignatureFile);
      throw e;
    }
    indexer.syncWithFilesystem(getHardwareFolder());

    packages = new LinkedHashMap<String, TargetPackage>();
    loadHardware(getHardwareFolder());
    loadContributedHardware(indexer);
    loadHardware(getSketchbookHardwareFolder());
    createToolPreferences(indexer);

    librariesIndexer = new LibrariesIndexer(BaseNoGui.getSettingsFolder(), indexer);
    File librariesIndexFile = librariesIndexer.getIndexFile();
    if (!librariesIndexFile.isFile()) {
      File defaultLibraryJsonFile = new File(getContentFile("dist"), "library_index.json");
      if (defaultLibraryJsonFile.isFile()) {
        FileUtils.copyFile(defaultLibraryJsonFile, librariesIndexFile);
      } else {
        FileOutputStream out = null;
        try {
          // Otherwise create an empty packages index
          out = new FileOutputStream(librariesIndexFile);
          out.write("{ \"libraries\" : [ ] }".getBytes());
        } catch (IOException e) {
          e.printStackTrace();
        } finally {
          IOUtils.closeQuietly(out);
        }
      }
    }
    try {
      librariesIndexer.parseIndex();
    } catch (JsonProcessingException e) {
      FileUtils.deleteIfExists(librariesIndexFile);
      throw e;
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
        packages.put(target, new LegacyTargetPackage(target, subfolder));
      } catch (TargetPlatformException e) {
        System.out.println("WARNING: Error loading hardware folder " + new File(folder, target));
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
    if (args.length == 0) {
      showError(_("No parameters"), _("No command line parameters found"), null);
    }
    System.setProperty("java.net.useSystemProxies", "true");

    Runtime.getRuntime().addShutdownHook(new Thread(DeleteFilesOnShutdown.INSTANCE));

    initPlatform();

    getPlatform().init();

    initPortableFolder();
    
    initParameters(args);

    checkInstallationFolder();

    init(args);
  }

  public static void checkInstallationFolder() {
    if (isIDEInstalledIntoSettingsFolder()) {
      showError(_("Incorrect IDE installation folder"), _("Your copy of the IDE is installed in a subfolder of your settings folder.\nPlease move the IDE to another folder."), 10);
    }
    if (isIDEInstalledIntoSketchbookFolder()) {
      showError(_("Incorrect IDE installation folder"), _("Your copy of the IDE is installed in a subfolder of your sketchbook.\nPlease move the IDE to another folder."), 10);
    }
  }

  public static boolean isIDEInstalledIntoSketchbookFolder() {
    return PreferencesData.has("sketchbook.path") && FileUtils.isSubDirectory(new File(PreferencesData.get("sketchbook.path")), new File(PreferencesData.get("runtime.ide.path")));
  }

  public static boolean isIDEInstalledIntoSettingsFolder() {
    try {
      return FileUtils.isSubDirectory(BaseNoGui.getPlatform().getSettingsFolder(), new File(PreferencesData.get("runtime.ide.path")));
    } catch (Exception e) {
      return false;
    }
  }

  static public void onBoardOrPortChange() {
    examplesFolder = getContentFile("examples");
    toolsFolder = getContentFile("tools");
    librariesFolders = new ArrayList<File>();

    // Add IDE libraries folder
    librariesFolders.add(getContentFile("libraries"));

    TargetPlatform targetPlatform = getTargetPlatform();
    if (targetPlatform != null) {
      String core = getBoardPreferences().get("build.core", "arduino");
      if (core.contains(":")) {
        String referencedCore = core.split(":")[0];
        TargetPlatform referencedPlatform = getTargetPlatform(referencedCore, targetPlatform.getId());
        if (referencedPlatform != null) {
          File referencedPlatformFolder = referencedPlatform.getFolder();
          // Add libraries folder for the referenced platform
          File folder = new File(referencedPlatformFolder, "libraries");
          librariesFolders.add(folder);
        }
      }
      File platformFolder = targetPlatform.getFolder();
      // Add libraries folder for the selected platform
      File folder = new File(platformFolder, "libraries");
      librariesFolders.add(folder);
    }

    // Add libraries folder for the sketchbook
    librariesFolders.add(getSketchbookLibrariesFolder());

    // Scan for libraries in each library folder.
    // Libraries located in the latest folders on the list can override
    // other libraries with the same name.
    BaseNoGui.librariesIndexer.setSketchbookLibrariesFolder(getSketchbookLibrariesFolder());
    BaseNoGui.librariesIndexer.setLibrariesFolders(librariesFolders);
    BaseNoGui.librariesIndexer.rescanLibraries();

    populateImportToLibraryTable();
  }

  static protected void loadContributedHardware(ContributionsIndexer indexer) {
    for (TargetPackage pack : indexer.createTargetPackages()) {
      packages.put(pack.getId(), pack);
    }
  }

  static private void createToolPreferences(ContributionsIndexer indexer) {
    // Remove previous runtime preferences
    final String prefix = "runtime.tools.";
    PreferencesData.removeAllKeysWithPrefix(prefix);

    for (ContributedTool tool : indexer.getInstalledTools()) {
      File installedFolder = tool.getDownloadableContribution(getPlatform()).getInstalledFolder();
      if (installedFolder != null) {
        PreferencesData.set(prefix + tool.getName() + ".path", installedFolder.getAbsolutePath());
        PreferencesData.set(prefix + tool.getName() + "-" + tool.getVersion() + ".path", installedFolder.getAbsolutePath());
      }
    }
  }

  static public void populateImportToLibraryTable() {
    // Populate importToLibraryTable. Each header filename maps to
    // a list of libraries. Compiler.java will use only the first
    // library on each list. The others are used only to advise
    // user of ambiguously matched and duplicate libraries.
    importToLibraryTable = new HashMap<String, LibraryList>();
    for (UserLibrary lib : librariesIndexer.getInstalledLibraries()) {
      try {
        String headers[] = headerListFromIncludePath(lib.getSrcFolder());
        for (String header : headers) {
          LibraryList list = importToLibraryTable.get(header);
          if (list == null) {
            // This is the first library found with this header
            list = new LibraryList();
            list.addFirst(lib);
            importToLibraryTable.put(header, list);
          } else {
            UserLibrary old = list.peekFirst();
            boolean useThisLib = true;
            // This is the case where 2 libraries have a .h header
            // with the same name.  We must decide which library to
            // use when a sketch has #include "name.h"
            //
            // When all other factors are equal, "libName" is
            // used in preference to "oldName", because getLibraries()
            // gives the library list in order from less specific to
            // more specific locations.
            //
            // But often one library is more clearly the user's
            // intention to use.  Many cases are tested, always first
            // for "libName", then for "oldName".
            //
            String name = header.substring(0, header.length() - 2); // name without ".h"
            String oldName = old.getInstalledFolder().getName();  // just the library folder name
            String libName = lib.getInstalledFolder().getName();  // just the library folder name
            //System.out.println("name conflict: " + name);
            //System.out.println(" old = " + oldName + " -> " + old.getInstalledFolder().getPath());
            //System.out.println(" new = " + libName + " -> " + lib.getInstalledFolder().getPath());
            String name_lc = name.toLowerCase();
            String oldName_lc = oldName.toLowerCase();
            String libName_lc = libName.toLowerCase();
            // always favor a perfect name match
            if (libName.equals(name)) {
            } else if (oldName.equals(name)) {
                useThisLib = false;
            // check for "-master" appended (zip file from github)
            } else if (libName.equals(name+"-master")) {
            } else if (oldName.equals(name+"-master")) {
                useThisLib = false;
            // next, favor a match with other stuff appended
            } else if (libName.startsWith(name)) {
            } else if (oldName.startsWith(name)) {
                useThisLib = false;
            // otherwise, favor a match with stuff prepended
            } else if (libName.endsWith(name)) {
            } else if (oldName.endsWith(name)) {
                useThisLib = false;
            // as a last resort, match if stuff prepended and appended
            } else if (libName.contains(name)) {
            } else if (oldName.contains(name)) {
                useThisLib = false;
            // repeat all the above tests, with case insensitive matching
            } else if (libName_lc.equals(name_lc)) {
            } else if (oldName_lc.equals(name_lc)) {
                useThisLib = false;
            } else if (libName_lc.equals(name_lc+"-master")) {
            } else if (oldName_lc.equals(name_lc+"-master")) {
                useThisLib = false;
            } else if (libName_lc.startsWith(name_lc)) {
            } else if (oldName_lc.startsWith(name_lc)) {
                useThisLib = false;
            } else if (libName_lc.endsWith(name_lc)) {
            } else if (oldName_lc.endsWith(name_lc)) {
                useThisLib = false;
            } else if (libName_lc.contains(name_lc)) {
            } else if (oldName_lc.contains(name_lc)) {
                useThisLib = false;
            } else {
              // none of these tests matched, so just default to "libName".
            }
            if (useThisLib) {
              list.addFirst(lib);
            } else {
              list.addLast(lib);
            }
          }
        }
      } catch (IOException e) {
        showWarning(_("Error"), I18n
            .format("Unable to list header files in {0}", lib.getSrcFolder()), e);
      }
    }
    // repeat for ALL libraries, to pick up duplicates not visible normally.
    // any new libraries found here are NEVER used, but they are added to the
    // end of already-found headers, to allow Compiler to report them if
    // the sketch tries to use them.
    for (UserLibrary lib : librariesIndexer.getInstalledLibrariesWithDuplicates()) {
      try {
        String headers[] = headerListFromIncludePath(lib.getSrcFolder());
        for (String header : headers) {
          LibraryList list = importToLibraryTable.get(header);
          if (list != null) {
            if (!(list.hasLibrary(lib))) {
              list.addLast(lib);
              //System.out.println(" duplicate lib: " + lib.getInstalledFolder().getPath());
            }
          }
        }
        } catch (IOException e) {
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
    if (files == null) {
      return;
    }

    for (String file : files) {
      if (file.equals(".") || file.equals("..")) continue;
      File dead = new File(dir, file);
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
    String portFile = port;
    if (port.startsWith("/dev/")) {
      portFile = portFile.substring(5);
    }
    PreferencesData.set("serial.port.file", portFile);
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
