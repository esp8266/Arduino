/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Compiler - default compiler class that connects to avr-gcc 
  Part of the Arduino project - http://www.arduino.cc/

  Modified by David A. Mellis

  Copyright (c) 2004-05 Hernando Barragan

  Processing version
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
  
  $Id$
*/

package processing.app;

import java.io.*;
import java.util.*;
import java.util.zip.*;
import javax.swing.*;

public class Compiler implements MessageConsumer {
  static final String BUGS_URL =
    "https://developer.berlios.de/bugs/?group_id=3590";
  static final String SUPER_BADNESS =
    "Compiler error, please submit this code to " + BUGS_URL;

  Sketch sketch;
  String buildPath;

  //String buildPath;
  //String className;
  //File includeFolder;
  RunnerException exception;
  //Editor editor;

  /*
  public Compiler(String buildPath, String className,
                     File includeFolder, Editor editor) {
    this.buildPath = buildPath;
    this.includeFolder = includeFolder;
    this.className = className;
    this.editor = editor;
  }


  public boolean compile(PrintStream leechErr) {
  */

  public Compiler() { }  // consider this a warning, you werkin soon.


  public boolean compile(Sketch sketch, String buildPath, Target target)
    throws RunnerException {

    this.sketch = sketch;
    this.buildPath = buildPath;

    // the pms object isn't used for anything but storage
    MessageStream pms = new MessageStream(this);

    String userdir = System.getProperty("user.dir") + File.separator;
    
//    LibraryManager libraryManager;
//
//    try {
//      libraryManager = new LibraryManager();
//    } catch (IOException e) {
//      throw new RunnerException(e.getMessage());
//    }
    String avrBasePath;
    if(Base.isMacOS()) {
    	avrBasePath = new String("hardware/tools/avr/bin/"); 
    }
    else if(Base.isLinux()) {
    	avrBasePath = new String("");     	
    }
    else {
    	avrBasePath = new String(userdir + "hardware/tools/avr/bin/"); 
    }
    
    List includePaths = new ArrayList();
    includePaths.add(target.getPath());
    // use lib directories as include paths
    for (int i = 0; i < sketch.importedLibraries.size(); i++) {
      includePaths.add(
        ((Library) sketch.importedLibraries.get(i)).getFolder().getPath());
    }
    
    List baseCommandLinker = new ArrayList(Arrays.asList(new String[] {
      avrBasePath + "avr-gcc",
      "-Os",
      "-Wl,--gc-sections",
      "-mmcu=" + Preferences.get("boards." + Preferences.get("board") + ".build.mcu"),
      "-o",
      buildPath + File.separator + sketch.name + ".elf"
    }));
    
    String runtimeLibraryName = buildPath + File.separator + "core.a";

    List baseCommandAR = new ArrayList(Arrays.asList(new String[] {
      avrBasePath + "avr-ar",
      "rcs",
      runtimeLibraryName
    }));

    // use lib object files
    for (Iterator i = sketch.importedLibraries.iterator(); i.hasNext(); ) {
      Library library = (Library) i.next();
      File[] objectFiles = library.getObjectFiles();
      for (int j = 0; j < objectFiles.length; j++)
        baseCommandLinker.add(objectFiles[j].getPath());
    }
    
    List baseCommandObjcopy = new ArrayList(Arrays.asList(new String[] {
      avrBasePath + "avr-objcopy",
      "-O",
      "-R",
    }));

    // make list of code files that need to be compiled and the object files
    // that they will be compiled to (includes code from the sketch and the
    // library for the target platform)
    List sourceNames = new ArrayList();
    List sourceNamesCPP = new ArrayList();
    List objectNames = new ArrayList();
    List objectNamesCPP = new ArrayList();
    List targetObjectNames = new ArrayList();
    List sketchObjectNames = new ArrayList();
    for (int i = 0; i < sketch.codeCount; i++) {
      if (sketch.code[i].preprocName != null) {
        if (sketch.code[i].preprocName.endsWith(".c")) {
          sourceNames.add(buildPath + File.separator + sketch.code[i].preprocName);
          objectNames.add(buildPath + File.separator + sketch.code[i].preprocName + ".o");
          sketchObjectNames.add(buildPath + File.separator + sketch.code[i].preprocName + ".o");
        } else if (sketch.code[i].preprocName.endsWith(".cpp")) {
          sourceNamesCPP.add(buildPath + File.separator + sketch.code[i].preprocName);
          objectNamesCPP.add(buildPath + File.separator + sketch.code[i].preprocName + ".o");
          sketchObjectNames.add(buildPath + File.separator + sketch.code[i].preprocName + ".o");
        } 
      }
    }
    for (Iterator iter = target.getSourceFilenames().iterator(); iter.hasNext(); ) {
      String filename = (String) iter.next();
      if (filename != null) {
        targetObjectNames.add(buildPath + File.separator + filename + ".o");
        if (filename.endsWith(".c")) {
          sourceNames.add(target.getPath() + File.separator + filename);
          objectNames.add(buildPath + File.separator + filename + ".o");
        } else if (filename.endsWith(".cpp")) {
          sourceNamesCPP.add(target.getPath() + File.separator + filename);
          objectNamesCPP.add(buildPath + File.separator + filename + ".o");
        } 
      }
    }
    
    baseCommandLinker.addAll(sketchObjectNames);
    //baseCommandLinker.addAll(targetObjectNames);
    baseCommandLinker.add(runtimeLibraryName);
    baseCommandLinker.add("-L" + buildPath);
    baseCommandLinker.add("-lm");

    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;

    int result = 0; // pre-initialized to quiet a bogus warning from jikes
    try {
      // execute the compiler, and create threads to deal
      // with the input and error streams
      //

      Process process;
      boolean compiling = true;
      for(int i = 0; i < sourceNames.size(); i++) {
        if (execAsynchronously(getCommandCompilerC(avrBasePath, includePaths,
          (String) sourceNames.get(i), (String) objectNames.get(i))) != 0)
          return false;
      }

      for(int i = 0; i < sourceNamesCPP.size(); i++) {
        if (execAsynchronously(getCommandCompilerCPP(avrBasePath, includePaths,
          (String) sourceNamesCPP.get(i), (String) objectNamesCPP.get(i))) != 0)
          return false;
      }

      for(int i = 0; i < targetObjectNames.size(); i++) {
		List commandAR = new ArrayList(baseCommandAR);
        commandAR.add(targetObjectNames.get(i));
        if (execAsynchronously(commandAR) != 0)
          return false;
      }

      if (execAsynchronously(baseCommandLinker) != 0)
        return false;

      List commandObjcopy;

      // Extract EEPROM data (from EEMEM directive) to .eep file.
      commandObjcopy = new ArrayList(baseCommandObjcopy);
      commandObjcopy.add(2, "ihex");
      commandObjcopy.set(3, "-j");
      commandObjcopy.add(".eeprom");
      commandObjcopy.add("--set-section-flags=.eeprom=alloc,load");
      commandObjcopy.add("--no-change-warnings");
      commandObjcopy.add("--change-section-lma");
      commandObjcopy.add(".eeprom=0");
      commandObjcopy.add(buildPath + File.separator + sketch.name + ".elf");
      commandObjcopy.add(buildPath + File.separator + sketch.name + ".eep");
      if (execAsynchronously(commandObjcopy) != 0)
        return false;

      commandObjcopy = new ArrayList(baseCommandObjcopy);
      commandObjcopy.add(2, "ihex");
      commandObjcopy.add(".eeprom"); // remove eeprom data
      commandObjcopy.add(buildPath + File.separator + sketch.name + ".elf");
      commandObjcopy.add(buildPath + File.separator + sketch.name + ".hex");
      if (execAsynchronously(commandObjcopy) != 0)
        return false;
    } catch (Exception e) {
      String msg = e.getMessage();
      if ((msg != null) && (msg.indexOf("avr-gcc: not found") != -1)) {
        //System.err.println("jikes is missing");
        Base.showWarning("Compiler error",
                            "Could not find the compiler.\n" +
                            "avr-gcc is missing from your PATH.", null);
        return false;

      } else {
        e.printStackTrace();
        result = -1;
      }
    }

    // an error was queued up by message(), barf this back to build()
    // which will barf it back to Editor. if you're having trouble
    // discerning the imagery, consider how cows regurgitate their food
    // to digest it, and the fact that they have five stomaches.
    //
    //System.out.println("throwing up " + exception);
    if (exception != null) throw exception;

    // if the result isn't a known, expected value it means that something
    // is fairly wrong, one possibility is that jikes has crashed.
    //
    if (result != 0 && result != 1 ) {
      //exception = new RunnerException(SUPER_BADNESS);
      //editor.error(exception);  // this will instead be thrown
      Base.openURL(BUGS_URL);
      throw new RunnerException(SUPER_BADNESS);
    }

    // success would mean that 'result' is set to zero
    return (result == 0); // ? true : false;
  }
  
