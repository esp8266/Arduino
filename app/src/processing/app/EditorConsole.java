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

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.SwingUtilities;
import javax.swing.Timer;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.Element;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;

import processing.app.helpers.OSUtils;


/**
 * Message console that sits below the editing area.
 * <P>
 * Debugging this class is tricky... If it's throwing exceptions,
 * don't take over System.err, and debug while watching just System.out
 * or just write println() or whatever directly to systemOut or systemErr.
 */
@SuppressWarnings("serial")
public class EditorConsole extends JScrollPane {
  Editor editor;

  JTextPane consoleTextPane;
  BufferedStyledDocument consoleDoc;

  SimpleAttributeSet stdStyle;
  SimpleAttributeSet errStyle;

  // Single static instance shared because there's only one real System.out.
  // Within the input handlers, the currentConsole variable will be used to
  // echo things to the correct location.
  
  public EditorConsole(Editor _editor) {
    editor = _editor;

    int maxLineCount = Preferences.getInteger("console.length");

    consoleDoc = new BufferedStyledDocument(4000, maxLineCount);
    consoleTextPane = new JTextPane(consoleDoc);
    consoleTextPane.setEditable(false);

    // necessary?
    SimpleAttributeSet leftAlignAttr = new SimpleAttributeSet();
    StyleConstants.setAlignment(leftAlignAttr, StyleConstants.ALIGN_LEFT);
    consoleDoc.setParagraphAttributes(0, 0, leftAlignAttr, true);

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
    setViewportView(consoleTextPane);

    // calculate height of a line of text in pixels
    // and size window accordingly
    FontMetrics metrics = getFontMetrics(font);
    int height = metrics.getAscent() + metrics.getDescent();
    int lines = Preferences.getInteger("console.lines");
    int sizeFudge = 6; //10; // unclear why this is necessary, but it is
    setPreferredSize(new Dimension(1024, (height * lines) + sizeFudge));
    setMinimumSize(new Dimension(1024, (height * 4) + sizeFudge));

    EditorConsoleStream.init();

    // to fix ugliness.. normally macosx java 1.3 puts an
    // ugly white border around this object, so turn it off.
    if (OSUtils.isMacOS()) {
      setBorder(null);
    }

    // periodically post buffered messages to the console
    // should the interval come from the preferences file?
    new Timer(250, new ActionListener() {
      public void actionPerformed(ActionEvent evt) {
        SwingUtilities.invokeLater(new Runnable() {
          @Override
          public void run() {
            // only if new text has been added
            if (consoleDoc.isChanged()) {
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
  synchronized void appendText(String txt, boolean e) {
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
}


// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


/**
 * Buffer updates to the console and output them in batches. For info, see:
 * http://java.sun.com/products/jfc/tsc/articles/text/element_buffer and
 * http://javatechniques.com/public/java/docs/gui/jtextpane-speed-part2.html
 * appendString() is called from multiple threads, and insertAll from the
 * swing event thread, so they need to be synchronized
 */
@SuppressWarnings("serial")
class BufferedStyledDocument extends DefaultStyledDocument {
  private List<ElementSpec> elements = new ArrayList<ElementSpec>();
  private int maxLineLength, maxLineCount;
  private int currentLineLength = 0;
  private boolean changed = false;

  public BufferedStyledDocument(int _maxLineLength, int _maxLineCount) {
    maxLineLength = _maxLineLength;
    maxLineCount = _maxLineCount;
  }

  /** buffer a string for insertion at the end of the DefaultStyledDocument */
  public synchronized void appendString(String text, AttributeSet a) {
    changed = true;
    char[] chars = text.toCharArray();
    int start = 0;
    int stop = 0;
    while (stop < chars.length) {
      char c = chars[stop];
      stop++;
      currentLineLength++;
      if (c == '\n' || c == '\r' || currentLineLength > maxLineLength) {
        elements.add(new ElementSpec(a, ElementSpec.ContentType, chars, start,
            stop - start));
        elements.add(new ElementSpec(a, ElementSpec.EndTagType));
        elements.add(new ElementSpec(a, ElementSpec.StartTagType));
        currentLineLength = 0;
        start = stop;
      }
    }
    elements.add(new ElementSpec(a, ElementSpec.ContentType, chars, start,
        stop - start));
  }

  /** insert the buffered strings */
  public synchronized void insertAll() {
    try {
      // Insert new elements at the bottom
      ElementSpec[] elementArray = elements.toArray(new ElementSpec[0]);
      insert(getLength(), elementArray);
      
      // check how many lines have been used
      // if too many, shave off a few lines from the beginning
      Element root = getDefaultRootElement();
      int lineCount = root.getElementCount();
      int overage = lineCount - maxLineCount;
      if (overage > 0) {
        // if 1200 lines, and 1000 lines is max,
        // find the position of the end of the 200th line
        Element lineElement = root.getElement(overage);
        if (lineElement == null)
          return; // do nuthin

        // remove to the end of the 200th line
        int endOffset = lineElement.getEndOffset();
        remove(0, endOffset);
      }
    } catch (BadLocationException e) {
      // ignore the error otherwise this will cause an infinite loop
      // maybe not a good idea in the long run?
    }
    elements.clear();
    changed = false;
  }

  public boolean isChanged() {
    return changed;
  }
}
