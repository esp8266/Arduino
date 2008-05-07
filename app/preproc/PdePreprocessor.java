/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  PdePreprocessor - wrapper for default ANTLR-generated parser
  Part of the Wiring project - http://wiring.org.co

  Copyright (c) 2004-05 Hernando Barragan

  Processing version Copyright (c) 2004-05 Ben Fry and Casey Reas
  Copyright (c) 2001-04 Massachusetts Institute of Technology

  ANTLR-generated parser and several supporting classes written
  by Dan Mosedale via funding from the Interaction Institute IVREA.

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

package processing.app.preproc;

import processing.app.*;
import processing.core.*;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

import antlr.*;
import antlr.collections.*;
import antlr.collections.impl.*;

import com.oroinc.text.regex.*;


public class PdePreprocessor {

  static final int JDK11 = 0;
  static final int JDK13 = 1;
  static final int JDK14 = 2;

  //static String defaultImports[][] = new String[3][];

  // these ones have the .* at the end, since a class name
  // might be at the end instead of .* whcih would make trouble
  // other classes using this can lop of the . and anything after
  // it to produce a package name consistently.
  public String extraImports[];

  static public final int STATIC = 0;  // formerly BEGINNER
  static public final int ACTIVE = 1;  // formerly INTERMEDIATE
  static public final int JAVA   = 2;  // formerly ADVANCED
  // static to make it easier for the antlr preproc to get at it
  static public int programType = -1;

  Reader programReader;
  String buildPath;

  // stores number of built user-defined function prototypes
  public int prototypeCount = 0;

  // stores number of included library headers written
  public int headerCount = 0;

  /**
   * These may change in-between (if the prefs panel adds this option)
   * so grab them here on construction.
   */
  public PdePreprocessor() {}

  /**
   * Used by PdeEmitter.dumpHiddenTokens()
   */
  //public static TokenStreamCopyingHiddenTokenFilter filter;
  
  /**
   * Returns the index of the first character that's not whitespace, a comment
   * or a pre-processor directive.
   */
  public int firstStatement(String in) {
    PatternMatcherInput input = new PatternMatcherInput(in);
    PatternCompiler compiler = new Perl5Compiler();
    PatternMatcher matcher = new Perl5Matcher();
    Pattern pattern = null;
    
    try {
      pattern = compiler.compile(
        // XXX: doesn't properly handle special single-quoted characters
        // whitespace
        "\\s+" + "|" +
        // multi-line comment
        "(/\\*[^*]*(?:\\*(?!/)[^*]*)*\\*/)" + "|" +
        // single-line comment
        "(//.*?$)" + "|" +
        // pre-processor directive
        "(#(?:\\\\\\n|.)*)",
        Perl5Compiler.MULTILINE_MASK);
    } catch (MalformedPatternException e) {
      throw new RuntimeException("Internal error in firstStatement()", e);
    }
      
    int i = 0;
    while (matcher.matchesPrefix(input, pattern)) {
      i = matcher.getMatch().endOffset(0);
      input.setCurrentOffset(i);
    }
    
    return i;
  }
  
  /**
   * Strips comments, pre-processor directives, single- and double-quoted
   * strings from a string.
   * @param in the String to strip
   * @return the stripped String
   */
  public String strip(String in) throws MalformedPatternException {
    PatternCompiler compiler = new Perl5Compiler();
    PatternMatcher matcher = new Perl5Matcher();
    Pattern pattern = compiler.compile(
      // XXX: doesn't properly handle special single-quoted characters
      // single-quoted character
      "('.')" + "|" +
      // double-quoted string
      "(\"(?:[^\"\\\\]|\\\\.)*\")" + "|" +
      // multi-line comment
      "(/\\*[^*]*(?:\\*(?!/)[^*]*)*\\*/)" + "|" +
      // single-line comment
      "(//.*?$)" + "|" +
      // pre-processor directive
      "(^\\s*#.*?$)",
      Perl5Compiler.MULTILINE_MASK);
      
    while (matcher.contains(in, pattern)) {
      MatchResult result = matcher.getMatch();
      // XXX: should preserve newlines in the result so that line numbers of
      // the stripped string correspond to those in the original source.
      in = in.substring(0, result.beginOffset(0)) + " " + in.substring(result.endOffset(0));
    }
    
    return in;
  }
  
