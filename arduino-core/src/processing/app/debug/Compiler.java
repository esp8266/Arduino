/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-08 Ben Fry and Casey Reas
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

package processing.app.debug;

import static processing.app.I18n._;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.SortedSet;
import java.util.TreeSet;

import cc.arduino.packages.BoardPort;
import cc.arduino.packages.Uploader;
import cc.arduino.packages.UploaderFactory;

import processing.app.BaseNoGui;
import processing.app.I18n;
import processing.app.PreferencesData;
import processing.app.SketchCode;
import processing.app.SketchData;
import processing.app.helpers.*;
import processing.app.helpers.filefilters.OnlyDirs;
import processing.app.packages.Library;
import processing.app.packages.LibraryList;
import processing.app.preproc.PdePreprocessor;
import processing.app.legacy.PApplet;

public class Compiler implements MessageConsumer {

  /**
   * File inside the build directory that contains the build options
   * used for the last build.
   */
  static final public String BUILD_PREFS_FILE = "buildprefs.txt";

  private SketchData sketch;
  private PreferencesMap prefs;
  private boolean verbose;

  private List<File> objectFiles;

  private boolean sketchIsCompiled;
  
  private RunnerException exception;

  /**
   * Listener interface for progress update on the GUI
   */
  public interface ProgressListener {
    public void progress(int percent);
  }

  private ProgressListener progressListener;

  static public String build(SketchData data, String buildPath, File tempBuildFolder, ProgressListener progListener, boolean verbose) throws RunnerException, PreferencesMapException {
    if (SketchData.checkSketchFile(data.getPrimaryFile()) == null)
      BaseNoGui.showError(_("Bad file selected"),
                          _("Bad sketch primary file or bad sketch directory structure"), null);

    String primaryClassName = data.getName() + ".cpp";
    Compiler compiler = new Compiler(data, buildPath, primaryClassName);
    File buildPrefsFile = new File(buildPath, BUILD_PREFS_FILE);
    String newBuildPrefs = compiler.buildPrefsString();

    // Do a forced cleanup (throw everything away) if the previous
    // build settings do not match the previous ones
    boolean prefsChanged = compiler.buildPreferencesChanged(buildPrefsFile, newBuildPrefs);
    compiler.cleanup(prefsChanged, tempBuildFolder);

    if (prefsChanged) {
      try {
        PrintWriter out = new PrintWriter(buildPrefsFile);
        out.print(newBuildPrefs);
        out.close();
      } catch (IOException e) {
        System.err.println(_("Could not write build preferences file"));
      }
    }

    compiler.setProgressListener(progListener);
    
    // compile the program. errors will happen as a RunnerException
    // that will bubble up to whomever called build().
    if (compiler.compile(verbose)) {
      compiler.size(compiler.getBuildPreferences());
      return primaryClassName;
    }
    return null;
  }
  
  static public Uploader getUploaderByPreferences(boolean noUploadPort) {
    TargetPlatform target = BaseNoGui.getTargetPlatform();
    String board = PreferencesData.get("board");

    if (noUploadPort)
    {
      return new UploaderFactory().newUploader(target.getBoards().get(board), null, noUploadPort);
    }
    else
    {
      BoardPort boardPort = BaseNoGui.getDiscoveryManager().find(PreferencesData.get("serial.port"));
      return new UploaderFactory().newUploader(target.getBoards().get(board), boardPort, noUploadPort);
    }
  }

  static public boolean upload(SketchData data, Uploader uploader, String buildPath, String suggestedClassName, boolean usingProgrammer, boolean noUploadPort, List<String> warningsAccumulator) throws Exception {

    if (uploader == null)
      uploader = getUploaderByPreferences(noUploadPort);

    boolean success = false;

    if (uploader.requiresAuthorization() && !PreferencesData.has(uploader.getAuthorizationKey())) {
      BaseNoGui.showError(_("Authorization required"),
                          _("No athorization data found"), null);
    }

    boolean useNewWarningsAccumulator = false;
    if (warningsAccumulator == null) {
      warningsAccumulator = new LinkedList<String>();
      useNewWarningsAccumulator = true;
    }

    try {
      success = uploader.uploadUsingPreferences(data.getFolder(), buildPath, suggestedClassName, usingProgrammer, warningsAccumulator);
    } finally {
      if (uploader.requiresAuthorization() && !success) {
        PreferencesData.remove(uploader.getAuthorizationKey());
      }
    }

    if (useNewWarningsAccumulator) {
      for (String warning : warningsAccumulator) {
        System.out.print(_("Warning"));
        System.out.print(": ");
        System.out.println(warning);
      }
    }

    return success;
  }

  /**
   * Create a new Compiler
   * @param _sketch Sketch object to be compiled.
   * @param _buildPath Where the temporary files live and will be built from.
   * @param _primaryClassName the name of the combined sketch file w/ extension
   */
  public Compiler(SketchData _sketch, String _buildPath, String _primaryClassName)
      throws RunnerException {
    sketch = _sketch;
    prefs = createBuildPreferences(_buildPath, _primaryClassName);

    // Start with an empty progress listener
    progressListener = new ProgressListener() {
      @Override
      public void progress(int percent) {
      }
    };
  }

