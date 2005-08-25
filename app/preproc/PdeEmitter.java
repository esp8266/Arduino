/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

package processing.app.preproc;

import processing.app.*;


/* Based on original code copyright (c) 2003 Andy Tripp <atripp@comcast.net>.
 * shipped under GPL with permission.
 */

import antlr.*;
import antlr.collections.*;
import antlr.collections.impl.*;
import java.io.*;
import java.util.*;

/**
 * PDEEmitter: A class that can take an ANTLR Java AST and produce
 * reasonably formatted Java code from it. To use it, create a
 * PDEEmitter object, call setOut() if you want to print to something
 * other than System.out, and then call print(), passing the
 * AST. Typically, the AST node that you pass would be the root of a
 * tree - the ROOT_ID node that represents a Java file.
 */

public class PdeEmitter implements PdeTokenTypes
{
  private PrintStream out = System.out;
  private PrintStream debug = System.err;
  private static int ALL = -1;
  private java.util.Stack stack = new java.util.Stack();
  private static String[] tokenNames;
  private final static int ROOT_ID = 0;
  static {
    setupTokenNames();
  }

  /*
  private static Hashtable publicMethods;
  private static final String publicMethodList[] = {
    "setup", "draw", //"loop",
    "mousePressed", "mouseReleased", "mouseClicked",
    "mouseEntered", "mouseExited",
    "mouseMoved", "mouseDragged",
    "keyPressed", "keyReleased", "keyTyped"
  };

  static {
    publicMethods = new Hashtable();
    for (int i = 0; i < publicMethodList.length; i++) {
      publicMethods.put(publicMethodList[i], new Object());
    }
  }
  */

  // Map each AST token type to a String
  private static void setupTokenNames() {
    tokenNames = new String[200];
    for (int i=0; i<tokenNames.length; i++) {
      tokenNames[i] = "ERROR:" + i;
    }

    tokenNames[POST_INC]="++";
    tokenNames[POST_DEC]="--";
    tokenNames[UNARY_MINUS]="-";
    tokenNames[UNARY_PLUS]="+";
    tokenNames[STAR]="*";
    tokenNames[ASSIGN]="=";
    tokenNames[PLUS_ASSIGN]="+=";
    tokenNames[MINUS_ASSIGN]="-=";
    tokenNames[STAR_ASSIGN]="*=";
    tokenNames[DIV_ASSIGN]="/=";
    tokenNames[MOD_ASSIGN]="%=";
    tokenNames[SR_ASSIGN]=">>=";
    tokenNames[BSR_ASSIGN]=">>>=";
    tokenNames[SL_ASSIGN]="<<=";
    tokenNames[BAND_ASSIGN]="&=";
    tokenNames[BXOR_ASSIGN]="^=";
    tokenNames[BOR_ASSIGN]="|=";
    tokenNames[QUESTION]="?";
    tokenNames[LOR]="||";
    tokenNames[LAND]="&&";
    tokenNames[BOR]="|";
    tokenNames[BXOR]="^";
    tokenNames[BAND]="&";
    tokenNames[NOT_EQUAL]="!=";
    tokenNames[EQUAL]="==";
    tokenNames[LT]="<";
    tokenNames[GT]=">";
    tokenNames[LE]="<=";
    tokenNames[GE]=">=";
    tokenNames[SL]="<<";
    tokenNames[SR]=">>";
    tokenNames[BSR]=">>>";
    tokenNames[PLUS]="+";
    tokenNames[MINUS]="-";
    tokenNames[DIV]="/";
    tokenNames[MOD]="%";
    tokenNames[INC]="++";
    tokenNames[DEC]="--";
    tokenNames[BNOT]="~";
    tokenNames[LNOT]="!";
    tokenNames[FINAL]="final";
    tokenNames[ABSTRACT]="abstract";
    tokenNames[LITERAL_package]="package";
    tokenNames[LITERAL_import]="import";
    tokenNames[LITERAL_void]="void";
    tokenNames[LITERAL_boolean]="boolean";
    tokenNames[LITERAL_byte]="byte";
    tokenNames[LITERAL_char]="char";
    tokenNames[LITERAL_short]="short";
    tokenNames[LITERAL_int]="int";
    tokenNames[LITERAL_float]="float";
    tokenNames[LITERAL_long]="long";
    tokenNames[LITERAL_double]="double";
    tokenNames[LITERAL_private]="private";
    tokenNames[LITERAL_public]="public";
    tokenNames[LITERAL_protected]="protected";
    tokenNames[LITERAL_static]="static";
    tokenNames[LITERAL_transient]="transient";
    tokenNames[LITERAL_native]="native";
    tokenNames[LITERAL_threadsafe]="threadsafe";
    tokenNames[LITERAL_synchronized]="synchronized";
    tokenNames[LITERAL_volatile]="volatile";
    tokenNames[LITERAL_class]="class";
    tokenNames[LITERAL_extends]="extends";
    tokenNames[LITERAL_interface]="interface";
    tokenNames[LITERAL_implements]="implements";
    tokenNames[LITERAL_throws]="throws";
    tokenNames[LITERAL_if]="if";
    tokenNames[LITERAL_else]="else";
    tokenNames[LITERAL_for]="for";
    tokenNames[LITERAL_while]="while";
    tokenNames[LITERAL_do]="do";
    tokenNames[LITERAL_break]="break";
    tokenNames[LITERAL_continue]="continue";
    tokenNames[LITERAL_return]="return";
    tokenNames[LITERAL_switch]="switch";
    tokenNames[LITERAL_throw]="throw";
    tokenNames[LITERAL_case]="case";
    tokenNames[LITERAL_default]="default";
    tokenNames[LITERAL_try]="try";
    tokenNames[LITERAL_finally]="finally";
    tokenNames[LITERAL_catch]="catch";
    tokenNames[LITERAL_instanceof]="instanceof";
    tokenNames[LITERAL_this]="this";
    tokenNames[LITERAL_super]="super";
    tokenNames[LITERAL_true]="true";
    tokenNames[LITERAL_false]="false";
    tokenNames[LITERAL_null]="null";
    tokenNames[LITERAL_new]="new";
    tokenNames[LITERAL_color]="int";    // PDE specific alias
  }

