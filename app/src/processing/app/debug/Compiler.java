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
import processing.app.Sketch;
import processing.app.SketchCode;
import processing.core.*;

import java.io.*;
import java.util.*;
import java.util.zip.*;

import org.eclipse.jdt.core.compiler.batch.BatchCompiler;
import org.eclipse.jdt.core.compiler.CompilationProgress;


public class Compiler {

  public Compiler() { }

  /**
   * Compile with ECJ.
   *
   * @param sketch Sketch object to be compiled.
   * @param buildPath Where the temporary files live and will be built from.
   * @return true if successful.
   * @throws RunnerException Only if there's a problem. Only then.
   */
  public boolean compile(Sketch sketch,
                         String buildPath,
                         String primaryClassName) throws RunnerException {
    // This will be filled in if anyone gets angry
    RunnerException exception = null;
    boolean success = false;

    String baseCommand[] = new String[] {
      "-Xemacs",
      //"-noExit",  // not necessary for ecj
      "-source", "1.5",
      "-target", "1.5",
      "-classpath", sketch.getClassPath(),
      "-nowarn", // we're not currently interested in warnings (works in ecj)
      "-d", buildPath // output the classes in the buildPath
    };
    //PApplet.println(baseCommand);

    // make list of code files that need to be compiled
    // (some files are skipped if they contain no class)
    String[] sourceFiles = new String[sketch.getCodeCount()];
    int sourceCount = 0;
    sourceFiles[sourceCount++] =
      new File(buildPath, primaryClassName + ".java").getAbsolutePath();

    for (SketchCode code : sketch.getCode()) {
      if (code.isExtension("java")) {
        String path = new File(buildPath, code.getFileName()).getAbsolutePath();
        sourceFiles[sourceCount++] = path;
      }
    }
    String[] command = new String[baseCommand.length + sourceCount];
    System.arraycopy(baseCommand, 0, command, 0, baseCommand.length);
    // append each of the files to the command string
    System.arraycopy(sourceFiles, 0, command, baseCommand.length, sourceCount);

    //PApplet.println(command);

    try {
      // Load errors into a local StringBuffer
      final StringBuffer errorBuffer = new StringBuffer();

      // Create single method dummy writer class to slurp errors from javac
      Writer internalWriter = new Writer() {
          public void write(char[] buf, int off, int len) {
            errorBuffer.append(buf, off, len);
          }

          public void flush() { }

          public void close() { }
        };
      // Wrap as a PrintWriter since that's what compile() wants
      PrintWriter writer = new PrintWriter(internalWriter);

      //result = com.sun.tools.javac.Main.compile(command, writer);

      CompilationProgress progress = null;
      PrintWriter outWriter = new PrintWriter(System.out);
      success = BatchCompiler.compile(command, outWriter, writer, progress);
      // Close out the stream for good measure
      writer.flush();
      writer.close();

      BufferedReader reader =
        new BufferedReader(new StringReader(errorBuffer.toString()));
      //System.err.println(errorBuffer.toString());

      String line = null;
      while ((line = reader.readLine()) != null) {
        //System.out.println("got line " + line);  // debug

        // get first line, which contains file name, line number,
        // and at least the first line of the error message
        String errorFormat = "([\\w\\d_]+.java):(\\d+):\\s*(.*):\\s*(.*)\\s*";
        String[] pieces = PApplet.match(line, errorFormat);
        //PApplet.println(pieces);

        // if it's something unexpected, die and print the mess to the console
        if (pieces == null) {
          exception = new RunnerException("Cannot parse error text: " + line);
          exception.hideStackTrace();
          // Send out the rest of the error message to the console.
          System.err.println(line);
          while ((line = reader.readLine()) != null) {
            System.err.println(line);
          }
          break;
        }

        // translate the java filename and line number into a un-preprocessed
        // location inside a source file or tab in the environment.
        String dotJavaFilename = pieces[1];
        // Line numbers are 1-indexed from javac
        int dotJavaLineIndex = PApplet.parseInt(pieces[2]) - 1;
        String errorMessage = pieces[4];

        int codeIndex = 0; //-1;
        int codeLine = -1;

        // first check to see if it's a .java file
        for (int i = 0; i < sketch.getCodeCount(); i++) {
          SketchCode code = sketch.getCode(i);
          if (code.isExtension("java")) {
            if (dotJavaFilename.equals(code.getFileName())) {
              codeIndex = i;
              codeLine = dotJavaLineIndex;
            }
          }
        }

        // if it's not a .java file, codeIndex will still be 0
        if (codeIndex == 0) {  // main class, figure out which tab
          //for (int i = 1; i < sketch.getCodeCount(); i++) {
          for (int i = 0; i < sketch.getCodeCount(); i++) {
            SketchCode code = sketch.getCode(i);

            if (code.isExtension("pde")) {
              if (code.getPreprocOffset() <= dotJavaLineIndex) {
                codeIndex = i;
                //System.out.println("i'm thinkin file " + i);
                codeLine = dotJavaLineIndex - code.getPreprocOffset();
              }
            }
          }

          //if (codeLine != -1) {
          //codeLine = dotJavaLineIndex - sketch.getCode(codeIndex).getPreprocOffset();
          //}
        }
        //System.out.println("code line now " + codeLine);
        exception = new RunnerException(errorMessage, codeIndex, codeLine, -1, false);

        // for a test case once message parsing is implemented,
        // use new Font(...) since that wasn't getting picked up properly.

        /*
        if (errorMessage.equals("cannot find symbol")) {
          handleCannotFindSymbol(reader, exception);

        } else if (errorMessage.indexOf("is already defined") != -1) {
          reader.readLine();  // repeats the line of code w/ error
          int codeColumn = caretColumn(reader.readLine());
          exception = new RunnerException(errorMessage,
                                          codeIndex, codeLine, codeColumn);

        } else if (errorMessage.startsWith("package") &&
                   errorMessage.endsWith("does not exist")) {
          // Because imports are stripped out and re-added to the 0th line of
          // the preprocessed code, codeLine will always be wrong for imports.
          exception = new RunnerException("P" + errorMessage.substring(1) +
                                          ". You might be missing a library.");
        } else {
          exception = new RunnerException(errorMessage);
        }
        */
        if (errorMessage.startsWith("The import ") &&
            errorMessage.endsWith("cannot be resolved")) {
          // The import poo cannot be resolved
          //import poo.shoe.blah.*;
          String what = errorMessage.substring("The import ".length());
          what = what.substring(0, what.indexOf(' '));
          System.err.println("Note that release 1.0, libraries must be " +
                             "installed in a folder named 'libraries' " +
                             "inside the 'sketchbook' folder.");
          exception.setMessage("The package " +
                               "\u201C" + what + "\u201D" +
                               " does not exist. " +
                               "You might be missing a library.");

          // Actually create the folder and open it for the user
          File sketchbookLibraries = Base.getSketchbookLibrariesFolder();
          if (!sketchbookLibraries.exists()) {
            if (sketchbookLibraries.mkdirs()) {
              Base.openFolder(sketchbookLibraries);
            }
          }

        } else if (errorMessage.endsWith("cannot be resolved to a type")) {
          // xxx cannot be resolved to a type
          //xxx c;

          String what = errorMessage.substring(0, errorMessage.indexOf(' '));

          if (what.equals("BFont") ||
              what.equals("BGraphics") ||
              what.equals("BImage")) {
            handleCrustyCode(exception);

          } else {
            exception.setMessage("Cannot find a class or type " +
                                 "named \u201C" + what + "\u201D");
          }

        } else if (errorMessage.endsWith("cannot be resolved")) {
          // xxx cannot be resolved
          //println(xxx);

          String what = errorMessage.substring(0, errorMessage.indexOf(' '));

          if (what.equals("LINE_LOOP") ||
              what.equals("LINE_STRIP") ||
              what.equals("framerate")) {
            handleCrustyCode(exception);

          } else {
            exception.setMessage("Cannot find anything " +
                                 "named \u201C" + what + "\u201D");
          }

        } else if (errorMessage.startsWith("Duplicate")) {
          // "Duplicate nested type xxx"
          // "Duplicate local variable xxx"

        } else {
          String[] parts = null;

          // The method xxx(String) is undefined for the type Temporary_XXXX_XXXX
          //xxx("blah");
          // The method xxx(String, int) is undefined for the type Temporary_XXXX_XXXX
          //xxx("blah", 34);
          // The method xxx(String, int) is undefined for the type PApplet
          //PApplet.sub("ding");
          String undefined =
            "The method (\\S+\\(.*\\)) is undefined for the type (.*)";
          parts = PApplet.match(errorMessage, undefined);
          if (parts != null) {
            if (parts[1].equals("framerate(int)") ||
                parts[1].equals("push()")) {
              handleCrustyCode(exception);
            } else {
              String mess = "The function " + parts[1] + " does not exist.";
              exception.setMessage(mess);
            }
            break;
          }
        }
        if (exception != null) {
          // The stack trace just shows that this happened inside the compiler,
          // which is a red herring. Don't ever show it for compiler stuff.
          exception.hideStackTrace();
          break;
        }
      }
    } catch (IOException e) {
      String bigSigh = "Error while compiling. (" + e.getMessage() + ")";
      exception = new RunnerException(bigSigh);
      e.printStackTrace();
      success = false;
    }
    // In case there was something else.
    if (exception != null) throw exception;

    return success;
  }


