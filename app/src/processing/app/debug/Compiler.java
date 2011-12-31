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
import java.io.FilenameFilter;
import java.io.IOException;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import processing.app.Base;
import processing.app.I18n;
import processing.app.Preferences;
import processing.app.Sketch;
import processing.app.SketchCode;
import processing.app.helpers.PreferencesMap;
import processing.app.helpers.StringReplacer;
import processing.core.PApplet;

public class Compiler implements MessageConsumer {
  static final String BUGS_URL =
    _("http://code.google.com/p/arduino/issues/list");
  static final String SUPER_BADNESS =
    I18n.format(_("Compiler error, please submit this code to {0}"), BUGS_URL);

  private Sketch sketch;

  private String buildPath;
  private String toolsPath;
  private String primaryClassName;
  private String board;
  private List<File> objectFiles;

  private PreferencesMap prefs;
  private boolean verbose;
	
  private RunnerException exception;
  
  /**
   * Compile sketch.
   *
   * @param _sketch Sketch object to be compiled.
   * @param _buildPath Where the temporary files live and will be built from.
   * @param _primaryClassName the name of the combined sketch file w/ extension
   * @return true if successful.
   * @throws RunnerException Only if there's a problem. Only then.
   */
  public boolean compile(Sketch _sketch, String _buildPath,
                         String _primaryClassName, boolean _verbose)
      throws RunnerException {
    sketch = _sketch;
    buildPath = _buildPath;
    primaryClassName = _primaryClassName;
    verbose = _verbose;
    objectFiles = new ArrayList<File>();

    PreferencesMap boardPreferences = Base.getBoardPreferences();

    // Check for null platform, and use system default if not found
    String platform = boardPreferences.get("platform");
    PreferencesMap platformPreferences;
    if (platform == null)
      platformPreferences = Base.getPlatformPreferences();
    else
      platformPreferences = Base.getPlatformPreferences(platform);

    // Merge all the global preference configuration in order of priority
    prefs = new PreferencesMap();
    prefs.putAll(Preferences.getMap());
    prefs.putAll(platformPreferences);
    prefs.putAll(boardPreferences);
    for (String k : prefs.keySet()) {
      if (prefs.get(k) == null)
        prefs.put(k, "");
    }

    toolsPath = prefs.get("compiler.path");
    if (toolsPath == null) {
      toolsPath = Base.getAvrBasePath();
      System.out.println("avrBasePath: " + toolsPath);
    } else {
      System.out.println("avrBasePath:exists: " + toolsPath);

      // Put in the system path in the compiler path if available
      MessageFormat compileFormat = new MessageFormat(toolsPath);
      String basePath = System.getProperty("user.dir");
      if (Base.isMacOS())
        basePath += "/Arduino.app/Contents/Resources/Java";
      Object[] Args = { basePath };
      toolsPath = compileFormat.format(Args);
      System.out.println("avrBasePath:new: " + toolsPath);
    }
    board = prefs.get("board");
    if (board == "")
      board = "_UNKNOWN";

    String core = prefs.get("build.core");
    if (core == null) {
      RunnerException re = new RunnerException(
          _("No board selected; please choose a board from the Tools > Board menu."));
      re.hideStackTrace();
      throw re;
    }

    File coreFolder;
    if (!core.contains(":")) {
      TargetPlatform t = Base.getTarget();
      coreFolder = new File(t.getFolder(), "cores");
      coreFolder = new File(coreFolder, core);
    } else {
    	String[] split = core.split(":", 3);
    	TargetPlatform t = Base.getTargetPlatform(split[0], split[1]);
      coreFolder = new File(t.getFolder(), "cores");
      coreFolder = new File(coreFolder, split[2]);
    }
    String corePath = coreFolder.getAbsolutePath();

    String variant = boardPreferences.get("build.variant");
    String variantPath = null;
    if (variant != null) {
      File variantFolder;
      if (!variant.contains(":")) {
        TargetPlatform t = Base.getTarget();
        variantFolder = new File(t.getFolder(), "variants");
        variantFolder = new File(variantFolder, variant);
      } else {
        String[] split = variant.split(":");
        TargetPlatform t = Base.getTargetPlatform(split[0], split[1]);
        variantFolder = new File(t.getFolder(), "variants");
        variantFolder = new File(variantFolder, split[2]);
      }
      variantPath = variantFolder.getAbsolutePath();
    }

    // 0. include paths for core + all libraries
    sketch.setCompilingProgress(20);
    List<String> includePaths = new ArrayList<String>();
    includePaths.add(corePath);
    if (variantPath != null)
      includePaths.add(variantPath);
    for (File file : sketch.getImportedLibraries())
      includePaths.add(file.getPath());

    // 1. compile the sketch (already in the buildPath)
    sketch.setCompilingProgress(30);
    compileSketch(includePaths);

    // 2. compile the libraries, outputting .o files to: <buildPath>/<library>/
    // Doesn't really use configPreferences
    sketch.setCompilingProgress(40);
    compileLibraries(includePaths);

    // 3. compile the core, outputting .o files to <buildPath> and then
    // collecting them into the core.a library file.
    sketch.setCompilingProgress(50);
    compileCore(corePath, variant, variantPath);

    // 4. link it all together into the .elf file
    sketch.setCompilingProgress(60);
    compileLink(corePath, includePaths);

    // 5. extract EEPROM data (from EEMEM directive) to .eep file.
    sketch.setCompilingProgress(70);
    compileEep(includePaths);

    // 6. build the .hex file
    sketch.setCompilingProgress(80);
    compileHex(includePaths);

    sketch.setCompilingProgress(90);
    return true;
  }

