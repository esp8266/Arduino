/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2006 Ben Fry and Casey Reas

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

import processing.app.*;
import processing.core.*;
import static processing.app.I18n._;

import java.io.*;


/**
 * Tool for auto-formatting code that interfaces to
 * <A HREF="http://jalopy.sourceforge.net/">Jalopy</A>. This is to replace
 * the buggy code formatter found in previous releases.
 */
  public class AutoFormat {
  Editor editor;


  public AutoFormat(Editor editor) {
    this.editor = editor;
  }


  public void show() {
    String originalText = editor.textarea.getText();
    int indentSize = Preferences.getInteger("editor.tabs.size");

    //

    String formattedText = null; //strOut.toString();
    if (formattedText.equals(originalText)) {
      editor.message(_("No changes necessary for Auto Format."));

    } else {
      // replace with new bootiful text
      // selectionEnd hopefully at least in the neighborhood
      editor.setText(formattedText, selectionEnd, selectionEnd);
      editor.sketch.setModified(true);

      /*
      // warn user if there are too many parens in either direction
      if (paren != 0) {
        editor.error("Warning: Too many " +
                     ((paren < 0) ? "right" : "left") +
                     " parentheses.");

      } else if (c_level != 0) {  // check braces only if parens are ok
        editor.error("Warning: Too many " +
                     ((c_level < 0) ? "right" : "left") +
                     " curly braces.");
      } else {
        editor.message("Auto Format finished.");
      }
      */
    }
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  private static class PluginImpl extends AbstractPlugin {
    JEditStatusBar statusBar;
    Project project;


    /**
     * Creates a new PluginImpl object.
     */
    public PluginImpl()
    {
      super(new JEditAppender());
    }


    public Project getActiveProject()
    {
      if (this.project == null)
        {
          this.project = new JEditProject();
        }

      return this.project;
    }


    public FileFormat getFileFormat()
    {
      // there is a bug(?) in jEdit's text area whereas inserting text with
      // DOS file format results in displaying EOF characters, so we always
      // use UNIX format and let jEdit handle the specified file format upon
      // file saving
      return FileFormat.UNIX;
    }


    public Frame getMainWindow()
    {
      return jEdit.getActiveView();
    }


    public StatusBar getStatusBar()
    {
      return this.statusBar;
    }


    public void afterEnd()
    {
      super.afterEnd();
      MessageView.getInstance().update();
    }


    /**
     * Formats the currently active buffer.
     */
    public void formatActive()
    {
      // only perform the action if the current Buffer contains
      // a Java source file
      //if (isJava(jEdit.getActiveView().getBuffer()))
      //{
      performAction(Action.FORMAT_ACTIVE);
      //}
    }


    /**
     * Formats the currently open buffers.
     */
    public void formatOpen()
    {
      performAction(Action.FORMAT_OPEN);
    }
  }
}