  /**
   * Fire up 'ole javac based on <a href="http://java.sun.com/j2se/1.5.0/docs/tooldocs/solaris/javac.html#proginterface">this interface</a>.
   *
   * @param sketch Sketch object to be compiled.
   * @param buildPath Where the temporary files live and will be built from.
   * @return
   * @throws RunnerException Only if there's a problem. Only then.
   */
//  public boolean compileJavac(Sketch sketch,
//                         String buildPath) throws RunnerException {
//    // This will be filled in if anyone gets angry
//    RunnerException exception = null;
//
//    String baseCommand[] = new String[] {
//      "-source", "1.5",
//      "-target", "1.5",
//      "-classpath", sketch.getClassPath(),
//      "-nowarn", // we're not currently interested in warnings (ignored?)
//      "-d", buildPath // output the classes in the buildPath
//    };
//    //PApplet.println(baseCommand);
//
//    // make list of code files that need to be compiled
//    // (some files are skipped if they contain no class)
//    String[] preprocNames = new String[sketch.getCodeCount()];
//    int preprocCount = 0;
//    for (int i = 0; i < sketch.getCodeCount(); i++) {
//      if (sketch.getCode(i).preprocName != null) {
//        preprocNames[preprocCount++] = sketch.getCode(i).preprocName;
//      }
//    }
//    String[] command = new String[baseCommand.length + preprocCount];
//    System.arraycopy(baseCommand, 0, command, 0, baseCommand.length);
//    // append each of the files to the command string
//    for (int i = 0; i < preprocCount; i++) {
//      command[baseCommand.length + i] =
//        buildPath + File.separator + preprocNames[i];
//    }
//    //PApplet.println(command);
//
//    int result = -1;  // needs to be set bad by default, in case hits IOE below
//
//    try {
//      // Load errors into a local StringBuffer
//      final StringBuffer errorBuffer = new StringBuffer();
//
//      // Create single method dummy writer class to slurp errors from javac
//      Writer internalWriter = new Writer() {
//          public void write(char[] buf, int off, int len) {
//            errorBuffer.append(buf, off, len);
//          }
//
//          public void flush() { }
//
//          public void close() { }
//        };
//      // Wrap as a PrintWriter since that's what compile() wants
//      PrintWriter writer = new PrintWriter(internalWriter);
//
//      result = com.sun.tools.javac.Main.compile(command, writer);
//
//      // Close out the stream for good measure
//      writer.flush();
//      writer.close();
//
////      BufferedReader reader =
////        new BufferedReader(new StringReader(errorBuffer.toString()));
//      //System.err.println(errorBuffer.toString());
//
////      String m = errorBuffer.toString();
//      //ParsePosition mp = new ParsePosition(0);
//
////      while (mp.getIndex() < m.length()) {  // reading messages
//      String line = null;
//      int lineIndex = 0;
//      String[] lines = PApplet.split(errorBuffer.toString(), '\n');
//      int lineCount = lines.length;
//      while (lineIndex < lineCount) {
//      //while ((line = reader.readLine()) != null) {
//        //System.out.println("got line " + line);  // debug
//
//        /*
//compiler.misc.count.error=\
//    {0} error
//compiler.misc.count.error.plural=\
//    {0} errors
//compiler.misc.count.warn=\
//    {0} warning
//compiler.misc.count.warn.plural=\
//    {0} warnings
//         */
//        // Check to see if this is the last line.
////        if ((PApplet.match(line, "\\d+ error[s]?") != null) ||
////            (PApplet.match(line, "\\d+ warning[s]?") != null)) {
////          break;
////        }
//        if (isCompilerMatch(line, "compiler.misc.count.error") ||
//            isCompilerMatch(line, "compiler.misc.count.error.plural") ||
//            isCompilerMatch(line, "compiler.misc.count.warn") ||
//            isCompilerMatch(line, "compiler.misc.count.warn.plural")) {
//            break;
//        }
//
//        // Hide these because people are getting confused
//        // http://dev.processing.org/bugs/show_bug.cgi?id=817
//        // com/sun/tools/javac/resources/compiler.properties
//        //if (line.startsWith("Note: ")) {
//        String compilerNote = compilerResources.getString("compiler.note.note");
//        MessageFormat noteFormat = new MessageFormat(compilerNote + " {0}");
//        Object[] noteFound;
//        try {
//          noteFound = noteFormat.parse(line);
//          if (noteFound != null) {
//            System.out.println("gefunden " + noteFound[0]);
//
//            /*
//          // if you mention serialVersionUID one more time, i'm kickin' you out
//          if (line.indexOf("serialVersionUID") != -1) continue;
//          // {0} uses unchecked or unsafe operations.
//          // Some input files use unchecked or unsafe operations.
//          if (line.indexOf("or unsafe operations") != -1) continue;
//          // {0} uses or overrides a deprecated API.
//          // Some input files use or override a deprecated API.
//          if (line.indexOf("or override") != -1) continue;
//          // Recompile with -Xlint:deprecation for details.
//          // Recompile with -Xlint:unchecked for details.
//          if (line.indexOf("Recompile with -Xlint:") != -1) continue;
//          System.err.println(line);
//             */
//            continue;
//          }
//        } catch (ParseException e) {
//          e.printStackTrace();
//        }
//
//        // get first line, which contains file name, line number,
//        // and at least the first line of the error message
//        String errorFormat = "([\\w\\d_]+.java):(\\d+):\\s*(.*)\\s*";
//        String[] pieces = PApplet.match(line, errorFormat);
//
//        // if it's something unexpected, die and print the mess to the console
//        if (pieces == null) {
//          exception = new RunnerException("Cannot parse error text: " + line);
//          exception.hideStackTrace();
//          // Send out the rest of the error message to the console.
//          System.err.println(line);
//          //while ((line = reader.readLine()) != null) {
//          for (int i = lineIndex; i < lineCount; i++) {
//            System.err.println(lines[i]);
//          }
//          break;
//        }
//
//        // translate the java filename and line number into a un-preprocessed
//        // location inside a source file or tab in the environment.
//        String dotJavaFilename = pieces[0];
//        // Line numbers are 1-indexed from javac
//        int dotJavaLineIndex = PApplet.parseInt(pieces[1]) - 1;
//        String errorMessage = pieces[2];
//
//        int codeIndex = -1;
//        int codeLine = -1;
//        for (int i = 0; i < sketch.getCodeCount(); i++) {
//          String name = sketch.getCode(i).preprocName;
//          if ((name != null) && dotJavaFilename.equals(name)) {
//            codeIndex = i;
//          }
//        }
//        //System.out.println("code index/line are " + codeIndex + " " + codeLine);
//        //System.out.println("java line number " + dotJavaLineIndex + " from " + dotJavaFilename);
//
//        if (codeIndex == 0) {  // main class, figure out which tab
//          for (int i = 1; i < sketch.getCodeCount(); i++) {
//            SketchCode code = sketch.getCode(i);
//
//            if (code.flavor == Sketch.PDE) {
//              if (code.preprocOffset <= dotJavaLineIndex) {
//                codeIndex = i;
//                //System.out.println("i'm thinkin file " + i);
//              }
//            }
//          }
//        }
//        //System.out.println("preproc offset is " + sketch.getCode(codeIndex).preprocOffset);
//        codeLine = dotJavaLineIndex - sketch.getCode(codeIndex).preprocOffset;
//        //System.out.println("code line now " + codeLine);
//        exception = new RunnerException(errorMessage, codeIndex, codeLine, -1, false);
//
//        // for a test case once message parsing is implemented,
//        // use new Font(...) since that wasn't getting picked up properly.
//
//        if (errorMessage.equals("cannot find symbol")) {
//          handleCannotFindSymbol(reader, exception);
//
//        } else if (errorMessage.indexOf("is already defined") != -1) {
//          reader.readLine();  // repeats the line of code w/ error
//          int codeColumn = caretColumn(reader.readLine());
//          exception = new RunnerException(errorMessage,
//                                          codeIndex, codeLine, codeColumn);
//
//        } else if (errorMessage.startsWith("package") &&
//                   errorMessage.endsWith("does not exist")) {
//          // Because imports are stripped out and re-added to the 0th line of
//          // the preprocessed code, codeLine will always be wrong for imports.
//          exception = new RunnerException("P" + errorMessage.substring(1) +
//                                          ". You might be missing a library.");
//        } else {
//          exception = new RunnerException(errorMessage);
//        }
//        if (exception != null) {
//          // The stack trace just shows that this happened inside the compiler,
//          // which is a red herring. Don't ever show it for compiler stuff.
//          exception.hideStackTrace();
//          break;
//        }
//      }
//    } catch (IOException e) {
//      String bigSigh = "Error while compiling. (" + e.getMessage() + ")";
//      exception = new RunnerException(bigSigh);
//      e.printStackTrace();
//      result = 1;
//    }
//    // In case there was something else.
//    if (exception != null) throw exception;
//
//    // Success means that 'result' is set to zero
//    return (result == 0);
//  }
//
//
//  boolean isCompilerMatch(String line, String format) {
//    return compilerMatch(line, format) != null;
//  }
//
//
//  Object[] compilerMatch(String line, String name) {
//    String format = compilerResources.getString(name);
//    MessageFormat mf = new MessageFormat(format);
//    Object[] matches = null;
//    try {
//      matches = mf.parse(line);
//    } catch (ParseException e) {
//      e.printStackTrace();
//    }
//    return matches;
//  }


//  boolean isCompilerMatch(String line, ParsePosition pos, String format) {
//    return compilerMatch(line, pos, format) != null;
//  }
//
//
//  Object[] compilerMatch(String line, ParsePosition pos, String name) {
//    String format = compilerResources.getString(name);
//    MessageFormat mf = new MessageFormat(format);
//    Object[] matches = mf.parse(line, pos);
//    return matches;
//  }