  /**
   * Removes the contents of all top-level curly brace pairs {}.
   * @param in the String to collapse
   * @return the collapsed String
   */
  private String collapseBraces(String in) {
    StringBuffer buffer = new StringBuffer();
    int nesting = 0;
    int start = 0;
    
    // XXX: need to keep newlines inside braces so we can determine the line
    // number of a prototype
    for (int i = 0; i < in.length(); i++) {
      if (in.charAt(i) == '{') {
        if (nesting == 0) {
          buffer.append(in.substring(start, i + 1));  // include the '{'
        }
        nesting++;
      }
      if (in.charAt(i) == '}') {
        nesting--;
        if (nesting == 0) {
          start = i; // include the '}'
        }
      }
    }
    
    buffer.append(in.substring(start));
    
    return buffer.toString();
  }
  
  public List prototypes(String in) throws MalformedPatternException {
    in = collapseBraces(strip(in));
    
    PatternMatcherInput input = new PatternMatcherInput(in);
    PatternCompiler compiler = new Perl5Compiler();
    PatternMatcher matcher = new Perl5Matcher();
    // XXX: doesn't handle ... varargs
    // XXX: doesn't handle function pointers
    Pattern pattern = compiler.compile(
      "[\\w\\[\\]\\*]+\\s+[\\[\\]\\*\\w\\s]+\\([,\\[\\]\\*\\w\\s]*\\)(?=\\s*\\{)");
    List matches = new ArrayList();
    
    while (matcher.contains(input, pattern)) {
      matches.add(matcher.getMatch().group(0) + ";");
    }
    
    return matches;
  }


