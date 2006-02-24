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
//import processing.core.*;

import java.io.*;

import antlr.*;
import antlr.collections.*;
import antlr.collections.impl.*;

import com.oroinc.text.regex.*;


public class PdePreprocessor {

  static final int JDK11 = 0;
  static final int JDK13 = 1;
  static final int JDK14 = 2;

  static String defaultImports[][] = new String[3][];

  // these ones have the .* at the end, since a class name
  // might be at the end instead of .* whcih would make trouble
  // other classes using this can lop of the . and anything after
  // it to produce a package name consistently.
  public String extraImports[];

  // imports just from the code folder, treated differently
  // than the others, since the imports are auto-generated.
  public String codeFolderImports[];

  static public final int STATIC = 0;  // formerly BEGINNER
  static public final int ACTIVE = 1;  // formerly INTERMEDIATE
  static public final int JAVA   = 2;  // formerly ADVANCED
  // static to make it easier for the antlr preproc to get at it
  static public int programType = -1;

  Reader programReader;
  String buildPath;

  // used for calling the ASTFactory to get the root node
  private static final int ROOT_ID = 0;
  
  // stores number of built user-defined function prototypes
  public int prototypeCount = 0;

  /**
   * These may change in-between (if the prefs panel adds this option)
   * so grab them here on construction.
   */
  public PdePreprocessor() {
/*    defaultImports[JDK11] =
      Base.split(Preferences.get("preproc.imports.jdk11"), ',');
    defaultImports[JDK13] =
      Base.split(Preferences.get("preproc.imports.jdk13"), ',');
    defaultImports[JDK14] =
      Base.split(Preferences.get("preproc.imports.jdk14"), ',');
*/  }


  /**
   * Used by PdeEmitter.dumpHiddenTokens()
   */
  //public static TokenStreamCopyingHiddenTokenFilter filter;


