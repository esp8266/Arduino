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

import processing.app.syntax.*;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;


/**
 * Filters key events for tab expansion/indent/etc.
 */
public class EditorListener {
  Editor editor;
  JEditTextArea textarea;

  boolean externalEditor;
  boolean expandTabs;
  String tabString;
  boolean autoIndent;

  int selectionStart, selectionEnd;
  int position;


  public EditorListener(Editor editor, JEditTextArea textarea) {
    this.editor = editor;
    this.textarea = textarea;

    // let him know that i'm leechin'
    textarea.editorListener = this;

    applyPreferences();
  }


  public void applyPreferences() {
    expandTabs = Preferences.getBoolean("editor.tabs.expand");
    int tabSize = Preferences.getInteger("editor.tabs.size");
    tabString = Editor.EMPTY.substring(0, tabSize);
    autoIndent = Preferences.getBoolean("editor.indent");
    externalEditor = Preferences.getBoolean("editor.external");
  }


  //public void setExternalEditor(boolean externalEditor) {
  //this.externalEditor = externalEditor;
  //}


  // called by JEditTextArea inside processKeyEvent
  public boolean keyPressed(KeyEvent event) {
    // don't do things if the textarea isn't editable
    if (externalEditor) return false;

    //deselect();  // this is for paren balancing
    char c = event.getKeyChar();
    int code = event.getKeyCode();

    //System.out.println(c + " " + code + " " + event);
    //System.out.println();

    if ((event.getModifiers() & KeyEvent.META_MASK) != 0) {
      //event.consume();  // does nothing
      return false;
    }

    // TODO i don't like these accessors. clean em up later.
    if (!editor.sketch.current.modified) {
      if ((code == KeyEvent.VK_BACK_SPACE) || (code == KeyEvent.VK_TAB) ||
          (code == KeyEvent.VK_ENTER) || ((c >= 32) && (c < 128))) {
        editor.sketch.setModified();
      }
    }

    switch ((int) c) {

    case 9:  // expand tabs
      if (expandTabs) {
        //tc.replaceSelection(tabString);
        textarea.setSelectedText(tabString);
        event.consume();
        return true;
      }
      break;

    case 10:  // auto-indent
    case 13:
      if (autoIndent) {
        char contents[] = textarea.getText().toCharArray();

        // this is the position of the caret, if the textarea
        // only used a single kind of line ending
        int origIndex = textarea.getCaretPosition() - 1;

        // walk through the array to the current caret position,
        // and count how many weirdo windows line endings there are,
        // which would be throwing off the caret position number
        int offset = 0;
        int realIndex = origIndex;
        for (int i = 0; i < realIndex-1; i++) {
          if ((contents[i] == 13) && (contents[i+1] == 10)) {
            offset++;
            realIndex++;
          }
        }

        // back up until \r \r\n or \n.. @#($* cross platform

        //System.out.println(origIndex + " offset = " + offset);
        origIndex += offset; // ARGH!#(* WINDOWS#@($*

        int index = origIndex;
        int spaceCount = 0;
        boolean finished = false;
        while ((index != -1) && (!finished)) {
          if ((contents[index] == 10) ||
              (contents[index] == 13)) {
            finished = true;
            index++; // maybe ?
          } else {
            index--;  // new
          }
        }
        while ((index < contents.length) && (index >= 0) &&
               (contents[index++] == ' ')) {
          spaceCount++;
        }

        // seems that \r is being inserted anyway
        // so no need to insert the platform's line separator
        String insertion = "\n" + Editor.EMPTY.substring(0, spaceCount);
        //tc.replaceSelection(insertion);
        textarea.setSelectedText(insertion);
        // microsoft vm version:
        //tc.setCaretPosition(oldCarrot + insertion.length() - 1);
        // sun vm version:
        //      tc.setCaretPosition(oldCarrot + insertion.length());
        event.consume();
        return true;
      }
      break;
    }
    return false;
  }
}
