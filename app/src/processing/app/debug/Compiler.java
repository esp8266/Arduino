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

import processing.app.Base;
import processing.app.Preferences;
import processing.app.Sketch;
import processing.app.SketchCode;
import processing.core.*;
import processing.app.I18n;
import static processing.app.I18n._;

import java.io.*;
import java.util.*;
import java.text.MessageFormat;


public class Compiler implements MessageConsumer {
  static final String BUGS_URL =
    _("http://code.google.com/p/arduino/issues/list");
  static final String SUPER_BADNESS =
    I18n.format(_("Compiler error, please submit this code to {0}"), BUGS_URL);

  Sketch sketch;
  String buildPath;
  String primaryClassName;
  String platform;
  String board;

  boolean verbose;
	
  RunnerException exception;
	
  HashMap<String, String> configPreferences;
  HashMap<String, String> platformPreferences;
	
  String avrBasePath;
  String corePath;
	
  List<File> objectFiles;
  ArrayList<String> includePaths;

  public Compiler() { }

  /**
   * Compile with avr-gcc.
   *
   * @param sketch Sketch object to be compiled.
   * @param buildPath Where the temporary files live and will be built from.
   * @param primaryClassName the name of the combined sketch file w/ extension
   * @return true if successful.
   * @throws RunnerException Only if there's a problem. Only then.
   */
  public boolean compile(Sketch sketch,
                         String buildPath,
                         String primaryClassName,
                         boolean verbose) throws RunnerException {
    this.sketch = sketch;
    this.buildPath = buildPath;
    this.primaryClassName = primaryClassName;
    this.verbose = verbose;
    objectFiles = new ArrayList<File>();

    Map<String, String> boardPreferences = Base.getBoardPreferences();
    
   	//Check for null platform, and use system default if not found
	platform = boardPreferences.get("platform");
	if (platform == null)
	{
	      platformPreferences = new HashMap(Base.getPlatformPreferences());
	}
	else
	{
		platformPreferences = new HashMap(Base.getPlatformPreferences(platform));
	}
   
    System.out.println("////////////////////////////compiler.java is doing stuff/////////////////");
	//Put all the global preference configuration into one Master configpreferences
    configPreferences = mergePreferences( Preferences.getMap(), platformPreferences, boardPreferences);
	avrBasePath = configPreferences.get("compiler.path");
	if (avrBasePath == null) 
	{
		avrBasePath = Base.getAvrBasePath();
		System.out.println("avrBasePath: " + avrBasePath);
	}
	else
	{
	    System.out.println("avrBasePath:exists: " + avrBasePath);

		//Put in the system path in the compiler path if available
		MessageFormat compileFormat = new MessageFormat(avrBasePath);	
		String basePath = System.getProperty("user.dir");
		if (Base.isMacOS()) {
			//logger.debug("basePath: " + basePath);
			basePath += "/Arduino.app/Contents/Resources/Java";
		}
		Object[] Args = {basePath};
		avrBasePath = compileFormat.format(  Args );
	    System.out.println("avrBasePath:new: " + avrBasePath);


	}
	this.board = configPreferences.get("board");
	if (this.board == "")
	{
		this.board = "_UNKNOWN";
	}    
    
	String core = configPreferences.get("build.core");
    if (core == null) {
    	RunnerException re = new RunnerException(_("No board selected; please choose a board from the Tools > Board menu."));
      re.hideStackTrace();
      throw re;
    }
    String corePath;
    
    if (core.indexOf(':') == -1) {
      Target t = Base.getTarget();
      File coreFolder = new File(new File(t.getFolder(), "cores"), core);
      corePath = coreFolder.getAbsolutePath();
    } else {
      Target t = Base.targetsTable.get(core.substring(0, core.indexOf(':')));
      File coreFolder = new File(t.getFolder(), "cores");
      coreFolder = new File(coreFolder, core.substring(core.indexOf(':') + 1));
      corePath = coreFolder.getAbsolutePath();
    }

    String variant = boardPreferences.get("build.variant");
    String variantPath = null;

    if (variant != null) {
      if (variant.indexOf(':') == -1) {
	    Target t = Base.getTarget();
	    File variantFolder = new File(new File(t.getFolder(), "variants"), variant);
	    variantPath = variantFolder.getAbsolutePath();
      } else {
	    Target t = Base.targetsTable.get(variant.substring(0, variant.indexOf(':')));
	    File variantFolder = new File(t.getFolder(), "variants");
	    variantFolder = new File(variantFolder, variant.substring(variant.indexOf(':') + 1));
	    variantPath = variantFolder.getAbsolutePath();
      }
    }

   // 0. include paths for core + all libraries
   sketch.setCompilingProgress(20);
   ArrayList<String> includePaths = new ArrayList<String>();
   includePaths.add(corePath);
   if (variantPath != null) includePaths.add(variantPath);
   for (File file : sketch.getImportedLibraries()) {
     includePaths.add(file.getPath());
   }

   // 1. compile the sketch (already in the buildPath)
   System.out.println("1. compileSketch");
   sketch.setCompilingProgress(30);
   compileSketch(avrBasePath, buildPath, includePaths, configPreferences);

   // 2. compile the libraries, outputting .o files to: <buildPath>/<library>/
   		// 2. compile the libraries, outputting .o files to:
		// <buildPath>/<library>/
		//Doesn't really use configPreferences
	System.out.println("2. compileLibraries");
	sketch.setCompilingProgress(40);
	compileLibraries(avrBasePath, buildPath, includePaths, configPreferences);
/*

   for (File libraryFolder : sketch.getImportedLibraries()) {
     File outputFolder = new File(buildPath, libraryFolder.getName());
     File utilityFolder = new File(libraryFolder, "utility");
     createFolder(outputFolder);
     // this library can use includes in its utility/ folder
     includePaths.add(utilityFolder.getAbsolutePath());
     objectFiles.addAll(
       compileFiles(avrBasePath, outputFolder.getAbsolutePath(), includePaths,
               findFilesInFolder(libraryFolder, "S", false),
               findFilesInFolder(libraryFolder, "c", false),
               findFilesInFolder(libraryFolder, "cpp", false),
               boardPreferences));
     outputFolder = new File(outputFolder, "utility");
     createFolder(outputFolder);
     objectFiles.addAll(
       compileFiles(avrBasePath, outputFolder.getAbsolutePath(), includePaths,
               findFilesInFolder(utilityFolder, "S", false),
               findFilesInFolder(utilityFolder, "c", false),
               findFilesInFolder(utilityFolder, "cpp", false),
               boardPreferences));
     // other libraries should not see this library's utility/ folder
     includePaths.remove(includePaths.size() - 1);
   }
*/

   // 3. compile the core, outputting .o files to <buildPath> and then
   // collecting them into the core.a library file.
   System.out.println("3. compileCore");
   System.out.println("corePath: " + corePath);
   sketch.setCompilingProgress(50);
   compileCore(avrBasePath, buildPath, corePath, variant, variantPath, configPreferences);

   
/*
  includePaths.clear();
  includePaths.add(corePath);  // include path for core only
  if (variantPath != null) includePaths.add(variantPath);
  List<File> coreObjectFiles =
    compileFiles(avrBasePath, buildPath, includePaths,
              findFilesInPath(corePath, "S", true),
              findFilesInPath(corePath, "c", true),
              findFilesInPath(corePath, "cpp", true),
              boardPreferences);

   String runtimeLibraryName = buildPath + File.separator + "core.a";
   List baseCommandAR = new ArrayList(Arrays.asList(new String[] {
     avrBasePath + "avr-ar",
     "rcs",
     runtimeLibraryName
   }));
   for(File file : coreObjectFiles) {
     List commandAR = new ArrayList(baseCommandAR);
     commandAR.add(file.getAbsolutePath());
     execAsynchronously(commandAR);
   }
*/
    // 4. link it all together into the .elf file
    sketch.setCompilingProgress(60);
    System.out.println("4. compileLink");
    compileLink(avrBasePath, buildPath, corePath, includePaths, configPreferences);

/*
    List baseCommandLinker = new ArrayList(Arrays.asList(new String[] {
      avrBasePath + "avr-gcc",
      "-Os",
      "-Wl,--gc-sections"+optRelax,
      "-mmcu=" + boardPreferences.get("build.mcu"),
      "-o",
      buildPath + File.separator + primaryClassName + ".elf"
    }));

    for (File file : objectFiles) {
      baseCommandLinker.add(file.getAbsolutePath());
    }

    baseCommandLinker.add(runtimeLibraryName);
    baseCommandLinker.add("-L" + buildPath);
    baseCommandLinker.add("-lm");

    execAsynchronously(baseCommandLinker);

    List baseCommandObjcopy = new ArrayList(Arrays.asList(new String[] {
      avrBasePath + "avr-objcopy",
      "-O",
      "-R",
    }));
    
    List commandObjcopy;
*/

    // 5. extract EEPROM data (from EEMEM directive) to .eep file.
    sketch.setCompilingProgress(70);
/*
    commandObjcopy = new ArrayList(baseCommandObjcopy);
    commandObjcopy.add(2, "ihex");
    commandObjcopy.set(3, "-j");
    commandObjcopy.add(".eeprom");
    commandObjcopy.add("--set-section-flags=.eeprom=alloc,load");
    commandObjcopy.add("--no-change-warnings");
    commandObjcopy.add("--change-section-lma");
    commandObjcopy.add(".eeprom=0");
    commandObjcopy.add(buildPath + File.separator + primaryClassName + ".elf");
    commandObjcopy.add(buildPath + File.separator + primaryClassName + ".eep");
    execAsynchronously(commandObjcopy);
*/
    System.out.println("5. compileEep");
    compileEep(avrBasePath, buildPath, includePaths, configPreferences);
    
    // 6. build the .hex file
    sketch.setCompilingProgress(80);
/*
    commandObjcopy = new ArrayList(baseCommandObjcopy);
    commandObjcopy.add(2, "ihex");
    commandObjcopy.add(".eeprom"); // remove eeprom data
    commandObjcopy.add(buildPath + File.separator + primaryClassName + ".elf");
    commandObjcopy.add(buildPath + File.separator + primaryClassName + ".hex");
    execAsynchronously(commandObjcopy);
*/
    System.out.println("6. compileHex");
    compileHex(avrBasePath, buildPath, includePaths, configPreferences);
    
    sketch.setCompilingProgress(90);
    return true;
  }


