/*
  Compiler - default compiler class that connects to the external compiler

  Part of the Arduino project - http://arduino.berlios.de

  Derived from the Processing project - http://processing.org

  Copyleft  2005 Massimo Banzi (arduino modifications)
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
*/

package processing.app;

import java.io.*;
import java.util.*;
import java.util.zip.*;
import javax.swing.*;

public class Compiler implements MessageConsumer {
  static final String BUGS_URL = "http://arduino.berlios.de";
  static final String SUPER_BADNESS = "Compiler error, please submit this code to " + BUGS_URL;

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

  public Compiler() { }  // null constructor

  public boolean compile(Sketch sketch, String buildPath)
      throws RunnerException {
      
    this.sketch = sketch;
    this.buildPath = buildPath;

    // the pms object isn't used for anything but storage
    MessageStream pms = new MessageStream(this);

    String userdir = System.getProperty("user.dir") + File.separator;
    
    System.out.println("Compiling Arduino program");
    Process process;
    String commandLine = "";
    //TODO test this in windows
    // FIXME: this is really nasty, it seems that MACOS is making the
    //        compilation inside the lib folder, while windows is doing it
    //        inside the work folder ... why why why  --DojoDave 
    if (Base.isWindows()) {
	    commandLine = userdir + "tools\\gnumake.exe  -C " + userdir + ". compile";
    } else if (Base.isMacOS()) {
	    commandLine = userdir + "tools/gnumake  -C " + userdir + "lib compile";
    }
    int result = 0;
    try {
     // System.out.println(commandLine);
     process = Runtime.getRuntime().exec(commandLine);
      
      
      new MessageSiphon(process.getInputStream(), this);
      new MessageSiphon(process.getErrorStream(), this);
      boolean compiling = true;
      while (compiling) {
        try {
          result = process.waitFor();
          compiling = false;
        } catch (InterruptedException ignored) { }
      }
    } catch (Exception e) {
      e.printStackTrace();
      System.out.println("Error: GNUMake probably couldn't be found");
      result = 99;
    }
    if(0 == result){
      System.out.println("Arduino Compilation Successful");
    }else{
      System.out.println("Arduino Compilation Unsuccessful (error: " + result + ")");
    }
    return (result == 0);
   
  }


  boolean firstErrorFound;
  boolean secondErrorFound;

  /**
   * Part of the MessageConsumer interface, this is called
   * whenever a piece (usually a line) of error message is spewed
   * out from the compiler. The errors are parsed for their contents
   * and line number, which is then reported back to Editor.
   * In Arduino v1 this is very very crude
   */
  public void message(String s) {
    // This receives messages as full lines, so a newline needs
    // to be added as they're printed to the console.
    //System.out.print(s);
    
    
	//if ((s.indexOf("warning:") != -1) && (s.indexOf("prog.c:") != -1) ) {
	//	 String[] result = s.split(":");
    // 	 for (int x=0; x<result.length; x++)
    //     System.out.println(x + " " +result[x]);
	//	//System.out.print(s);
	//	//totalErrors++;
	//}
	if (((s.indexOf("error:") != -1) || (s.indexOf("warning:") != -1)) && (s.indexOf("prog.c:") != -1) ) {
		 String[] result = s.split(":");
         // 0 = prog.c
         // 1 = line number
         // 2 = error or warning
         // 3 = message
         // TODO put the length of the header file into a Preference!!!
         //if ((s.indexOf("(Each undeclared") == 0) && (s.indexOf("for each function it appears in") == 0)) {
		 	int lineNum = Integer.parseInt(result[1]) - 15;
		 	System.out.print( result[2] + " at line " + lineNum + " " + result[3]);
		 //}
		
	}

    

  }

  public void processing_message(String s) {

    // ignore cautions
    if (s.indexOf("Caution") != -1) return;

    // jikes always uses a forward slash character as its separator,
    // so replace any platform-specific separator characters before
    // attemping to compare
    //
    String buildPathSubst = buildPath.replace(File.separatorChar, '/') + "/";

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
      int colon = s1.indexOf(':');
      int lineNumber = Integer.parseInt(s1.substring(0, colon));
      //System.out.println("pde / line number: " + lineNumber);

      if (fileIndex == 0) {  // main class, figure out which tab
        for (int i = 1; i < sketch.codeCount; i++) {
          if (sketch.code[i].flavor == Sketch.PDE) {
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
      int err = s1.indexOf("Error:");
      if (err != -1) {

        // if the first error has already been found, then this must be
        // (at least) the second error found
        if (firstErrorFound) {
          secondErrorFound = true;
          return;
        }

        // if executing at this point, this is *at least* the first error
        firstErrorFound = true;

        //err += "error:".length();
        String description = s1.substring(err + "Error:".length());
        description = description.trim();

        String hasLoop = "The method \"void loop();\" with default access";
        if (description.indexOf(hasLoop) != -1) {
          description =
            "Rename loop() to draw() in Processing 0070 and higher";
        }

        String constructorProblem =
          "No applicable overload was found for a constructor of type";
        if (description.indexOf(constructorProblem) != -1) {
          //"simong.particles.ParticleSystem". Perhaps you wanted the overloaded version "ParticleSystem();" instead?
          int nextSentence = description.indexOf("\".") + 3;
          description = description.substring(nextSentence);
        }

        String overloadProblem = "No applicable overload";
        if (description.indexOf(overloadProblem) != -1) {
          int nextSentence = description.indexOf("\".") + 3;
          description = description.substring(nextSentence);
        }

        // c:/fry/processing/build/windows/work/lib/build/Temporary_6858_2476.java:1:34:1:41: Semantic Error: You need to modify your classpath, sourcepath, bootclasspath, and/or extdirs setup. Package "poo/shoe" could not be found in:
        String classpathProblem = "You need to modify your classpath";
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

/*
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

*/
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
        if (list[i].toLowerCase().endsWith(".jar") ||
            list[i].toLowerCase().endsWith(".zip")) {
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
  static public String[] packageListFromClassPath(String path) {
    Hashtable table = new Hashtable();
    String pieces[] =
      Base.split(path, File.pathSeparatorChar);

    for (int i = 0; i < pieces.length; i++) {
      //System.out.println("checking piece '" + pieces[i] + "'");
      if (pieces[i].length() == 0) continue;

      if (pieces[i].toLowerCase().endsWith(".jar") ||
          pieces[i].toLowerCase().endsWith(".zip")) {
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