  /**
   * Check if the build preferences used on the previous build in
   * buildPath match the ones given.
   */
  protected boolean buildPreferencesChanged(File buildPrefsFile, String newBuildPrefs) {
    // No previous build, so no match
    if (!buildPrefsFile.exists())
      return true;

    String previousPrefs;
    try {
      previousPrefs = FileUtils.readFileToString(buildPrefsFile);
    } catch (IOException e) {
      System.err.println(_("Could not read prevous build preferences file, rebuilding all"));
      return true;
    }

    if (!previousPrefs.equals(newBuildPrefs)) {
      System.out.println(_("Build options changed, rebuilding all"));
      return true;
    } else {
      return false;
    }
  }

  /**
   * Returns the build preferences of the given compiler as a string.
   * Only includes build-specific preferences, to make sure unrelated
   * preferences don't cause a rebuild (in particular preferences that
   * change on every start, like last.ide.xxx.daterun). */
  protected String buildPrefsString() {
    PreferencesMap buildPrefs = getBuildPreferences();
    String res = "";
    SortedSet<String> treeSet = new TreeSet<String>(buildPrefs.keySet());
    for (String k : treeSet) {
      if (k.startsWith("build.") || k.startsWith("compiler.") || k.startsWith("recipes."))
        res += k + " = " + buildPrefs.get(k) + "\n";
    }
    return res;
  }

  protected void setProgressListener(ProgressListener _progressListener) {
    progressListener = (_progressListener == null ?
                        new ProgressListener() {
                          @Override
                          public void progress(int percent) {
                          }
                        } : _progressListener);
  }
  
  /**
   * Cleanup temporary files used during a build/run.
   */
  protected void cleanup(boolean force, File tempBuildFolder) {
    // if the java runtime is holding onto any files in the build dir, we
    // won't be able to delete them, so we need to force a gc here
    System.gc();

    if (force) {
      // delete the entire directory and all contents
      // when we know something changed and all objects
      // need to be recompiled, or if the board does not
      // use setting build.dependency
      //Base.removeDir(tempBuildFolder);

      // note that we can't remove the builddir itself, otherwise
      // the next time we start up, internal runs using Runner won't
      // work because the build dir won't exist at startup, so the classloader
      // will ignore the fact that that dir is in the CLASSPATH in run.sh
      BaseNoGui.removeDescendants(tempBuildFolder);
    } else {
      // delete only stale source files, from the previously
      // compiled sketch.  This allows multiple windows to be
      // used.  Keep everything else, which might be reusable
      if (tempBuildFolder.exists()) {
        String files[] = tempBuildFolder.list();
        for (String file : files) {
          if (file.endsWith(".c") || file.endsWith(".cpp") || file.endsWith(".s")) {
            File deleteMe = new File(tempBuildFolder, file);
            if (!deleteMe.delete()) {
              System.err.println("Could not delete " + deleteMe);
            }
          }
        }
      }
    }

    // Create a fresh applet folder (needed before preproc is run below)
    //tempBuildFolder.mkdirs();
  }

  protected void size(PreferencesMap prefs) throws RunnerException {
    String maxTextSizeString = prefs.get("upload.maximum_size");
    String maxDataSizeString = prefs.get("upload.maximum_data_size");
    if (maxTextSizeString == null)
      return;
    long maxTextSize = Integer.parseInt(maxTextSizeString);
    long maxDataSize = -1;
    if (maxDataSizeString != null)
      maxDataSize = Integer.parseInt(maxDataSizeString);
    Sizer sizer = new Sizer(prefs);
    long[] sizes;
    try {
      sizes = sizer.computeSize();
    } catch (RunnerException e) {
      System.err.println(I18n.format(_("Couldn't determine program size: {0}"),
                                     e.getMessage()));
      return;
    }

    long textSize = sizes[0];
    long dataSize = sizes[1];
    System.out.println();
    System.out.println(I18n
                       .format(_("Sketch uses {0} bytes ({2}%%) of program storage space. Maximum is {1} bytes."),
                               textSize, maxTextSize, textSize * 100 / maxTextSize));
    if (dataSize >= 0) {
      if (maxDataSize > 0) {
        System.out
            .println(I18n
                .format(
                        _("Global variables use {0} bytes ({2}%%) of dynamic memory, leaving {3} bytes for local variables. Maximum is {1} bytes."),
                        dataSize, maxDataSize, dataSize * 100 / maxDataSize,
                        maxDataSize - dataSize));
      } else {
        System.out.println(I18n
            .format(_("Global variables use {0} bytes of dynamic memory."), dataSize));
      }
    }

    if (textSize > maxTextSize)
      throw new RunnerException(
          _("Sketch too big; see http://www.arduino.cc/en/Guide/Troubleshooting#size for tips on reducing it."));

    if (maxDataSize > 0 && dataSize > maxDataSize)
      throw new RunnerException(
          _("Not enough memory; see http://www.arduino.cc/en/Guide/Troubleshooting#size for tips on reducing your footprint."));

    int warnDataPercentage = Integer.parseInt(prefs.get("build.warn_data_percentage"));
    if (maxDataSize > 0 && dataSize > maxDataSize*warnDataPercentage/100)
      System.err.println(_("Low memory available, stability problems may occur."));
  }