  /**
   * Specify a PrintStream to print to. System.out is the default.
   * @param out the PrintStream to print to
   */
  public void setOut(PrintStream out) {
    this.out = out;
  }
  private String name(AST ast) {
    return tokenNames[ast.getType()];
  }
  private String name(int type) {
    return tokenNames[type];
  }

  /**
   * Find a child of the given AST that has the given type
   * @returns a child AST of the given type. If it can't find a child of the
   *          given type, return null.
   */
  private AST getChild(AST ast, int childType) {
    AST child = ast.getFirstChild();
    while (child != null) {
      if (child.getType() == childType) {
        // debug.println("getChild: found:" + name(ast));
        return child;
      }
      child = child.getNextSibling();
    }
    return null;
  }

  /**
   * Dump the list of hidden tokens linked to after the AST node passed in.
   * Most hidden tokens are dumped from this function.
   */
  private void dumpHiddenAfter(AST ast) {
    dumpHiddenTokens(((antlr.CommonASTWithHiddenTokens)ast).getHiddenAfter());
  }

  /**
   * Dump the list of hidden tokens linked to before the AST node passed in.
   * The only time hidden tokens need to be dumped with this function is when
   * dealing parts of the tree where automatic tree construction was
   * turned off with the ! operator in the grammar file and the nodes were
   * manually constructed in such a way that the usual tokens don't have the
   * necessary hiddenAfter links.
   */
  private void dumpHiddenBefore(AST ast) {

    antlr.CommonHiddenStreamToken
      child = null,
      parent = ((antlr.CommonASTWithHiddenTokens)ast).getHiddenBefore();

    // if there aren't any hidden tokens here, quietly return
    //
    if (parent == null) {
      return;
    }

    // traverse back to the head of the list of tokens before this node
    do {
      child = parent;
      parent = child.getHiddenBefore();
    } while (parent != null);

    // dump that list
    dumpHiddenTokens(child);
  }

  /**
   * Dump the list of hidden tokens linked to from the token passed in.
   */
  private void dumpHiddenTokens(antlr.CommonHiddenStreamToken t) {
    for ( ; t != null ; t=PdePreprocessor.filter.getHiddenAfter(t) ) {
      out.print(t.getText());
    }
  }

  /**
   * Print the children of the given AST
   * @param ast The AST to print
   * @returns true iff anything was printed
   */
  private boolean printChildren(AST ast) throws RunnerException {
    boolean ret = false;
    AST child = ast.getFirstChild();
    while (child != null) {
      ret = true;
      print(child);
      child = child.getNextSibling();
    }
    return ret;
  }

  /**
   * Tells whether an AST has any children or not.
   * @return true iff the AST has at least one child
   */
  private boolean hasChildren(AST ast) {
    return (ast.getFirstChild() != null);
  }

