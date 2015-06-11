/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-10 Ben Fry and Casey Reas
  Copyright (c) 2001-04 Massachusetts Institute of Technology

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 2
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

package processing.app;

import cc.arduino.contributions.BuiltInCoreIsNewerCheck;
import cc.arduino.contributions.DownloadableContributionVersionComparator;
import cc.arduino.contributions.VersionHelper;
import cc.arduino.contributions.libraries.*;
import cc.arduino.contributions.libraries.ui.LibraryManagerUI;
import cc.arduino.contributions.packages.ContributedPlatform;
import cc.arduino.contributions.packages.ContributionInstaller;
import cc.arduino.contributions.packages.ContributionsIndexer;
import cc.arduino.contributions.packages.ui.ContributionManagerUI;
import cc.arduino.files.DeleteFilesOnShutdown;
import cc.arduino.packages.DiscoveryManager;
import cc.arduino.utils.Progress;
import cc.arduino.view.*;
import cc.arduino.view.Event;
import com.google.common.base.Predicate;
import com.google.common.base.Predicates;
import com.google.common.collect.Collections2;
import org.apache.commons.compress.utils.IOUtils;
import org.apache.commons.lang3.StringUtils;
import processing.app.debug.TargetBoard;
import processing.app.debug.TargetPackage;
import processing.app.debug.TargetPlatform;
import processing.app.helpers.*;
import processing.app.helpers.filefilters.OnlyDirs;
import processing.app.helpers.filefilters.OnlyFilesWithExtension;
import processing.app.javax.swing.filechooser.FileNameExtensionFilter;
import processing.app.legacy.PApplet;
import processing.app.macosx.ThinkDifferent;
import processing.app.packages.LibraryList;
import processing.app.packages.UserLibrary;
import processing.app.syntax.PdeKeywords;
import processing.app.syntax.SketchTextAreaDefaultInputMap;
import processing.app.tools.MenuScroller;
import processing.app.tools.ZipDeflater;

import javax.swing.*;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import java.util.List;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;

import static processing.app.I18n._;


/**
 * The base class for the main processing application.
 * Primary role of this class is for platform identification and
 * general interaction with the system (launching URLs, loading
 * files and images, etc) that comes from that.
 */
public class Base {

  public static final Predicate<UserLibrary> CONTRIBUTED = new Predicate<UserLibrary>() {
    @Override
    public boolean apply(UserLibrary library) {
      return library.getTypes() == null || library.getTypes().isEmpty() || library.getTypes().contains("Contributed");
    }
  };

  private static final int RECENT_SKETCHES_MAX_SIZE = 5;

  private static boolean commandLine;
  public static volatile Base INSTANCE;

  public static SplashScreenHelper splashScreenHelper = new SplashScreenHelper(SplashScreen.getSplashScreen());
  public static Map<String, Object> FIND_DIALOG_STATE = new HashMap<String, Object>();

  // set to true after the first time the menu is built.
  // so that the errors while building don't show up again.
  boolean builtOnce;

  // classpath for all known libraries for p5
  // (both those in the p5/libs folder and those with lib subfolders
  // found in the sketchbook)
  static public String librariesClassPath;

  // Location for untitled items
  static File untitledFolder;

  // p5 icon for the window
//  static Image icon;

  //  int editorCount;
  List<Editor> editors = Collections.synchronizedList(new ArrayList<Editor>());
  Editor activeEditor;

  // these menus are shared so that the board and serial port selections
  // are the same for all windows (since the board and serial port that are
  // actually used are determined by the preferences, which are shared)
  private List<JMenu> boardsCustomMenus;
  private volatile Action openBoardsManager;

  private final PdeKeywords pdeKeywords;
  private final List<JMenuItem> recentSketchesMenuItems;

  static public void main(String args[]) throws Exception {
    System.setProperty("awt.useSystemAAFontSettings", "on");
    System.setProperty("swing.aatext", "true");
    System.setProperty("java.net.useSystemProxies", "true");

    splashScreenHelper.splashText(_("Loading configuration..."));

    if (OSUtils.isMacOS()) {
      ThinkDifferent.init();
    }

    try {
      guardedMain(args);
    } catch (Throwable e) {
      e.printStackTrace(System.err);
      System.exit(255);
    }
  }

  static public void guardedMain(String args[]) throws Exception {
    Runtime.getRuntime().addShutdownHook(new Thread(DeleteFilesOnShutdown.INSTANCE));

    BaseNoGui.initLogger();

    initLogger();

    BaseNoGui.initPlatform();

    BaseNoGui.getPlatform().init();

    BaseNoGui.initPortableFolder();

    BaseNoGui.initParameters(args);

    System.setProperty("swing.aatext", PreferencesData.get("editor.antialias", "true"));

    BaseNoGui.initVersion();

//    if (System.getProperty("mrj.version") != null) {
//      //String jv = System.getProperty("java.version");
//      String ov = System.getProperty("os.version");
//      if (ov.startsWith("10.5")) {
//        System.setProperty("apple.laf.useScreenMenuBar", "true");
//      }
//    }

    /*
    commandLine = false;
    if (args.length >= 2) {
      if (args[0].startsWith("--")) {
        commandLine = true;
      }
    }

    if (PApplet.javaVersion < 1.5f) {
      //System.err.println("no way man");
      Base.showError("Need to install Java 1.5",
                     "This version of Processing requires    \n" +
                     "Java 1.5 or later to run properly.\n" +
                     "Please visit java.com to upgrade.", null);
    }
    */

//    // Set the look and feel before opening the window
//    try {
//      platform.setLookAndFeel();
//    } catch (Exception e) {
//      System.err.println("Non-fatal error while setting the Look & Feel.");
//      System.err.println("The error message follows, however Processing should run fine.");
//      System.err.println(e.getMessage());
//      //e.printStackTrace();
//    }

    // Use native popups so they don't look so crappy on osx
    JPopupMenu.setDefaultLightWeightPopupEnabled(false);

    // Don't put anything above this line that might make GUI,
    // because the platform has to be inited properly first.

    // Make sure a full JDK is installed
    //initRequirements();

    // setup the theme coloring fun
    Theme.init();

    // Set the look and feel before opening the window
    try {
      BaseNoGui.getPlatform().setLookAndFeel();
    } catch (Exception e) {
      String mess = e.getMessage();
      if (mess.indexOf("ch.randelshofer.quaqua.QuaquaLookAndFeel") == -1) {
        System.err.println(_("Non-fatal error while setting the Look & Feel."));
        System.err.println(_("The error message follows, however Arduino should run fine."));
        System.err.println(mess);
      }
    }

    // Create a location for untitled sketches
    untitledFolder = BaseNoGui.createTempFolder("untitled");
    DeleteFilesOnShutdown.add(untitledFolder);

    INSTANCE = new Base(args);
  }

  
  static public void initLogger() {
    Handler consoleHandler = new ConsoleLogger();
    consoleHandler.setLevel(Level.ALL);
    consoleHandler.setFormatter(new LogFormatter("%1$tl:%1$tM:%1$tS [%4$7s] %2$s: %5$s%n"));
    
    Logger globalLogger = Logger.getLogger(Logger.GLOBAL_LOGGER_NAME);
    globalLogger.setLevel(consoleHandler.getLevel());
    
    // Remove default
    Handler[] handlers = globalLogger.getHandlers();
    for(Handler handler : handlers) {
        globalLogger.removeHandler(handler);
    }
    Logger root = Logger.getLogger("");
    handlers = root.getHandlers();
    for(Handler handler : handlers) {
      root.removeHandler(handler);
    }
    
    globalLogger.addHandler(consoleHandler);
    
    Logger.getLogger("cc.arduino.packages.autocomplete").setParent(globalLogger);
    Logger.getLogger("br.com.criativasoft.cpluslibparser").setParent(globalLogger);
    Logger.getLogger(Base.class.getPackage().getName()).setParent(globalLogger);
    
  }


  static protected void setCommandLine() {
    commandLine = true;
  }


  static protected boolean isCommandLine() {
    return commandLine;
  }

  // Returns a File object for the given pathname. If the pathname
  // is not absolute, it is interpreted relative to the current
  // directory when starting the IDE (which is not the same as the
  // current working directory!).
  static public File absoluteFile(String path) {
    return BaseNoGui.absoluteFile(path);
  }

