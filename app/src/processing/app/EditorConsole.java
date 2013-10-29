/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-06 Ben Fry and Casey Reas
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
import static processing.app.I18n._;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.text.*;

import java.util.*;


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
  BufferedStyledDocument consoleDoc;

  MutableAttributeSet stdStyle;
  MutableAttributeSet errStyle;

  int maxLineCount;

  static File errFile;
  static File outFile;
  static File tempFolder;

  // Single static instance shared because there's only one real System.out.
  // Within the input handlers, the currentConsole variable will be used to
  // echo things to the correct location.
  
  static public PrintStream systemOut;
  static public PrintStream systemErr;

  static PrintStream consoleOut;
  static PrintStream consoleErr;

  static OutputStream stdoutFile;
  static OutputStream stderrFile;

  static EditorConsole currentConsole;
  

  public EditorConsole(Editor editor) {
    this.editor = editor;

    maxLineCount = Preferences.getInteger("console.length");

    consoleDoc = new BufferedStyledDocument(10000, maxLineCount);
    consoleTextPane = new JTextPane(consoleDoc);
    consoleTextPane.setEditable(false);

    // necessary?
    MutableAttributeSet standard = new SimpleAttributeSet();
    StyleConstants.setAlignment(standard, StyleConstants.ALIGN_LEFT);
    consoleDoc.setParagraphAttributes(0, 0, standard, true);

    // build styles for different types of console output
    Color bgColor    = Theme.getColor("console.color");
    Color fgColorOut = Theme.getColor("console.output.color");
    Color fgColorErr = Theme.getColor("console.error.color");
    Font consoleFont = Theme.getFont("console.font");
    Font editorFont = Preferences.getFont("editor.font");
    Font font = new Font(consoleFont.getName(), consoleFont.getStyle(), editorFont.getSize());

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

      // Create a temporary folder which will have a randomized name. Has to 
      // be randomized otherwise another instance of Processing (or one of its 
      // sister IDEs) might collide with the file causing permissions problems. 
      // The files and folders are not deleted on exit because they may be 
      // needed for debugging or bug reporting.
      tempFolder = Base.createTempFolder("console");
      tempFolder.deleteOnExit();
      try {
        String outFileName = Preferences.get("console.output.file");
        if (outFileName != null) {
          outFile = new File(tempFolder, outFileName);
          outFile.deleteOnExit();
          stdoutFile = new FileOutputStream(outFile);
        }

        String errFileName = Preferences.get("console.error.file");
        if (errFileName != null) {
          errFile = new File(tempFolder, errFileName);
          errFile.deleteOnExit();
          stderrFile = new FileOutputStream(errFile);
        }
      } catch (IOException e) {
        Base.showWarning(_("Console Error"),
                         _("A problem occurred while trying to open the\nfiles used to store the console output."), e);
      }
      consoleOut = new PrintStream(new EditorConsoleStream(false));
      consoleErr = new PrintStream(new EditorConsoleStream(true));
    
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

    // periodically post buffered messages to the console
    // should the interval come from the preferences file?
    new javax.swing.Timer(250, new ActionListener() {
      public void actionPerformed(ActionEvent evt) {
        SwingUtilities.invokeLater(new Runnable() {
          @Override
          public void run() {
            // only if new text has been added
            if (consoleDoc.hasAppendage) {
              // insert the text that's been added in the meantime
              consoleDoc.insertAll();
              // always move to the end of the text as it's added
              consoleTextPane.setCaretPosition(consoleDoc.getLength());
            }
          }
        });
      }
    }).start();
  }

  
  static public void setEditor(Editor editor) {
    currentConsole = editor.console;
  }
  

  /**
   * Close the streams so that the temporary files can be deleted.
   * <p/>
   * File.deleteOnExit() cannot be used because the stdout and stderr
   * files are inside a folder, and have to be deleted before the
   * folder itself is deleted, which can't be guaranteed when using
   * the deleteOnExit() method.
   */
  public void handleQuit() {
    // replace original streams to remove references to console's streams
    System.setOut(systemOut);
    System.setErr(systemErr);

    // close the PrintStream
    consoleOut.close();
    consoleErr.close();

    // also have to close the original FileOutputStream
    // otherwise it won't be shut down completely
    try {
      stdoutFile.close();
      stderrFile.close();
    } catch (IOException e) {
      e.printStackTrace(systemOut);
    }

    outFile.delete();
    errFile.delete();
    tempFolder.delete();
  }


  public void write(byte b[], int offset, int length, boolean err) {
    // we could do some cross platform CR/LF mangling here before outputting
    // add text to output document
    message(new String(b, offset, length), err, false);
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
   * Append a piece of text to the console.
   * <P>
   * Swing components are NOT thread-safe, and since the MessageSiphon
   * instantiates new threads, and in those callbacks, they often print
   * output to stdout and stderr, which are wrapped by EditorConsoleStream
   * and eventually leads to EditorConsole.appendText(), which directly
   * updates the Swing text components, causing deadlock.
   * <P>
   * Updates are buffered to the console and displayed at regular
   * intervals on Swing's event-dispatching thread. (patch by David Mellis)
   */
  synchronized private void appendText(String txt, boolean e) {
    consoleDoc.appendString(txt, e ? errStyle : stdStyle);
  }


  public void clear() {
    try {
      consoleDoc.remove(0, consoleDoc.getLength());
    } catch (BadLocationException e) {
      // ignore the error otherwise this will cause an infinite loop
      // maybe not a good idea in the long run?
    }
  }
  
  
  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
  
  
  private static class EditorConsoleStream extends OutputStream {
    //static EditorConsole current;
    final boolean err; // whether stderr or stdout
    final byte single[] = new byte[1];

    public EditorConsoleStream(boolean err) {
      this.err = err;
    }

    public void close() { }

    public void flush() { }

    public void write(byte b[]) {  // appears never to be used
      if (currentConsole != null) {
        currentConsole.write(b, 0, b.length, err);
      } else {
        try {
          if (err) {
            systemErr.write(b);
          } else {
            systemOut.write(b);
          }
        } catch (IOException e) { }  // just ignore, where would we write?
      }

      OutputStream echo = err ? stderrFile : stdoutFile;
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

    public void write(byte b[], int offset, int length) {
      if (currentConsole != null) {
        currentConsole.write(b, offset, length, err);
      } else {
        if (err) {
          systemErr.write(b, offset, length);
        } else {
          systemOut.write(b, offset, length);
        }
      }

      OutputStream echo = err ? stderrFile : stdoutFile;
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
      if (currentConsole != null) {
        currentConsole.write(single, 0, 1, err);
      } else {
        // redirect for all the extra handling above
        write(new byte[] { (byte) b }, 0, 1);
      }

      OutputStream echo = err ? stderrFile : stdoutFile;
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
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


/**
 * Buffer updates to the console and output them in batches. For info, see:
 * http://java.sun.com/products/jfc/tsc/articles/text/element_buffer and
 * http://javatechniques.com/public/java/docs/gui/jtextpane-speed-part2.html
 * appendString() is called from multiple threads, and insertAll from the
 * swing event thread, so they need to be synchronized
 */
class BufferedStyledDocument extends DefaultStyledDocument {
  ArrayList<ElementSpec> elements = new ArrayList<ElementSpec>();
  int maxLineLength, maxLineCount;
  int currentLineLength = 0;
  boolean needLineBreak = false;
  boolean hasAppendage = false;

  public BufferedStyledDocument(int maxLineLength, int maxLineCount) {
    this.maxLineLength = maxLineLength;
    this.maxLineCount = maxLineCount;
  }

  /** buffer a string for insertion at the end of the DefaultStyledDocument */
  public synchronized void appendString(String str, AttributeSet a) {
    // do this so that it's only updated when needed (otherwise console
    // updates every 250 ms when an app isn't even running.. see bug 180)
    hasAppendage = true;

    // process each line of the string
    while (str.length() > 0) {
      // newlines within an element have (almost) no effect, so we need to
      // replace them with proper paragraph breaks (start and end tags)
      if (needLineBreak || currentLineLength > maxLineLength) {
        elements.add(new ElementSpec(a, ElementSpec.EndTagType));
        elements.add(new ElementSpec(a, ElementSpec.StartTagType));
        currentLineLength = 0;
      }

      if (str.indexOf('\n') == -1) {
        elements.add(new ElementSpec(a, ElementSpec.ContentType,
          str.toCharArray(), 0, str.length()));
        currentLineLength += str.length();
        needLineBreak = false;
        str = str.substring(str.length()); // eat the string
      } else {
        elements.add(new ElementSpec(a, ElementSpec.ContentType,
          str.toCharArray(), 0, str.indexOf('\n') + 1));
        needLineBreak = true;
        str = str.substring(str.indexOf('\n') + 1); // eat the line
      }
    }
  }

  /** insert the buffered strings */
  public synchronized void insertAll() {
    ElementSpec[] elementArray = new ElementSpec[elements.size()];
    elements.toArray(elementArray);

    try {
      // check how many lines have been used so far
      // if too many, shave off a few lines from the beginning
      Element element = super.getDefaultRootElement();
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
        super.remove(0, endOffset);
      }
      super.insert(super.getLength(), elementArray);

    } catch (BadLocationException e) {
      // ignore the error otherwise this will cause an infinite loop
      // maybe not a good idea in the long run?
    }
    elements.clear();
    hasAppendage = false;
  }
}