  private List<File> compileFiles(String avrBasePath,
                                  String buildPath, ArrayList<String> includePaths,
                                  ArrayList<File> sSources, 
                                  ArrayList<File> cSources, ArrayList<File> cppSources,
                                  Map<String, String> boardPreferences)
    throws RunnerException {

    List<File> objectPaths = new ArrayList<File>();
    
    for (File file : sSources) {
      String objectPath = buildPath + File.separator + file.getName() + ".o";
      objectPaths.add(new File(objectPath));
      execAsynchronously(getCommandCompilerS(avrBasePath, includePaths,
                                             file.getAbsolutePath(),
                                             objectPath,
                                             configPreferences));
    }
 		
    for (File file : cSources) {
        String objectPath = buildPath + File.separator + file.getName() + ".o";
        String dependPath = buildPath + File.separator + file.getName() + ".d";
        File objectFile = new File(objectPath);
        File dependFile = new File(dependPath);
        objectPaths.add(objectFile);
        if (is_already_compiled(file, objectFile, dependFile, boardPreferences)) continue;
        execAsynchronously(getCommandCompilerC(avrBasePath, includePaths,
                                               file.getAbsolutePath(),
                                               objectPath,
                                               configPreferences));
    }

    for (File file : cppSources) {
        String objectPath = buildPath + File.separator + file.getName() + ".o";
        String dependPath = buildPath + File.separator + file.getName() + ".d";
        File objectFile = new File(objectPath);
        File dependFile = new File(dependPath);
        objectPaths.add(objectFile);
        if (is_already_compiled(file, objectFile, dependFile, boardPreferences)) continue;
        execAsynchronously(getCommandCompilerCPP(avrBasePath, includePaths,
                                                 file.getAbsolutePath(),
                                                 objectPath,
                                                 configPreferences));
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
      
    //eliminate any empty array entries
    List<String> stringList = new ArrayList<String>();
    for(String string : command) {
  	 string = string.trim();
     if(string != null && string.length() > 0) {
       stringList.add(string);
    }
   }
   command = stringList.toArray(new String[stringList.size()]);
   
    int result = 0;
    
    if (verbose || Preferences.getBoolean("build.verbose")) {
      for(int j = 0; j < command.length; j++) {
        System.out.print(command[j] + " ");
      }
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
    if (exception != null) { throw exception; }

    if (result > 1) {
      // a failure in the tool (e.g. unable to locate a sub-executable)
      System.err.println(
	  I18n.format(_("{0} returned {1}"), command[0], result));
    }

    if (result != 0) {
      RunnerException re = new RunnerException(_("Error compiling."));
      re.hideStackTrace();
      throw re;
    }
    System.out.println("execAsync: Done.");
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

  /////////////////////////////////////////////////////////////////////////////
/*
  static private List getCommandCompilerS(String avrBasePath, List includePaths,
    String sourceName, String objectName, Map<String, String> boardPreferences) {
    List baseCommandCompiler = new ArrayList(Arrays.asList(new String[] {
      avrBasePath + "avr-gcc",
      "-c", // compile, don't link
      "-g", // include debugging info (so errors include line numbers)
      "-assembler-with-cpp",
      "-mmcu=" + boardPreferences.get("build.mcu"),
      "-DF_CPU=" + boardPreferences.get("build.f_cpu"),
      "-DARDUINO=" + Base.REVISION,
    }));

    for (int i = 0; i < includePaths.size(); i++) {
      baseCommandCompiler.add("-I" + (String) includePaths.get(i));
    }

    baseCommandCompiler.add(sourceName);
    baseCommandCompiler.add("-o"+ objectName);

    return baseCommandCompiler;
  }
*/

	// ///////////////////////////////////////////////////////////////////////////
	static private String[] getCommandCompilerS(String avrBasePath,
			ArrayList<String> includePaths, String sourceName, String objectName,
			HashMap<String, String> configPreferences) 
			{
		System.out.println("getCommandCompilerS: start");	
		String baseCommandString = configPreferences.get("recipe.cpp.o.pattern");
		MessageFormat compileFormat = new MessageFormat(baseCommandString);	
		//getIncludes to String
		
		String includes = preparePaths(includePaths);
		Object[] Args = {
				avrBasePath,
				configPreferences.get("compiler.cpp.cmd"),
				configPreferences.get("compiler.S.flags"),
				configPreferences.get("compiler.cpudef"),
				configPreferences.get("build.mcu"),				
				configPreferences.get("build.f_cpu"),
				configPreferences.get("software"),
				Base.REVISION,
				includes,
				sourceName,
				objectName
		};
				
		String command = compileFormat.format(  Args );	
		String[] commandArray = command.split("\\|");	
		return commandArray;
	}

/*
  
  static private List getCommandCompilerC(String avrBasePath, List includePaths,
    String sourceName, String objectName, Map<String, String> boardPreferences) {

    List baseCommandCompiler = new ArrayList(Arrays.asList(new String[] {
      avrBasePath + "avr-gcc",
      "-c", // compile, don't link
      "-g", // include debugging info (so errors include line numbers)
      "-Os", // optimize for size
      Preferences.getBoolean("build.verbose") ? "-Wall" : "-w", // show warnings if verbose
      "-ffunction-sections", // place each function in its own section
      "-fdata-sections",
      "-mmcu=" + boardPreferences.get("build.mcu"),
      "-DF_CPU=" + boardPreferences.get("build.f_cpu"),
      "-MMD", // output dependancy info
      "-DARDUINO=" + Base.REVISION,
    }));
		
    for (int i = 0; i < includePaths.size(); i++) {
      baseCommandCompiler.add("-I" + (String) includePaths.get(i));
    }

    baseCommandCompiler.add(sourceName);
    baseCommandCompiler.add("-o");
    baseCommandCompiler.add(objectName);

    return baseCommandCompiler;
  }
	*/
	
	//removed static
	private String[] getCommandCompilerC(String avrBasePath,
			ArrayList<String> includePaths, String sourceName, String objectName,
			HashMap<String, String> configPreferences) 
			{
		System.out.println("getCommandCompilerC: start");	
		String baseCommandString = configPreferences.get("recipe.c.o.pattern");
		MessageFormat compileFormat = new MessageFormat(baseCommandString);	
		//getIncludes to String
		String includes = preparePaths(includePaths);

		Object[] Args = {
				avrBasePath,
				configPreferences.get("compiler.c.cmd"),
				configPreferences.get("compiler.c.flags"),
				configPreferences.get("compiler.cpudef"),
				configPreferences.get("build.mcu"),				
				configPreferences.get("build.f_cpu"),
				configPreferences.get("software"),
				Base.REVISION,
				includes,
				sourceName,
				objectName
		};
						
		String command = compileFormat.format(  Args );	
		String[] commandArray = command.split("\\|");	
		return commandArray;	
	}
/*
	
  static private List getCommandCompilerCPP(String avrBasePath,
    List includePaths, String sourceName, String objectName,
    Map<String, String> boardPreferences) {
    
    List baseCommandCompilerCPP = new ArrayList(Arrays.asList(new String[] {
      avrBasePath + "avr-g++",
      "-c", // compile, don't link
      "-g", // include debugging info (so errors include line numbers)
      "-Os", // optimize for size
      Preferences.getBoolean("build.verbose") ? "-Wall" : "-w", // show warnings if verbose
      "-fno-exceptions",
      "-ffunction-sections", // place each function in its own section
      "-fdata-sections",
      "-mmcu=" + boardPreferences.get("build.mcu"),
      "-DF_CPU=" + boardPreferences.get("build.f_cpu"),
      "-MMD", // output dependancy info
      "-DARDUINO=" + Base.REVISION,
    }));

    for (int i = 0; i < includePaths.size(); i++) {
      baseCommandCompilerCPP.add("-I" + (String) includePaths.get(i));
    }

    baseCommandCompilerCPP.add(sourceName);
    baseCommandCompilerCPP.add("-o");
    baseCommandCompilerCPP.add(objectName);

    return baseCommandCompilerCPP;
  }
*/

	static private String[] getCommandCompilerCPP(String avrBasePath,
			ArrayList<String> includePaths, String sourceName, String objectName,
			HashMap<String, String> configPreferences) 
			{
		System.out.println("getCommandCompilerCPP: start");	
		String baseCommandString = configPreferences.get("recipe.cpp.o.pattern");
		MessageFormat compileFormat = new MessageFormat(baseCommandString);	
		//getIncludes to String
		String includes = preparePaths(includePaths);

		Object[] Args = {
				avrBasePath,
				configPreferences.get("compiler.cpp.cmd"),
				configPreferences.get("compiler.cpp.flags"),
				configPreferences.get("compiler.cpudef"),
				configPreferences.get("build.mcu"),				
				configPreferences.get("build.f_cpu"),
				configPreferences.get("software"),
				Base.REVISION,
				includes,
				sourceName,
				objectName
		};
						
		String command = compileFormat.format(  Args );	
		String[] commandArray = command.split("\\|");	

		/*
		System.out.println("command:" + command);
		for (int ii = 0; ii < commandArray.length; ii++)
		{
			System.out.println("'" + commandArray[ii] + "'");
		} 
		*/
		return commandArray;	
	}


  /////////////////////////////////////////////////////////////////////////////

  static private void createFolder(File folder) throws RunnerException {
    if (folder.isDirectory()) return;
    if (!folder.mkdir())
      throw new RunnerException("Couldn't create: " + folder);
  }

  /**
   * Given a folder, return a list of the header files in that folder (but
   * not the header files in its sub-folders, as those should be included from
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
  
  static public ArrayList<File> findFilesInPath(String path, String extension,
                                                boolean recurse) {
                                                	System.out.println("findFilesInPath: " + path);
    return findFilesInFolder(new File(path), extension, recurse);
  }
  
  static public ArrayList<File> findFilesInFolder(File folder, String extension,
                                                  boolean recurse) {
    ArrayList<File> files = new ArrayList<File>();
    
    if (folder.listFiles() == null) return files;
    
    for (File file : folder.listFiles()) {
      if (file.getName().startsWith(".")) continue; // skip hidden files
      
      if (file.getName().endsWith("." + extension))
        files.add(file);
        
      if (recurse && file.isDirectory()) {
        files.addAll(findFilesInFolder(file, extension, true));
      }
    }
    
    return files;
  }
  
  
  	// 1. compile the sketch (already in the buildPath)
	void compileSketch(String avrBasePath, String buildPath, ArrayList<String> includePaths, HashMap<String, String> configPreferences)
	throws RunnerException 
	{
		System.out.println("compileSketch: start");        
		System.out.println("includePaths: ");
		    for (int i = 0; i < includePaths.size(); i++) {
     			System.out.println("-I" + (String) includePaths.get(i));
    		}
    		
		//logger.debug("compileSketch: start");	
		this.objectFiles.addAll(compileFiles(avrBasePath, buildPath, includePaths,
				findFilesInPath(buildPath, "S", false),
				findFilesInPath(buildPath, "c", false),
				findFilesInPath(buildPath, "cpp", false), 
				configPreferences));
	}
	
	// 2. compile the libraries, outputting .o files to:
	// <buildPath>/<library>/
	void compileLibraries (String avrBasePath, String buildPath,
			ArrayList<String> includePaths, 
			HashMap<String, String> configPreferences) 
		throws RunnerException 
	{
	System.out.println("compileLibraries: start");
	    
   for (File libraryFolder : sketch.getImportedLibraries()) {
   		System.out.println("libraryFolder: " + libraryFolder);
     File outputFolder = new File(buildPath, libraryFolder.getName());
     File utilityFolder = new File(libraryFolder, "utility");
     createFolder(outputFolder);
     // this library can use includes in its utility/ folder
     includePaths.add(utilityFolder.getAbsolutePath());
     //debug  includePaths
     System.out.println("includePaths: ");
     for (int i = 0; i < includePaths.size(); i++) {
     	System.out.println("-I" + (String) includePaths.get(i));
    }

     
     objectFiles.addAll(
       compileFiles(avrBasePath, outputFolder.getAbsolutePath(), includePaths,
               findFilesInFolder(libraryFolder, "S", false),
               findFilesInFolder(libraryFolder, "c", false),
               findFilesInFolder(libraryFolder, "cpp", false),
               configPreferences));
     outputFolder = new File(outputFolder, "utility");
     createFolder(outputFolder);
     objectFiles.addAll(
       compileFiles(avrBasePath, outputFolder.getAbsolutePath(), includePaths,
               findFilesInFolder(utilityFolder, "S", false),
               findFilesInFolder(utilityFolder, "c", false),
               findFilesInFolder(utilityFolder, "cpp", false),
               configPreferences));
     // other libraries should not see this library's utility/ folder
     includePaths.remove(includePaths.size() - 1);
   }
  }
	
	// 3. compile the core, outputting .o files to <buildPath> and then
	// collecting them into the core.a library file.
	void compileCore (String avrBasePath, String buildPath, 
			String corePath, String variant, String variantPath, 
			HashMap<String, String> configPreferences) 
		throws RunnerException 
	{
		System.out.println("compileCore(...) start");

		ArrayList<String>  includePaths =  new ArrayList();
	    includePaths.add(corePath); //include core path only
        if (variantPath != null) includePaths.add(variantPath);
        
         //debug  includePaths
        System.out.println("includePaths: ");
     	for (int i = 0; i < includePaths.size(); i++) {
     		System.out.println("-I" + (String) includePaths.get(i));
    	}

		String baseCommandString = configPreferences.get("recipe.ar.pattern");
		String commandString = "";
		MessageFormat compileFormat = new MessageFormat(baseCommandString);	
		System.out.println("corePath: " + corePath);		
		List<File> coreObjectFiles	 = compileFiles(
				avrBasePath, 
				buildPath,
				includePaths, 
				findFilesInPath(corePath, "S", true),
				findFilesInPath(corePath, "c", true),
				findFilesInPath(corePath, "cpp", true), 
				configPreferences);
		
		for (File file : coreObjectFiles) {
			//List commandAR = new ArrayList(baseCommandAR);
			//commandAR = commandAR +  file.getAbsolutePath();
		
			Object[] Args = {
				avrBasePath,
				configPreferences.get("compiler.ar.cmd"),
				configPreferences.get("compiler.ar.flags"),
				//corePath,
				buildPath + File.separator,
				"core.a",
				//objectName
				file.getAbsolutePath()
			};
		System.out.println("compileCore(...) substitute");

			commandString = compileFormat.format(  Args );
		    String[] commandArray = commandString.split("\\|");	
			execAsynchronously(commandArray);
			
			
		}
	}
			
	// 4. link it all together into the .elf file
	void compileLink(String avrBasePath, String buildPath, 
			String corePath, ArrayList<String> includePaths, 
			HashMap<String, String> configPreferences) 
		throws RunnerException 
	{	
	    // For atmega2560, need --relax linker option to link larger
	    // programs correctly.
	    String optRelax = "";
	    if (configPreferences.get("build.mcu").equals("atmega2560"))
	      optRelax = ",--relax";
	    
		System.out.println("compileLink: start");
		String baseCommandString = configPreferences.get("recipe.c.combine.pattern");
		String commandString = "";
		MessageFormat compileFormat = new MessageFormat(baseCommandString);	
		String objectFileList = "";
		
		for (File file : objectFiles) {
			objectFileList = objectFileList + file.getAbsolutePath() + "|";
		}
		System.out.println("objectFileList: " + objectFileList);

			Object[] Args = {
				avrBasePath,
				configPreferences.get("compiler.c.elf.cmd"),
				configPreferences.get("compiler.c.elf.flags")+optRelax,
				configPreferences.get("compiler.cpudef"),
				configPreferences.get("build.mcu"),				
				buildPath + File.separator,
				primaryClassName,
				objectFileList,
				buildPath + File.separator + "core.a",
				buildPath,
				corePath,	
				configPreferences.get("ldscript"),	
			};
			commandString = compileFormat.format(  Args );
		    String[] commandArray = commandString.split("\\|");	
			execAsynchronously(commandArray);
	}

	// 5. extract EEPROM data (from EEMEM directive) to .eep file.
	void compileEep (String avrBasePath, String buildPath, ArrayList<String> includePaths, HashMap<String, String> configPreferences) 
		throws RunnerException 
	{
		//logger.debug("compileEep: start");
		String baseCommandString = configPreferences.get("recipe.objcopy.eep.pattern");
		String commandString = "";
		MessageFormat compileFormat = new MessageFormat(baseCommandString);	
		String objectFileList = "";
		
		Object[] Args = {
			avrBasePath,
			configPreferences.get("compiler.objcopy.cmd"),
			configPreferences.get("compiler.objcopy.eep.flags"),
			buildPath + File.separator + primaryClassName,
			buildPath + File.separator + primaryClassName
			};
		commandString = compileFormat.format(  Args );		
		String[] commandArray = commandString.split("\\|");	
	    execAsynchronously(commandArray);
	}
	
	// 6. build the .hex file
	void compileHex (String avrBasePath, String buildPath, ArrayList<String> includePaths, HashMap<String, String> configPreferences) 
		throws RunnerException 
	{
		//logger.debug("compileHex: start");
		String baseCommandString = configPreferences.get("recipe.objcopy.hex.pattern");
		String commandString = "";
		MessageFormat compileFormat = new MessageFormat(baseCommandString);	
		String objectFileList = "";
	
		Object[] Args = {
			avrBasePath,
			configPreferences.get("compiler.elf2hex.cmd"),
			configPreferences.get("compiler.elf2hex.flags"),
			buildPath + File.separator + primaryClassName,
			buildPath + File.separator + primaryClassName
			};
		commandString = compileFormat.format(  Args );						
		String[] commandArray = commandString.split("\\|");	
		execAsynchronously(commandArray);	
	}
  
  	//merge all the preferences file in the correct order of precedence
	HashMap mergePreferences(Map Preferences,  Map platformPreferences, Map boardPreferences)
	{
		HashMap _map = new HashMap();
		
	    Iterator iterator = Preferences.entrySet().iterator();
       
        while(iterator.hasNext())
  	    {
  	    	Map.Entry pair = (Map.Entry)iterator.next();
  	    	if (pair.getValue() == null)
  	    	{
  	    		_map.put(pair.getKey(), "");
  	    	}
  	    	else
  	    	{
  	    		_map.put(pair.getKey(), pair.getValue());
  	    	}
	    }
	    
		//logger.debug("Done: Preferences");
		
		iterator = platformPreferences.entrySet().iterator();
       
       while(iterator.hasNext())
  	    {
  	    	Map.Entry pair = (Map.Entry)iterator.next();
  	    	
  	    	if (pair.getValue() == null)
  	    	{
  	    		_map.put(pair.getKey(), "");
  	    	}
  	    	else
  	    	{
  	    		_map.put(pair.getKey(), pair.getValue());
  	    	}
            //System.out.println(pair.getKey() + " = " + pair.getValue());
	    }

		//System.out.println("Done: platformPreferences");
		iterator = boardPreferences.entrySet().iterator();

        while(iterator.hasNext())
  	    {
  	    	Map.Entry pair = (Map.Entry)iterator.next();
  	    	
  	    	if (pair.getValue() == null)
  	    	{
  	    		_map.put(pair.getKey(), "");
  	    	}
  	    	else
  	    	{
  	    		_map.put(pair.getKey(), pair.getValue());
  	    	}
            //System.out.println(pair.getKey() + " = " + pair.getValue());
	    }
		//System.out.println("Done: boardPreferences");
        

	return _map;
	}
	
	private static String preparePaths(ArrayList<String> includePaths) {
	//getIncludes to String
		//logger.debug("Start: Prepare paths");
		String includes = "";
		for (int i = 0; i < includePaths.size(); i++) 
		{
			includes = includes + (" -I" + (String) includePaths.get(i)) + "|";
		}
		//logger.debug("Paths prepared: " + includes);
		return includes;
	}
	
  
}