  public int execAsynchronously(List commandList)
    throws RunnerException, IOException {
    String[] command = new String[commandList.size()];
    commandList.toArray(command);
    int result = 0;
    
    if (Preferences.getBoolean("build.verbose")) {
      for(int j = 0; j < command.length; j++) {
        System.out.print(command[j] + " ");
      }
      System.out.println();
    }

    Process process = Runtime.getRuntime().exec(command);
    
    new MessageSiphon(process.getInputStream(), this);
    new MessageSiphon(process.getErrorStream(), this);

    // wait for the process to finish.  if interrupted
    // before waitFor returns, continue waiting
    boolean compiling = true;
    while (compiling) {
      try {
        result = process.waitFor();
        //System.out.println("result is " + result);
        compiling = false;
      } catch (InterruptedException ignored) { }
    }
    
    if (exception != null)  {
      exception.hideStackTrace = true;
      throw exception;
    }
    
    return result;
  }


  boolean firstErrorFound;
  boolean secondErrorFound;

  /**
   * Part of the MessageConsumer interface, this is called
   * whenever a piece (usually a line) of error message is spewed
   * out from the compiler. The errors are parsed for their contents
   * and line number, which is then reported back to Editor.
   */
  public void message(String s) {
    // This receives messages as full lines, so a newline needs
    // to be added as they're printed to the console.
    //System.err.print(s);

    // ignore cautions
    if (s.indexOf("warning") != -1) return;

    // jikes always uses a forward slash character as its separator,
    // so replace any platform-specific separator characters before
    // attemping to compare
    //
    //String buildPathSubst = buildPath.replace(File.separatorChar, '/') + "/";
    String buildPathSubst = buildPath.replace(File.separatorChar,File.separatorChar) + File.separatorChar;

    String partialTempPath = null;
    int partialStartIndex = -1; //s.indexOf(partialTempPath);
    int fileIndex = -1;  // use this to build a better exception

    // iterate through the project files to see who's causing the trouble
    for (int i = 0; i < sketch.codeCount; i++) {
      if (sketch.code[i].preprocName == null) continue;

      partialTempPath = buildPathSubst + sketch.code[i].preprocName;
      partialStartIndex = s.indexOf(partialTempPath);
      if (partialStartIndex != -1) {
        fileIndex = i;
        //System.out.println("fileIndex is " + fileIndex);
        break;
      }
    }
    //+ className + ".java";

    // if the partial temp path appears in the error message...
    //
    //int partialStartIndex = s.indexOf(partialTempPath);
    if (partialStartIndex != -1) {

      // skip past the path and parse the int after the first colon
      //
      String s1 = s.substring(partialStartIndex +
                              partialTempPath.length() + 1);
      //System.out.println(s1);
      int colon = s1.indexOf(':');

      if (s1.indexOf("In function") != -1 || colon == -1) {
        System.err.print(s1);
        //firstErrorFound = true;
        return;
      }

      int lineNumber;
      try {
        lineNumber = Integer.parseInt(s1.substring(0, colon));
      } catch (NumberFormatException e) {
        System.err.print(s1);
        return;
      }
      
      // the "1" corresponds to the amount of lines written to the main code
      // file by PdePreprocessor's writeHeader() routine before prototypes
      if (fileIndex == 0)
        lineNumber -= 1;
        
      //System.out.println("pde / line number: " + lineNumber);
      
      if (fileIndex == 0) {  // main class, figure out which tab
        for (int i = 1; i < sketch.codeCount; i++) {
          if (sketch.code[i].flavor == Sketch.PDE) {
            //System.out.println("preprocOffset "+ sketch.code[i].preprocOffset);
            if (sketch.code[i].preprocOffset < lineNumber) {
              fileIndex = i;
              //System.out.println("i'm thinkin file " + i);
            }
          }
        }
        if (fileIndex != 0) {  // if found another culprit
          lineNumber -= sketch.code[fileIndex].preprocOffset;
          //System.out.println("i'm sayin line " + lineNumber);
        }
      }

      //String s2 = s1.substring(colon + 2);
      int err = s1.indexOf(":");
      if (err != -1) {

        // if the first error has already been found, then this must be
        // (at least) the second error found
        if (firstErrorFound) {
          secondErrorFound = true;
          return;
        }

        // if executing at this point, this is *at least* the first error
        firstErrorFound = true;

        err += ":".length();
        String description = s1.substring(err);
        description = description.trim();
        System.err.print(description);

/*        String hasLoop = "The method \"void loop();\" with default access";
        if (description.indexOf(hasLoop) != -1) {
          description =
            "Rename loop() to draw() in Processing 0070 and higher";
        }
*/
/*        String constructorProblem =
          "No applicable overload was found for a constructor of type";
        if (description.indexOf(constructorProblem) != -1) {
          //"simong.particles.ParticleSystem". Perhaps you wanted the overloaded version "ParticleSystem();" instead?
          int nextSentence = description.indexOf("\".") + 3;
          description = description.substring(nextSentence);
        }
*/
/*        String overloadProblem = "No applicable overload";
        if (description.indexOf(overloadProblem) != -1) {
          int nextSentence = description.indexOf("\".") + 3;
          description = description.substring(nextSentence);
        }
*/
        // c:/fry/processing/build/windows/work/lib/build/Temporary_6858_2476.java:1:34:1:41: Semantic Error: You need to modify your classpath, sourcepath, bootclasspath, and/or extdirs setup. Package "poo/shoe" could not be found in:
/*        String classpathProblem = "You need to modify your classpath";
        if (description.indexOf(classpathProblem) != -1) {
          if (description.indexOf("quicktime/std") != -1) {
            // special case for the quicktime libraries
            description =
              "To run sketches that use the Processing video library, " +
              "you must first install QuickTime for Java.";

          } else {
            int nextSentence = description.indexOf(". Package") + 2;
            description =
              description.substring(nextSentence, description.indexOf(':')) +
              " the code folder or in any libraries.";
          }
        }
*/
        //System.out.println("description = " + description);
        //System.out.println("creating exception " + exception);
        exception = new RunnerException(description, fileIndex, lineNumber-1, -1);

        // NOTE!! major change here, this exception will be queued
        // here to be thrown by the compile() function
        //editor.error(exception);

      } else {
        System.err.println("i suck: " + s);
      }

    } else {
      // this isn't the start of an error line, so don't attempt to parse
      // a line number out of it.

      // if the second error hasn't been discovered yet, these lines
      // are probably associated with the first error message,
      // which is already in the status bar, and are likely to be
      // of interest to the user, so spit them to the console.
      //
      if (!secondErrorFound) {
        System.err.println(s);
      }
    }
  }
  