  /**
   * preprocesses a pde file and write out a java file
   * @return the classname of the exported Java
   */
  //public String write(String program, String buildPath, String name,
  //                  String extraImports[]) throws java.lang.Exception {
  public String write(String program, String buildPath,
                      String name, String codeFolderPackages[])
    throws java.lang.Exception {
    // if the program ends with no CR or LF an OutOfMemoryError will happen.
    // not gonna track down the bug now, so here's a hack for it:
    if ((program.length() > 0) &&
        program.charAt(program.length()-1) != '\n') {
      program += "\n";
    }

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
    String mess = "^\\s*(import\\s+)(\\S+)(\\s*;)";
    java.util.Vector imports = new java.util.Vector();

    Pattern pattern = null;
    try {
      pattern = compiler.compile(mess);
    } catch (MalformedPatternException e) {
      e.printStackTrace();
      return null;
    }

    /*
    do {
      PatternMatcherInput input = new PatternMatcherInput(program);
      if (!matcher.contains(input, pattern)) break;

      MatchResult result = matcher.getMatch();
      String piece1 = result.group(1).toString();
      String piece2 = result.group(2).toString();  // the package name
      String piece3 = result.group(3).toString();
      String piece = piece1 + piece2 + piece3;
      int len = piece.length();

      //imports.add(piece);
      imports.add(piece2);
      int idx = program.indexOf(piece);
      // just remove altogether?
      program = program.substring(0, idx) + program.substring(idx + len);

      //System.out.println("removing " + piece);

    } while (true);
    */

    extraImports = new String[imports.size()];
    imports.copyInto(extraImports);

    // if using opengl, add it to the special imports
    /*
    if (Preferences.get("renderer").equals("opengl")) {
      extraImports = new String[imports.size() + 1];
      imports.copyInto(extraImports);
      extraImports[extraImports.length - 1] = "processing.opengl.*";
    }
    */

    /*
    if (codeFolderPackages != null) {
      extraImports = new String[importsCount + codeFolderPackages.length];
      imports.copyInto(extraImports);
      for (int i = 0; i < codeFolderPackages.length; i++) {
        extraImports[importsCount + i] = codeFolderPackages[i] + ".*";
      }
      codeFolderImports = null;
    }
    */

    if (codeFolderPackages != null) {
      codeFolderImports = new String[codeFolderPackages.length];
      for (int i = 0; i < codeFolderPackages.length; i++) {
        codeFolderImports[i] = codeFolderPackages[i] + ".*";
      }
    } else {
      codeFolderImports = null;
    }

    // do this after the program gets re-combobulated
    this.programReader = new StringReader(program);
    this.buildPath = buildPath;
    
    // create function prototypes
    mess = "^(\\w+)\\s+(\\w+)\\s*\\(([^)]*)\\)\\s*{";
    pattern = null;
    try {
      pattern = compiler.compile(mess);
    } catch (MalformedPatternException e) {
      e.printStackTrace();
      return null;
    }
    PatternMatcherInput input = new PatternMatcherInput(program);
    MatchResult result;
    String returntype, functioname, parameterlist, prototype;
    java.util.LinkedList prototypes = new java.util.LinkedList();
    //System.out.println("prototypes:");
    //if (Preferences.get("build.extension").equals("cpp")) {
      while(matcher.contains(input, pattern)){
        result = matcher.getMatch();
        //System.out.println(result);
        returntype = result.group(1).toString();
        functioname = result.group(2).toString();
        parameterlist = result.group(3).toString().replace('\n', ' ');
        prototype = returntype + " " + functioname + "(" + parameterlist + ");";
        if(0 == functioname.compareTo("setup")){
          continue;
        }
        if(0 == functioname.compareTo("loop")){
          continue;
        }
        prototypes.add(prototype);
        //System.out.println(prototype);
      }
    //}
    // store # of prototypes so that line number reporting can be adjusted
    prototypeCount = prototypes.size();
  

    // create a lexer with the stream reader, and tell it to handle
    // hidden tokens (eg whitespace, comments) since we want to pass these
    // through so that the line numbers when the compiler reports errors
    // match those that will be highlighted in the PDE IDE
    //
    WLexer lexer  = new WLexer(programReader);
    //lexer.setTokenObjectClass("antlr.CommonHiddenStreamToken");
    lexer.setTokenObjectClass("processing.app.preproc.CToken");
    lexer.initialize();

    // create the filter for hidden tokens and specify which tokens to
    // hide and which to copy to the hidden text
    //
    /*filter = new TokenStreamCopyingHiddenTokenFilter(lexer);
    filter.hide(WParser.CPPComment);
    filter.hide(WParser.Comment);
    filter.hide(WParser.Whitespace);
    filter.copy(WParser.SEMI);
    filter.copy(WParser.LPAREN);
    filter.copy(WParser.RPAREN);
    filter.copy(WParser.LCURLY);
    filter.copy(WParser.RCURLY);
    filter.copy(WParser.COMMA);
    filter.copy(WParser.RBRACK);
    filter.copy(WParser.LBRACK);
    filter.copy(WParser.COLON);
    */
    // create a parser and set what sort of AST should be generated
    //
    //PdeRecognizer parser = new PdeRecognizer(filter);
    WParser parser = new WParser(lexer);

    // use our extended AST class
    //
    //parser.setASTNodeClass("antlr.ExtendedCommonASTWithHiddenTokens");
    parser.setASTNodeType(TNode.class.getName());
    TNode.setTokenVocabulary("processing.app.preproc.WTokenTypes");

    // start parsing at the compilationUnit non-terminal
    //
    //parser.pdeProgram();
    //parser.translationUnit();

    // set up the AST for traversal by PdeEmitter
    //
    ASTFactory factory = new ASTFactory();
    AST parserAST = parser.getAST();
    AST rootNode = factory.create(ROOT_ID, "AST ROOT");
    rootNode.setFirstChild(parserAST);

    // unclear if this actually works, but it's worth a shot
    //
    //((CommonAST)parserAST).setVerboseStringConversion(
    //  true, parser.getTokenNames());
    // (made to use the static version because of jikes 1.22 warning)
    CommonAST.setVerboseStringConversion(true, parser.getTokenNames());

    // if this is an advanced program, the classname is already defined.
    //
    if (programType == JAVA) {
      name = getFirstClassName(parserAST);
    }

    // if 'null' was passed in for the name, but this isn't
    // a 'java' mode class, then there's a problem, so punt.
    //
    if (name == null) return null;

    // output the code
    //
    WEmitter emitter = new WEmitter(lexer.getPreprocessorInfoChannel());
    //File streamFile = new File(buildPath, name + "." + Preferences.get("build.extension"));
    File streamFile = new File(buildPath, name + ".cpp");
    PrintStream stream = new PrintStream(new FileOutputStream(streamFile));

    //writeHeader(stream, extraImports, name);
    writeHeader(stream, name, prototypes);
    //added to write the pde code to the cpp file
    writeProgram(stream, name, program);
    emitter.setASTNodeType(TNode.class.getName());
    emitter.setOut(stream);
    //emitter.printDeclarations(rootNode);
    //emitter.print(rootNode);
    //emitter.translationUnit(parser.getAST());

    writeFooter(stream);
    stream.close();

    // if desired, serialize the parse tree to an XML file.  can
    // be viewed usefully with Mozilla or IE

    if (Preferences.getBoolean("preproc.output_parse_tree")) {

      stream = new PrintStream(new FileOutputStream("parseTree.xml"));
      stream.println("<?xml version=\"1.0\"?>");
      stream.println("<document>");
      OutputStreamWriter writer = new OutputStreamWriter(stream);
      if (parserAST != null) {
        ((CommonAST)parserAST).xmlSerialize(writer);
      }
      writer.flush();
      stream.println("</document>");
      writer.close();
    }

    return name;
  }