  /**
   * Compile sketch.
   * @param _verbose
   *
   * @return true if successful.
   * @throws RunnerException Only if there's a problem. Only then.
   */
  public boolean compile(boolean _verbose) throws RunnerException, PreferencesMapException {
    preprocess(prefs.get("build.path"));
    
    verbose = _verbose || PreferencesData.getBoolean("build.verbose");
    sketchIsCompiled = false;
    objectFiles = new ArrayList<File>();

    // 0. include paths for core + all libraries
    progressListener.progress(20);
    List<File> includeFolders = new ArrayList<File>();
    includeFolders.add(prefs.getFile("build.core.path"));
    if (prefs.getFile("build.variant.path") != null)
      includeFolders.add(prefs.getFile("build.variant.path"));
    for (Library lib : importedLibraries) {
      if (verbose)
        System.out.println(I18n
            .format(_("Using library {0} in folder: {1} {2}"), lib.getName(),
                    lib.getFolder(), lib.isLegacy() ? "(legacy)" : ""));
      includeFolders.add(lib.getSrcFolder());
    }
    if (verbose)
      System.out.println();

    List<String> archs = new ArrayList<String>();
    archs.add(BaseNoGui.getTargetPlatform().getId());
    if (prefs.containsKey("architecture.override_check")) {
      String[] overrides = prefs.get("architecture.override_check").split(",");
      archs.addAll(Arrays.asList(overrides));
    }
    for (Library lib : importedLibraries) {
      if (!lib.supportsArchitecture(archs)) {
        System.err.println(I18n
            .format(_("WARNING: library {0} claims to run on {1} "
                + "architecture(s) and may be incompatible with your"
                + " current board which runs on {2} architecture(s)."), lib
                .getName(), lib.getArchitectures(), archs));
        System.err.println();
      }
    }
    
    // 1. compile the sketch (already in the buildPath)
    progressListener.progress(30);
    compileSketch(includeFolders);
    sketchIsCompiled = true;

    // 2. compile the libraries, outputting .o files to: <buildPath>/<library>/
    // Doesn't really use configPreferences
    progressListener.progress(40);
    compileLibraries(includeFolders);

    // 3. compile the core, outputting .o files to <buildPath> and then
    // collecting them into the core.a library file.
    progressListener.progress(50);
    compileCore();

    // 4. link it all together into the .elf file
    progressListener.progress(60);
    compileLink();

    // 5. extract EEPROM data (from EEMEM directive) to .eep file.
    progressListener.progress(70);
    runRecipe("recipe.objcopy.eep.pattern");

    // 6. build the .hex file
    progressListener.progress(80);
    runRecipe("recipe.objcopy.hex.pattern");

    progressListener.progress(90);
    return true;
  }

  private PreferencesMap createBuildPreferences(String _buildPath,
                                                String _primaryClassName)
      throws RunnerException {
    
    if (BaseNoGui.getBoardPreferences() == null) {
      RunnerException re = new RunnerException(
          _("No board selected; please choose a board from the Tools > Board menu."));
      re.hideStackTrace();
      throw re;
    }

    // Check if the board needs a platform from another package 
    TargetPlatform targetPlatform = BaseNoGui.getTargetPlatform();
    TargetPlatform corePlatform = null;
    PreferencesMap boardPreferences = BaseNoGui.getBoardPreferences();
    String core = boardPreferences.get("build.core");
    if (core.contains(":")) {
      String[] split = core.split(":");
      core = split[1];
      corePlatform = BaseNoGui.getTargetPlatform(split[0], targetPlatform.getId());
      if (corePlatform == null) {
        RunnerException re = new RunnerException(I18n
            .format(_("Selected board depends on '{0}' core (not installed)."),
                    split[0]));
        re.hideStackTrace();
        throw re;
      }
    }
    
    // Merge all the global preference configuration in order of priority
    PreferencesMap p = new PreferencesMap();
    p.putAll(PreferencesData.getMap());
    if (corePlatform != null)
      p.putAll(corePlatform.getPreferences());
    p.putAll(targetPlatform.getPreferences());
    p.putAll(BaseNoGui.getBoardPreferences());
    for (String k : p.keySet()) {
      if (p.get(k) == null)
        p.put(k, "");
    }

    p.put("build.path", _buildPath);
    p.put("build.project_name", _primaryClassName);
    p.put("build.arch", targetPlatform.getId().toUpperCase());
    
    // Platform.txt should define its own compiler.path. For
    // compatibility with earlier 1.5 versions, we define a (ugly,
    // avr-specific) default for it, but this should be removed at some
    // point.
    if (!p.containsKey("compiler.path")) {
      System.err.println(_("Third-party platform.txt does not define compiler.path. Please report this to the third-party hardware maintainer."));
      p.put("compiler.path", BaseNoGui.getAvrBasePath());
    }

    // Core folder
    TargetPlatform tp = corePlatform;
    if (tp == null)
      tp = targetPlatform;
    File coreFolder = new File(tp.getFolder(), "cores");
    coreFolder = new File(coreFolder, core);
    p.put("build.core", core);
    p.put("build.core.path", coreFolder.getAbsolutePath());
    
    // System Folder
    File systemFolder = tp.getFolder();
    systemFolder = new File(systemFolder, "system");
    p.put("build.system.path", systemFolder.getAbsolutePath());
    
    // Variant Folder
    String variant = p.get("build.variant");
    if (variant != null) {
      TargetPlatform t;
      if (!variant.contains(":")) {
        t = targetPlatform;
      } else {
        String[] split = variant.split(":", 2);
        t = BaseNoGui.getTargetPlatform(split[0], targetPlatform.getId());
        variant = split[1];
      }
      File variantFolder = new File(t.getFolder(), "variants");
      variantFolder = new File(variantFolder, variant);
      p.put("build.variant.path", variantFolder.getAbsolutePath());
    } else {
      p.put("build.variant.path", "");
    }
    
    return p;
  }