  static private List getCommandCompilerC(String avrBasePath, List includePaths,
    String sourceName, String objectName) {
    List baseCommandCompiler = new ArrayList(Arrays.asList(new String[] {
      avrBasePath + "avr-gcc",
      "-c", // compile, don't link
      "-g", // include debugging info (so errors include line numbers)
      "-Os", // optimize for size
      "-w", // surpress all warnings
      "-ffunction-sections", // place each function in its own section
      "-fdata-sections",
      "-mmcu=" + Preferences.get("boards." + Preferences.get("board") + ".build.mcu"),
      "-DF_CPU=" + Preferences.get("boards." + Preferences.get("board") + ".build.f_cpu"),
    }));

    for (int i = 0; i < includePaths.size(); i++) {
      baseCommandCompiler.add("-I" + (String) includePaths.get(i));
    }
    
    baseCommandCompiler.add(sourceName);
    baseCommandCompiler.add("-o"+ objectName);
    
    return baseCommandCompiler;
  }
  
  
  static private List getCommandCompilerCPP(String avrBasePath,
    List includePaths, String sourceName, String objectName) {
      List baseCommandCompilerCPP = new ArrayList(Arrays.asList(new String[] {
      avrBasePath + "avr-g++",
      "-c", // compile, don't link
      "-g", // include debugging info (so errors include line numbers)
      "-Os", // optimize for size
      "-w", // surpress all warnings
      "-fno-exceptions",
      "-ffunction-sections", // place each function in its own section
      "-fdata-sections",
      "-mmcu=" + Preferences.get("boards." + Preferences.get("board") + ".build.mcu"),
      "-DF_CPU=" + Preferences.get("boards." + Preferences.get("board") + ".build.f_cpu"),
    }));

    for (int i = 0; i < includePaths.size(); i++) {
      baseCommandCompilerCPP.add("-I" + (String) includePaths.get(i));
    }
    
    baseCommandCompilerCPP.add(sourceName);
    baseCommandCompilerCPP.add("-o"+ objectName);
    
    return baseCommandCompilerCPP;
  }

  
  