  /**
   * Gets the best node in the subtree for printing.  This really means
   * the next node which could potentially have hiddenBefore data.  It's
   * usually the first printable leaf, but not always.
   *
   * @param includeThisNode     Should this node be included in the search?
   *                            If false, only descendants are searched.
   *
   * @return the first printable leaf node in an AST
   */
  private AST getBestPrintableNode(AST ast, boolean includeThisNode) {
    AST child;

    if (includeThisNode) {
      child = ast;
    } else {
      child = ast.getFirstChild();
    }

    if (child != null) {

      switch (child.getType()) {

        // the following node types are printing nodes that print before
        // any children, but then also recurse over children.  So they
        // may have hiddenBefore chains that need to be printed first.  Many
        // statements and all unary expression types qualify.  Return these
        // nodes directly
      case CLASS_DEF:
      case LITERAL_if:
      case LITERAL_for:
      case LITERAL_while:
      case LITERAL_do:
      case LITERAL_break:
      case LITERAL_continue:
      case LITERAL_return:
      case LITERAL_switch:
      case LITERAL_try:
      case LITERAL_throw:
      case LITERAL_synchronized:
      case LITERAL_assert:
      case BNOT:
      case LNOT:
      case INC:
      case DEC:
      case UNARY_MINUS:
      case UNARY_PLUS:
        return child;

        // Some non-terminal node types (at the moment, I only know of
        // MODIFIERS, but there may be other such types), can be
        // leaves in the tree but not have any children.  If this is
        // such a node, move on to the next sibling.
      case MODIFIERS:
        if (child.getFirstChild() == null ) {
          return getBestPrintableNode(child.getNextSibling(), false);
        }
        // new jikes doesn't like fallthrough, so just duplicated here:
        return getBestPrintableNode(child, false);

      default:
        return getBestPrintableNode(child, false);
      }
    }

    return ast;
  }

  /**
   * Prints a binary operator
   */
  private void printBinaryOperator(AST ast) throws RunnerException {
    print(ast.getFirstChild());
    out.print(name(ast));
    dumpHiddenAfter(ast);
    print(ast.getFirstChild().getNextSibling());
  }

