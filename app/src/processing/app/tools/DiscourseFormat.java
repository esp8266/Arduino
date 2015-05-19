/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2005-06 Ignacio Manuel Gonzalez Moreta.
  Copyright (c) 2006-08 Ben Fry and Casey Reas

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

package processing.app.tools;

import java.awt.*;
import java.awt.datatransfer.*;

import javax.swing.text.BadLocationException;
import javax.swing.text.Segment;

import org.fife.ui.rsyntaxtextarea.Token;

import processing.app.*;
import processing.app.syntax.*;

/**
 * Format for Discourse Tool
 * <p/>
 * Original code by <A HREF="http://usuarios.iponet.es/imoreta">owd</A>.
 * Revised and updated for revision 0108 by Ben Fry (10 March 2006).
 * This code may later be moved to its own 'Tool' plugin, but is included
 * with release 0108+ while features for the "Tools" menu are in testing.
 * <p/>
 * Updated for 0122 to simply copy the code directly to the clipboard,
 * rather than opening a new window.
 * <p/>
 * Updated for 0144 to only format the selected lines.
 * <p/>
 * Updated for 1.5.8 - Simplification, using RSyntaxTextArea TokenImpl formatter (08 dec 2014 - Ricardo JL Rufino)
 * <p/>
 * Notes from the original source:
 * Discourse.java This is a dirty-mix source.
 * NOTE that: No macs and no keyboard. Unreliable source.
 * Only format processing code using fontMetrics.
 * It works under my windows XP + PentiumIV + Processing 0091.
 */
public class DiscourseFormat {

  private Editor editor;
  // JTextArea of the actual Editor
  private SketchTextArea textarea;
  private boolean html;


  /**
   * Creates a new window with the formated (YaBB tags) sketchcode
   * from the actual Processing Tab ready to send to the processing discourse
   * web (copy & paste)
   */
  public DiscourseFormat(Editor editor, boolean html) {
    this.editor = editor;
    this.textarea = editor.getTextArea();
    this.html = html;
  }


  /**
   * Format and render sketch code.
   */
  public void show() {
    // [code] tag cancels other tags, using [quote]
    StringBuilder cf = new StringBuilder(html ? "<pre>\n" : "[code]\n");

    int selStart = textarea.getSelectionStart();
    int selStop = textarea.getSelectionEnd();

    int startLine;
    int stopLine;
    try {
      startLine = textarea.getLineOfOffset(selStart);
      stopLine = textarea.getLineOfOffset(selStop);
    } catch (BadLocationException e) {
      return;
    }

    // If no selection, convert all the lines
    if (selStart == selStop) {
      startLine = 0;
      stopLine = textarea.getLineCount() - 1;
    } else {
      // Make sure the selection doesn't end at the beginning of the last line
      try {
        if (textarea.getLineStartOffset(stopLine) == selStop) {
          stopLine--;
        }
      } catch (BadLocationException e) {
      }
    }

    // Read the code line by line
    for (int i = startLine; i <= stopLine; i++) {
      appendFormattedLine(cf, i);
    }

    cf.append(html ? "\n</pre>" : "\n[/code]");

    StringSelection formatted = new StringSelection(cf.toString());
    Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
    clipboard.setContents(formatted, new ClipboardOwner() {
        public void lostOwnership(Clipboard clipboard, Transferable contents) {
          // i don't care about ownership
        }
      });
    Clipboard unixclipboard = Toolkit.getDefaultToolkit().getSystemSelection();
    if (unixclipboard != null) unixclipboard.setContents(formatted, null);

    editor.statusNotice("Code formatted for " + (html ? "HTML" : "the Arduino forum") + " has been copied to the clipboard.");
  }

  /**
    * Append a char to a StringBuilder while escaping for proper display in HTML.
    * @param c input char to escape
    * @param buffer StringBuilder to append html-safe version of c to.
    */
  private void appendToHTML(char c, StringBuilder buffer) {
    if (!html) {
      buffer.append(c);
    } else if (c == '<') {
      buffer.append("&lt;");
    } else if (c == '>') {
      buffer.append("&gt;");
    } else if (c == '&') {
      buffer.append("&amp;");
    } else if (c > 127) {
      buffer.append("&#" + ((int) c) + ";");  // use unicode entity
    } else {
      buffer.append(c);  // normal character
    }
  }

  // A terrible headache...
  public void appendFormattedLine(StringBuilder cf, int line) {
    Segment segment = new Segment();

    // get line text from parent text area
    textarea.getTextLine(line, segment);
    
    char[] segmentArray = segment.array;
    int segmentOffset = segment.offset;
    int segmentCount = segment.count;
//    int width = 0;

    if (!html) {
      for (int j = 0; j < segmentCount; j++) {
        char c = segmentArray[j + segmentOffset];
        appendToHTML(c, cf);
//        int charWidth;
//        if (c == '\t') {
//          charWidth = (int) painter.nextTabStop(width, j) - width;
//        } else {
//          charWidth = fm.charWidth(c);
//        }
//        width += charWidth;
      }

    } else {
      
      Token tokenList = textarea.getTokenListForLine(line);
      
      while(tokenList != null){
        if(tokenList.getType() == Token.NULL){
          cf.append('\n');
        }else if(tokenList.isPaintable()){
          tokenList.appendHTMLRepresentation(cf, textarea, false);
        }
        
        tokenList = tokenList.getNextToken();
      }
  
    }
  }
}