  public Base(String[] args) throws Exception {
    BaseNoGui.notifier = new GUIUserNotifier(this);
    this.recentSketchesMenuItems = new LinkedList<JMenuItem>();

    CommandlineParser parser = new CommandlineParser(args);
    parser.parseArgumentsPhase1();

    BaseNoGui.checkInstallationFolder();

    String sketchbookPath = BaseNoGui.getSketchbookPath();

    // If no path is set, get the default sketchbook folder for this platform
    if (sketchbookPath == null) {
      File defaultFolder = getDefaultSketchbookFolderOrPromptForIt();
      if (BaseNoGui.getPortableFolder() != null)
        PreferencesData.set("sketchbook.path", BaseNoGui.getPortableSketchbookFolder());
      else
        PreferencesData.set("sketchbook.path", defaultFolder.getAbsolutePath());
      if (!defaultFolder.exists()) {
        defaultFolder.mkdirs();
      }
    }

    splashScreenHelper.splashText(_("Initializing packages..."));
    BaseNoGui.initPackages();
    splashScreenHelper.splashText(_("Preparing boards..."));
    rebuildBoardsMenu();

    // Setup board-dependent variables.
    onBoardOrPortChange();

    this.pdeKeywords = new PdeKeywords();
    this.pdeKeywords.reload();

    parser.parseArgumentsPhase2();

    for (String path : parser.getFilenames()) {
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

      boolean showEditor = parser.isGuiMode();
      if (!parser.isForceSavePrefs())
        PreferencesData.setDoSave(showEditor);
      if (handleOpen(file, nextEditorLocation(), showEditor, false) == null) {
        String mess = I18n.format(_("Failed to open sketch: \"{0}\""), path);
        // Open failure is fatal in upload/verify mode
        if (parser.isVerifyOrUploadMode())
          showError(null, mess, 2);
        else
          showWarning(null, mess, null);
      }
    }

    // Save the preferences. For GUI mode, this happens in the quit
    // handler, but for other modes we should also make sure to save
    // them.
    PreferencesData.save();

    if (parser.isInstallBoard()) {
      ContributionsIndexer indexer = new ContributionsIndexer(BaseNoGui.getSettingsFolder(), BaseNoGui.getPlatform());
      ContributionInstaller installer = new ContributionInstaller(indexer, BaseNoGui.getPlatform()) {
        private String lastStatus = "";

        @Override
        protected void onProgress(Progress progress) {
          if (!lastStatus.equals(progress.getStatus())) {
            System.out.println(progress.getStatus());
          }
          lastStatus = progress.getStatus();
        }
      };
      List<String> downloadedPackageIndexFiles = installer.updateIndex();
      installer.deleteUnknownFiles(downloadedPackageIndexFiles);
      indexer.parseIndex();
      indexer.syncWithFilesystem(BaseNoGui.getHardwareFolder());

      String[] boardToInstallParts = parser.getBoardToInstall().split(":");

      ContributedPlatform selected = null;
      if (boardToInstallParts.length == 3) {
        selected = indexer.getIndex().findPlatform(boardToInstallParts[0], boardToInstallParts[1], VersionHelper.valueOf(boardToInstallParts[2]).toString());
      } else if (boardToInstallParts.length == 2) {
        List<ContributedPlatform> platformsByName = indexer.getIndex().findPlatforms(boardToInstallParts[0], boardToInstallParts[1]);
        Collections.sort(platformsByName, new DownloadableContributionVersionComparator());
        if (!platformsByName.isEmpty()) {
          selected = platformsByName.get(platformsByName.size() - 1);
        }
      }
      if (selected == null) {
        System.out.println(_("Selected board is not available"));
        System.exit(1);
      }

      ContributedPlatform installed = indexer.getInstalled(boardToInstallParts[0], boardToInstallParts[1]);

      if (!selected.isReadOnly()) {
        installer.install(selected);
      }

      if (installed != null && !installed.isReadOnly()) {
        installer.remove(installed);
      }

      System.exit(0);

    } else if (parser.isInstallLibrary()) {
      LibrariesIndexer indexer = new LibrariesIndexer(BaseNoGui.getSettingsFolder(), new ContributionsIndexer(BaseNoGui.getSettingsFolder(), BaseNoGui.getPlatform()));
      LibraryInstaller installer = new LibraryInstaller(indexer, BaseNoGui.getPlatform()) {
        private String lastStatus = "";

        @Override
        protected void onProgress(Progress progress) {
          if (!lastStatus.equals(progress.getStatus())) {
            System.out.println(progress.getStatus());
          }
          lastStatus = progress.getStatus();
        }
      };
      indexer.parseIndex();
      BaseNoGui.onBoardOrPortChange();
      indexer.setSketchbookLibrariesFolder(BaseNoGui.getSketchbookLibrariesFolder());
      indexer.setLibrariesFolders(BaseNoGui.getLibrariesPath());
      installer.updateIndex();

      for (String library : parser.getLibraryToInstall().split(",")) {
        String[] libraryToInstallParts = library.split(":");

        ContributedLibrary selected = null;
        if (libraryToInstallParts.length == 2) {
          selected = indexer.getIndex().find(libraryToInstallParts[0], VersionHelper.valueOf(libraryToInstallParts[1]).toString());
        } else if (libraryToInstallParts.length == 1) {
          List<ContributedLibrary> librariesByName = indexer.getIndex().find(libraryToInstallParts[0]);
          Collections.sort(librariesByName, new DownloadableContributionVersionComparator());
          if (!librariesByName.isEmpty()) {
            selected = librariesByName.get(librariesByName.size() - 1);
          }
        }
        if (selected == null) {
          System.out.println(_("Selected library is not available"));
          System.exit(1);
        }

        ContributedLibrary installed = indexer.getIndex().getInstalled(libraryToInstallParts[0]);
        if (selected.isReadOnly()) {
          installer.remove(installed);
        } else {
          installer.install(selected, installed);
        }
      }

      System.exit(0);

    } else if (parser.isVerifyOrUploadMode()) {
      splashScreenHelper.close();
      // Set verbosity for command line build
      PreferencesData.set("build.verbose", "" + parser.isDoVerboseBuild());
      PreferencesData.set("upload.verbose", "" + parser.isDoVerboseUpload());
      PreferencesData.set("runtime.preserve.temp.files", Boolean.toString(parser.isPreserveTempFiles()));

      // Make sure these verbosity preferences are only for the
      // current session
      PreferencesData.setDoSave(false);

      Editor editor = editors.get(0);

      if (parser.isUploadMode()) {
        splashScreenHelper.splashText(_("Verifying and uploading..."));
        editor.exportHandler.run();
      } else {
        splashScreenHelper.splashText(_("Verifying..."));
        editor.runHandler.run();
      }

      // Error during build or upload
      int res = editor.status.mode;
      if (res == EditorStatus.ERR)
        System.exit(1);

      // No errors exit gracefully
      System.exit(0);
    } else if (parser.isGuiMode()) {
      splashScreenHelper.splashText(_("Starting..."));

      installKeyboardInputMap();

      // Check if there were previously opened sketches to be restored
      restoreSketches();

      // Create a new empty window (will be replaced with any files to be opened)
      if (editors.isEmpty()) {
        handleNew();
      }

      // Check for updates
      if (PreferencesData.getBoolean("update.check")) {
        new UpdateCheck(this);
      }

      new Thread(new BuiltInCoreIsNewerCheck(this)).start();

    } else if (parser.isNoOpMode()) {
      // Do nothing (intended for only changing preferences)
      System.exit(0);
    } else if (parser.isGetPrefMode()) {
      BaseNoGui.dumpPrefs(parser);
    }
  }

  private void installKeyboardInputMap() {
    UIManager.put("RSyntaxTextAreaUI.inputMap", new SketchTextAreaDefaultInputMap());
  }

  /**
   * Post-constructor setup for the editor area. Loads the last
   * sketch that was used (if any), and restores other Editor settings.
   * The complement to "storePreferences", this is called when the
   * application is first launched.
   *
   * @throws Exception
   */
  protected boolean restoreSketches() throws Exception {
    // figure out window placement

    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    boolean windowPositionValid = true;

    if (PreferencesData.get("last.screen.height") != null) {
      // if screen size has changed, the window coordinates no longer
      // make sense, so don't use them unless they're identical
      int screenW = PreferencesData.getInteger("last.screen.width");
      int screenH = PreferencesData.getInteger("last.screen.height");

      if ((screen.width != screenW) || (screen.height != screenH)) {
        windowPositionValid = false;
      }
      /*
      int windowX = Preferences.getInteger("last.window.x");
      int windowY = Preferences.getInteger("last.window.y");
      if ((windowX < 0) || (windowY < 0) ||
          (windowX > screenW) || (windowY > screenH)) {
        windowPositionValid = false;
      }
      */
    } else {
      windowPositionValid = false;
    }

    // Iterate through all sketches that were open last time p5 was running.
    // If !windowPositionValid, then ignore the coordinates found for each.

    // Save the sketch path and window placement for each open sketch
    int count = PreferencesData.getInteger("last.sketch.count");
    int opened = 0;
    for (int i = count - 1; i >= 0; i--) {
      String path = PreferencesData.get("last.sketch" + i + ".path");
      if (path == null) {
        continue;
      }
      if (BaseNoGui.getPortableFolder() != null) {
        File absolute = new File(BaseNoGui.getPortableFolder(), path);
        try {
          path = absolute.getCanonicalPath();
        } catch (IOException e) {
          // path unchanged.
        }
      }
      int[] location;
      if (windowPositionValid) {
        String locationStr = PreferencesData.get("last.sketch" + i + ".location");
        location = PApplet.parseInt(PApplet.split(locationStr, ','));
      } else {
        location = nextEditorLocation();
      }
      // If file did not exist, null will be returned for the Editor
      if (handleOpen(new File(path), location, true, false, false) != null) {
        opened++;
      }
    }
    return (opened > 0);
  }


  /**
   * Store list of sketches that are currently open.
   * Called when the application is quitting and documents are still open.
   */
  protected void storeSketches() {
    // Save the width and height of the screen
    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    PreferencesData.setInteger("last.screen.width", screen.width);
    PreferencesData.setInteger("last.screen.height", screen.height);

    String untitledPath = untitledFolder.getAbsolutePath();

    // Save the sketch path and window placement for each open sketch
    LinkedList<Editor> reverseEditors = new LinkedList<Editor>(editors);
    Collections.reverse(reverseEditors);
    int index = 0;
    for (Editor editor : reverseEditors) {
      String path = editor.getSketch().getMainFilePath();
      // In case of a crash, save untitled sketches if they contain changes.
      // (Added this for release 0158, may not be a good idea.)
      if (path.startsWith(untitledPath) && !editor.getSketch().isModified()) {
        continue;
      }
      if (BaseNoGui.getPortableFolder() != null) {
        path = FileUtils.relativePath(BaseNoGui.getPortableFolder().toString(), path);
        if (path == null) {
          continue;
        }
      }
      PreferencesData.set("last.sketch" + index + ".path", path);

      int[] location = editor.getPlacement();
      String locationStr = PApplet.join(PApplet.str(location), ",");
      PreferencesData.set("last.sketch" + index + ".location", locationStr);
      index++;
    }
    PreferencesData.setInteger("last.sketch.count", index);
  }