  static String bootClassPath;

  static public String calcBootClassPath() {
    if (bootClassPath == null) {
      String additional = "";
      if (Base.isMacOS()) {
        additional =
          contentsToClassPath(new File("/System/Library/Java/Extensions/"));
      }
      bootClassPath =  System.getProperty("sun.boot.class.path") + additional;
    }
    return bootClassPath;
  }


  ///


  /**
   * Return the path for a folder, with appended paths to
   * any .jar or .zip files inside that folder.
   * This will prepend a colon (or whatever the path separator is)
   * so that it can be directly appended to another path string.
   *
   * This will always add the root folder as well, and doesn't bother
   * checking to see if there are any .class files in the folder or
   * within a subfolder.
   */
  static public String contentsToClassPath(File folder) {
    if (folder == null) return "";

    StringBuffer abuffer = new StringBuffer();
    String sep = System.getProperty("path.separator");

    try {
      // add the folder itself in case any unzipped files
      String path = folder.getCanonicalPath();
      abuffer.append(sep);
      abuffer.append(path);

      if (!path.endsWith(File.separator)) {
        path += File.separator;
      }
      //System.out.println("path is " + path);

      String list[] = folder.list();
      for (int i = 0; i < list.length; i++) {
        if (list[i].toLowerCase().endsWith(".o") ||
            list[i].toLowerCase().endsWith(".a")) {
          abuffer.append(sep);
          abuffer.append(path);
          abuffer.append(list[i]);
        }
      }
    } catch (IOException e) {
      e.printStackTrace();  // this would be odd
    }
    //System.out.println("included path is " + abuffer.toString());
    //packageListFromClassPath(abuffer.toString());  // WHY?
    return abuffer.toString();
  }
}