  private List<File> compileFiles(File outputPath, File sourcePath,
                                  boolean recurse, List<File> includeFolders)
          throws RunnerException, PreferencesMapException {
    List<File> sSources = findFilesInFolder(sourcePath, "S", recurse);
    List<File> cSources = findFilesInFolder(sourcePath, "c", recurse);
    List<File> cppSources = findFilesInFolder(sourcePath, "cpp", recurse);
    List<File> objectPaths = new ArrayList<File>();

    for (File file : sSources) {
      File objectFile = new File(outputPath, file.getName() + ".o");
      objectPaths.add(objectFile);
      String[] cmd = getCommandCompilerByRecipe(includeFolders, file, objectFile, "recipe.S.o.pattern");
      execAsynchronously(cmd);
    }
 		
    for (File file : cSources) {
      File objectFile = new File(outputPath, file.getName() + ".o");
      File dependFile = new File(outputPath, file.getName() + ".d");
      objectPaths.add(objectFile);
      if (isAlreadyCompiled(file, objectFile, dependFile, prefs))
        continue;
      String[] cmd = getCommandCompilerByRecipe(includeFolders, file, objectFile, "recipe.c.o.pattern");
      execAsynchronously(cmd);
    }

    for (File file : cppSources) {
      File objectFile = new File(outputPath, file.getName() + ".o");
      File dependFile = new File(outputPath, file.getName() + ".d");
      objectPaths.add(objectFile);
      if (isAlreadyCompiled(file, objectFile, dependFile, prefs))
        continue;
      String[] cmd = getCommandCompilerByRecipe(includeFolders, file, objectFile, "recipe.cpp.o.pattern");
      execAsynchronously(cmd);
    }
    
    return objectPaths;
  }

  /**
   * Strip escape sequences used in makefile dependency files (.d)
   * https://github.com/arduino/Arduino/issues/2255#issuecomment-57645845
   *
   * @param line
   * @return
   */
  protected static String unescapeDepFile(String line) {
    // Replaces: "\\" -> "\"
    // Replaces: "\ " -> " "
    // Replaces: "\#" -> "#"
    line = line.replaceAll("\\\\([ #\\\\])", "$1");
    // Replaces: "$$" -> "$"
    line = line.replace("$$", "$");
    return line;
  }

  private boolean isAlreadyCompiled(File src, File obj, File dep, Map<String, String> prefs) {
    boolean ret=true;
    try {
      //System.out.println("\n  isAlreadyCompiled: begin checks: " + obj.getPath());
      if (!obj.exists()) return false;  // object file (.o) does not exist
      if (!dep.exists()) return false;  // dep file (.d) does not exist
      long src_modified = src.lastModified();
      long obj_modified = obj.lastModified();
      if (src_modified >= obj_modified) return false;  // source modified since object compiled
      if (src_modified >= dep.lastModified()) return false;  // src modified since dep compiled
      BufferedReader reader = new BufferedReader(new FileReader(dep.getPath()));
      String line;
      boolean need_obj_parse = true;
      while ((line = reader.readLine()) != null) {
        if (line.endsWith("\\")) {
          line = line.substring(0, line.length() - 1);
        }
        line = line.trim();
        line = unescapeDepFile(line);
        if (line.length() == 0) continue; // ignore blank lines
        if (need_obj_parse) {
          // line is supposed to be the object file - make sure it really is!
          if (line.endsWith(":")) {
            line = line.substring(0, line.length() - 1);
            String objpath = obj.getCanonicalPath();
            File linefile = new File(line);
            String linepath = linefile.getCanonicalPath();
            //System.out.println("  isAlreadyCompiled: obj =  " + objpath);
            //System.out.println("  isAlreadyCompiled: line = " + linepath);
            if (objpath.compareTo(linepath) == 0) {
              need_obj_parse = false;
              continue;
            } else {
              ret = false;  // object named inside .d file is not the correct file!
              break;
            }
          } else {
            ret = false;  // object file supposed to end with ':', but didn't
            break;
          }
        } else {
          // line is a prerequisite file
          File prereq = new File(line);
          if (!prereq.exists()) {
            ret = false;  // prerequisite file did not exist
            break;
          }
          if (prereq.lastModified() >= obj_modified) {
            ret = false;  // prerequisite modified since object was compiled
            break;
          }
          //System.out.println("  isAlreadyCompiled:  prerequisite ok");
        }
      }
      reader.close();
    } catch (Exception e) {
      return false;  // any error reading dep file = recompile it
    }
    if (ret && verbose) {
      System.out.println(I18n.format(_("Using previously compiled file: {0}"), obj.getPath()));
    }
    return ret;
  }