  protected void storeRecentSketches(Sketch sketch) {
    if (sketch.isUntitled()) {
      return;
    }

    Set<String> sketches = new LinkedHashSet<String>();
    sketches.add(sketch.getMainFilePath());
    sketches.addAll(PreferencesData.getCollection("recent.sketches"));

    PreferencesData.setCollection("recent.sketches", sketches);
  }

  protected void removeRecentSketchPath(String path) {
    Collection<String> sketches = new LinkedList<String>(PreferencesData.getCollection("recent.sketches"));
    sketches.remove(path);
    PreferencesData.setCollection("recent.sketches", sketches);
  }

  // Because of variations in native windowing systems, no guarantees about
  // changes to the focused and active Windows can be made. Developers must
  // never assume that this Window is the focused or active Window until this
  // Window receives a WINDOW_GAINED_FOCUS or WINDOW_ACTIVATED event.
  protected void handleActivated(Editor whichEditor) {
    activeEditor = whichEditor;
    activeEditor.rebuildRecentSketchesMenu();
    if (PreferencesData.getBoolean("editor.external")) {
      try {
        activeEditor.getSketch().load(true);
      } catch (IOException e) {
        // noop
      }
    }

    // set the current window to be the console that's getting output
    EditorConsoleStream.setCurrent(activeEditor.console);
  }


  protected int[] nextEditorLocation() {
    int defaultWidth = PreferencesData.getInteger("editor.window.width.default");
    int defaultHeight = PreferencesData.getInteger("editor.window.height.default");

    if (activeEditor == null) {
      Rectangle screen = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice().getDefaultConfiguration().getBounds();
      // If no current active editor, use default placement
      return new int[]{
              (screen.width - defaultWidth) / 2,
              (screen.height - defaultHeight) / 2,
              defaultWidth, defaultHeight, 0
      };

    } else {
      Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();

      // With a currently active editor, open the new window
      // using the same dimensions, but offset slightly.
      synchronized (editors) {
        final int OVER = 50;
        // In release 0160, don't
        //location = activeEditor.getPlacement();
        Editor lastOpened = activeEditor;
        int[] location = lastOpened.getPlacement();
        // Just in case the bounds for that window are bad
        location[0] += OVER;
        location[1] += OVER;

        if (location[0] == OVER ||
                location[2] == OVER ||
                location[0] + location[2] > screen.width ||
                location[1] + location[3] > screen.height) {
          // Warp the next window to a randomish location on screen.
          return new int[]{
                  (int) (Math.random() * (screen.width - defaultWidth)),
                  (int) (Math.random() * (screen.height - defaultHeight)),
                  defaultWidth, defaultHeight, 0
          };
        }

        return location;
      }
    }
  }


  // .................................................................


  boolean breakTime = false;
  String[] months = {
          "jan", "feb", "mar", "apr", "may", "jun",
          "jul", "aug", "sep", "oct", "nov", "dec"
  };

  /**
   * Handle creating a sketch folder, return its base .pde file
   * or null if the operation was canceled.
   *
   * @param shift    whether shift is pressed, which will invert prompt setting
   * @param noPrompt disable prompt, no matter the setting
   */
  protected File createNewUntitled() throws IOException {
    File newbieDir = null;
    String newbieName = null;

    // In 0126, untitled sketches will begin in the temp folder,
    // and then moved to a new location because Save will default to Save As.
    File sketchbookDir = BaseNoGui.getSketchbookFolder();
    File newbieParentDir = untitledFolder;

    // Use a generic name like sketch_031008a, the date plus a char
    int index = 0;
    //SimpleDateFormat formatter = new SimpleDateFormat("yyMMdd");
    //SimpleDateFormat formatter = new SimpleDateFormat("MMMdd");
    //String purty = formatter.format(new Date()).toLowerCase();
    Calendar cal = Calendar.getInstance();
    int day = cal.get(Calendar.DAY_OF_MONTH);  // 1..31
    int month = cal.get(Calendar.MONTH);  // 0..11
    String purty = months[month] + PApplet.nf(day, 2);
    do {
      if (index == 26) {
        // In 0159, avoid running past z by sending people outdoors.
        if (!breakTime) {
          showWarning(_("Time for a Break"),
                  _("You've reached the limit for auto naming of new sketches\n" +
                          "for the day. How about going for a walk instead?"), null);
          breakTime = true;
        } else {
          showWarning(_("Sunshine"),
                  _("No really, time for some fresh air for you."), null);
        }
        return null;
      }
      newbieName = "sketch_" + purty + ((char) ('a' + index));
      newbieDir = new File(newbieParentDir, newbieName);
      index++;
      // Make sure it's not in the temp folder *and* it's not in the sketchbook
    } while (newbieDir.exists() || new File(sketchbookDir, newbieName).exists());

    // Make the directory for the new sketch
    newbieDir.mkdirs();

    // Make an empty pde file
    File newbieFile = new File(newbieDir, newbieName + ".ino");
    if (!newbieFile.createNewFile()) {
      throw new IOException();
    }
    FileUtils.copyFile(new File(getContentFile("examples"), "01.Basics" + File.separator + "BareMinimum" + File.separator + "BareMinimum.ino"), newbieFile);
    return newbieFile;
  }


  /**
   * Create a new untitled document in a new sketch window.
   *
   * @throws Exception
   */
  public void handleNew() throws Exception {
    try {
      File file = createNewUntitled();
      if (file != null) {
        Editor editor = handleOpen(file, true);
      }

    } catch (IOException e) {
      if (activeEditor != null) {
        activeEditor.statusError(e);
      }
    }
  }


  /**
   * Replace the sketch in the current window with a new untitled document.
   */
  public void handleNewReplace() {
    if (!activeEditor.checkModified()) {
      return;  // sketch was modified, and user canceled
    }
    // Close the running window, avoid window boogers with multiple sketches
    activeEditor.internalCloseRunner();

    // Actually replace things
    handleNewReplaceImpl();
  }


  protected void handleNewReplaceImpl() {
    try {
      File file = createNewUntitled();
      if (file != null) {
        activeEditor.handleOpenInternal(file);
        activeEditor.untitled = true;
      }
//      return true;

    } catch (IOException e) {
      activeEditor.statusError(e);
//      return false;
    }
  }


  /**
   * Open a sketch, replacing the sketch in the current window.
   *
   * @param path Location of the primary pde file for the sketch.
   */
  public void handleOpenReplace(File file) {
    if (!activeEditor.checkModified()) {
      return;  // sketch was modified, and user canceled
    }
    // Close the running window, avoid window boogers with multiple sketches
    activeEditor.internalCloseRunner();

    boolean loaded = activeEditor.handleOpenInternal(file);
    if (!loaded) {
      // replace the document without checking if that's ok
      handleNewReplaceImpl();
    }
  }


  /**
   * Prompt for a sketch to open, and open it in a new window.
   *
   * @throws Exception
   */
  public void handleOpenPrompt() throws Exception {
    // get the frontmost window frame for placing file dialog
    FileDialog fd = new FileDialog(activeEditor, _("Open an Arduino sketch..."), FileDialog.LOAD);
    File lastFolder = new File(PreferencesData.get("last.folder", BaseNoGui.getSketchbookFolder().getAbsolutePath()));
    if (lastFolder.exists() && lastFolder.isFile()) {
      lastFolder = lastFolder.getParentFile();
    }
    fd.setDirectory(lastFolder.getAbsolutePath());

    // Only show .pde files as eligible bachelors
    fd.setFilenameFilter(new FilenameFilter() {
      public boolean accept(File dir, String name) {
        return name.toLowerCase().endsWith(".ino")
                || name.toLowerCase().endsWith(".pde");
      }
    });

    fd.setVisible(true);

    String directory = fd.getDirectory();
    String filename = fd.getFile();

    // User canceled selection
    if (filename == null) return;

    File inputFile = new File(directory, filename);

    PreferencesData.set("last.folder", inputFile.getAbsolutePath());
    handleOpen(inputFile);
  }


  /**
   * Open a sketch in a new window.
   *
   * @param file File to open
   * @return the Editor object, so that properties (like 'untitled')
   * can be set by the caller
   * @throws Exception
   */
  public Editor handleOpen(File file) throws Exception {
    return handleOpen(file, false);
  }

  public Editor handleOpen(File file, boolean untitled) throws Exception {
    return handleOpen(file, nextEditorLocation(), true, untitled);
  }

  protected Editor handleOpen(File file, int[] location, boolean showEditor, boolean untitled) throws Exception {
    return handleOpen(file, location, showEditor, true, untitled);
  }

