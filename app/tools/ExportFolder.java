/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  ExportFolder - tool to export all sketches within a certain folder
  Part of the Processing project - http://processing.org

  Copyright (c) 2005-06 Ben Fry and Casey Reas

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

import java.io.*;
import java.text.*;
import java.util.*;
import java.util.zip.*;

import javax.swing.*;


public class ExportFolder {
  Editor editor;
  static JFileChooser fc;


  public ExportFolder(Editor editor) {
    this.editor = editor;

    if (fc == null) {
      fc = new JFileChooser();
      fc.setSelectedFile(new File(Sketchbook.getSketchbookPath()));
      fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
    }
  }


  public void show() {
    if (fc.showOpenDialog(new JDialog()) != JFileChooser.APPROVE_OPTION) {
      return;
    }

    File folder = fc.getSelectedFile();
    // export everything under this folder

    Vector sketches = new Vector();
    try {
      addSketches(sketches, folder);
    } catch (IOException e) {
      e.printStackTrace();
    }

    boolean success = true;
    int counter = 0;

    try {
      // iterate through the list
      Enumeration en = sketches.elements();
      while (en.hasMoreElements()) {
        editor.message("Exporting sketch " + (++counter) +
                       " of " + sketches.size());
        String path = (String) en.nextElement();
        editor.handleOpen(path);
        // success may not be that useful, usually an ex is thrown
        success = editor.sketch.exportApplet(new Target(
                System.getProperty("user.dir") + File.separator + "hardware" +
                File.separator + "cores",
                Preferences.get("boards." + Preferences.get("board") + ".build.core")));
        if (!success) break;
        //System.out.println("success was " + success);
      }
    } catch (Exception e) {
      editor.error(e);
      success = false;
      //e.printStackTrace();
    }

    if (success) {
      editor.message("Done exporting.");
    } // else the error message will be visible
  }


  protected void addSketches(Vector sketches, File folder) throws IOException {

    // skip .DS_Store files, etc
    if (!folder.isDirectory()) return; // false;

    System.out.println(folder.getAbsolutePath());

    String list[] = folder.list();
    // if a bad folder or something like that, this might come back null
    if (list == null) return; // false;

    for (int i = 0; i < list.length; i++) {
      if (list[i].charAt(0) == '.') continue;

      File subfolder = new File(folder, list[i]);
      File entry = new File(subfolder, list[i] + ".pde");
      // if a .pde file of the same prefix as the folder exists..
      if (entry.exists()) {
        sketches.add(entry.getAbsolutePath());

      } else if (subfolder.isDirectory()) {  // only follow if a dir
        addSketches(sketches, subfolder);
      }
    }
  }
}
