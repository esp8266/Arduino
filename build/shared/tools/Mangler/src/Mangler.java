/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2008 Ben Fry and Casey Reas

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

package com.transformers.supermangletron;


import java.text.SimpleDateFormat;
import java.util.Date;

import javax.swing.JOptionPane;

import processing.app.Editor;
import processing.app.tools.Tool;


/**
 * Example Tools menu entry.
 */
public class Mangler implements Tool {
  Editor editor;


  public void init(Editor editor) {
    this.editor = editor;
  }


  public String getMenuTitle() {
    return "Mangle Selection";
  }


  public void run() {
    String sketchName = editor.getSketch().getName();

    Object[] options = { "Yes, please", "No, thanks" };
    int result = JOptionPane.showOptionDialog(editor,
                                              "Is " + sketchName +
                                              " ready for destruction?",
                                              "Super Mangle Tron",
                                              JOptionPane.YES_NO_OPTION,
                                              JOptionPane.QUESTION_MESSAGE,
                                              null,
                                              options,
                                              options[1]);
    if (result == JOptionPane.YES_OPTION) {
      mangleSelection();
    }
  }


  protected void mangleSelection() {
    if (editor.isSelectionActive()) {
      String selection = editor.getSelectedText();
      char[] stuff = selection.toCharArray();
      // Randomly swap a bunch of characters in the text
      for (int i = 0; i < stuff.length / 10; i++) {
        int a = (int) (Math.random() * stuff.length);
        int b = (int) (Math.random() * stuff.length);
        if (stuff[a] == '\n' || stuff[b] == '\n') {
          continue;  // skip newline characters
        }
        stuff[a] = selection.charAt(b);
        stuff[b] = selection.charAt(a);
      }
      editor.startCompoundEdit();
      editor.setSelectedText(new String(stuff));
      editor.stopCompoundEdit();
      editor.statusNotice("Now that feels better, doesn't it?");

    } else {
      editor.statusError("No selection, no dice.");
    }
  }
}