  // Tell-tale signs of old code copied and pasted from the web.
  // Detect classes BFont, BGraphics, BImage; methods framerate, push;
  // and variables LINE_LOOP and LINE_STRIP.
//  static HashMap crusties = new HashMap();
//  static {
//    crusties.put("BFont", new Object());
//    crusties.put("BGraphics", new Object());
//    crusties.put("BImage", new Object());
//    crusties.put("framerate", new Object());
//    crusties.put("push", new Object());
//    crusties.put("LINE_LOOP", new Object());
//    crusties.put("LINE_STRIP", new Object());
//  }


//  void handleCannotFindSymbol(BufferedReader reader,
//                              RunnerException rex) throws IOException {
//    String symbolLine = reader.readLine();
//    String locationLine = reader.readLine();
//    /*String codeLine =*/ reader.readLine();
//    String caretLine = reader.readLine();
//    rex.setCodeColumn(caretColumn(caretLine));
//
//    String[] pieces =
//      PApplet.match(symbolLine, "symbol\\s*:\\s*(\\w+)\\s+(.*)");
//    if (pieces != null) {
//      if (pieces[0].equals("class") ||
//          pieces[0].equals("variable")) {
//        rex.setMessage("Cannot find a " + pieces[0] + " " +
//                       "named \u201C" + pieces[1] + "\u201D");
//        if (crusties.get(pieces[1]) != null) {
//          handleCrustyCode(rex);
//        }
//
//      } else if (pieces[0].equals("method")) {
//        int leftParen = pieces[1].indexOf("(");
//        int rightParen = pieces[1].indexOf(")");
//
//        String methodName = pieces[1].substring(0, leftParen);
//        String methodParams = pieces[1].substring(leftParen + 1, rightParen);
//
//        String message =
//          "Cannot find a function named \u201C" + methodName + "\u201D";
//        if (methodParams.length() > 0) {
//          if (methodParams.indexOf(',') != -1) {
//            message += " with parameters ";
//          } else {
//            message += " with parameter ";
//          }
//          message += methodParams;
//        }
//
//        String locationClass = "location: class ";
//        if (locationLine.startsWith(locationClass) &&
//            // don't include the class name when it's a temp class
//            locationLine.indexOf("Temporary_") == -1) {
//          String className = locationLine.substring(locationClass.length());
//          // If no dot exists, -1 + 1 is 0, so this will have no effect.
//          className = className.substring(className.lastIndexOf('.') + 1);
//          int bracket = className.indexOf('[');
//          if (bracket == -1) {
//            message += " in class " + className;
//          } else {
//            className = className.substring(0, bracket);
//            message += " for an array of " + className + " objects";
//          }
//        }
//        message += ".";
//        rex.setMessage(message);
//
//        // On second thought, make sure this isn't just some alpha/beta code
//        if (crusties.get(methodName) != null) {
//          handleCrustyCode(rex);
//        }
//
//      } else {
//        System.out.println(symbolLine);
//      }
//    }
//  }


