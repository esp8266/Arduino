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
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;

import processing.app.Base;
import processing.app.I18n;
import processing.app.Preferences;
import processing.app.Sketch;
import processing.app.SketchCode;
import processing.app.helpers.FileUtils;
import processing.app.helpers.PreferencesMap;
import processing.app.helpers.ProcessUtils;
import processing.app.helpers.StringReplacer;
import processing.app.helpers.filefilters.OnlyDirs;
import processing.app.packages.Library;
import processing.core.PApplet;

public class Compiler implements MessageConsumer {
  static final String BUGS_URL =
    _("http://github.com/arduino/Arduino/issues");
  static final String SUPER_BADNESS =
    I18n.format(_("Compiler error, please submit this code to {0}"), BUGS_URL);

  private Sketch sketch;

  private List<File> objectFiles;

  private PreferencesMap prefs;
  private boolean verbose;
  private boolean sketchIsCompiled;
  private String targetArch;
  
  private RunnerException exception;

  /**
   * Create a new Compiler
   * @param _sketch Sketch object to be compiled.
   * @param _buildPath Where the temporary files live and will be built from.
   * @param _primaryClassName the name of the combined sketch file w/ extension
   */
  public Compiler(Sketch _sketch, String _buildPath, String _primaryClassName)
      throws RunnerException {
    sketch = _sketch;
    prefs = createBuildPreferences(_buildPath, _primaryClassName);
  }