  /**
   * preprocesses a pde file and write out a java file
   * @param pretty true if should also space out/indent lines
   * @return the classname of the exported Java
   */
  //public String write(String program, String buildPath, String name,
  //                  String extraImports[]) throws java.lang.Exception {
  public String write(String program, String buildPath,
                      String name, String codeFolderPackages[],
                      Target target)
    throws java.lang.Exception {
    // if the program ends with no CR or LF an OutOfMemoryError will happen.
    // not gonna track down the bug now, so here's a hack for it:
    // bug filed at http://dev.processing.org/bugs/show_bug.cgi?id=5
    //if ((program.length() > 0) &&
    //program.charAt(program.length()-1) != '\n') {
      program += "\n";
    //}

    // if the program ends with an unterminated multiline comment,
    // an OutOfMemoryError or NullPointerException will happen.
    // again, not gonna bother tracking this down, but here's a hack.
    // http://dev.processing.org/bugs/show_bug.cgi?id=16
    Sketch.scrubComments(program);
    // this returns the scrubbed version, but more important for this
    // function, it'll check to see if there are errors with the comments.

    if (Preferences.getBoolean("preproc.substitute_unicode")) {
      // check for non-ascii chars (these will be/must be in unicode format)
      char p[] = program.toCharArray();
      int unicodeCount = 0;
      for (int i = 0; i < p.length; i++) {
        if (p[i] > 127) unicodeCount++;
      }
      // if non-ascii chars are in there, convert to unicode escapes
      if (unicodeCount != 0) {
        // add unicodeCount * 5.. replacing each unicode char
        // with six digit uXXXX sequence (xxxx is in hex)
        // (except for nbsp chars which will be a replaced with a space)
        int index = 0;
        char p2[] = new char[p.length + unicodeCount*5];
        for (int i = 0; i < p.length; i++) {
          if (p[i] < 128) {
            p2[index++] = p[i];

          } else if (p[i] == 160) {  // unicode for non-breaking space
            p2[index++] = ' ';

          } else {
            int c = p[i];
            p2[index++] = '\\';
            p2[index++] = 'u';
            char str[] = Integer.toHexString(c).toCharArray();
            // add leading zeros, so that the length is 4
            //for (int i = 0; i < 4 - str.length; i++) p2[index++] = '0';
            for (int m = 0; m < 4 - str.length; m++) p2[index++] = '0';
            System.arraycopy(str, 0, p2, index, str.length);
            index += str.length;
          }
        }
        program = new String(p2, 0, index);
      }
    }
    
    // if this guy has his own imports, need to remove them
    // just in case it's not an advanced mode sketch
    PatternMatcher matcher = new Perl5Matcher();
    PatternCompiler compiler = new Perl5Compiler();
    //String mess = "^\\s*(import\\s+\\S+\\s*;)";
    //String mess = "^\\s*(import\\s+)(\\S+)(\\s*;)";
    String mess = "^\\s*#include\\s+[<\"](\\S+)[\">]";
    java.util.Vector imports = new java.util.Vector();

    Pattern pattern = null;
    try {
      pattern = compiler.compile(mess);
    } catch (MalformedPatternException e) {
      e.printStackTrace();
      return null;
    }

    PatternMatcherInput input = new PatternMatcherInput(program);    
    while (matcher.contains(input, pattern)) {
      imports.add(matcher.getMatch().group(1));
    }    

    extraImports = new String[imports.size()];
    imports.copyInto(extraImports);

    // do this after the program gets re-combobulated
    this.programReader = new StringReader(program);
    this.buildPath = buildPath;
    
    List prototypes = prototypes(program);
    
    // store # of prototypes so that line number reporting can be adjusted
    prototypeCount = prototypes.size();
  
    if (name == null) return null;

    // output the code
    File streamFile = new File(buildPath, name + ".cpp");
    PrintStream stream = new PrintStream(new FileOutputStream(streamFile));

    writeHeader(stream);
    //added to write the pde code to the cpp file
    writeProgram(stream, program, prototypes);
    writeFooter(stream, target);
    stream.close();

    return name;
  }

  // Write the pde program to the cpp file
  void writeProgram(PrintStream out, String program, List prototypes) {
    int prototypeInsertionPoint = firstStatement(program);
  
    out.print(program.substring(0, prototypeInsertionPoint));
    out.print("#include \"WProgram.h\"\n");    
    
    // print user defined prototypes
    for (int i = 0; i < prototypes.size(); i++) {
      out.print(prototypes.get(i) + "\n");
    }
    
    out.print(program.substring(prototypeInsertionPoint));
  }


  /**
   * Write any required header material (eg imports, class decl stuff)
   *
   * @param out                 PrintStream to write it to.
   */
  void writeHeader(PrintStream out) throws IOException {}

  /**
   * Write any necessary closing text.
   *
   * @param out         PrintStream to write it to.
   */
  void writeFooter(PrintStream out, Target target) throws java.lang.Exception {
    // Open the file main.cxx and copy its entire contents to the bottom of the
    // generated sketch .cpp file...

    String mainFileName = target.getPath() + File.separator + "main.cxx";
    FileReader reader = null;
    reader = new FileReader(mainFileName);

    LineNumberReader mainfile = new LineNumberReader(reader);

    String line;
    while ((line = mainfile.readLine()) != null) {
        out.print(line + "\n");
    }

    mainfile.close();
  }


  static String advClassName = "";

  /**
   * Find the first CLASS_DEF node in the tree, and return the name of the
   * class in question.
   *
   * XXXdmose right now, we're using a little hack to the grammar to get
   * this info.  In fact, we should be descending the AST passed in.
   */
  String getFirstClassName(AST ast) {

    String t = advClassName;
    advClassName = "";

    return t;
  }

}