  void handleCrustyCode(RunnerException rex) {
    rex.setMessage("This code needs to be updated, " +
                   "please read the \u201Cchanges\u201D reference.");
    Base.showReference("changes.html");
  }


  protected int caretColumn(String caretLine) {
    return caretLine.indexOf("^");
  }


  /////////////////////////////////////////////////////////////////////////////


  /**
   * Given a folder, return a list of absolute paths to all jar or zip files
   * inside that folder, separated by pathSeparatorChar.
   *
   * This will prepend a colon (or whatever the path separator is)
   * so that it can be directly appended to another path string.
   *
   * As of 0136, this will no longer add the root folder as well.
   *
   * This function doesn't bother checking to see if there are any .class
   * files in the folder or within a subfolder.
   */
  static public String contentsToClassPath(File folder) {
    if (folder == null) return "";

    StringBuffer abuffer = new StringBuffer();
    String sep = System.getProperty("path.separator");

    try {
      String path = folder.getCanonicalPath();

//    disabled as of 0136
      // add the folder itself in case any unzipped files
//      abuffer.append(sep);
//      abuffer.append(path);
//
      // When getting the name of this folder, make sure it has a slash
      // after it, so that the names of sub-items can be added.
      if (!path.endsWith(File.separator)) {
        path += File.separator;
      }

      String list[] = folder.list();
      for (int i = 0; i < list.length; i++) {
        // Skip . and ._ files. Prior to 0125p3, .jar files that had
        // OS X AppleDouble files associated would cause trouble.
        if (list[i].startsWith(".")) continue;

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
      PApplet.split(path, File.pathSeparatorChar);

    for (int i = 0; i < pieces.length; i++) {
      //System.out.println("checking piece '" + pieces[i] + "'");
      if (pieces[i].length() == 0) continue;

      if (pieces[i].toLowerCase().endsWith(".jar") ||
          pieces[i].toLowerCase().endsWith(".zip")) {
        //System.out.println("checking " + pieces[i]);
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
  }
}