  /**
   * Compile sketch.
   *
   * @return true if successful.
   * @throws RunnerException Only if there's a problem. Only then.
   */
  public boolean compile(boolean _verbose) throws RunnerException {
    verbose = _verbose || Preferences.getBoolean("build.verbose");
    sketchIsCompiled = false;
    objectFiles = new ArrayList<File>();

    // 0. include paths for core + all libraries
    sketch.setCompilingProgress(20);
    List<File> includeFolders = new ArrayList<File>();
    includeFolders.add(prefs.getFile("build.core.path"));
    if (prefs.getFile("build.variant.path") != null)
      includeFolders.add(prefs.getFile("build.variant.path"));
    for (Library lib : sketch.getImportedLibraries()) {
      if (verbose)
        System.out.println(I18n
            .format(_("Using library {0} in folder: {1} {2}"), lib.getName(),
                    lib.getFolder(), lib.isLegacy() ? "(legacy)" : ""));
      includeFolders.add(lib.getSrcFolder());
    }
    if (verbose)
      System.out.println();

    List<String> archs = new ArrayList<String>();
    archs.add(Base.getTargetPlatform().getId());
    if (prefs.containsKey("architecture.override_check")) {
      String[] overrides = prefs.get("architecture.override_check").split(",");
      archs.addAll(Arrays.asList(overrides));
    }
    for (Library lib : sketch.getImportedLibraries()) {
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
    sketch.setCompilingProgress(30);
    compileSketch(includeFolders);
    sketchIsCompiled = true;

    // 2. compile the libraries, outputting .o files to: <buildPath>/<library>/
    // Doesn't really use configPreferences
    sketch.setCompilingProgress(40);
    compileLibraries(includeFolders);

    // 3. compile the core, outputting .o files to <buildPath> and then
    // collecting them into the core.a library file.
    sketch.setCompilingProgress(50);
    compileCore();

    // 4. link it all together into the .elf file
    sketch.setCompilingProgress(60);
    compileLink();

    // 5. extract EEPROM data (from EEMEM directive) to .eep file.
    sketch.setCompilingProgress(70);
    compileEep();

    // 6. build the .hex file
    sketch.setCompilingProgress(80);
    compileHex();

    sketch.setCompilingProgress(90);
    return true;
  }

  private PreferencesMap createBuildPreferences(String _buildPath,
                                                String _primaryClassName)
      throws RunnerException {
    
    if (Base.getBoardPreferences() == null) {
      RunnerException re = new RunnerException(
          _("No board selected; please choose a board from the Tools > Board menu."));
      re.hideStackTrace();
      throw re;
    }

    // Check if the board needs a platform from another package 
    TargetPlatform targetPlatform = Base.getTargetPlatform();
    TargetPlatform corePlatform = null;
    PreferencesMap boardPreferences = Base.getBoardPreferences();
    String core = boardPreferences.get("build.core");
    if (core.contains(":")) {
      String[] split = core.split(":");
      core = split[1];
      corePlatform = Base.getTargetPlatform(split[0], targetPlatform.getId());
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
    p.putAll(Preferences.getMap());
    if (corePlatform != null)
      p.putAll(corePlatform.getPreferences());
    p.putAll(targetPlatform.getPreferences());
    p.putAll(Base.getBoardPreferences());
    for (String k : p.keySet()) {
      if (p.get(k) == null)
        p.put(k, "");
    }

    p.put("build.path", _buildPath);
    p.put("build.project_name", _primaryClassName);
    targetArch = targetPlatform.getId();
    p.put("build.arch", targetArch.toUpperCase());
    
    if (!p.containsKey("compiler.path"))
      p.put("compiler.path", Base.getAvrBasePath());

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
        t = Base.getTargetPlatform(split[0], targetPlatform.getId());
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
      throws RunnerException {
    List<File> sSources = findFilesInFolder(sourcePath, "S", recurse);
    List<File> cSources = findFilesInFolder(sourcePath, "c", recurse);
    List<File> cppSources = findFilesInFolder(sourcePath, "cpp", recurse);
    List<File> objectPaths = new ArrayList<File>();

    for (File file : sSources) {
      File objectFile = new File(outputPath, file.getName() + ".o");
      objectPaths.add(objectFile);
      String[] cmd = getCommandCompilerS(includeFolders, file, objectFile);
      execAsynchronously(cmd);
    }
 		
    for (File file : cSources) {
      File objectFile = new File(outputPath, file.getName() + ".o");
      File dependFile = new File(outputPath, file.getName() + ".d");
      objectPaths.add(objectFile);
      if (isAlreadyCompiled(file, objectFile, dependFile, prefs))
        continue;
      String[] cmd = getCommandCompilerC(includeFolders, file, objectFile);
      execAsynchronously(cmd);
    }

    for (File file : cppSources) {
      File objectFile = new File(outputPath, file.getName() + ".o");
      File dependFile = new File(outputPath, file.getName() + ".d");
      objectPaths.add(objectFile);
      if (isAlreadyCompiled(file, objectFile, dependFile, prefs))
        continue;
      String[] cmd = getCommandCompilerCPP(includeFolders, file, objectFile);
      execAsynchronously(cmd);
    }
    
    return objectPaths;
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

  boolean firstErrorFound;
  boolean secondErrorFound;

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

    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;

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
        e = sketch.placeException(error, pieces[1], PApplet.parseInt(pieces[2]) - 1);
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
    
    System.err.print(s);
  }

  private String[] getCommandCompilerS(List<File> includeFolders,
                                       File sourceFile, File objectFile)
      throws RunnerException {
    String includes = prepareIncludes(includeFolders);
    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("ide_version", "" + Base.REVISION);
    dict.put("includes", includes);
    dict.put("source_file", sourceFile.getAbsolutePath());
    dict.put("object_file", objectFile.getAbsolutePath());

    try {
      String cmd = prefs.get("recipe.S.o.pattern");
      return StringReplacer.formatAndSplit(cmd, dict, true);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
  }

  private String[] getCommandCompilerC(List<File> includeFolders,
                                       File sourceFile, File objectFile)
      throws RunnerException {
    String includes = prepareIncludes(includeFolders);

    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("ide_version", "" + Base.REVISION);
    dict.put("includes", includes);
    dict.put("source_file", sourceFile.getAbsolutePath());
    dict.put("object_file", objectFile.getAbsolutePath());

    String cmd = prefs.get("recipe.c.o.pattern");
    try {
      return StringReplacer.formatAndSplit(cmd, dict, true);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
  }

  private String[] getCommandCompilerCPP(List<File> includeFolders,
                                         File sourceFile, File objectFile)
      throws RunnerException {
    String includes = prepareIncludes(includeFolders);

    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("ide_version", "" + Base.REVISION);
    dict.put("includes", includes);
    dict.put("source_file", sourceFile.getAbsolutePath());
    dict.put("object_file", objectFile.getAbsolutePath());

    String cmd = prefs.get("recipe.cpp.o.pattern");
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
  void compileSketch(List<File> includeFolders) throws RunnerException {
    File buildPath = prefs.getFile("build.path");
    objectFiles.addAll(compileFiles(buildPath, buildPath, false, includeFolders));
  }

  // 2. compile the libraries, outputting .o files to:
  // <buildPath>/<library>/
  void compileLibraries(List<File> includeFolders) throws RunnerException {
    for (Library lib : sketch.getImportedLibraries()) {
      compileLibrary(lib, includeFolders);
    }
  }

  private void compileLibrary(Library lib, List<File> includeFolders)
      throws RunnerException {
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

  private void recursiveCompileFilesInFolder(File srcBuildFolder, File srcFolder, List<File> includeFolders) throws RunnerException {
    compileFilesInFolder(srcBuildFolder, srcFolder, includeFolders);
    for (File subFolder : srcFolder.listFiles(new OnlyDirs())) {
      File subBuildFolder = new File(srcBuildFolder, subFolder.getName());
      recursiveCompileFilesInFolder(subBuildFolder, subFolder, includeFolders);
    }
  }

  private void compileFilesInFolder(File buildFolder, File srcFolder, List<File> includeFolders) throws RunnerException {
    createFolder(buildFolder);
    List<File> objects = compileFiles(buildFolder, srcFolder, false, includeFolders);
    objectFiles.addAll(objects);
  }

  // 3. compile the core, outputting .o files to <buildPath> and then
  // collecting them into the core.a library file.
  void compileCore()
      throws RunnerException {

    File coreFolder = prefs.getFile("build.core.path");
    File variantFolder = prefs.getFile("build.variant.path");
    File buildFolder = prefs.getFile("build.path");

    List<File> includeFolders = new ArrayList<File>();
    includeFolders.add(coreFolder); // include core path only
    if (variantFolder != null)
      includeFolders.add(variantFolder);

    List<File> objectFiles = compileFiles(buildFolder, coreFolder, true,
                                          includeFolders);
    if (variantFolder != null)
      objectFiles.addAll(compileFiles(buildFolder, variantFolder, true,
                                      includeFolders));

    for (File file : objectFiles) {

      PreferencesMap dict = new PreferencesMap(prefs);
      dict.put("ide_version", "" + Base.REVISION);
      dict.put("archive_file", "core.a");
      dict.put("object_file", file.getAbsolutePath());

      String[] cmdArray;
      try {
        String cmd = prefs.get("recipe.ar.pattern");
        cmdArray = StringReplacer.formatAndSplit(cmd, dict, true);
      } catch (Exception e) {
        throw new RunnerException(e);
      }
      execAsynchronously(cmdArray);
    }
  }
			
  // 4. link it all together into the .elf file
  void compileLink()
      throws RunnerException {

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
    dict.put("ide_version", "" + Base.REVISION);

    String[] cmdArray;
    try {
      String cmd = prefs.get("recipe.c.combine.pattern");
      cmdArray = StringReplacer.formatAndSplit(cmd, dict, true);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
    execAsynchronously(cmdArray);
  }

  // 5. extract EEPROM data (from EEMEM directive) to .eep file.
  void compileEep() throws RunnerException {
    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("ide_version", "" + Base.REVISION);

    String[] cmdArray;
    try {
      String cmd = prefs.get("recipe.objcopy.eep.pattern");
      cmdArray = StringReplacer.formatAndSplit(cmd, dict, true);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
    execAsynchronously(cmdArray);
  }
	
  // 6. build the .hex file
  void compileHex() throws RunnerException {
    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("ide_version", "" + Base.REVISION);

    String[] cmdArray;
    try {
      String cmd = prefs.get("recipe.objcopy.hex.pattern");
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
}
