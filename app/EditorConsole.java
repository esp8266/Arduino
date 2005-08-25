/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

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

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.text.*;


/**
 * Message console that sits below the editing area.
 * <P>
 * Debugging this class is tricky... If it's throwing exceptions,
 * don't take over System.err, and debug while watching just System.out
 * or just write println() or whatever directly to systemOut or systemErr.
 */
public class EditorConsole extends JScrollPane {
  Editor editor;

  JTextPane consoleTextPane;
  StyledDocument consoleDoc;

  MutableAttributeSet stdStyle;
  MutableAttributeSet errStyle;

  boolean cerror;

  //int maxCharCount;
  int maxLineCount;

  static PrintStream systemOut;
  static PrintStream systemErr;

  static PrintStream consoleOut;
  static PrintStream consoleErr;

  static OutputStream stdoutFile;
  static OutputStream stderrFile;


  public EditorConsole(Editor editor) {
    this.editor = editor;

    maxLineCount = Preferences.getInteger("console.length");

    consoleTextPane = new JTextPane();
    consoleTextPane.setEditable(false);
    consoleDoc = consoleTextPane.getStyledDocument();

    // necessary?
    MutableAttributeSet standard = new SimpleAttributeSet();
    StyleConstants.setAlignment(standard, StyleConstants.ALIGN_LEFT);
    consoleDoc.setParagraphAttributes(0, 0, standard, true);

    // build styles for different types of console output
    Color bgColor    = Preferences.getColor("console.color");
    Color fgColorOut = Preferences.getColor("console.output.color");
    Color fgColorErr = Preferences.getColor("console.error.color");
    Font font        = Preferences.getFont("console.font");

    stdStyle = new SimpleAttributeSet();
    StyleConstants.setForeground(stdStyle, fgColorOut);
    StyleConstants.setBackground(stdStyle, bgColor);
    StyleConstants.setFontSize(stdStyle, font.getSize());
    StyleConstants.setFontFamily(stdStyle, font.getFamily());
    StyleConstants.setBold(stdStyle, font.isBold());
    StyleConstants.setItalic(stdStyle, font.isItalic());

    errStyle = new SimpleAttributeSet();
    StyleConstants.setForeground(errStyle, fgColorErr);
    StyleConstants.setBackground(errStyle, bgColor);
    StyleConstants.setFontSize(errStyle, font.getSize());
    StyleConstants.setFontFamily(errStyle, font.getFamily());
    StyleConstants.setBold(errStyle, font.isBold());
    StyleConstants.setItalic(errStyle, font.isItalic());

    consoleTextPane.setBackground(bgColor);

    // add the jtextpane to this scrollpane
    this.setViewportView(consoleTextPane);

    // calculate height of a line of text in pixels
    // and size window accordingly
    FontMetrics metrics = this.getFontMetrics(font);
    int height = metrics.getAscent() + metrics.getDescent();
    int lines = Preferences.getInteger("console.lines"); //, 4);
    int sizeFudge = 6; //10; // unclear why this is necessary, but it is
    setPreferredSize(new Dimension(1024, (height * lines) + sizeFudge));
    setMinimumSize(new Dimension(1024, (height * 4) + sizeFudge));

    if (systemOut == null) {
      systemOut = System.out;
      systemErr = System.err;

      try {
        String outFileName = Preferences.get("console.output.file");
        if (outFileName != null) {
          stdoutFile = new FileOutputStream(outFileName);
        }

        String errFileName = Preferences.get("console.error.file");
        if (errFileName != null) {
          stderrFile = new FileOutputStream(outFileName);
        }
      } catch (IOException e) {
        Base.showWarning("Console Error",
                         "A problem occurred while trying to open the\n" +
                         "files used to store the console output.", e);
      }

      consoleOut =
        new PrintStream(new EditorConsoleStream(this, false, stdoutFile));
      consoleErr =
        new PrintStream(new EditorConsoleStream(this, true, stderrFile));

      if (Preferences.getBoolean("console")) {
        try {
          System.setOut(consoleOut);
          System.setErr(consoleErr);
        } catch (Exception e) {
          e.printStackTrace(systemOut);
        }
      }
    }

    // to fix ugliness.. normally macosx java 1.3 puts an
    // ugly white border around this object, so turn it off.
    if (Base.isMacOS()) {
      setBorder(null);
    }
  }


  public void write(byte b[], int offset, int length, boolean err) {
    if (err != cerror) {
      // advance the line because switching between err/out streams
      // potentially, could check whether we're already on a new line
      message("", cerror, true);
    }

    // we could do some cross platform CR/LF mangling here before outputting

    // add text to output document
    message(new String(b, offset, length), err, false);
    // set last error state
    cerror = err;
  }