  protected Editor handleOpen(File file, int[] location, boolean showEditor, boolean storeOpenedSketches, boolean untitled) throws Exception {
    if (!file.exists()) return null;

    // Cycle through open windows to make sure that it's not already open.
    String path = file.getAbsolutePath();
    for (Editor editor : editors) {
      if (editor.getSketch().getMainFilePath().equals(path)) {
        editor.toFront();
        return editor;
      }
    }

    final Editor editor = new Editor(this, file, location, BaseNoGui.getPlatform());

    // Make sure that the sketch actually loaded
    if (editor.getSketch() == null) {
      return null;  // Just walk away quietly
    }

    editor.untitled = untitled;

    editors.add(editor);

    if (storeOpenedSketches) {
      // Store information on who's open and running
      // (in case there's a crash or something that can't be recovered)
      storeSketches();
      storeRecentSketches(editor.getSketch());
      rebuildRecentSketchesMenuItems();
      PreferencesData.save();
    }

    // now that we're ready, show the window
    // (don't do earlier, cuz we might move it based on a window being closed)
    if (showEditor) {
      SwingUtilities.invokeLater(new Runnable() {
        @Override
        public void run() {
          editor.setVisible(true);
        }
      });
    }

    return editor;
  }

  protected void rebuildRecentSketchesMenuItems() {
    Set<File> recentSketches = new LinkedHashSet<File>() {

      @Override
      public boolean add(File file) {
        if (size() >= RECENT_SKETCHES_MAX_SIZE) {
          return false;
        }
        return super.add(file);
      }
    };

    for (String path : PreferencesData.getCollection("recent.sketches")) {
      File file = new File(path);
      if (file.exists()) {
        recentSketches.add(file);
      }
    }

    recentSketchesMenuItems.clear();
    for (final File recentSketch : recentSketches) {
      JMenuItem recentSketchMenuItem = new JMenuItem(recentSketch.getParentFile().getName());
      recentSketchMenuItem.addActionListener(new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent actionEvent) {
          try {
            handleOpen(recentSketch);
          } catch (Exception e) {
            e.printStackTrace();
          }
        }
      });
      recentSketchesMenuItems.add(recentSketchMenuItem);
    }
  }


  /**
   * Close a sketch as specified by its editor window.
   *
   * @param editor Editor object of the sketch to be closed.
   * @return true if succeeded in closing, false if canceled.
   */
  public boolean handleClose(Editor editor) {
    // Check if modified
//    boolean immediate = editors.size() == 1;
    if (!editor.checkModified()) {
      return false;
    }

    // Close the running window, avoid window boogers with multiple sketches
    editor.internalCloseRunner();

    if (editors.size() == 1) {
      // This will store the sketch count as zero
      editors.remove(editor);
      try {
        Editor.serialMonitor.close();
      } catch (Exception e) {
        //ignore
      }
      storeSketches();
      rebuildRecentSketchesMenuItems();

      // Save out the current prefs state
      PreferencesData.save();

      // Since this wasn't an actual Quit event, call System.exit()
      System.exit(0);

    } else {
      // More than one editor window open,
      // proceed with closing the current window.
      editor.setVisible(false);
      editor.dispose();
//      for (int i = 0; i < editorCount; i++) {
//        if (editor == editors[i]) {
//          for (int j = i; j < editorCount-1; j++) {
//            editors[j] = editors[j+1];
//          }
//          editorCount--;
//          // Set to null so that garbage collection occurs
//          editors[editorCount] = null;
//        }
//      }
      editors.remove(editor);
    }
    return true;
  }


  /**
   * Handler for File &rarr; Quit.
   *
   * @return false if canceled, true otherwise.
   */
  public boolean handleQuit() {
    // If quit is canceled, this will be replaced anyway
    // by a later handleQuit() that is not canceled.
    storeSketches();
    try {
      Editor.serialMonitor.close();
    } catch (Exception e) {
      // ignore
    }

    if (handleQuitEach()) {
      // make sure running sketches close before quitting
      for (Editor editor : editors) {
        editor.internalCloseRunner();
      }
      // Save out the current prefs state
      PreferencesData.save();

      if (!OSUtils.isMacOS()) {
        // If this was fired from the menu or an AppleEvent (the Finder),
        // then Mac OS X will send the terminate signal itself.
        System.exit(0);
      }
      return true;
    }
    return false;
  }


  /**
   * Attempt to close each open sketch in preparation for quitting.
   *
   * @return false if canceled along the way
   */
  protected boolean handleQuitEach() {
    for (Editor editor : editors) {
      if (!editor.checkModified()) {
        return false;
      }
    }
    return true;
  }


  // .................................................................


  /**
   * Asynchronous version of menu rebuild to be used on save and rename
   * to prevent the interface from locking up until the menus are done.
   */
  public void rebuildSketchbookMenus() {
    //System.out.println("async enter");
    //new Exception().printStackTrace();
    SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        //System.out.println("starting rebuild");
        rebuildSketchbookMenu(Editor.sketchbookMenu);
        rebuildToolbarMenu(Editor.toolbarMenu);
        //System.out.println("done with rebuild");
      }
    });
    //System.out.println("async exit");
  }


  protected void rebuildToolbarMenu(JMenu menu) {
    JMenuItem item;
    menu.removeAll();

    // Add the single "Open" item
    item = Editor.newJMenuItem(_("Open..."), 'O');
    item.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        try {
          handleOpenPrompt();
        } catch (Exception e1) {
          e1.printStackTrace();
        }
      }
    });
    menu.add(item);
    menu.addSeparator();

    // Add a list of all sketches and subfolders
    try {
      boolean sketches = addSketches(menu, BaseNoGui.getSketchbookFolder(), true);
      if (sketches) menu.addSeparator();
    } catch (IOException e) {
      e.printStackTrace();
    }

    // Add each of the subfolders of examples directly to the menu
    try {
      boolean found = addSketches(menu, BaseNoGui.getExamplesFolder(), true);
      if (found) menu.addSeparator();
    } catch (IOException e) {
      e.printStackTrace();
    }
  }


  protected void rebuildSketchbookMenu(JMenu menu) {
    //System.out.println("rebuilding sketchbook menu");
    //new Exception().printStackTrace();
    try {
      menu.removeAll();
      addSketches(menu, BaseNoGui.getSketchbookFolder(), false);
      //addSketches(menu, getSketchbookFolder());
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  public LibraryList getIDELibs() {
    LibraryList installedLibraries = new LibraryList(BaseNoGui.librariesIndexer.getInstalledLibraries());
    List<UserLibrary> libs = new LinkedList<UserLibrary>(Collections2.filter(new LinkedList<UserLibrary>(installedLibraries), Predicates.not(CONTRIBUTED)));
    return new LibraryList(libs);
  }

  public LibraryList getUserLibs() {
    LibraryList installedLibraries = new LibraryList(BaseNoGui.librariesIndexer.getInstalledLibraries());
    List<UserLibrary> libs = new LinkedList<UserLibrary>(Collections2.filter(new LinkedList<UserLibrary>(installedLibraries), CONTRIBUTED));
    return new LibraryList(libs);
  }

  private List<ContributedLibrary> getSortedLibraries() {
    List<ContributedLibrary> installedLibraries = new LinkedList<ContributedLibrary>(BaseNoGui.librariesIndexer.getInstalledLibraries());
    Collections.sort(installedLibraries, new LibraryByTypeComparator());
    Collections.sort(installedLibraries, new LibraryOfSameTypeComparator());
    return installedLibraries;
  }

  public void rebuildImportMenu(JMenu importMenu) {
    if (importMenu == null)
      return;
    importMenu.removeAll();

    JMenuItem menu = new JMenuItem(_("Manage Libraries..."));
    menu.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        openManageLibrariesDialog();
      }
    });
    importMenu.add(menu);
    importMenu.addSeparator();

    JMenuItem addLibraryMenuItem = new JMenuItem(_("Add .ZIP Library..."));
    addLibraryMenuItem.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Base.this.handleAddLibrary();
        Base.this.onBoardOrPortChange();
        Base.this.rebuildImportMenu(Editor.importMenu);
        Base.this.rebuildExamplesMenu(Editor.examplesMenu);
      }
    });
    importMenu.add(addLibraryMenuItem);
    importMenu.addSeparator();

    // Split between user supplied libraries and IDE libraries
    TargetPlatform targetPlatform = BaseNoGui.getTargetPlatform();

    if (targetPlatform != null) {
      List<ContributedLibrary> libs = getSortedLibraries();
      String lastLibType = null;
      for (ContributedLibrary lib : libs) {
        if (lastLibType == null || !lastLibType.equals(lib.getTypes().get(0))) {
          if (lastLibType != null) {
            importMenu.addSeparator();
          }
          lastLibType = lib.getTypes().get(0);
          JMenuItem platformItem = new JMenuItem(I18n.format(_("{0} libraries"), lastLibType));
          platformItem.setEnabled(false);
          importMenu.add(platformItem);
        }

        AbstractAction action = new AbstractAction(lib.getName()) {
          public void actionPerformed(ActionEvent event) {
            UserLibrary l = (UserLibrary) getValue("library");
            try {
              activeEditor.getSketch().importLibrary(l);
            } catch (IOException e) {
              showWarning(_("Error"), I18n.format("Unable to list header files in {0}", l.getSrcFolder()), e);
            }
          }
        };
        action.putValue("library", lib);

        // Add new element at the bottom
        JMenuItem item = new JMenuItem(action);
        item.putClientProperty("library", lib);
        importMenu.add(item);
      }
    }
  }

  public void rebuildExamplesMenu(JMenu menu) {
    if (menu == null)
      return;
    try {
      menu.removeAll();

      // Add examples from distribution "example" folder
      boolean found = addSketches(menu, BaseNoGui.getExamplesFolder(), false);
      if (found) menu.addSeparator();

      // Add examples from libraries
      LibraryList ideLibs = getIDELibs();
      ideLibs.sort();
      for (UserLibrary lib : ideLibs)
        addSketchesSubmenu(menu, lib, false);

      LibraryList userLibs = getUserLibs();
      if (userLibs.size() > 0) {
        menu.addSeparator();
        userLibs.sort();
        for (UserLibrary lib : userLibs)
          addSketchesSubmenu(menu, lib, false);
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  public void onBoardOrPortChange() {
    BaseNoGui.onBoardOrPortChange();

    // Update editors status bar
    for (Editor editor : editors) {
      editor.onBoardOrPortChange();
    }
  }

  private void openManageLibrariesDialog() {
    @SuppressWarnings("serial")
    LibraryManagerUI managerUI = new LibraryManagerUI(activeEditor, BaseNoGui.getPlatform()) {
      @Override
      protected void onIndexesUpdated() throws Exception {
        BaseNoGui.initPackages();
        rebuildBoardsMenu();
        onBoardOrPortChange();
        setIndexer(BaseNoGui.librariesIndexer);
      }
    };
    managerUI.setLocationRelativeTo(activeEditor);
    managerUI.setIndexer(BaseNoGui.librariesIndexer);
    managerUI.setVisible(true);
    // Manager dialog is modal, waits here until closed

    //handleAddLibrary();
    onBoardOrPortChange();
    rebuildImportMenu(Editor.importMenu);
    rebuildExamplesMenu(Editor.examplesMenu);
  }

  private void openInstallBoardDialog(final String filterText) throws Exception {
    // Create dialog for contribution manager
    @SuppressWarnings("serial")
    ContributionManagerUI managerUI = new ContributionManagerUI(activeEditor, BaseNoGui.getPlatform()) {
      @Override
      protected void onIndexesUpdated() throws Exception {
        BaseNoGui.initPackages();
        rebuildBoardsMenu();
        setIndexer(BaseNoGui.indexer);
        if (StringUtils.isNotEmpty(filterText)) {
          setFilterText(filterText);
        }

      }
    };
    managerUI.setLocationRelativeTo(activeEditor);
    managerUI.setIndexer(BaseNoGui.indexer);
    managerUI.setVisible(true);
    // Installer dialog is modal, waits here until closed

    // Reload all boards (that may have been installed/updated/removed)
    BaseNoGui.initPackages();
    rebuildBoardsMenu();
    onBoardOrPortChange();
  }

  public void rebuildBoardsMenu() throws Exception {
    boardsCustomMenus = new LinkedList<JMenu>();

    // The first custom menu is the "Board" selection submenu
    JMenu boardMenu = new JMenu(_("Board"));
    boardMenu.putClientProperty("removeOnWindowDeactivation", true);
    MenuScroller.setScrollerFor(boardMenu);

    openBoardsManager = new AbstractAction(_("Boards Manager...")) {
      public void actionPerformed(ActionEvent actionevent) {
        String filterText = "";
        if (actionevent instanceof cc.arduino.view.Event) {
          filterText = ((Event) actionevent).getPayload().get("filterText").toString();
        }
        try {
          openInstallBoardDialog(filterText);
        } catch (Exception e) {
          //TODO show error
          e.printStackTrace();
        }
      }
    };
    boardMenu.add(new JMenuItem(openBoardsManager));
    boardsCustomMenus.add(boardMenu);

    // If there are no platforms installed we are done
    if (BaseNoGui.packages.size() == 0)
      return;

    // Separate "Install boards..." command from installed boards
    boardMenu.add(new JSeparator());

    // Generate custom menus for all platforms
    Set<String> customMenusTitles = new HashSet<String>();
    for (TargetPackage targetPackage : BaseNoGui.packages.values()) {
      for (TargetPlatform targetPlatform : targetPackage.platforms()) {
        customMenusTitles.addAll(targetPlatform.getCustomMenus().values());
      }
    }
    for (String customMenuTitle : customMenusTitles) {
      JMenu customMenu = new JMenu(_(customMenuTitle));
      customMenu.putClientProperty("removeOnWindowDeactivation", true);
      boardsCustomMenus.add(customMenu);
    }

    List<JMenuItem> menuItemsToClickAfterStartup = new LinkedList<JMenuItem>();

    ButtonGroup boardsButtonGroup = new ButtonGroup();
    Map<String, ButtonGroup> buttonGroupsMap = new HashMap<String, ButtonGroup>();

    // Cycle through all packages
    boolean first = true;
    for (TargetPackage targetPackage : BaseNoGui.packages.values()) {
      // For every package cycle through all platform
      for (TargetPlatform targetPlatform : targetPackage.platforms()) {

        // Add a separator from the previous platform
        if (!first)
          boardMenu.add(new JSeparator());
        first = false;

        // Add a title for each platform
        String platformLabel = targetPlatform.getPreferences().get("name");
        if (platformLabel != null && !targetPlatform.getBoards().isEmpty()) {
          JMenuItem menuLabel = new JMenuItem(_(platformLabel));
          menuLabel.setEnabled(false);
          boardMenu.add(menuLabel);
        }

        // Cycle through all boards of this platform
        for (TargetBoard board : targetPlatform.getBoards().values()) {
          JMenuItem item = createBoardMenusAndCustomMenus(boardsCustomMenus, menuItemsToClickAfterStartup,
                  buttonGroupsMap,
                  board, targetPlatform, targetPackage);
          boardMenu.add(item);
          boardsButtonGroup.add(item);
        }
      }
    }

    if (menuItemsToClickAfterStartup.isEmpty()) {
      menuItemsToClickAfterStartup.add(selectFirstEnabledMenuItem(boardMenu));
    }

    for (JMenuItem menuItemToClick : menuItemsToClickAfterStartup) {
      menuItemToClick.setSelected(true);
      menuItemToClick.getAction().actionPerformed(new ActionEvent(this, -1, ""));
    }
  }

  private JRadioButtonMenuItem createBoardMenusAndCustomMenus(
          final List<JMenu> boardsCustomMenus, List<JMenuItem> menuItemsToClickAfterStartup,
          Map<String, ButtonGroup> buttonGroupsMap,
          TargetBoard board, TargetPlatform targetPlatform, TargetPackage targetPackage)
          throws Exception {
    String selPackage = PreferencesData.get("target_package");
    String selPlatform = PreferencesData.get("target_platform");
    String selBoard = PreferencesData.get("board");

    String boardId = board.getId();
    String packageName = targetPackage.getId();
    String platformName = targetPlatform.getId();

    // Setup a menu item for the current board
    @SuppressWarnings("serial")
    Action action = new AbstractAction(board.getName()) {
      public void actionPerformed(ActionEvent actionevent) {
        BaseNoGui.selectBoard((TargetBoard) getValue("b"));
        filterVisibilityOfSubsequentBoardMenus(boardsCustomMenus, (TargetBoard) getValue("b"), 1);

        onBoardOrPortChange();
        rebuildImportMenu(Editor.importMenu);
        rebuildExamplesMenu(Editor.examplesMenu);
      }
    };
    action.putValue("b", board);

    JRadioButtonMenuItem item = new JRadioButtonMenuItem(action);

    if (selBoard.equals(boardId) && selPackage.equals(packageName)
            && selPlatform.equals(platformName)) {
      menuItemsToClickAfterStartup.add(item);
    }

    PreferencesMap customMenus = targetPlatform.getCustomMenus();
    for (final String menuId : customMenus.keySet()) {
      String title = customMenus.get(menuId);
      JMenu menu = getBoardCustomMenu(_(title));

      if (board.hasMenu(menuId)) {
        PreferencesMap boardCustomMenu = board.getMenuLabels(menuId);
        for (String customMenuOption : boardCustomMenu.keySet()) {
          @SuppressWarnings("serial")
          Action subAction = new AbstractAction(_(boardCustomMenu.get(customMenuOption))) {
            public void actionPerformed(ActionEvent e) {
              PreferencesData.set("custom_" + menuId, ((TargetBoard) getValue("board")).getId() + "_" + getValue("custom_menu_option"));
              onBoardOrPortChange();
            }
          };
          subAction.putValue("board", board);
          subAction.putValue("custom_menu_option", customMenuOption);

          if (!buttonGroupsMap.containsKey(menuId)) {
            buttonGroupsMap.put(menuId, new ButtonGroup());
          }

          JRadioButtonMenuItem subItem = new JRadioButtonMenuItem(subAction);
          menu.add(subItem);
          buttonGroupsMap.get(menuId).add(subItem);

          String selectedCustomMenuEntry = PreferencesData.get("custom_" + menuId);
          if (selBoard.equals(boardId) && (boardId + "_" + customMenuOption).equals(selectedCustomMenuEntry)) {
            menuItemsToClickAfterStartup.add(subItem);
          }
        }
      }
    }

    return item;
  }

  private void filterVisibilityOfSubsequentBoardMenus(List<JMenu> boardsCustomMenus, TargetBoard board,
                                                      int fromIndex) {
    for (int i = fromIndex; i < boardsCustomMenus.size(); i++) {
      JMenu menu = boardsCustomMenus.get(i);
      for (int m = 0; m < menu.getItemCount(); m++) {
        JMenuItem menuItem = menu.getItem(m);
        menuItem.setVisible(menuItem.getAction().getValue("board").equals(board));
      }
      menu.setVisible(ifThereAreVisibleItemsOn(menu));

      if (menu.isVisible()) {
        JMenuItem visibleSelectedOrFirstMenuItem = selectVisibleSelectedOrFirstMenuItem(menu);
        if (!visibleSelectedOrFirstMenuItem.isSelected()) {
          visibleSelectedOrFirstMenuItem.setSelected(true);
          visibleSelectedOrFirstMenuItem.getAction().actionPerformed(null);
        }
      }
    }
  }

  private static boolean ifThereAreVisibleItemsOn(JMenu menu) {
    for (int i = 0; i < menu.getItemCount(); i++) {
      if (menu.getItem(i).isVisible()) {
        return true;
      }
    }
    return false;
  }

  private JMenu getBoardCustomMenu(String label) throws Exception {
    for (JMenu menu : boardsCustomMenus) {
      if (label.equals(menu.getText())) {
        return menu;
      }
    }
    throw new Exception("Custom menu not found!");
  }

  private static JMenuItem selectVisibleSelectedOrFirstMenuItem(JMenu menu) {
    JMenuItem firstVisible = null;
    for (int i = 0; i < menu.getItemCount(); i++) {
      JMenuItem item = menu.getItem(i);
      if (item != null && item.isVisible()) {
        if (item.isSelected()) {
          return item;
        }
        if (firstVisible == null) {
          firstVisible = item;
        }
      }
    }

    if (firstVisible != null) {
      return firstVisible;
    }

    throw new IllegalStateException("Menu has no enabled items");
  }

  private static JMenuItem selectFirstEnabledMenuItem(JMenu menu) {
    for (int i = 1; i < menu.getItemCount(); i++) {
      JMenuItem item = menu.getItem(i);
      if (item != null && item.isEnabled()) {
        return item;
      }
    }
    throw new IllegalStateException("Menu has no enabled items");
  }

  public void rebuildProgrammerMenu(JMenu menu) {
    menu.removeAll();
    ButtonGroup group = new ButtonGroup();
    for (TargetPackage targetPackage : BaseNoGui.packages.values()) {
      for (TargetPlatform targetPlatform : targetPackage.platforms()) {
        for (String programmer : targetPlatform.getProgrammers().keySet()) {
          String id = targetPackage.getId() + ":" + programmer;

          @SuppressWarnings("serial")
          AbstractAction action = new AbstractAction(targetPlatform
                  .getProgrammer(programmer).get("name")) {
            public void actionPerformed(ActionEvent actionevent) {
              PreferencesData.set("programmer", "" + getValue("id"));
            }
          };
          action.putValue("id", id);
          JMenuItem item = new JRadioButtonMenuItem(action);
          if (PreferencesData.get("programmer").equals(id))
            item.setSelected(true);
          group.add(item);
          menu.add(item);
        }
      }
    }
  }

  /**
   * Scan a folder recursively, and add any sketches found to the menu
   * specified. Set the openReplaces parameter to true when opening the sketch
   * should replace the sketch in the current window, or false when the
   * sketch should open in a new window.
   */
  protected boolean addSketches(JMenu menu, File folder, final boolean replaceExisting) throws IOException {
    if (folder == null)
      return false;

    if (!folder.isDirectory()) return false;

    File[] files = folder.listFiles();
    // If a bad folder or unreadable or whatever, this will come back null
    if (files == null) return false;

    // Alphabetize files, since it's not always alpha order
    Arrays.sort(files, new Comparator<File>() {
      @Override
      public int compare(File file, File file2) {
        return file.getName().compareToIgnoreCase(file2.getName());
      }
    });

    boolean ifound = false;

    for (File subfolder : files) {
      if (FileUtils.isSCCSOrHiddenFile(subfolder)) {
        continue;
      }

      if (!subfolder.isDirectory()) continue;

      if (addSketchesSubmenu(menu, subfolder.getName(), subfolder, replaceExisting)) {
        ifound = true;
      }
    }

    return ifound;
  }

  private boolean addSketchesSubmenu(JMenu menu, UserLibrary lib,
                                     boolean replaceExisting)
          throws IOException {
    return addSketchesSubmenu(menu, lib.getName(), lib.getInstalledFolder(),
            replaceExisting);
  }

  private boolean addSketchesSubmenu(JMenu menu, String name, File folder,
                                     final boolean replaceExisting) throws IOException {

    ActionListener listener = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        String path = e.getActionCommand();
        File file = new File(path);
        if (file.exists()) {
          boolean replace = replaceExisting;
          if ((e.getModifiers() & ActionEvent.SHIFT_MASK) != 0) {
            replace = !replace;
          }
          if (replace) {
            handleOpenReplace(file);
          } else {
            try {
              handleOpen(file);
            } catch (Exception e1) {
              e1.printStackTrace();
            }
          }
        } else {
          showWarning(_("Sketch Does Not Exist"),
                  _("The selected sketch no longer exists.\n"
                          + "You may need to restart Arduino to update\n"
                          + "the sketchbook menu."), null);
        }
      }
    };

    File entry = new File(folder, name + ".ino");
    if (!entry.exists() && (new File(folder, name + ".pde")).exists())
      entry = new File(folder, name + ".pde");

    // if a .pde file of the same prefix as the folder exists..
    if (entry.exists()) {

      if (!BaseNoGui.isSanitaryName(name)) {
        if (!builtOnce) {
          String complaining = I18n
                  .format(
                          _("The sketch \"{0}\" cannot be used.\n"
                                  + "Sketch names must contain only basic letters and numbers\n"
                                  + "(ASCII-only with no spaces, "
                                  + "and it cannot start with a number).\n"
                                  + "To get rid of this message, remove the sketch from\n"
                                  + "{1}"), name, entry.getAbsolutePath());
          showMessage(_("Ignoring sketch with bad name"), complaining);
        }
        return false;
      }

      JMenuItem item = new JMenuItem(name);
      item.addActionListener(listener);
      item.setActionCommand(entry.getAbsolutePath());
      menu.add(item);
      return true;
    }

    // don't create an extra menu level for a folder named "examples"
    if (folder.getName().equals("examples"))
      return addSketches(menu, folder, replaceExisting);

    // not a sketch folder, but maybe a subfolder containing sketches
    JMenu submenu = new JMenu(name);
    boolean found = addSketches(submenu, folder, replaceExisting);
    if (found) {
      menu.add(submenu);
      MenuScroller.setScrollerFor(submenu);
    }
    return found;
  }

  protected void addLibraries(JMenu menu, LibraryList libs) throws IOException {

    LibraryList list = new LibraryList(libs);
    list.sort();

    for (UserLibrary lib : list) {
      @SuppressWarnings("serial")
      AbstractAction action = new AbstractAction(lib.getName()) {
        public void actionPerformed(ActionEvent event) {
          UserLibrary l = (UserLibrary) getValue("library");
          try {
            activeEditor.getSketch().importLibrary(l);
          } catch (IOException e) {
            showWarning(_("Error"), I18n.format("Unable to list header files in {0}", l.getSrcFolder()), e);
          }
        }
      };
      action.putValue("library", lib);

      // Add new element at the bottom
      JMenuItem item = new JMenuItem(action);
      item.putClientProperty("library", lib);
      menu.add(item);

      // XXX: DAM: should recurse here so that library folders can be nested
    }
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

  /**
   * Show the About box.
   */
  @SuppressWarnings("serial")
  public void handleAbout() {
    final Image image = getLibImage("about.png", activeEditor);
    final Window window = new Window(activeEditor) {
      public void paint(Graphics g) {
        g.drawImage(image, 0, 0, null);

        Graphics2D g2 = (Graphics2D) g;
        g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
                RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);

        g.setFont(new Font("SansSerif", Font.PLAIN, 11));
        g.setColor(Color.white);
        g.drawString(BaseNoGui.VERSION_NAME_LONG, 33, 20);
      }
    };
    window.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        window.dispose();
      }
    });
    int w = image.getWidth(activeEditor);
    int h = image.getHeight(activeEditor);
    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    window.setBounds((screen.width - w) / 2, (screen.height - h) / 2, w, h);
    window.setLocationRelativeTo(activeEditor);
    window.setVisible(true);
  }


  /**
   * Show the Preferences window.
   */
  public void handlePrefs() {
    cc.arduino.view.preferences.Preferences dialog = new cc.arduino.view.preferences.Preferences(activeEditor, this);
    if (activeEditor != null) {
      dialog.setLocationRelativeTo(activeEditor);
    }
    dialog.setVisible(true);
  }

  // XXX: Remove this method and make librariesIndexer non-static
  static public LibraryList getLibraries() {
    return BaseNoGui.librariesIndexer.getInstalledLibraries();
  }

  public List<JMenu> getBoardsCustomMenus() {
    return boardsCustomMenus;
  }

  static public String getSketchbookLibrariesPath() {
    return BaseNoGui.getSketchbookLibrariesFolder().getAbsolutePath();
  }

  public File getDefaultSketchbookFolderOrPromptForIt() {

    File sketchbookFolder = BaseNoGui.getDefaultSketchbookFolder();

    if (sketchbookFolder == null) {
      sketchbookFolder = promptSketchbookLocation();
    }

    // create the folder if it doesn't exist already
    boolean result = true;
    if (!sketchbookFolder.exists()) {
      result = sketchbookFolder.mkdirs();
    }

    if (!result) {
      showError(_("You forgot your sketchbook"),
              _("Arduino cannot run because it could not\n" +
                      "create a folder to store your sketchbook."), null);
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

    String prompt = _("Select (or create new) folder for sketches...");
    folder = selectFolder(prompt, null, null);
    if (folder == null) {
      System.exit(0);
    }
    return folder;
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
    try {
      BaseNoGui.getPlatform().openURL(url);

    } catch (Exception e) {
      showWarning(_("Problem Opening URL"),
              I18n.format(_("Could not open the URL\n{0}"), url), e);
    }
  }


  /**
   * Used to determine whether to disable the "Show Sketch Folder" option.
   *
   * @return true If a means of opening a folder is known to be available.
   */
  static protected boolean openFolderAvailable() {
    return BaseNoGui.getPlatform().openFolderAvailable();
  }


  /**
   * Implements the other cross-platform headache of opening
   * a folder in the machine's native file browser.
   */
  static public void openFolder(File file) {
    try {
      BaseNoGui.getPlatform().openFolder(file);

    } catch (Exception e) {
      showWarning(_("Problem Opening Folder"),
              I18n.format(_("Could not open the folder\n{0}"), file.getAbsolutePath()), e);
    }
  }


  // .................................................................


  static public File selectFolder(String prompt, File folder, Component parent) {
    JFileChooser fc = new JFileChooser();
    fc.setDialogTitle(prompt);
    if (folder != null) {
      fc.setSelectedFile(folder);
    }
    fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

    int returned = fc.showOpenDialog(parent);
    if (returned == JFileChooser.APPROVE_OPTION) {
      return fc.getSelectedFile();
    }
    return null;
  }


  // .................................................................


  /**
   * Give this Frame a Processing icon.
   */
  static public void setIcon(Frame frame) {
    // don't use the low-res icon on Mac OS X; the window should
    // already have the right icon from the .app file.
    if (OSUtils.isMacOS()) return;
    
    // don't use the low-res icon on Linux
    if (OSUtils.isLinux()){
      String current = System.getProperty("user.dir");
      Image image = Toolkit.getDefaultToolkit().createImage(current + "/lib/arduino.png");
      frame.setIconImage(image);
      return;
    }

    Image image = Toolkit.getDefaultToolkit().createImage(PApplet.ICON_IMAGE);
    frame.setIconImage(image);
  }


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
  static public void registerWindowCloseKeys(JRootPane root,
                                             ActionListener disposer) {
    KeyStroke stroke = KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0);
    root.registerKeyboardAction(disposer, stroke,
            JComponent.WHEN_IN_FOCUSED_WINDOW);

    int modifiers = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
    stroke = KeyStroke.getKeyStroke('W', modifiers);
    root.registerKeyboardAction(disposer, stroke,
            JComponent.WHEN_IN_FOCUSED_WINDOW);
  }


  // .................................................................


  static public void showReference(String filename) {
    showReference("reference/arduino.cc/en", filename);
  }

  static public void showReference(String prefix, String filename) {
    File referenceFolder = getContentFile(prefix);
    File referenceFile = new File(referenceFolder, filename);
    if (!referenceFile.exists())
      referenceFile = new File(referenceFolder, filename + ".html");
    
    if(referenceFile.exists()){
      openURL(referenceFile.getAbsolutePath());
    }else{
      showWarning(_("Problem Opening URL"), I18n.format(_("Could not open the URL\n{0}"), referenceFile), null); 
    }
  }

  public static void showEdisonGettingStarted() {
    showReference("reference/Edison_help_files", "ArduinoIDE_guide_edison");
  }

  static public void showArduinoGettingStarted() {
    if (OSUtils.isMacOS()) {
      showReference("Guide/MacOSX");
    } else if (OSUtils.isWindows()) {
      showReference("Guide/Windows");
    } else {
      openURL("http://www.arduino.cc/playground/Learning/Linux");
    }
  }

  static public void showReference() {
    showReference("Reference/HomePage");
  }


  static public void showEnvironment() {
    showReference("Guide/Environment");
  }


  static public void showTroubleshooting() {
    showReference("Guide/Troubleshooting");
  }


  static public void showFAQ() {
    showReference("Main/FAQ");
  }


  // .................................................................


  /**
   * "No cookie for you" type messages. Nothing fatal or all that
   * much of a bummer, but something to notify the user about.
   */
  static public void showMessage(String title, String message) {
    BaseNoGui.showMessage(title, message);
  }


  /**
   * Non-fatal error message with optional stack trace side dish.
   */
  static public void showWarning(String title, String message, Exception e) {
    BaseNoGui.showWarning(title, message, e);
  }


  static public void showError(String title, String message, Throwable e) {
    showError(title, message, e, 1);
  }

  static public void showError(String title, String message, int exit_code) {
    showError(title, message, null, exit_code);
  }

  /**
   * Show an error message that's actually fatal to the program.
   * This is an error that can't be recovered. Use showWarning()
   * for errors that allow P5 to continue running.
   */
  static public void showError(String title, String message, Throwable e, int exit_code) {
    BaseNoGui.showError(title, message, e, exit_code);
  }


  // ...................................................................


  // incomplete
  static public int showYesNoCancelQuestion(Editor editor, String title,
                                            String primary, String secondary) {
    if (!OSUtils.isMacOS()) {
      int result =
              JOptionPane.showConfirmDialog(null, primary + "\n" + secondary, title,
                      JOptionPane.YES_NO_CANCEL_OPTION,
                      JOptionPane.QUESTION_MESSAGE);
      return result;
//    if (result == JOptionPane.YES_OPTION) {
//
//    } else if (result == JOptionPane.NO_OPTION) {
//      return true;  // ok to continue
//
//    } else if (result == JOptionPane.CANCEL_OPTION) {
//      return false;
//
//    } else {
//      throw new IllegalStateException();
//    }

    } else {
      // Pane formatting adapted from the Quaqua guide
      // http://www.randelshofer.ch/quaqua/guide/joptionpane.html
      JOptionPane pane =
              new JOptionPane("<html> " +
                      "<head> <style type=\"text/css\">" +
                      "b { font: 13pt \"Lucida Grande\" }" +
                      "p { font: 11pt \"Lucida Grande\"; margin-top: 8px }" +
                      "</style> </head>" +
                      "<b>Do you want to save changes to this sketch<BR>" +
                      " before closing?</b>" +
                      "<p>If you don't save, your changes will be lost.",
                      JOptionPane.QUESTION_MESSAGE);

      String[] options = new String[]{
              "Save", "Cancel", "Don't Save"
      };
      pane.setOptions(options);

      // highlight the safest option ala apple hig
      pane.setInitialValue(options[0]);

      // on macosx, setting the destructive property places this option
      // away from the others at the lefthand side
      pane.putClientProperty("Quaqua.OptionPane.destructiveOption",
              new Integer(2));

      JDialog dialog = pane.createDialog(editor, null);
      dialog.setVisible(true);

      Object result = pane.getValue();
      if (result == options[0]) {
        return JOptionPane.YES_OPTION;
      } else if (result == options[1]) {
        return JOptionPane.CANCEL_OPTION;
      } else if (result == options[2]) {
        return JOptionPane.NO_OPTION;
      } else {
        return JOptionPane.CLOSED_OPTION;
      }
    }
  }


