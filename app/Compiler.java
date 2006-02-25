/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Compiler - default compiler class that connects to avr-gcc 
  Part of the Arduino project - http://arduino.berlios.de/

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

    String baseCommandCompiler[] = new String[] {
      ((!Base.isMacOS()) ? "tools/avr/bin/avr-gcc" :
        userdir + "tools/avr/bin/avr-gcc"),
      "-c", // compile, don't link
      "-g", // include debugging info (so errors include line numbers)
      "-Os", // optimize for size
      "-I" + target.getPath(),
      "-w", // surpress all warnings
      "-mmcu=" + Preferences.get("build.mcu"),
      "-DF_CPU=" + Preferences.get("build.f_cpu"),
      " ",
      " "
    };

    String baseCommandCompilerCPP[] = new String[] {
      ((!Base.isMacOS()) ? "tools/avr/bin/avr-g++" :
        userdir + "tools/avr/bin/avr-g++"),
      "-c", // compile, don't link
      "-g", // include debugging info (so errors include line numbers)
      "-Os", // optimize for size
      "-I" + target.getPath(),
      "-w", // surpress all warnings
      "-fno-exceptions",
      "-mmcu=" + Preferences.get("build.mcu"),
      "-DF_CPU=" + Preferences.get("build.f_cpu"),
      " ",
      " "
    };

    String baseCommandLinker[] = new String[] {
      ((!Base.isMacOS()) ? "tools/avr/bin/avr-gcc" :
        userdir + "tools/avr/bin/avr-gcc"),
      " ",
      "-mmcu=" + Preferences.get("build.mcu"),
      "-o",
      " ",
//      ((!Base.isMacOS()) ? "" : userdir) + "lib/uart.o",
//      ((!Base.isMacOS()) ? "" : userdir) + "lib/buffer.o",
//      ((!Base.isMacOS()) ? "" : userdir) + "lib/timer.o",
//      ((!Base.isMacOS()) ? "" : userdir) + "lib/wiring.o",
//      ((!Base.isMacOS()) ? "" : userdir) + "lib/pins_arduino.o",
      //((!Base.isMacOS()) ? "lib/WApplet.o" :
        //userdir + "lib/WApplet.o"),
      //((!Base.isMacOS()) ? "lib/WSerial.o" :
        //userdir + "lib/WSerial.o"),
      //((!Base.isMacOS()) ? "lib/WTimer.o" :
        //userdir + "lib/WTimer.o"),
      //((!Base.isMacOS()) ? "lib/Servo.o" :
        //userdir + "lib/Servo.o"),
      ////((!Base.isMacOS()) ? "lib/Wire.o" :
      ////  userdir + "lib/Wire.o"),
      ////((!Base.isMacOS()) ? "lib/WServo.o" :
      ////  userdir + "lib/WServo.o"),
      //((!Base.isMacOS()) ? "lib/WDisplay.o" :
        //userdir + "lib/WDisplay.o"),
      //((!Base.isMacOS()) ? "lib/WEncoder.o" :
        //userdir + "lib/WEncoder.o"),
      //((!Base.isMacOS()) ? "lib/WInterrupts.o" :
        //userdir + "lib/WInterrupts.o"),
      //((!Base.isMacOS()) ? "lib/WCounter.o" :
        //userdir + "lib/WCounter.o"),
      //((!Base.isMacOS()) ? "tools/avr/avr/lib/libm.a" :
        //userdir + "tools/avr/avr/lib/libm.a")
    };

    String baseCommandObjcopy[] = new String[] {
      ((!Base.isMacOS()) ? "tools/avr/bin/avr-objcopy" :
        userdir + "tools/avr/bin/avr-objcopy"),
      "-O",
      " ",
      "-R",
      " ",
      " ",
      " "
    };

    /*String baseCommand[] = new String[] {
      // user.dir is folder containing P5 (and therefore jikes)
      // macosx needs the extra path info. linux doesn't like it, though
      // windows doesn't seem to care. write once, headache anywhere.
      ((!Base.isMacOS()) ? "jikes" :
       System.getProperty("user.dir") + File.separator + "jikes"),

      // this doesn't help much.. also java 1.4 seems to not support
      // -source 1.1 for javac, and jikes seems to also have dropped it.
      // for versions of jikes that don't complain, "final int" inside
      // a function doesn't throw an error, so it could just be a
      // ms jvm error that this sort of thing doesn't work. blech.
      //"-source",
      //"1.1",

      // necessary to make output classes compatible with 1.1
      // i.e. so that exported applets can work with ms jvm on the web
      "-target",
      Preferences.get("preproc.jdk_version"),  //"1.1",
      // let the incompatability headache begin

      // used when run without a vm ("expert" mode)
      "-bootclasspath",
      calcBootClassPath(),

      // needed for macosx so that the classpath is set properly
      // also for windows because qtjava will most likely be here
      // and for linux, it just doesn't hurt
      "-classpath",
      sketch.classPath, //calcClassPath(includeFolder),

      "-nowarn", // we're not currently interested in warnings
      "+E", // output errors in machine-parsable format
      "-d", buildPath // output the classes in the buildPath
      //buildPath + File.separator + className + ".java" // file to compile
    };*/

    // make list of code files that need to be compiled and the object files
    // that they will be compiled to (includes code from the sketch and the
    // library for the target platform)
    String sourceNames[] = new String[sketch.codeCount + target.getSourceFilenames().size()];
    String sourceNamesCPP[] = new String[sketch.codeCount + target.getSourceFilenames().size()];
    String objectNames[] = new String[sketch.codeCount + target.getSourceFilenames().size()];
    String objectNamesCPP[] = new String[sketch.codeCount + target.getSourceFilenames().size()];
    int fileCount = 0;
    int fileCountCPP = 0;
    for (int i = 0; i < sketch.codeCount; i++) {
      if (sketch.code[i].preprocName != null) {
        if (sketch.code[i].preprocName.endsWith(".c")) {
          sourceNames[fileCount] = buildPath + File.separator + sketch.code[i].preprocName;
          objectNames[fileCount++] = buildPath + File.separator + sketch.code[i].preprocName + ".o";
        } else if (sketch.code[i].preprocName.endsWith(".cpp")) {
          sourceNamesCPP[fileCountCPP] = buildPath + File.separator + sketch.code[i].preprocName;
          objectNamesCPP[fileCountCPP++] = buildPath + File.separator + sketch.code[i].preprocName + ".o";
        } 
      }
    }
    for (Iterator iter = target.getSourceFilenames().iterator(); iter.hasNext(); ) {
      String filename = (String) iter.next();
      if (filename != null) {
        if (filename.endsWith(".c")) {
          sourceNames[fileCount] = target.getPath() + File.separator + filename;
          objectNames[fileCount++] = buildPath + File.separator + filename + ".o";
        } else if (filename.endsWith(".cpp")) {
          sourceNamesCPP[fileCountCPP] = target.getPath() + File.separator + filename;
          objectNamesCPP[fileCountCPP++] = buildPath + File.separator + filename + ".o";
        } 
      }
    }
    
    
    /*
    String commandCompiler[] = new String[baseCommandCompiler.length + preprocCount];
    System.arraycopy(baseCommandCompiler, 0, commandCompiler, 0, baseCommandCompiler.length);
    // append each of the files to the command string
    for (int i = 0; i < preprocCount; i++) {
      commandCompiler[baseCommandCompiler.length + i] =
        buildPath + File.separator + preprocNames[i];
    }

    String commandCompilerCPP[] = new String[baseCommandCompilerCPP.length + preprocCountCPP];
    System.arraycopy(baseCommandCompilerCPP, 0, commandCompilerCPP, 0, baseCommandCompilerCPP.length);
    for (int i = 0; i < preprocCountCPP; i++) {
      commandCompilerCPP[baseCommandCompilerCPP.length + i] = 
        buildPath + File.separator + preprocNamesCPP[i];
    }
    */
    //PApplet.printarr(command);

    baseCommandLinker[1] = "-Os -Wl,-u,vfprintf -lprintf_flt -lm,-Map=" +
      ((!Base.isMacOS()) ? buildPath : userdir + buildPath)
       + File.separator + sketch.name + ".map,--cref";
    baseCommandLinker[4] = ((!Base.isMacOS()) ? buildPath 
      : buildPath) + File.separator + sketch.name + ".elf";
    String commandLinker[] = new String[baseCommandLinker.length + fileCount +
      fileCountCPP + target.getObjectFilenames().size()];
    System.arraycopy(baseCommandLinker, 0, commandLinker, 0, baseCommandLinker.length);
    int idx = 0;
    for(int i = 0; i < fileCount; i++, idx++) {
      commandLinker[baseCommandLinker.length + idx] = objectNames[i];
    }
    for(int i = 0; i < fileCountCPP; i++, idx++) {
      commandLinker[baseCommandLinker.length + idx] = objectNamesCPP[i];
    }
    for(Iterator iter = target.getObjectFilenames().iterator(); iter.hasNext(); idx++) {
      commandLinker[baseCommandLinker.length + idx] = target.getPath() + File.separator + iter.next(); //already ends in ".o"
    }
      
    /*String command[] = new String[baseCommand.length + preprocCount];
    System.arraycopy(baseCommand, 0, command, 0, baseCommand.length);
    // append each of the files to the command string
    for (int i = 0; i < preprocCount; i++) {
      command[baseCommand.length + i] =
        buildPath + File.separator + preprocNames[i];
    }
    //PApplet.printarr(command);
    */
    /*
    String command[] = new String[baseCommand.length + sketch.codeCount];
    System.arraycopy(baseCommand, 0, command, 0, baseCommand.length);
    // append each of the files to the command string
    for (int i = 0; i < sketch.codeCount; i++) {
      command[baseCommand.length + i] =
        buildPath + File.separator + sketch.code[i].preprocName;
    }
    */

    //for (int i = 0; i < command.length; i++) {
      //System.out.println("cmd " + i + "  " + command[i]);
    //}

    firstErrorFound = false;  // haven't found any errors yet
    secondErrorFound = false;

    int result = 0; // pre-initialized to quiet a bogus warning from jikes
    try {
      // execute the compiler, and create threads to deal
      // with the input and error streams
      //

      Process process;
      boolean compiling = true;
      for(int i = 0; i < fileCount; i++) {
        baseCommandCompiler[8] = sourceNames[i];
        baseCommandCompiler[9] = "-o"+ objectNames[i];
        //System.arraycopy(baseCommandCompiler.length
        //for(int j = 0; j < baseCommandCompiler.length; j++) {
        //  System.out.println(baseCommandCompiler[j]);
        //}
        process = Runtime.getRuntime().exec(baseCommandCompiler);
        new MessageSiphon(process.getInputStream(), this);
        new MessageSiphon(process.getErrorStream(), this);

        // wait for the process to finish.  if interrupted
        // before waitFor returns, continue waiting
        //
        compiling = true;
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
        if(result!=0)
          return false;
      }

      for(int i = 0; i < fileCountCPP; i++) {
        baseCommandCompilerCPP[9] = sourceNamesCPP[i];
        baseCommandCompilerCPP[10] = "-o"+ objectNamesCPP[i];
        //for(int j = 0; j < baseCommandCompilerCPP.length; j++) {
        //  System.out.println(baseCommandCompilerCPP[j]);
        //}
        process = Runtime.getRuntime().exec(baseCommandCompilerCPP);
        new MessageSiphon(process.getInputStream(), this);
        new MessageSiphon(process.getErrorStream(), this);

        // wait for the process to finish.  if interrupted
        // before waitFor returns, continue waiting
        //
        compiling = true;
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
        if(result!=0)
          return false;
      }


      //for(int j = 0; j < commandLinker.length; j++) {
      //  System.out.println(commandLinker[j]);
      //}
      process = Runtime.getRuntime().exec(commandLinker);
      new MessageSiphon(process.getInputStream(), this);
      new MessageSiphon(process.getErrorStream(), this);
      compiling = true;
      while(compiling) {
        try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException intExc) { }
      }
      if (exception != null) {
        exception.hideStackTrace = true;
        throw exception;
      }
      if(result!=0)
        return false;

      /*for(int j = 0; j < baseCommandObjcopy.length; j++) {
        System.out.println(baseCommandObjcopy[j]);
      }*/
      baseCommandObjcopy[2] = "srec";
      baseCommandObjcopy[4] = ".eeprom";
      baseCommandObjcopy[5] = buildPath + File.separator + sketch.name + ".elf";
      baseCommandObjcopy[6] = buildPath + File.separator + sketch.name + ".rom";
      process = Runtime.getRuntime().exec(baseCommandObjcopy);
      new MessageSiphon(process.getInputStream(), this);
      new MessageSiphon(process.getErrorStream(), this);
      compiling = true;
      while(compiling) {
        try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException intExc) { }
      }
      if (exception != null) {
        exception.hideStackTrace = true;
        throw exception;
      }
      if(result!=0)
        return false;

      baseCommandObjcopy[2] = "ihex";
      baseCommandObjcopy[4] = ".flash";
      baseCommandObjcopy[5] = buildPath + File.separator + sketch.name + ".elf";
      baseCommandObjcopy[6] = buildPath + File.separator + sketch.name + ".hex";
      process = Runtime.getRuntime().exec(baseCommandObjcopy);
      new MessageSiphon(process.getInputStream(), this);
      new MessageSiphon(process.getErrorStream(), this);
      compiling = true;
      while(compiling) {
        try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException intExc) { }
      }
      if (exception != null) {
        exception.hideStackTrace = true;
        throw exception;
      }
      if(result!=0)
        return false;

      /*Process process = Runtime.getRuntime().exec(command);
      new MessageSiphon(process.getInputStream(), this);
      new MessageSiphon(process.getErrorStream(), this);

      // wait for the process to finish.  if interrupted
      // before waitFor returns, continue waiting
      //
      boolean compiling = true;
      while (compiling) {
        try {
          result = process.waitFor();
          //System.out.println("result is " + result);
          compiling = false;
        } catch (InterruptedException ignored) { }
      }*/

    } catch (Exception e) {
      String msg = e.getMessage();
      if ((msg != null) && (msg.indexOf("avr-gcc: not found") != -1)) {
        //System.err.println("jikes is missing");
        Base.showWarning("Compiler error",
                            "Could not find the compiler.\n" +
                            "avr-gcc is missing from your PATH,\n" +
                            "see readme.txt for help.", null);
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


  /**
   * A classpath, separated by the path separator, will contain
   * a series of .jar/.zip files or directories containing .class
   * files, or containing subdirectories that have .class files.
   *
   * @param path the input classpath
   * @return array of possible package names
   */
/*  static public String[] packageListFromClassPath(String path) {
    Hashtable table = new Hashtable();
    String pieces[] =
      Base.split(path, File.pathSeparatorChar);

    for (int i = 0; i < pieces.length; i++) {
      //System.out.println("checking piece '" + pieces[i] + "'");
      if (pieces[i].length() == 0) continue;

      if (pieces[i].toLowerCase().endsWith(".o") ||
          pieces[i].toLowerCase().endsWith(".a")) {
        packageListFromZip(pieces[i], table);

      } else {  // it's another type of file or directory
        File dir = new File(pieces[i]);
        if (dir.exists() && dir.isDirectory()) {
          packageListFromFolder(dir, null, table);
          //importCount = magicImportsRecursive(dir, null,
          //                                  table);
                                              //imports, importCount);
        }
      }
    }
    int tableCount = table.size();
    String output[] = new String[tableCount];
    int index = 0;
    Enumeration e = table.keys();
    while (e.hasMoreElements()) {
      output[index++] = ((String) e.nextElement()).replace('/', '.');
    }
    //System.arraycopy(imports, 0, output, 0, importCount);
    //PApplet.printarr(output);
    return output;
  }
*/

  static private void packageListFromZip(String filename, Hashtable table) {
    try {
      ZipFile file = new ZipFile(filename);
      Enumeration entries = file.entries();
      while (entries.hasMoreElements()) {
        ZipEntry entry = (ZipEntry) entries.nextElement();

        if (!entry.isDirectory()) {
          String name = entry.getName();

          if (name.endsWith(".class")) {
            int slash = name.lastIndexOf('/');
            if (slash == -1) continue;

            String pname = name.substring(0, slash);
            if (table.get(pname) == null) {
              table.put(pname, new Object());
            }
          }
        }
      }
    } catch (IOException e) {
      System.err.println("Ignoring " + filename + " (" + e.getMessage() + ")");
      //e.printStackTrace();
    }
  }


  /**
   * Make list of package names by traversing a directory hierarchy.
   * Each time a class is found in a folder, add its containing set
   * of folders to the package list. If another folder is found,
   * walk down into that folder and continue.
   */
  static private void packageListFromFolder(File dir, String sofar,
                                            Hashtable table) {
                                          //String imports[],
                                          //int importCount) {
    //System.err.println("checking dir '" + dir + "'");
    boolean foundClass = false;
    String files[] = dir.list();

    for (int i = 0; i < files.length; i++) {
      if (files[i].equals(".") || files[i].equals("..")) continue;

      File sub = new File(dir, files[i]);
      if (sub.isDirectory()) {
        String nowfar =
          (sofar == null) ? files[i] : (sofar + "." + files[i]);
        packageListFromFolder(sub, nowfar, table);
        //System.out.println(nowfar);
        //imports[importCount++] = nowfar;
        //importCount = magicImportsRecursive(sub, nowfar,
        //                                  imports, importCount);
      } else if (!foundClass) {  // if no classes found in this folder yet
        if (files[i].endsWith(".class")) {
          //System.out.println("unique class: " + files[i] + " for " + sofar);
          table.put(sofar, new Object());
          foundClass = true;
        }
      }
    }
    //return importCount;
  }

  /*
  static public int magicImportsRecursive(File dir, String sofar,
                                          Hashtable table) {
                                          //String imports[],
                                          //int importCount) {
    System.err.println("checking dir '" + dir + "'");
    String files[] = dir.list();
    for (int i = 0; i < files.length; i++) {
      if (files[i].equals(".") || files[i].equals("..")) continue;

      File sub = new File(dir, files[i]);
      if (sub.isDirectory()) {
        String nowfar = (sofar == null) ?
          files[i] : (sofar + "." + files[i]);
        //System.out.println(nowfar);
        imports[importCount++] = nowfar;

        importCount = magicImportsRecursive(sub, nowfar,
                                            imports, importCount);
      }
    }
    return importCount;
  }
  */
}