  private List<File> compileFiles(String outputPath, File sourcePath,
                                  boolean recurse, List<String> includePaths)
      throws RunnerException {
    List<File> sSources = findFilesInFolder(sourcePath, "S", recurse);
    List<File> cSources = findFilesInFolder(sourcePath, "c", recurse);
    List<File> cppSources = findFilesInFolder(sourcePath, "cpp", recurse);
    List<File> objectPaths = new ArrayList<File>();

    for (File file : sSources) {
      String objectPath = outputPath + File.separator + file.getName() + ".o";
      objectPaths.add(new File(objectPath));
      String[] cmd = getCommandCompilerS(includePaths, file.getAbsolutePath(),
                                         objectPath);
      execAsynchronously(cmd);
    }
 		
    for (File file : cSources) {
      String objectPath = outputPath + File.separator + file.getName() + ".o";
      String dependPath = outputPath + File.separator + file.getName() + ".d";
      File objectFile = new File(objectPath);
      File dependFile = new File(dependPath);
      objectPaths.add(objectFile);
      if (is_already_compiled(file, objectFile, dependFile, prefs))
        continue;
      String[] cmd = getCommandCompilerC(includePaths, file.getAbsolutePath(),
                                         objectPath);
      execAsynchronously(cmd);
    }

    for (File file : cppSources) {
      String objectPath = outputPath + File.separator + file.getName() + ".o";
      String dependPath = outputPath + File.separator + file.getName() + ".d";
      File objectFile = new File(objectPath);
      File dependFile = new File(dependPath);
      objectPaths.add(objectFile);
      if (is_already_compiled(file, objectFile, dependFile, prefs))
        continue;
      String[] cmd = getCommandCompilerCPP(includePaths,
                                           file.getAbsolutePath(), objectPath);
      execAsynchronously(cmd);
    }
    
    return objectPaths;
  }