  // added sync for 0091.. not sure if it helps or hinders
  synchronized public void message(String what, boolean err, boolean advance) {
    if (err) {
      systemErr.print(what);
      //systemErr.print("CE" + what);
    } else {
      systemOut.print(what);
      //systemOut.print("CO" + what);
    }

    if (advance) {
      appendText("\n", err);
      if (err) {
        systemErr.println();
      } else {
        systemOut.println();
      }
    }

    // to console display
    appendText(what, err);
    // moved down here since something is punting
  }


  /**
   * append a piece of text to the console.
   * <P>
   * Swing components are NOT thread-safe, and since the MessageSiphon
   * instantiates new threads, and in those callbacks, they often print
   * output to stdout and stderr, which are wrapped by EditorConsoleStream
   * and eventually leads to EditorConsole.appendText(), which directly
   * updates the Swing text components, causing deadlock.
   * <P>
   * A quick hack from Francis Li (who found this to be a problem)
   * wraps the contents of appendText() into a Runnable and uses
   * SwingUtilities.invokeLater() to ensure that the updates only
   * occur on the main event dispatching thread, and that appears
   * to have solved the problem.
   * <P>
   * unfortunately this is probably extremely slow and helping cause
   * some of the general print() and println() mess.. need to fix
   * up so that it's using a proper queue instead.
   */
  synchronized private void appendText(String txt, boolean e) {
    final String text = txt;
    final boolean err = e;
    SwingUtilities.invokeLater(new Runnable() {
        public void run() {
          try {
            // check how many lines have been used so far
            // if too many, shave off a few lines from the beginning
            Element element = consoleDoc.getDefaultRootElement();
            int lineCount = element.getElementCount();
            int overage = lineCount - maxLineCount;
            if (overage > 0) {
              // if 1200 lines, and 1000 lines is max,
              // find the position of the end of the 200th line
              //systemOut.println("overage is " + overage);
              Element lineElement = element.getElement(overage);
              if (lineElement == null) return;  // do nuthin

              int endOffset = lineElement.getEndOffset();
              // remove to the end of the 200th line
              consoleDoc.remove(0, endOffset);
            }

            // make sure this line doesn't go over 32k chars
            lineCount = element.getElementCount(); // may have changed
            Element currentElement = element.getElement(lineCount-1);
            int currentStart = currentElement.getStartOffset();
            int currentEnd = currentElement.getEndOffset();
            //systemOut.println(currentEnd - currentStart);
            if (currentEnd - currentStart > 10000) {   // force a newline
              consoleDoc.insertString(consoleDoc.getLength(), "\n",
                                      err ? errStyle : stdStyle);
            }

            // add the text to the end of the console,
            consoleDoc.insertString(consoleDoc.getLength(), text,
                                    err ? errStyle : stdStyle);

            // always move to the end of the text as it's added
            consoleTextPane.setCaretPosition(consoleDoc.getLength());

          } catch (BadLocationException e) {
            // ignore the error otherwise this will cause an infinite loop
            // maybe not a good idea in the long run?
          }
        }
      });
  }


  public void clear() {
    try {
      consoleDoc.remove(0, consoleDoc.getLength());
    } catch (BadLocationException e) {
      // ignore the error otherwise this will cause an infinite loop
      // maybe not a good idea in the long run?
    }
  }
}


class EditorConsoleStream extends OutputStream {
  EditorConsole parent;
  boolean err; // whether stderr or stdout
  byte single[] = new byte[1];
  OutputStream echo;

  public EditorConsoleStream(EditorConsole parent,
                             boolean err, OutputStream echo) {
    this.parent = parent;
    this.err = err;
    this.echo = echo;
  }

  public void close() { }

  public void flush() { }

  public void write(byte b[]) {  // appears never to be used
    parent.write(b, 0, b.length, err);
    if (echo != null) {
      try {
        echo.write(b); //, 0, b.length);
        echo.flush();
      } catch (IOException e) {
        e.printStackTrace();
        echo = null;
      }
    }
  }

  public void write(byte b[], int offset, int length) {
    parent.write(b, offset, length, err);
    if (echo != null) {
      try {
        echo.write(b, offset, length);
        echo.flush();
      } catch (IOException e) {
        e.printStackTrace();
        echo = null;
      }
    }
  }

  public void write(int b) {
    single[0] = (byte)b;
    parent.write(single, 0, 1, err);
    if (echo != null) {
      try {
        echo.write(b);
        echo.flush();
      } catch (IOException e) {
        e.printStackTrace();
        echo = null;
      }
    }
  }
}
