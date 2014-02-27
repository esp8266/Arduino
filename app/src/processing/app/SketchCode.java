/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  SketchCode - data class for a single file inside a sketch
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-08 Ben Fry and Casey Reas
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

import java.io.*;

import javax.swing.text.Document;
import javax.swing.undo.*;
import static processing.app.I18n._;


/**
 * Represents a single tab of a sketch. 
 */
public class SketchCode {
  /** Pretty name (no extension), not the full file name */
  private String prettyName;

  /** File object for where this code is located */
  private File file;

  /** Extension for this file (no dots, and in lowercase). */ 
  private String extension;

  /** Text of the program text for this tab */
  private String program;

  /** Document object for this tab. Currently this is a SyntaxDocument. */
  private Document document;

  /**
   * Undo Manager for this tab, each tab keeps track of their own
   * Editor.undo will be set to this object when this code is the tab
   * that's currently the front.
   */
  private UndoManager undo = new UndoManager();

  // saved positions from last time this tab was used
  private int selectionStart;
  private int selectionStop;
  private int scrollPosition;

  private boolean modified;

  /** name of .java file after preproc */
//  private String preprocName; 
  /** where this code starts relative to the concat'd code */
  private int preprocOffset;  


  public SketchCode(File file, String extension) {
    this.file = file;
    this.extension = extension;

    makePrettyName();

    try {
      load();
    } catch (IOException e) {
      System.err.println(
        I18n.format(_("Error while loading code {0}"), file.getName()));
    }
  }


  protected void makePrettyName() {
    prettyName = file.getName();
    int dot = prettyName.lastIndexOf('.');
    prettyName = prettyName.substring(0, dot);
  }


  public File getFile() {
    return file;
  }
  
  
  protected boolean fileExists() {
    return file.exists();
  }
  
  
  protected boolean fileReadOnly() {
    return !file.canWrite();
  }
  
  
  protected boolean deleteFile() {
    return file.delete();
  }
  
  
  protected boolean renameTo(File what, String ext) {
    boolean success = file.renameTo(what);
    if (success) {
      file = what;
      extension = ext;
      makePrettyName();
    }
    return success;
  }
  
  
  protected void copyTo(File dest) throws IOException {
    Base.saveFile(program, dest);
  }
  

  public String getFileName() {
    return file.getName();
  }
  
  
  public String getPrettyName() {
    return prettyName;
  }
  
  
  public String getExtension() {
    return extension;
  }
  
  
  public boolean isExtension(String what) {
    return extension.equals(what);
  }
  
  
  public String getProgram() {
    return program;
  }
  
  
  public void setProgram(String replacement) {
    program = replacement;
  }
  
  
  public int getLineCount() {
    return Base.countLines(program);
  }
  
  
  public void setModified(boolean modified) {
    this.modified = modified;
  }


  public boolean isModified() {
    return modified;
  }


//  public void setPreprocName(String preprocName) {
//    this.preprocName = preprocName;
//  }
//
//
//  public String getPreprocName() {
//    return preprocName;
//  }


  public void setPreprocOffset(int preprocOffset) {
    this.preprocOffset = preprocOffset;
  }


  public int getPreprocOffset() {
    return preprocOffset;
  }
  
  
  public void addPreprocOffset(int extra) {
    preprocOffset += extra;
  }


  public Document getDocument() {
    return document;
  }
  
  
  public void setDocument(Document d) {
    document = d;
  }
  
  
  public UndoManager getUndo() {
    return undo;
  }
  
  
  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 

  
  // TODO these could probably be handled better, since it's a general state
  // issue that's read/write from only one location in Editor (on tab switch.)
  
  
  public int getSelectionStart() {
    return selectionStart;
  }
  
  
  public int getSelectionStop() {
    return selectionStop;
  }
  
  
  public int getScrollPosition() {
    return scrollPosition;
  }
  
  
  protected void setState(String p, int start, int stop, int pos) {
    program = p;
    selectionStart = start;
    selectionStop = stop;
    scrollPosition = pos;
  }
  
  
  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
  
  
  /**
   * Load this piece of code from a file.
   */
  public void load() throws IOException {
    program = Base.loadFile(file);

    if (program.indexOf('\uFFFD') != -1) {
      System.err.println(
        I18n.format(
          _("\"{0}\" contains unrecognized characters." +
            "If this code was created with an older version of Processing," +
            "you may need to use Tools -> Fix Encoding & Reload to update" +
            "the sketch to use UTF-8 encoding. If not, you may need to" +
            "delete the bad characters to get rid of this warning."),
          file.getName()
        )
      );
      System.err.println();
    }
    
    setModified(false);
  }


  /**
   * Save this piece of code, regardless of whether the modified
   * flag is set or not.
   */
  public void save() throws IOException {
    // TODO re-enable history
    //history.record(s, SketchHistory.SAVE);

    Base.saveFile(program, file);
    setModified(false);
  }


  /**
   * Save this file to another location, used by Sketch.saveAs()
   */
  public void saveAs(File newFile) throws IOException {
    Base.saveFile(program, newFile);
  }
}