  private boolean is_already_compiled(File src, File obj, File dep, Map<String, String> prefs) {
    boolean ret=true;
    try {
      //System.out.println("\n  is_already_compiled: begin checks: " + obj.getPath());
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
            //System.out.println("  is_already_compiled: obj =  " + objpath);
            //System.out.println("  is_already_compiled: line = " + linepath);
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
          //System.out.println("  is_already_compiled:  prerequisite ok");
        }
      }
      reader.close();
    } catch (Exception e) {
      return false;  // any error reading dep file = recompile it
    }
    if (ret && (verbose || Preferences.getBoolean("build.verbose"))) {
      System.out.println("  Using previously compiled: " + obj.getPath());
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
      if (!string.isEmpty())
        stringList.add(string);
    }
    command = stringList.toArray(new String[stringList.size()]);

    int result = 0;

    if (verbose || Preferences.getBoolean("build.verbose")) {
      for (String c : command)
        System.out.print(c + " ");
      System.out.println();
    }

    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;

    Process process;
    
    try {
      process = Runtime.getRuntime().exec(command);
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
        if (in.thread != null)
          in.thread.join();
        if (err.thread != null)
          err.thread.join();
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
              "has been renamed to EthernetClient.\n\n");
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

      RunnerException e = sketch.placeException(error, pieces[1], PApplet.parseInt(pieces[2]) - 1);

      // replace full file path with the name of the sketch tab (unless we're
      // in verbose mode, in which case don't modify the compiler output)
      if (e != null && !verbose) {
        SketchCode code = sketch.getCode(e.getCodeIndex());
        String fileName = code.isExtension(sketch.getDefaultExtension()) ? code.getPrettyName() : code.getFileName();
        s = fileName + ":" + e.getCodeLine() + ": error: " + pieces[3] + msg;        
      }
            
      if (exception == null && e != null) {
        exception = e;
        exception.hideStackTrace();
      }      
    }
    
    System.err.print(s);
  }

  private String[] getCommandCompilerS(List<String> includePaths,
                                       String sourceName, String objectName)
      throws RunnerException {
    String includes = preparePaths(includePaths);
    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("ide_version", "" + Base.REVISION);
    dict.put("includes", includes);
    dict.put("source_file", sourceName);
    dict.put("object_file", objectName);
    dict.put("toolchain_path", toolsPath);

    try {
      String cmd = prefs.get("recipe.S.o.pattern");
      return StringReplacer.formatAndSplit(cmd, dict);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
  }

  private String[] getCommandCompilerC(List<String> includePaths,
                                       String sourceName, String objectName)
      throws RunnerException {
    String includes = preparePaths(includePaths);

    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("ide_version", "" + Base.REVISION);
    dict.put("includes", includes);
    dict.put("source_file", sourceName);
    dict.put("object_file", objectName);
    dict.put("toolchain_path", toolsPath);

    String cmd = prefs.get("recipe.c.o.pattern");
    try {
      return StringReplacer.formatAndSplit(cmd, dict);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
  }

  private String[] getCommandCompilerCPP(List<String> includePaths,
                                         String sourceName, String objectName)
      throws RunnerException {
    String includes = preparePaths(includePaths);

    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("ide_version", "" + Base.REVISION);
    dict.put("includes", includes);
    dict.put("source_file", sourceName);
    dict.put("object_file", objectName);
    dict.put("toolchain_path", toolsPath);

    String cmd = prefs.get("recipe.cpp.o.pattern");
    try {
      return StringReplacer.formatAndSplit(cmd, dict);
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

  /**
   * Given a folder, return a list of the header files in that folder (but not
   * the header files in its sub-folders, as those should be included from
   * within the header files at the top-level).
   */
  static public String[] headerListFromIncludePath(String path) {
    FilenameFilter onlyHFiles = new FilenameFilter() {
      public boolean accept(File dir, String name) {
        return name.endsWith(".h");
      }
    };

    return (new File(path)).list(onlyHFiles);
  }

  static public List<File> findFilesInFolder(File folder, String extension,
                                             boolean recurse) {
    List<File> files = new ArrayList<File>();

    if (folder.listFiles() == null)
      return files;

    for (File file : folder.listFiles()) {
      if (file.getName().startsWith("."))
        continue; // skip hidden files

      if (file.getName().endsWith("." + extension))
        files.add(file);

      if (recurse && file.isDirectory()) {
        files.addAll(findFilesInFolder(file, extension, true));
      }
    }

    return files;
  }
  
  // 1. compile the sketch (already in the buildPath)
  void compileSketch(List<String> includePaths) throws RunnerException {
    objectFiles.addAll(compileFiles(buildPath, new File(buildPath), false,
                                    includePaths));
  }

  // 2. compile the libraries, outputting .o files to:
  // <buildPath>/<library>/
  void compileLibraries(List<String> includePaths) throws RunnerException {

    for (File libraryFolder : sketch.getImportedLibraries()) {
      File outputFolder = new File(buildPath, libraryFolder.getName());
      File utilityFolder = new File(libraryFolder, "utility");
      createFolder(outputFolder);
      // this library can use includes in its utility/ folder
      includePaths.add(utilityFolder.getAbsolutePath());

      objectFiles.addAll(compileFiles(outputFolder.getAbsolutePath(),
                                      libraryFolder, false, includePaths));
      outputFolder = new File(outputFolder, "utility");
      createFolder(outputFolder);
      objectFiles.addAll(compileFiles(outputFolder.getAbsolutePath(),
                                      utilityFolder, false, includePaths));
      // other libraries should not see this library's utility/ folder
      includePaths.remove(includePaths.size() - 1);
    }
  }
	
  // 3. compile the core, outputting .o files to <buildPath> and then
  // collecting them into the core.a library file.
  void compileCore(String corePath, String variant, String variantPath)
      throws RunnerException {

    List<String> includePaths = new ArrayList<String>();
    includePaths.add(corePath); // include core path only
    if (variantPath != null)
      includePaths.add(variantPath);

    List<File> coreObjectFiles = compileFiles(buildPath, new File(corePath),
                                              true, includePaths);

    for (File file : coreObjectFiles) {

      PreferencesMap dict = new PreferencesMap(prefs);
      dict.put("toolchain_path", toolsPath);
      dict.put("ide_version", "" + Base.REVISION);
      dict.put("build_path", buildPath + File.separator);
      dict.put("archive_file", "core.a");
      dict.put("object_file", file.getAbsolutePath());

      String[] cmdArray;
      try {
        String cmd = prefs.get("recipe.ar.pattern");
        cmdArray = StringReplacer.formatAndSplit(cmd, dict);
      } catch (Exception e) {
        throw new RunnerException(e);
      }
      execAsynchronously(cmdArray);
    }
  }
			
  // 4. link it all together into the .elf file
  void compileLink(String corePath, List<String> includePaths)
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
    dict.put("compiler.c.elf.flags", dict
        .get("compiler.c.elf.flags" + optRelax));
    dict.put("toolchain_path", toolsPath);
    dict.put("build_path", buildPath + File.separator);
    dict.put("archive_file", "core.a");
    dict.put("project_name", primaryClassName);
    dict.put("object_files", objectFileList);
    dict.put("ide_version", "" + Base.REVISION);
    dict.put("core_path", corePath);

    String[] cmdArray;
    try {
      String cmd = prefs.get("recipe.c.combine.pattern");
      cmdArray = StringReplacer.formatAndSplit(cmd, dict);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
    execAsynchronously(cmdArray);
  }

  // 5. extract EEPROM data (from EEMEM directive) to .eep file.
  void compileEep(List<String> includePaths) throws RunnerException {
    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("toolchain_path", toolsPath);
    dict.put("build_path", buildPath + File.separator);
    dict.put("project_name", primaryClassName);
    dict.put("ide_version", "" + Base.REVISION);

    String[] cmdArray;
    try {
      String cmd = prefs.get("recipe.objcopy.eep.pattern");
      cmdArray = StringReplacer.formatAndSplit(cmd, dict);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
    execAsynchronously(cmdArray);
  }
	
  // 6. build the .hex file
  void compileHex(List<String> includePaths) throws RunnerException {
    PreferencesMap dict = new PreferencesMap(prefs);
    dict.put("toolchain_path", toolsPath);
    dict.put("build_path", buildPath + File.separator);
    dict.put("project_name", primaryClassName);
    dict.put("ide_version", "" + Base.REVISION);

    String[] cmdArray;
    try {
      String cmd = prefs.get("recipe.objcopy.hex.pattern");
      cmdArray = StringReplacer.formatAndSplit(cmd, dict);
    } catch (Exception e) {
      throw new RunnerException(e);
    }
    execAsynchronously(cmdArray);
  }

  private static String preparePaths(List<String> includePaths) {
    String res = "";
    for (String p : includePaths)
      res += " \"-I" + p + '"';

    // Remove first space
    return res.substring(1);
  }
}