  /**
   * Either succeeds or throws a RunnerException fit for public consumption.
   */
  private void execAsynchronously(String[] command) throws RunnerException {

    // eliminate any empty array entries
    List<String> stringList = new ArrayList<String>();
    for (String string : command) {
      string = string.trim();
      if (string.length() != 0)
        stringList.add(string);
    }
    command = stringList.toArray(new String[stringList.size()]);
    if (command.length == 0)
      return;
    int result = 0;

    if (verbose) {
      for (String c : command)
        System.out.print(c + " ");
      System.out.println();
    }

    Process process;
    try {
      process = ProcessUtils.exec(command);
    } catch (IOException e) {
      RunnerException re = new RunnerException(e.getMessage());
      re.hideStackTrace();
      throw re;
    }

    MessageSiphon in = new MessageSiphon(process.getInputStream(), this);
    MessageSiphon err = new MessageSiphon(process.getErrorStream(), this);

    // wait for the process to finish.  if interrupted
    // before waitFor returns, continue waiting
    boolean compiling = true;
    while (compiling) {
      try {
        in.join();
        err.join();
        result = process.waitFor();
        //System.out.println("result is " + result);
        compiling = false;
      } catch (InterruptedException ignored) { }
    }

    // an error was queued up by message(), barf this back to compile(),
    // which will barf it back to Editor. if you're having trouble
    // discerning the imagery, consider how cows regurgitate their food
    // to digest it, and the fact that they have five stomaches.
    //
    //System.out.println("throwing up " + exception);
    if (exception != null)
      throw exception;

    if (result > 1) {
      // a failure in the tool (e.g. unable to locate a sub-executable)
      System.err
          .println(I18n.format(_("{0} returned {1}"), command[0], result));
    }

    if (result != 0) {
      RunnerException re = new RunnerException(_("Error compiling."));
      re.hideStackTrace();
      throw re;
    }
  }

  /**
   * Part of the MessageConsumer interface, this is called
   * whenever a piece (usually a line) of error message is spewed
   * out from the compiler. The errors are parsed for their contents
   * and line number, which is then reported back to Editor.
   */
  public void message(String s) {
    int i;

    // remove the build path so people only see the filename
    // can't use replaceAll() because the path may have characters in it which
    // have meaning in a regular expression.
    if (!verbose) {
      String buildPath = prefs.get("build.path");
      while ((i = s.indexOf(buildPath + File.separator)) != -1) {
        s = s.substring(0, i) + s.substring(i + (buildPath + File.separator).length());
      }
    }
  
    // look for error line, which contains file name, line number,
    // and at least the first line of the error message
    String errorFormat = "([\\w\\d_]+.\\w+):(\\d+):\\s*error:\\s*(.*)\\s*";
    String[] pieces = PApplet.match(s, errorFormat);

//    if (pieces != null && exception == null) {
//      exception = sketch.placeException(pieces[3], pieces[1], PApplet.parseInt(pieces[2]) - 1);
//      if (exception != null) exception.hideStackTrace();
//    }
    
    if (pieces != null) {
      String error = pieces[3], msg = "";
      
      if (pieces[3].trim().equals("SPI.h: No such file or directory")) {
        error = _("Please import the SPI library from the Sketch > Import Library menu.");
        msg = _("\nAs of Arduino 0019, the Ethernet library depends on the SPI library." +
              "\nYou appear to be using it or another library that depends on the SPI library.\n\n");
      }
      
      if (pieces[3].trim().equals("'BYTE' was not declared in this scope")) {
        error = _("The 'BYTE' keyword is no longer supported.");
        msg = _("\nAs of Arduino 1.0, the 'BYTE' keyword is no longer supported." +
              "\nPlease use Serial.write() instead.\n\n");
      }
      
      if (pieces[3].trim().equals("no matching function for call to 'Server::Server(int)'")) {
        error = _("The Server class has been renamed EthernetServer.");
        msg = _("\nAs of Arduino 1.0, the Server class in the Ethernet library " +
              "has been renamed to EthernetServer.\n\n");
      }
      
      if (pieces[3].trim().equals("no matching function for call to 'Client::Client(byte [4], int)'")) {
        error = _("The Client class has been renamed EthernetClient.");
        msg = _("\nAs of Arduino 1.0, the Client class in the Ethernet library " +
              "has been renamed to EthernetClient.\n\n");
      }
      
      if (pieces[3].trim().equals("'Udp' was not declared in this scope")) {
        error = _("The Udp class has been renamed EthernetUdp.");
        msg = _("\nAs of Arduino 1.0, the Udp class in the Ethernet library " +
              "has been renamed to EthernetUdp.\n\n");
      }
      
      if (pieces[3].trim().equals("'class TwoWire' has no member named 'send'")) {
        error = _("Wire.send() has been renamed Wire.write().");
        msg = _("\nAs of Arduino 1.0, the Wire.send() function was renamed " +
              "to Wire.write() for consistency with other libraries.\n\n");
      }
      
      if (pieces[3].trim().equals("'class TwoWire' has no member named 'receive'")) {
        error = _("Wire.receive() has been renamed Wire.read().");
        msg = _("\nAs of Arduino 1.0, the Wire.receive() function was renamed " +
              "to Wire.read() for consistency with other libraries.\n\n");
      }

      if (pieces[3].trim().equals("'Mouse' was not declared in this scope")) {
        error = _("'Mouse' only supported on the Arduino Leonardo");
        //msg = _("\nThe 'Mouse' class is only supported on the Arduino Leonardo.\n\n");
      }
      
      if (pieces[3].trim().equals("'Keyboard' was not declared in this scope")) {
        error = _("'Keyboard' only supported on the Arduino Leonardo");
        //msg = _("\nThe 'Keyboard' class is only supported on the Arduino Leonardo.\n\n");
      }
      
      RunnerException e = null;
      if (!sketchIsCompiled) {
        // Place errors when compiling the sketch, but never while compiling libraries
        // or the core.  The user's sketch might contain the same filename!
        e = placeException(error, pieces[1], PApplet.parseInt(pieces[2]) - 1);
      }

      // replace full file path with the name of the sketch tab (unless we're
      // in verbose mode, in which case don't modify the compiler output)
      if (e != null && !verbose) {
        SketchCode code = sketch.getCode(e.getCodeIndex());
        String fileName = (code.isExtension("ino") || code.isExtension("pde")) ? code.getPrettyName() : code.getFileName();
        int lineNum = e.getCodeLine() + 1;
        s = fileName + ":" + lineNum + ": error: " + pieces[3] + msg;        
      }
            
      if (exception == null && e != null) {
        exception = e;
        exception.hideStackTrace();
      }      
    }
    
    if (s.contains("undefined reference to `SPIClass::begin()'") &&
        s.contains("libraries/Robot_Control")) {
      String error = _("Please import the SPI library from the Sketch > Import Library menu.");
      exception = new RunnerException(error);
    }

    if (s.contains("undefined reference to `Wire'") &&
        s.contains("libraries/Robot_Control")) {
      String error = _("Please import the Wire library from the Sketch > Import Library menu.");
      exception = new RunnerException(error);
    }
		
    System.err.print(s);
  }