//if (result == JOptionPane.YES_OPTION) {
  //
//      } else if (result == JOptionPane.NO_OPTION) {
//        return true;  // ok to continue
  //
//      } else if (result == JOptionPane.CANCEL_OPTION) {
//        return false;
  //
//      } else {
//        throw new IllegalStateException();
//      }

  static public int showYesNoQuestion(Frame editor, String title,
                                      String primary, String secondary) {
    if (!OSUtils.isMacOS()) {
      return JOptionPane.showConfirmDialog(editor,
              "<html><body>" +
                      "<b>" + primary + "</b>" +
                      "<br>" + secondary, title,
              JOptionPane.YES_NO_OPTION,
              JOptionPane.QUESTION_MESSAGE);
    } else {
      // Pane formatting adapted from the Quaqua guide
      // http://www.randelshofer.ch/quaqua/guide/joptionpane.html
      JOptionPane pane =
              new JOptionPane("<html> " +
                      "<head> <style type=\"text/css\">" +
                      "b { font: 13pt \"Lucida Grande\" }" +
                      "p { font: 11pt \"Lucida Grande\"; margin-top: 8px }" +
                      "</style> </head>" +
                      "<b>" + primary + "</b>" +
                      "<p>" + secondary + "</p>",
                      JOptionPane.QUESTION_MESSAGE);

      String[] options = new String[]{
              "Yes", "No"
      };
      pane.setOptions(options);

      // highlight the safest option ala apple hig
      pane.setInitialValue(options[0]);

      JDialog dialog = pane.createDialog(editor, null);
      dialog.setVisible(true);

      Object result = pane.getValue();
      if (result == options[0]) {
        return JOptionPane.YES_OPTION;
      } else if (result == options[1]) {
        return JOptionPane.NO_OPTION;
      } else {
        return JOptionPane.CLOSED_OPTION;
      }
    }
  }


  /**
   * Retrieve a path to something in the Processing folder. Eventually this
   * may refer to the Contents subfolder of Processing.app, if we bundle things
   * up as a single .app file with no additional folders.
   */