  // Write the pde program to the cpp file
  void writeProgram(PrintStream out, String className, String program) {
    out.print(program);
  }


  /**
   * Write any required header material (eg imports, class decl stuff)
   *
   * @param out                 PrintStream to write it to.
   * @param exporting           Is this being exported from PDE?
   * @param name                Name of the class being created.
   */
  void writeHeader(PrintStream out, String className, java.util.LinkedList prototypes) {
    out.print("#include \"WProgram.h\"\n");

    // print user defined prototypes
    while(0 < prototypes.size()){
      out.print(prototypes.removeFirst() + "\n");
    }

    // emit emports that are needed for classes from the code folder
    if (extraImports != null) {
      for (int i = 0; i < extraImports.length; i++) {
        out.print("#include \"" + extraImports[i] + "\"\n");
      }
    }

    if (codeFolderImports != null) {
      for (int i = 0; i < codeFolderImports.length; i++) {
        out.print("import " + codeFolderImports[i] + "; ");
      }
    }

    // emit standard imports (read from pde.properties)
    // for each language level that's being used.
/*    String jdkVersionStr = Preferences.get("preproc.jdk_version");

    int jdkVersion = JDK11;  // default
    if (jdkVersionStr.equals("1.3")) { jdkVersion = JDK13; };
    if (jdkVersionStr.equals("1.4")) { jdkVersion = JDK14; };

    for (int i = 0; i <= jdkVersion; i++) {
      for (int j = 0; j < defaultImports[i].length; j++) {
        out.print("import " + defaultImports[i][j] + ".*; ");
      }
    }

    //boolean opengl = Preferences.get("renderer").equals("opengl");
    //if (opengl) {
    //out.println("import processing.opengl.*; ");
    //}
*/
/*    if (programType < JAVA) {
      // open the class definition
      out.print("public class " + className + " extends ");
      //if (opengl) {
      //out.print("PAppletGL");
      //} else {
      out.print("PApplet");
      //}
      out.print(" {");

      if (programType == STATIC) {
        // now that size() and background() can go inside of draw()
        // actually, use setup(), because when running externally
        // the applet size needs to be set before the window is drawn,
        // meaning that after setup() things need to be ducky.
        //out.print("public void draw() {");
        out.print("public void setup() {");
      }
    }
*/  }

  /**
   * Write any necessary closing text.
   *
   * @param out         PrintStream to write it to.
   */
  void writeFooter(PrintStream out) {
    //out.print("}");

/*    if (programType == STATIC) {
      // close off draw() definition
      out.print("noLoop(); ");
      out.print("}");
    }

    if (programType < JAVA) {
      // close off the class definition
      out.print("}");
    }
*/  }


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