  private String[] getCommandCompilerByRecipe(List<File> includeFolders, File sourceFile, File objectFile, String recipe) throws PreferencesMapException, RunnerException {
    String includes = prepareIncludes(includeFolders);
    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("ide_version", "" + BaseNoGui.REVISION);
    dict.put("includes", includes);
    dict.put("source_file", sourceFile.getAbsolutePath());
    dict.put("object_file", objectFile.getAbsolutePath());

    String cmd = prefs.getOrExcept(recipe);
    try {
      return StringReplacer.formatAndSplit(cmd, dict, true);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
  }

  /////////////////////////////////////////////////////////////////////////////

  private void createFolder(File folder) throws RunnerException {
    if (folder.isDirectory())
      return;
    if (!folder.mkdir())
      throw new RunnerException("Couldn't create: " + folder);
  }

  static public List<File> findFilesInFolder(File folder, String extension,
                                             boolean recurse) {
    List<File> files = new ArrayList<File>();

    if (FileUtils.isSCCSOrHiddenFile(folder)) {
      return files;
    }

    File[] listFiles = folder.listFiles();
    if (listFiles == null) {
      return files;
    }

    for (File file : listFiles) {
      if (FileUtils.isSCCSOrHiddenFile(file)) {
        continue; // skip hidden files
      }

      if (file.getName().endsWith("." + extension))
        files.add(file);

      if (recurse && file.isDirectory()) {
        files.addAll(findFilesInFolder(file, extension, true));
      }
    }

    return files;
  }
  
  // 1. compile the sketch (already in the buildPath)
  void compileSketch(List<File> includeFolders) throws RunnerException, PreferencesMapException {
    File buildPath = prefs.getFile("build.path");
    objectFiles.addAll(compileFiles(buildPath, buildPath, false, includeFolders));
  }

  // 2. compile the libraries, outputting .o files to:
  // <buildPath>/<library>/
  void compileLibraries(List<File> includeFolders) throws RunnerException, PreferencesMapException {
    for (Library lib : importedLibraries) {
      compileLibrary(lib, includeFolders);
    }
  }

  private void compileLibrary(Library lib, List<File> includeFolders)
          throws RunnerException, PreferencesMapException {
    File libFolder = lib.getSrcFolder();
    File libBuildFolder = prefs.getFile(("build.path"), lib.getName());

    if (lib.useRecursion()) {
      // libBuildFolder == {build.path}/LibName
      // libFolder      == {lib.path}/src
      recursiveCompileFilesInFolder(libBuildFolder, libFolder, includeFolders);

    } else {
      // libFolder          == {lib.path}/
      // utilityFolder      == {lib.path}/utility
      // libBuildFolder     == {build.path}/LibName
      // utilityBuildFolder == {build.path}/LibName/utility
      File utilityFolder = new File(libFolder, "utility");
      File utilityBuildFolder = new File(libBuildFolder, "utility");

      includeFolders.add(utilityFolder);
      compileFilesInFolder(libBuildFolder, libFolder, includeFolders);
      compileFilesInFolder(utilityBuildFolder, utilityFolder, includeFolders);

      // other libraries should not see this library's utility/ folder
      includeFolders.remove(utilityFolder);
    }
  }

  private void recursiveCompileFilesInFolder(File srcBuildFolder, File srcFolder, List<File> includeFolders) throws RunnerException, PreferencesMapException {
    compileFilesInFolder(srcBuildFolder, srcFolder, includeFolders);
    for (File subFolder : srcFolder.listFiles(new OnlyDirs())) {
      File subBuildFolder = new File(srcBuildFolder, subFolder.getName());
      recursiveCompileFilesInFolder(subBuildFolder, subFolder, includeFolders);
    }
  }

  private void compileFilesInFolder(File buildFolder, File srcFolder, List<File> includeFolders) throws RunnerException, PreferencesMapException {
    createFolder(buildFolder);
    List<File> objects = compileFiles(buildFolder, srcFolder, false, includeFolders);
    objectFiles.addAll(objects);
  }

  // 3. compile the core, outputting .o files to <buildPath> and then
  // collecting them into the core.a library file.
  // Also compiles the variant (if it supplies actual source files),
  // which are included in the link directly (not through core.a)
  void compileCore()
          throws RunnerException, PreferencesMapException {

    File coreFolder = prefs.getFile("build.core.path");
    File variantFolder = prefs.getFile("build.variant.path");
    File buildFolder = prefs.getFile("build.path");

    List<File> includeFolders = new ArrayList<File>();
    includeFolders.add(coreFolder); // include core path only
    if (variantFolder != null)
      includeFolders.add(variantFolder);


    if (variantFolder != null)
      objectFiles.addAll(compileFiles(buildFolder, variantFolder, true,
                                      includeFolders));

    File afile = new File(buildFolder, "core.a");

    List<File> coreObjectFiles = compileFiles(buildFolder, coreFolder, true,
                                              includeFolders);

    // See if the .a file is already uptodate
    if (afile.exists()) {
      boolean changed = false;
      for (File file : coreObjectFiles) {
        if (file.lastModified() > afile.lastModified()) {
          changed = true;
          break;
        }
      }

      // If none of the object files is newer than the .a file, don't
      // bother rebuilding the .a file. There is a small corner case
      // here: If a source file was removed, but no other source file
      // was modified, this will not rebuild core.a even when it
      // should. It's hard to fix and not a realistic case, so it
      // shouldn't be a problem.
      if (!changed) {
        if (verbose)
          System.out.println(I18n.format(_("Using previously compiled file: {0}"), afile.getPath()));
        return;
      }
    }

    // Delete the .a file, to prevent any previous code from lingering
    afile.delete();

    try {
      for (File file : coreObjectFiles) {

        PreferencesMap dict = new PreferencesMap(prefs);
        dict.put("ide_version", "" + BaseNoGui.REVISION);
        dict.put("archive_file", afile.getName());
        dict.put("object_file", file.getAbsolutePath());

        String[] cmdArray;
        String cmd = prefs.getOrExcept("recipe.ar.pattern");
        try {
          cmdArray = StringReplacer.formatAndSplit(cmd, dict, true);
        } catch (Exception e) {
          throw new RunnerException(e);
        }
        execAsynchronously(cmdArray);
      }
    } catch (RunnerException e) {
      afile.delete();
      throw e;
    }
  }
			
  // 4. link it all together into the .elf file
  void compileLink()
          throws RunnerException, PreferencesMapException {

    // TODO: Make the --relax thing in configuration files.

    // For atmega2560, need --relax linker option to link larger
    // programs correctly.
    String optRelax = "";
    if (prefs.get("build.mcu").equals("atmega2560"))
      optRelax = ",--relax";

    String objectFileList = "";
    for (File file : objectFiles)
      objectFileList += " \"" + file.getAbsolutePath() + '"';
    objectFileList = objectFileList.substring(1);

    PreferencesMap dict = new PreferencesMap(prefs);
    String flags = dict.get("compiler.c.elf.flags") + optRelax;
    dict.put("compiler.c.elf.flags", flags);
    dict.put("archive_file", "core.a");
    dict.put("object_files", objectFileList);
    dict.put("ide_version", "" + BaseNoGui.REVISION);

    String[] cmdArray;
    String cmd = prefs.getOrExcept("recipe.c.combine.pattern");
    try {
      cmdArray = StringReplacer.formatAndSplit(cmd, dict, true);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
    execAsynchronously(cmdArray);
  }

  void runRecipe(String recipe) throws RunnerException, PreferencesMapException {
    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("ide_version", "" + BaseNoGui.REVISION);

    String[] cmdArray;
    String cmd = prefs.getOrExcept(recipe);
    try {
      cmdArray = StringReplacer.formatAndSplit(cmd, dict, true);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
    execAsynchronously(cmdArray);
  }

  private static String prepareIncludes(List<File> includeFolders) {
    String res = "";
    for (File p : includeFolders)
      res += " \"-I" + p.getAbsolutePath() + '"';

    // Remove first space
    return res.substring(1);
  }
  
  public PreferencesMap getBuildPreferences() {
    return prefs;
  }
  
  /**
   * Build all the code for this sketch.
   *
   * In an advanced program, the returned class name could be different,
   * which is why the className is set based on the return value.
   * A compilation error will burp up a RunnerException.
   *
   * Setting purty to 'true' will cause exception line numbers to be incorrect.
   * Unless you know the code compiles, you should first run the preprocessor
   * with purty set to false to make sure there are no errors, then once
   * successful, re-export with purty set to true.
   *
   * @param buildPath Location to copy all the .java files
   * @return null if compilation failed, main class name if not
   */
  public void preprocess(String buildPath) throws RunnerException {
    preprocess(buildPath, new PdePreprocessor());
  }

  public void preprocess(String buildPath, PdePreprocessor preprocessor) throws RunnerException {

    // 1. concatenate all .pde files to the 'main' pde
    //    store line number for starting point of each code bit

    StringBuffer bigCode = new StringBuffer();
    int bigCount = 0;
    for (SketchCode sc : sketch.getCodes()) {
      if (sc.isExtension("ino") || sc.isExtension("pde")) {
        sc.setPreprocOffset(bigCount);
        // These #line directives help the compiler report errors with
        // correct the filename and line number (issue 281 & 907)
        bigCode.append("#line 1 \"" + sc.getFileName() + "\"\n");
        bigCode.append(sc.getProgram());
        bigCode.append('\n');
        bigCount += sc.getLineCount();
      }
    }

    // Note that the headerOffset isn't applied until compile and run, because
    // it only applies to the code after it's been written to the .java file.
    int headerOffset = 0;
    try {
      headerOffset = preprocessor.writePrefix(bigCode.toString());
    } catch (FileNotFoundException fnfe) {
      fnfe.printStackTrace();
      String msg = _("Build folder disappeared or could not be written");
      throw new RunnerException(msg);
    }

    // 2. run preproc on that code using the sugg class name
    //    to create a single .java file and write to buildpath

    try {
      // Output file
      File streamFile = new File(buildPath, sketch.getName() + ".cpp");
      FileOutputStream outputStream = new FileOutputStream(streamFile);
      preprocessor.write(outputStream);
      outputStream.close();
    } catch (FileNotFoundException fnfe) {
      fnfe.printStackTrace();
      String msg = _("Build folder disappeared or could not be written");
      throw new RunnerException(msg);
    } catch (RunnerException pe) {
      // RunnerExceptions are caught here and re-thrown, so that they don't
      // get lost in the more general "Exception" handler below.
      throw pe;

    } catch (Exception ex) {
      // TODO better method for handling this?
      System.err.println(I18n.format(_("Uncaught exception type: {0}"), ex.getClass()));
      ex.printStackTrace();
      throw new RunnerException(ex.toString());
    }

    // grab the imports from the code just preproc'd

    importedLibraries = new LibraryList();
    for (String item : preprocessor.getExtraImports()) {
      Library lib = BaseNoGui.importToLibraryTable.get(item);
      if (lib != null && !importedLibraries.contains(lib)) {
        importedLibraries.add(lib);
      }
    }

    // 3. then loop over the code[] and save each .java file

    for (SketchCode sc : sketch.getCodes()) {
      if (sc.isExtension("c") || sc.isExtension("cpp") || sc.isExtension("h")) {
        // no pre-processing services necessary for java files
        // just write the the contents of 'program' to a .java file
        // into the build directory. uses byte stream and reader/writer
        // shtuff so that unicode bunk is properly handled
        String filename = sc.getFileName(); //code[i].name + ".java";
        try {
          BaseNoGui.saveFile(sc.getProgram(), new File(buildPath, filename));
        } catch (IOException e) {
          e.printStackTrace();
          throw new RunnerException(I18n.format(_("Problem moving {0} to the build folder"), filename));
        }

      } else if (sc.isExtension("ino") || sc.isExtension("pde")) {
        // The compiler and runner will need this to have a proper offset
        sc.addPreprocOffset(headerOffset);
      }
    }
  }


  /**
   * List of library folders.
   */
  private LibraryList importedLibraries;

  /**
   * Map an error from a set of processed .java files back to its location
   * in the actual sketch.
   * @param message The error message.
   * @param dotJavaFilename The .java file where the exception was found.
   * @param dotJavaLine Line number of the .java file for the exception (0-indexed!)
   * @return A RunnerException to be sent to the editor, or null if it wasn't
   *         possible to place the exception to the sketch code.
   */
  public RunnerException placeException(String message,
                                        String dotJavaFilename,
                                        int dotJavaLine) {
     // Placing errors is simple, because we inserted #line directives
     // into the preprocessed source.  The compiler gives us correct
     // the file name and line number.  :-)
     for (SketchCode code : sketch.getCodes()) {
       if (dotJavaFilename.equals(code.getFileName())) {
         return new RunnerException(message, sketch.indexOfCode(code), dotJavaLine);
       }
     }
     return null;
  }

}