//  static public String getContentsPath(String filename) {
//    String basePath = System.getProperty("user.dir");
//    /*
//      // do this later, when moving to .app package
//    if (PApplet.platform == PConstants.MACOSX) {
//      basePath = System.getProperty("processing.contents");
//    }
//    */
//    return basePath + File.separator + filename;
//  }


  /**
   * Get a path for something in the Processing lib folder.
   */
  /*
  static public String getLibContentsPath(String filename) {
    String libPath = getContentsPath("lib/" + filename);
    File libDir = new File(libPath);
    if (libDir.exists()) {
      return libPath;
    }
//    was looking into making this run from Eclipse, but still too much mess
//    libPath = getContents("build/shared/lib/" + what);
//    libDir = new File(libPath);
//    if (libDir.exists()) {
//      return libPath;
//    }
    return null;
  }
  */
  static public File getContentFile(String name) {
    return BaseNoGui.getContentFile(name);
  }


  /**
   * Get an image associated with the current color theme.
   */
  static public Image getThemeImage(String name, Component who) {
    return getLibImage("theme/" + name, who);
  }


  /**
   * Return an Image object from inside the Processing lib folder.
   */
  static public Image getLibImage(String name, Component who) {
    Toolkit tk = Toolkit.getDefaultToolkit();

    File imageLocation = new File(getContentFile("lib"), name);
    Image image = tk.getImage(imageLocation.getAbsolutePath());
    MediaTracker tracker = new MediaTracker(who);
    tracker.addImage(image, 0);
    try {
      tracker.waitForAll();
    } catch (InterruptedException e) {
    }
    return image;
  }


  // ...................................................................


  /**
   * Get the number of lines in a file by counting the number of newline
   * characters inside a String (and adding 1).
   */
  static public int countLines(String what) {
    return BaseNoGui.countLines(what);
  }


  /**
   * Same as PApplet.loadBytes(), however never does gzip decoding.
   */
  static public byte[] loadBytesRaw(File file) throws IOException {
    int size = (int) file.length();
    FileInputStream input = null;
    try {
      input = new FileInputStream(file);
      byte buffer[] = new byte[size];
      int offset = 0;
      int bytesRead;
      while ((bytesRead = input.read(buffer, offset, size - offset)) != -1) {
        offset += bytesRead;
        if (bytesRead == 0) break;
      }
      return buffer;
    } finally {
      IOUtils.closeQuietly(input);
    }
  }


  /**
   * Read from a file with a bunch of attribute/value pairs
   * that are separated by = and ignore comments with #.
   */
  static public HashMap<String, String> readSettings(File inputFile) {
    HashMap<String, String> outgoing = new HashMap<String, String>();
    if (!inputFile.exists()) return outgoing;  // return empty hash

    String lines[] = PApplet.loadStrings(inputFile);
    for (int i = 0; i < lines.length; i++) {
      int hash = lines[i].indexOf('#');
      String line = (hash == -1) ?
              lines[i].trim() : lines[i].substring(0, hash).trim();
      if (line.length() == 0) continue;

      int equals = line.indexOf('=');
      if (equals == -1) {
        System.err.println("ignoring illegal line in " + inputFile);
        System.err.println("  " + line);
        continue;
      }
      String attr = line.substring(0, equals).trim();
      String valu = line.substring(equals + 1).trim();
      outgoing.put(attr, valu);
    }
    return outgoing;
  }


  static public void copyFile(File sourceFile,
                              File targetFile) throws IOException {
    InputStream from = null;
    OutputStream to = null;
    try {
      from = new BufferedInputStream(new FileInputStream(sourceFile));
      to = new BufferedOutputStream(new FileOutputStream(targetFile));
      byte[] buffer = new byte[16 * 1024];
      int bytesRead;
      while ((bytesRead = from.read(buffer)) != -1) {
        to.write(buffer, 0, bytesRead);
      }
      to.flush();
    } finally {
      IOUtils.closeQuietly(from);
      IOUtils.closeQuietly(to);
    }

    targetFile.setLastModified(sourceFile.lastModified());
  }


  /**
   * Grab the contents of a file as a string.
   */
  static public String loadFile(File file) throws IOException {
    return BaseNoGui.loadFile(file);
  }


  /**
   * Spew the contents of a String object out to a file.
   */
  static public void saveFile(String str, File file) throws IOException {
    BaseNoGui.saveFile(str, file);
  }


  /**
   * Copy a folder from one place to another. This ignores all dot files and
   * folders found in the source directory, to avoid copying silly .DS_Store
   * files and potentially troublesome .svn folders.
   */
  static public void copyDir(File sourceDir,
                             File targetDir) throws IOException {
    targetDir.mkdirs();
    String files[] = sourceDir.list();
    if (files == null) {
      throw new IOException("Unable to list files from " + sourceDir);
    }
    for (String file : files) {
      // Ignore dot files (.DS_Store), dot folders (.svn) while copying
      if (file.charAt(0) == '.') continue;
      //if (files[i].equals(".") || files[i].equals("..")) continue;
      File source = new File(sourceDir, file);
      File target = new File(targetDir, file);
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
    BaseNoGui.removeDir(dir);
  }


  /**
   * Recursively remove all files within a directory,
   * used with removeDir(), or when the contents of a dir
   * should be removed, but not the directory itself.
   * (i.e. when cleaning temp files from lib/build)
   */
  static public void removeDescendants(File dir) {
    BaseNoGui.removeDescendants(dir);
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
   * Recursively creates a list of all files within the specified folder,
   * and returns a list of their relative paths.
   * Ignores any files/folders prefixed with a dot.
   */
  static public String[] listFiles(String path, boolean relative) {
    return listFiles(new File(path), relative);
  }


  static public String[] listFiles(File folder, boolean relative) {
    String path = folder.getAbsolutePath();
    Vector<String> vector = new Vector<String>();
    listFiles(relative ? (path + File.separator) : "", path, vector);
    String outgoing[] = new String[vector.size()];
    vector.copyInto(outgoing);
    return outgoing;
  }


  static protected void listFiles(String basePath,
                                  String path, Vector<String> vector) {
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

  public void handleAddLibrary() {
    JFileChooser fileChooser = new JFileChooser(System.getProperty("user.home"));
    fileChooser.setDialogTitle(_("Select a zip file or a folder containing the library you'd like to add"));
    fileChooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
    fileChooser.setFileFilter(new FileNameExtensionFilter(_("ZIP files or folders"), "zip"));

    Dimension preferredSize = fileChooser.getPreferredSize();
    fileChooser.setPreferredSize(new Dimension(preferredSize.width + 200, preferredSize.height + 200));

    int returnVal = fileChooser.showOpenDialog(activeEditor);

    if (returnVal != JFileChooser.APPROVE_OPTION) {
      return;
    }

    File sourceFile = fileChooser.getSelectedFile();
    File tmpFolder = null;

    try {
      // unpack ZIP
      if (!sourceFile.isDirectory()) {
        try {
          tmpFolder = FileUtils.createTempFolder();
          ZipDeflater zipDeflater = new ZipDeflater(sourceFile, tmpFolder);
          zipDeflater.deflate();
          File[] foldersInTmpFolder = tmpFolder.listFiles(new OnlyDirs());
          if (foldersInTmpFolder.length != 1) {
            throw new IOException(_("Zip doesn't contain a library"));
          }
          sourceFile = foldersInTmpFolder[0];
        } catch (IOException e) {
          activeEditor.statusError(e);
          return;
        }
      }

      File libFolder = sourceFile;
      if (FileUtils.isSubDirectory(new File(PreferencesData.get("sketchbook.path")), libFolder)) {
        activeEditor.statusError(_("A subfolder of your sketchbook is not a valid library"));
        return;
      }

      if (FileUtils.isSubDirectory(libFolder, new File(PreferencesData.get("sketchbook.path")))) {
        activeEditor.statusError(_("You can't import a folder that contains your sketchbook"));
        return;
      }

      String libName = libFolder.getName();
      if (!BaseNoGui.isSanitaryName(libName)) {
        String mess = I18n.format(_("The library \"{0}\" cannot be used.\n"
                        + "Library names must contain only basic letters and numbers.\n"
                        + "(ASCII only and no spaces, and it cannot start with a number)"),
                libName);
        activeEditor.statusError(mess);
        return;
      }

      String[] headers;
      if (new File(libFolder, "library.properties").exists()) {
        headers = BaseNoGui.headerListFromIncludePath(UserLibrary.create(libFolder).getSrcFolder());
      } else {
        headers = BaseNoGui.headerListFromIncludePath(libFolder);
      }
      if (headers.length == 0) {
        activeEditor.statusError(_("Specified folder/zip file does not contain a valid library"));
        return;
      }

      // copy folder
      File destinationFolder = new File(BaseNoGui.getSketchbookLibrariesFolder(), sourceFile.getName());
      if (!destinationFolder.mkdir()) {
        activeEditor.statusError(I18n.format(_("A library named {0} already exists"), sourceFile.getName()));
        return;
      }
      try {
        FileUtils.copy(sourceFile, destinationFolder);
      } catch (IOException e) {
        activeEditor.statusError(e);
        return;
      }
      activeEditor.statusNotice(_("Library added to your libraries. Check \"Include library\" menu"));
    } catch (IOException e) {
      // FIXME error when importing. ignoring :(
    } finally {
      // delete zip created temp folder, if exists
      FileUtils.recursiveDelete(tmpFolder);
    }
  }

  public static DiscoveryManager getDiscoveryManager() {
    return BaseNoGui.getDiscoveryManager();
  }

  public Editor getActiveEditor() {
    return activeEditor;
  }

  public boolean hasActiveEditor() {
    return activeEditor != null;
  }

  public List<Editor> getEditors() {
    return new LinkedList<Editor>(editors);
  }

  public Action getOpenBoardsManager() {
    return openBoardsManager;
  }

  public PdeKeywords getPdeKeywords() {
    return pdeKeywords;
  }

  public List<JMenuItem> getRecentSketchesMenuItems() {
    return recentSketchesMenuItems;
  }
}