  /**
   * Print the given AST. Call this function to print your PDE code.
   *
   * It works by making recursive calls to print children.
   * So the code below is one big "switch" statement on the passed AST type.
   */
  public void print (AST ast) throws RunnerException {
    if (ast == null) {
      return;
    }

    AST parent = null;
    if (!stack.isEmpty()) {
      parent = (AST) stack.peek();
    }
    stack.push(ast);

    AST child1 = ast.getFirstChild();
    AST child2 = null;
    AST child3 = null;
    if (child1 != null) {
      child2 = child1.getNextSibling();
      if (child2 != null) {
        child3 = child2.getNextSibling();
      }
    }

    switch(ast.getType()) {
      // The top of the tree looks like this:
      //  ROOT_ID  "Whatever.java"
      //   package
      //   imports
      //   class definition
    case ROOT_ID:
      dumpHiddenTokens(PdePreprocessor.filter.getInitialHiddenToken());
      printChildren(ast);
      break;

      // supporting a "package" statement in a PDE program has
      // a bunch of issues with it that need to dealt in the compilation
      // code too, so this isn't actually tested.
    case PACKAGE_DEF:
      out.print("package");
      dumpHiddenAfter(ast);
      print(ast.getFirstChild());
      break;

      // IMPORT has exactly one child
    case IMPORT:
      out.print("import");
      dumpHiddenAfter(ast);
      print(ast.getFirstChild());
      break;

    case CLASS_DEF:
    case INTERFACE_DEF:
      print(getChild(ast, MODIFIERS));
      if (ast.getType() == CLASS_DEF) {
        out.print("class");
      } else {
        out.print("interface");
      }
      dumpHiddenBefore(getChild(ast, IDENT));
      print(getChild(ast, IDENT));
      print(getChild(ast, EXTENDS_CLAUSE));
      print(getChild(ast, IMPLEMENTS_CLAUSE));
      print(getChild(ast, OBJBLOCK));
      break;

    case EXTENDS_CLAUSE:
      if (hasChildren(ast)) {
        out.print("extends");
        dumpHiddenBefore(getBestPrintableNode(ast, false));
        printChildren(ast);
      }
      break;

    case IMPLEMENTS_CLAUSE:
      if (hasChildren(ast)) {
        out.print("implements");
        dumpHiddenBefore(getBestPrintableNode(ast, false));
        printChildren(ast);
      }
      break;

      // DOT always has exactly two children.
    case DOT:
      print(child1);
      out.print(".");
      dumpHiddenAfter(ast);
      print(child2);
      break;

    case MODIFIERS:
    case OBJBLOCK:
    case CTOR_DEF:
      //case METHOD_DEF:
    case PARAMETERS:
    case PARAMETER_DEF:
    case VARIABLE_DEF:
    case TYPE:
    case SLIST:
    case ELIST:
    case ARRAY_DECLARATOR:
    case TYPECAST:
    case EXPR:
    case ARRAY_INIT:
    case FOR_INIT:
    case FOR_CONDITION:
    case FOR_ITERATOR:
    case METHOD_CALL:
    case INSTANCE_INIT:
    case INDEX_OP:
    case SUPER_CTOR_CALL:
    case CTOR_CALL:
      printChildren(ast);
      break;

    case METHOD_DEF:
      // kids seem to be: MODIFIERS TYPE setup PARAMETERS
      //AST parent = (AST) stack.peek();
      AST modifiersChild = ast.getFirstChild();
      AST typeChild = modifiersChild.getNextSibling();
      AST methodNameChild = typeChild.getNextSibling();
      AST parametersChild = methodNameChild.getNextSibling();

      // to output, use print(child) on each of the four

      /*
      // 1. figure out if this is setup, draw, or loop
      String methodName = methodNameChild.getText();
      if (publicMethods.get(methodName) != null) {
        // make sure this feller is public
        boolean foundPublic = false;
        AST child = modifiersChild.getFirstChild();
        while (child != null) {
          if (child.getText().equals("public")) {
            foundPublic = true;
            child = null;
          } else {
            //out.print("." + child.getText() + ".");
            child = child.getNextSibling();
          }
        }
        if (!foundPublic) {
          out.print("public ");
        }
      */

      // if this method doesn't have a specifier, make it public
      // (useful for setup/keyPressed/etc)
      boolean foundSpecifier = false;
      AST child = modifiersChild.getFirstChild();
      while (child != null) {
        String childText = child.getText();
        if (childText.equals("public") ||
            childText.equals("protected") ||
            childText.equals("private")) {
          foundSpecifier = true;
          child = null;
        } else {
          //out.print("." + child.getText() + ".");
          child = child.getNextSibling();
        }
      }
      if (!foundSpecifier) {
        out.print("public ");
      }
      printChildren(ast);  // everything is fine
      break;

      // if we have two children, it's of the form "a=0"
      // if just one child, it's of the form "=0" (where the
      // lhs is above this AST).
    case ASSIGN:
      if (child2 != null) {
        print(child1);
        out.print("=");
        dumpHiddenAfter(ast);
        print(child2);
      }
      else {
        out.print("=");
        dumpHiddenAfter(ast);
        print(child1);
      }
      break;

      // binary operators:
    case PLUS:
    case MINUS:
    case DIV:
    case MOD:
    case NOT_EQUAL:
    case EQUAL:
    case LT:
    case GT:
    case LE:
    case GE:
    case LOR:
    case LAND:
    case BOR:
    case BXOR:
    case BAND:
    case SL:
    case SR:
    case BSR:
    case LITERAL_instanceof:
    case PLUS_ASSIGN:
    case MINUS_ASSIGN:
    case STAR_ASSIGN:
    case DIV_ASSIGN:
    case MOD_ASSIGN:
    case SR_ASSIGN:
    case BSR_ASSIGN:
    case SL_ASSIGN:
    case BAND_ASSIGN:
    case BXOR_ASSIGN:
    case BOR_ASSIGN:
      printBinaryOperator(ast);
      break;


    case LITERAL_for:
      out.print(name(ast));
      dumpHiddenAfter(ast);
      printChildren(ast);
      break;

    case POST_INC:
    case POST_DEC:
      print(child1);
      out.print(name(ast));
      dumpHiddenAfter(ast);
      break;

      // unary operators:
    case BNOT:
    case LNOT:
    case INC:
    case DEC:
    case UNARY_MINUS:
    case UNARY_PLUS:
      out.print(name(ast));
      dumpHiddenAfter(ast);
      print(child1);
      break;

    case LITERAL_new:
      out.print("new");
      dumpHiddenAfter(ast);
      print(child1);
      print(child2);
      // "new String[] {...}": the stuff in {} is child3
      if (child3 != null) {
        print(child3);
      }
      break;

    case LITERAL_return:
      out.print("return");
      dumpHiddenAfter(ast);
      print(child1);
      break;

    case STATIC_INIT:
      out.print("static");
      dumpHiddenBefore(getBestPrintableNode(ast, false));
      print(child1);
      break;

    case LITERAL_switch:
      out.print("switch");
      dumpHiddenAfter(ast);
      printChildren(ast);
      break;

    case CASE_GROUP:
      printChildren(ast);
      break;

    case LITERAL_case:
      out.print("case");
      dumpHiddenAfter(ast);
      printChildren(ast);
      break;

    case LITERAL_default:
      out.print("default");
      dumpHiddenAfter(ast);
      printChildren(ast);
      break;

    case NUM_INT:
    case CHAR_LITERAL:
    case STRING_LITERAL:
    case NUM_FLOAT:
      out.print(ast.getText());
      dumpHiddenAfter(ast);
      break;

    case LITERAL_private:
    case LITERAL_public:
    case LITERAL_protected:
    case LITERAL_static:
    case LITERAL_transient:
    case LITERAL_native:
    case LITERAL_threadsafe:
    case LITERAL_synchronized:
    case LITERAL_volatile:
    case FINAL:
    case ABSTRACT:
    case LITERAL_package:
    case LITERAL_void:
    case LITERAL_boolean:
    case LITERAL_byte:
    case LITERAL_char:
    case LITERAL_short:
    case LITERAL_int:
    case LITERAL_float:
    case LITERAL_long:
    case LITERAL_double:
    case LITERAL_true:
    case LITERAL_false:
    case LITERAL_null:
    case SEMI:
    case LITERAL_this:
    case LITERAL_super:
    case LITERAL_continue:
    case LITERAL_break:
      out.print(name(ast));
      dumpHiddenAfter(ast);
      break;

    case EMPTY_STAT:
    case EMPTY_FIELD:
      break;

      // yuck:  Distinguish between "import x.y.*" and "x = 1 * 3"
    case STAR:
      if (hasChildren(ast)) {   // the binary mult. operator
        printBinaryOperator(ast);
      }
      else {    // the special "*" in import:
        out.print("*");
        dumpHiddenAfter(ast);
      }
      break;

    case LITERAL_throws:
      out.print("throws");
      dumpHiddenAfter(ast);
      printChildren(ast);
      break;

    case LITERAL_if:
      out.print("if");
      dumpHiddenAfter(ast);
      print(child1);    // the "if" condition: an EXPR
      print(child2);    // the "then" clause is an SLIST
      if (child3 != null) {
        out.print("else");
        dumpHiddenBefore(getBestPrintableNode(child3, true));
        print(child3);  // optional "else" clause: an SLIST
      }
      break;

    case LITERAL_while:
      out.print("while");
      dumpHiddenAfter(ast);
      printChildren(ast);
      break;

    case LITERAL_do:
      out.print("do");
      dumpHiddenAfter(ast);
      print(child1);            // an SLIST
      out.print("while");
      dumpHiddenBefore(getBestPrintableNode(child2, false));
      print(child2);            // an EXPR
      break;

    case LITERAL_try:
      out.print("try");
      dumpHiddenAfter(ast);
      printChildren(ast);
      break;

    case LITERAL_catch:
      out.print("catch");
      dumpHiddenAfter(ast);
      printChildren(ast);
      break;

      // the first child is the "try" and the second is the SLIST
    case LITERAL_finally:
      out.print("finally");
      dumpHiddenAfter(ast);
      printChildren(ast);
      break;

    case LITERAL_throw:
      out.print("throw");
      dumpHiddenAfter(ast);
      print(child1);
      break;

      // the dreaded trinary operator
    case QUESTION:
      print(child1);
      out.print("?");
      dumpHiddenAfter(ast);
      print(child2);
      print(child3);
      break;

      // pde specific or modified tokens start here

      // Image -> BImage, Font -> BFont as appropriate
    case IDENT:
      /*
      if (ast.getText().equals("Image") &&
          Preferences.getBoolean("preproc.substitute_image")) { //, true)) {
        out.print("BImage");
      } else if (ast.getText().equals("Font") &&
                 Preferences.getBoolean("preproc.substitute_font")) { //, true)) {
        out.print("BFont");
      } else {
      */
      out.print(ast.getText());
      //}
      dumpHiddenAfter(ast);
      break;

      // the color datatype is just an alias for int
    case LITERAL_color:
      out.print("int");
      dumpHiddenAfter(ast);
      break;

    case WEBCOLOR_LITERAL:
      if (ast.getText().length() != 6) {
        System.err.println("Internal error: incorrect length of webcolor " +
                           "literal should have been detected sooner.");
        break;
      }
      out.print("0xff" + ast.getText());
      dumpHiddenAfter(ast);
      break;

      // allow for stuff like int(43.2).
    case CONSTRUCTOR_CAST:

      AST nonTerminalTypeNode = child1;
      AST terminalTypeNode = child1.getFirstChild();
      AST exprToCast = child2;

      /*
        // if this is a string type, add .valueOf()
      if (nonTerminalTypeNode.getType() == PdeRecognizer.TYPE &&
          terminalTypeNode.getText().equals("String")) {

        out.print(terminalTypeNode.getText() + ".valueOf");
        dumpHiddenAfter(terminalTypeNode);
        print(exprToCast);

        // if the expresion to be cast is a string literal, try and parse it.
        //
        // ideally, we'd be able to do this for all expressions with a
        // string type, not just string literals.  however, the parser
        // doesn't currently track expression type, and for full
        // functionality, we'd need to do semantic analysis to handle
        // imports so that we could know the return types of method calls.
        //
      } else if (exprToCast.getFirstChild().getType() == STRING_LITERAL ) {

        switch (terminalTypeNode.getType()) {

        case PdeRecognizer.LITERAL_byte:
          out.print("Byte.parseByte");
          dumpHiddenAfter(terminalTypeNode);
          print(exprToCast);
          break;

        case PdeRecognizer.LITERAL_double:
          out.print("(new Double");
          dumpHiddenAfter(terminalTypeNode);
          out.print(exprToCast.getFirstChild().getText() + ").doubleValue()");
          dumpHiddenAfter(exprToCast.getFirstChild());
          break;

        case PdeRecognizer.LITERAL_float:
          out.print("(new Float");
          dumpHiddenAfter(terminalTypeNode);
          out.print(exprToCast.getFirstChild().getText() + ").floatValue()");
          dumpHiddenAfter(exprToCast.getFirstChild());
          break;

        case PdeRecognizer.LITERAL_int:
        case PdeRecognizer.LITERAL_color:
          out.print("Integer.parseInt");
          dumpHiddenAfter(terminalTypeNode);
          print(exprToCast);
          break;

        case PdeRecognizer.LITERAL_long:
          out.print("Long.parseLong");
          break;

        case PdeRecognizer.LITERAL_short:
          out.print("Short.parseShort");
          break;

        default:
          throw new RunnerException(Compiler.SUPER_BADNESS);
        }

        // for builtin types, use regular casting syntax
      } else {
      */

      // result of below is (int)(4.0
      //out.print("(");
      //out.print(terminalTypeNode.getText() + ")"); // typename
      //dumpHiddenAfter(terminalTypeNode);
      //print(exprToCast);
      //}

      //out.print("(");
      String pooType = terminalTypeNode.getText();
      out.print("PApplet.to" +
                Character.toUpperCase(pooType.charAt(0)) +
                pooType.substring(1));
      dumpHiddenAfter(terminalTypeNode);  // the left paren
      print(exprToCast);
      //out.print("x)");

      break;


      // making floating point literals default to floats, not doubles
    case NUM_DOUBLE:
      out.print(ast.getText());
      if (Preferences.getBoolean("preproc.substitute_floats")) { //, true) ) {
        out.print("f");
      }
      dumpHiddenAfter(ast);
      break;


    default:
      debug.println("Invalid type:" + ast.getType());
      break;


/* The following are tokens, but I don't think JavaRecognizer
   ever produces an AST with one of these types:
   case COMMA:
   case LITERAL_implements:
   case LITERAL_class:
   case LITERAL_extends:
   case EOF:
   case NULL_TREE_LOOKAHEAD:
   case BLOCK:
   case LABELED_STAT:   // refuse to implement on moral grounds :)
   case LITERAL_import:
   case LBRACK:
   case RBRACK:
   case LCURLY:
   case RCURLY:
   case LPAREN:
   case RPAREN:
   case LITERAL_else:   // else is a child of "if" AST
   case COLON:          // part of the trinary operator
   case WS:             // whitespace
   case ESC:
   case HEX_DIGIT:
   case VOCAB:

   case EXPONENT:       // exponents and float suffixes are left in the NUM_FLOAT
   case FLOAT_SUFFIX
*/
    }

    stack.pop();
  }
}
