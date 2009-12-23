/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-09 Ben Fry and Casey Reas
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

import processing.app.debug.AvrdudeUploader;
import processing.app.debug.Compiler;
import processing.app.debug.RunnerException;
import processing.app.debug.Sizer;
import processing.app.debug.Uploader;
import processing.app.preproc.*;
import processing.core.*;

import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.io.*;
import java.util.*;
import java.util.zip.*;

import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;


/**
 * Stores information about files in the current sketch
 */
public class Sketch {
  static private File tempBuildFolder;

  private Editor editor;

  /** main pde file for this sketch. */
  private File primaryFile;

  /**
   * Name of sketch, which is the name of main file
   * (without .pde or .java extension)
   */
  private String name;

  /** true if any of the files have been modified. */
  private boolean modified;

  /** folder that contains this sketch */
  private File folder;

  /** data folder location for this sketch (may not exist yet) */
  private File dataFolder;

  /** code folder location for this sketch (may not exist yet) */
  private File codeFolder;

  private SketchCode current;
  private int currentIndex;
  /**
   * Number of sketchCode objects (tabs) in the current sketch. Note that this
   * will be the same as code.length, because the getCode() method returns
   * just the code[] array, rather than a copy of it, or an array that's been
   * resized to just the relevant files themselves.
   * http://dev.processing.org/bugs/show_bug.cgi?id=940
   */
  private int codeCount;
  private SketchCode[] code;

  /** Class name for the PApplet, as determined by the preprocessor. */
  private String appletClassName;
  /** Class path determined during build. */
  private String classPath;

  /**
   * This is *not* the "Processing" libraries path, this is the Java libraries
   * path, as in java.library.path=BlahBlah, which identifies search paths for
   * DLLs or JNILIBs.
   */
  private String libraryPath;
  private ArrayList<File> importedLibraries;

  /**
   * path is location of the main .pde file, because this is also
   * simplest to use when opening the file from the finder/explorer.
   */
  public Sketch(Editor editor, String path) throws IOException {
    this.editor = editor;

    primaryFile = new File(path);

    // get the name of the sketch by chopping .pde or .java
    // off of the main file name
    String mainFilename = primaryFile.getName();
    int suffixLength = getDefaultExtension().length() + 1;
    name = mainFilename.substring(0, mainFilename.length() - suffixLength);

    // lib/build must exist when the application is started
    // it is added to the CLASSPATH by default, but if it doesn't
    // exist when the application is started, then java will remove
    // the entry from the CLASSPATH, causing Runner to fail.
    //
    /*
    tempBuildFolder = new File(TEMP_BUILD_PATH);
    if (!tempBuildFolder.exists()) {
      tempBuildFolder.mkdirs();
      Base.showError("Required folder missing",
                        "A required folder was missing from \n" +
                        "from your installation of Processing.\n" +
                        "It has now been replaced, please restart    \n" +
                        "the application to complete the repair.", null);
    }
    */
    tempBuildFolder = Base.getBuildFolder();
    //Base.addBuildFolderToClassPath();

    folder = new File(new File(path).getParent());
    //System.out.println("sketch dir is " + folder);

    load();
  }


  /**
   * Build the list of files.
   * <P>
   * Generally this is only done once, rather than
   * each time a change is made, because otherwise it gets to be
   * a nightmare to keep track of what files went where, because
   * not all the data will be saved to disk.
   * <P>
   * This also gets called when the main sketch file is renamed,
   * because the sketch has to be reloaded from a different folder.
   * <P>
   * Another exception is when an external editor is in use,
   * in which case the load happens each time "run" is hit.
   */
  protected void load() {
    codeFolder = new File(folder, "code");
    dataFolder = new File(folder, "data");

    // get list of files in the sketch folder
    String list[] = folder.list();

    // reset these because load() may be called after an
    // external editor event. (fix for 0099)
    codeCount = 0;

    code = new SketchCode[list.length];

    String[] extensions = getExtensions();

    for (String filename : list) {
      // Ignoring the dot prefix files is especially important to avoid files
      // with the ._ prefix on Mac OS X. (You'll see this with Mac files on
      // non-HFS drives, i.e. a thumb drive formatted FAT32.)
      if (filename.startsWith(".")) continue;

      // Don't let some wacko name a directory blah.pde or bling.java.
      if (new File(folder, filename).isDirectory()) continue;

      // figure out the name without any extension
      String base = filename;
      // now strip off the .pde and .java extensions
      for (String extension : extensions) {
        if (base.toLowerCase().endsWith("." + extension)) {
          base = base.substring(0, base.length() - (extension.length() + 1));

          // Don't allow people to use files with invalid names, since on load,
          // it would be otherwise possible to sneak in nasty filenames. [0116]
          if (Sketch.isSanitaryName(base)) {
            code[codeCount++] =
              new SketchCode(new File(folder, filename), extension);
          }
        }
      }
    }
    // Remove any code that wasn't proper
    code = (SketchCode[]) PApplet.subset(code, 0, codeCount);

    // move the main class to the first tab
    // start at 1, if it's at zero, don't bother
    for (int i = 1; i < codeCount; i++) {
      //if (code[i].file.getName().equals(mainFilename)) {
      if (code[i].getFile().equals(primaryFile)) {
        SketchCode temp = code[0];
        code[0] = code[i];
        code[i] = temp;
        break;
      }
    }

    // sort the entries at the top
    sortCode();

    // set the main file to be the current tab
    if (editor != null) {
      setCurrentCode(0);
    }
  }


  protected void replaceCode(SketchCode newCode) {
    for (int i = 0; i < codeCount; i++) {
      if (code[i].getFileName().equals(newCode.getFileName())) {
        code[i] = newCode;
        break;
      }
    }
  }


  protected void insertCode(SketchCode newCode) {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    // add file to the code/codeCount list, resort the list
    //if (codeCount == code.length) {
      code = (SketchCode[]) PApplet.append(code, newCode);
    codeCount++;
    //}
    //code[codeCount++] = newCode;
    }


  protected void sortCode() {
    // cheap-ass sort of the rest of the files
    // it's a dumb, slow sort, but there shouldn't be more than ~5 files
    for (int i = 1; i < codeCount; i++) {
      int who = i;
      for (int j = i + 1; j < codeCount; j++) {
        if (code[j].getFileName().compareTo(code[who].getFileName()) < 0) {
          who = j;  // this guy is earlier in the alphabet
        }
      }
      if (who != i) {  // swap with someone if changes made
        SketchCode temp = code[who];
        code[who] = code[i];
        code[i] = temp;
      }
    }
  }


  boolean renamingCode;

  /**
   * Handler for the New Code menu option.
   */
  public void handleNewCode() {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    // if read-only, give an error
    if (isReadOnly()) {
      // if the files are read-only, need to first do a "save as".
      Base.showMessage("Sketch is Read-Only",
                       "Some files are marked \"read-only\", so you'll\n" +
                       "need to re-save the sketch in another location,\n" +
                       "and try again.");
      return;
    }

    renamingCode = false;
    editor.status.edit("Name for new file:", "");
  }


  /**
   * Handler for the Rename Code menu option.
   */
  public void handleRenameCode() {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    if (currentIndex == 0 && editor.untitled) {
      Base.showMessage("Sketch is Untitled",
                       "How about saving the sketch first \n" +
                       "before trying to rename it?");
      return;
    }

    // if read-only, give an error
    if (isReadOnly()) {
      // if the files are read-only, need to first do a "save as".
      Base.showMessage("Sketch is Read-Only",
                       "Some files are marked \"read-only\", so you'll\n" +
                       "need to re-save the sketch in another location,\n" +
                       "and try again.");
      return;
    }

    // ask for new name of file (internal to window)
    // TODO maybe just popup a text area?
    renamingCode = true;
    String prompt = (currentIndex == 0) ?
      "New name for sketch:" : "New name for file:";
    String oldName = (current.isExtension("pde")) ?
      current.getPrettyName() : current.getFileName();
    editor.status.edit(prompt, oldName);
  }


  /**
   * This is called upon return from entering a new file name.
   * (that is, from either newCode or renameCode after the prompt)
   * This code is almost identical for both the newCode and renameCode
   * cases, so they're kept merged except for right in the middle
   * where they diverge.
   */
  protected void nameCode(String newName) {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    // Add the extension here, this simplifies some of the logic below.
    if (newName.indexOf('.') == -1) {
      newName += "." + getDefaultExtension();
    }

    // if renaming to the same thing as before, just ignore.
    // also ignoring case here, because i don't want to write
    // a bunch of special stuff for each platform
    // (osx is case insensitive but preserving, windows insensitive,
    // *nix is sensitive and preserving.. argh)
    if (renamingCode) {
      if (newName.equalsIgnoreCase(current.getFileName())) {
        // exit quietly for the 'rename' case.
        // if it's a 'new' then an error will occur down below
        return;
      }
    }

    newName = newName.trim();
    if (newName.equals("")) return;

    int dot = newName.indexOf('.');
    if (dot == 0) {
      Base.showWarning("Problem with rename",
                       "The name cannot start with a period.", null);
      return;
    }

    String newExtension = newName.substring(dot+1).toLowerCase();
    if (!validExtension(newExtension)) {
      Base.showWarning("Problem with rename",
                       "\"." + newExtension + "\"" +
                       "is not a valid extension.", null);
      return;
    }

    // Don't let the user create the main tab as a .java file instead of .pde
    if (!isDefaultExtension(newExtension)) {
      if (renamingCode) {  // If creating a new tab, don't show this error
        if (current == code[0]) {  // If this is the main tab, disallow
          Base.showWarning("Problem with rename",
                           "The main file can't use an extension.\n" +
                           "(It may be time for your to graduate to a\n" +
                           "\"real\" programming environment)", null);
          return;
        }
      }
    }

    // dots are allowed for the .pde and .java, but not in the name
    // make sure the user didn't name things poo.time.pde
    // or something like that (nothing against poo time)
    String shortName = newName.substring(0, dot);
    String sanitaryName = Sketch.sanitizeName(shortName);
    if (!shortName.equals(sanitaryName)) {
      newName = sanitaryName + "." + newExtension;
    }

    // Make sure no .pde *and* no .java files with the same name already exist
    // http://dev.processing.org/bugs/show_bug.cgi?id=543
    for (SketchCode c : code) {
      if (sanitaryName.equalsIgnoreCase(c.getPrettyName())) {
        Base.showMessage("Nope",
                         "A file named \"" + c.getFileName() + "\" already exists\n" +
                         "in \"" + folder.getAbsolutePath() + "\"");
        return;
      }
    }

    File newFile = new File(folder, newName);
//    if (newFile.exists()) {  // yay! users will try anything
//      Base.showMessage("Nope",
//                       "A file named \"" + newFile + "\" already exists\n" +
//                       "in \"" + folder.getAbsolutePath() + "\"");
//      return;
//    }

//    File newFileHidden = new File(folder, newName + ".x");
//    if (newFileHidden.exists()) {
//      // don't let them get away with it if they try to create something
//      // with the same name as something hidden
//      Base.showMessage("No Way",
//                       "A hidden tab with the same name already exists.\n" +
//                       "Use \"Unhide\" to bring it back.");
//      return;
//    }

    if (renamingCode) {
      if (currentIndex == 0) {
        // get the new folder name/location
        String folderName = newName.substring(0, newName.indexOf('.'));
        File newFolder = new File(folder.getParentFile(), folderName);
        if (newFolder.exists()) {
          Base.showWarning("Cannot Rename",
                           "Sorry, a sketch (or folder) named " +
                           "\"" + newName + "\" already exists.", null);
          return;
        }

        // unfortunately this can't be a "save as" because that
        // only copies the sketch files and the data folder
        // however this *will* first save the sketch, then rename

        // first get the contents of the editor text area
        if (current.isModified()) {
          current.setProgram(editor.getText());
          try {
            // save this new SketchCode
            current.save();
          } catch (Exception e) {
            Base.showWarning("Error", "Could not rename the sketch. (0)", e);
            return;
          }
        }

        if (!current.renameTo(newFile, newExtension)) {
          Base.showWarning("Error",
                           "Could not rename \"" + current.getFileName() +
                           "\" to \"" + newFile.getName() + "\"", null);
          return;
        }

        // save each of the other tabs because this is gonna be re-opened
        try {
          for (int i = 1; i < codeCount; i++) {
            code[i].save();
          }
        } catch (Exception e) {
          Base.showWarning("Error", "Could not rename the sketch. (1)", e);
          return;
        }

        // now rename the sketch folder and re-open
        boolean success = folder.renameTo(newFolder);
        if (!success) {
          Base.showWarning("Error", "Could not rename the sketch. (2)", null);
          return;
        }
        // if successful, set base properties for the sketch

        File newMainFile = new File(newFolder, newName + ".pde");
        String newMainFilePath = newMainFile.getAbsolutePath();

        // having saved everything and renamed the folder and the main .pde,
        // use the editor to re-open the sketch to re-init state
        // (unfortunately this will kill positions for carets etc)
        editor.handleOpenUnchecked(newMainFilePath,
                                   currentIndex,
                                   editor.getSelectionStart(),
                                   editor.getSelectionStop(),
                                   editor.getScrollPosition());

        // get the changes into the sketchbook menu
        // (re-enabled in 0115 to fix bug #332)
        editor.base.rebuildSketchbookMenus();

      } else {  // else if something besides code[0]
        if (!current.renameTo(newFile, newExtension)) {
          Base.showWarning("Error",
                           "Could not rename \"" + current.getFileName() +
                           "\" to \"" + newFile.getName() + "\"", null);
          return;
        }
      }

    } else {  // creating a new file
      try {
        if (!newFile.createNewFile()) {
          // Already checking for IOException, so make our own.
          throw new IOException("createNewFile() returned false");
        }
      } catch (IOException e) {
        Base.showWarning("Error",
                         "Could not create the file \"" + newFile + "\"\n" +
                         "in \"" + folder.getAbsolutePath() + "\"", e);
        return;
      }
      SketchCode newCode = new SketchCode(newFile, newExtension);
      //System.out.println("new code is named " + newCode.getPrettyName() + " " + newCode.getFile());
      insertCode(newCode);
    }

    // sort the entries
    sortCode();

    // set the new guy as current
    setCurrentCode(newName);

    // update the tabs
    editor.header.rebuild();
  }


  /**
   * Remove a piece of code from the sketch and from the disk.
   */
  public void handleDeleteCode() {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    // if read-only, give an error
    if (isReadOnly()) {
      // if the files are read-only, need to first do a "save as".
      Base.showMessage("Sketch is Read-Only",
                       "Some files are marked \"read-only\", so you'll\n" +
                       "need to re-save the sketch in another location,\n" +
                       "and try again.");
      return;
    }

    // confirm deletion with user, yes/no
    Object[] options = { "OK", "Cancel" };
    String prompt = (currentIndex == 0) ?
      "Are you sure you want to delete this sketch?" :
      "Are you sure you want to delete \"" + current.getPrettyName() + "\"?";
    int result = JOptionPane.showOptionDialog(editor,
                                              prompt,
                                              "Delete",
                                              JOptionPane.YES_NO_OPTION,
                                              JOptionPane.QUESTION_MESSAGE,
                                              null,
                                              options,
                                              options[0]);
    if (result == JOptionPane.YES_OPTION) {
      if (currentIndex == 0) {
        // need to unset all the modified flags, otherwise tries
        // to do a save on the handleNew()

        // delete the entire sketch
        Base.removeDir(folder);

        // get the changes into the sketchbook menu
        //sketchbook.rebuildMenus();

        // make a new sketch, and i think this will rebuild the sketch menu
        //editor.handleNewUnchecked();
        //editor.handleClose2();
        editor.base.handleClose(editor);

      } else {
        // delete the file
        if (!current.deleteFile()) {
          Base.showMessage("Couldn't do it",
                           "Could not delete \"" +
                           current.getFileName() + "\".");
          return;
        }

        // remove code from the list
        removeCode(current);

        // just set current tab to the main tab
        setCurrentCode(0);

        // update the tabs
        editor.header.repaint();
      }
    }
  }


  protected void removeCode(SketchCode which) {
    // remove it from the internal list of files
    // resort internal list of files
    for (int i = 0; i < codeCount; i++) {
      if (code[i] == which) {
        for (int j = i; j < codeCount-1; j++) {
          code[j] = code[j+1];
        }
        codeCount--;
        code = (SketchCode[]) PApplet.shorten(code);
        return;
      }
    }
    System.err.println("removeCode: internal error.. could not find code");
  }


  /**
   * Move to the previous tab.
   */
  public void handlePrevCode() {
    int prev = currentIndex - 1;
    if (prev < 0) prev = codeCount-1;
    setCurrentCode(prev);
  }


  /**
   * Move to the next tab.
   */
  public void handleNextCode() {
    setCurrentCode((currentIndex + 1) % codeCount);
  }


  /**
   * Sets the modified value for the code in the frontmost tab.
   */
  public void setModified(boolean state) {
    //System.out.println("setting modified to " + state);
    //new Exception().printStackTrace();
    current.setModified(state);
    calcModified();
  }


  protected void calcModified() {
    modified = false;
    for (int i = 0; i < codeCount; i++) {
      if (code[i].isModified()) {
        modified = true;
        break;
      }
    }
    editor.header.repaint();

    if (Base.isMacOS()) {
      // http://developer.apple.com/qa/qa2001/qa1146.html
      Object modifiedParam = modified ? Boolean.TRUE : Boolean.FALSE;
      editor.getRootPane().putClientProperty("windowModified", modifiedParam);
    }
  }


  public boolean isModified() {
    return modified;
  }


  /**
   * Save all code in the current sketch.
   */
  public boolean save() throws IOException {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    // first get the contents of the editor text area
    if (current.isModified()) {
      current.setProgram(editor.getText());
    }

    // don't do anything if not actually modified
    //if (!modified) return false;

    if (isReadOnly()) {
      // if the files are read-only, need to first do a "save as".
      Base.showMessage("Sketch is read-only",
                       "Some files are marked \"read-only\", so you'll\n" +
                       "need to re-save this sketch to another location.");
      // if the user cancels, give up on the save()
      if (!saveAs()) return false;
    }

    for (int i = 0; i < codeCount; i++) {
      if (code[i].isModified()) code[i].save();
    }
    calcModified();
    return true;
  }


  /**
   * Handles 'Save As' for a sketch.
   * <P>
   * This basically just duplicates the current sketch folder to
   * a new location, and then calls 'Save'. (needs to take the current
   * state of the open files and save them to the new folder..
   * but not save over the old versions for the old sketch..)
   * <P>
   * Also removes the previously-generated .class and .jar files,
   * because they can cause trouble.
   */
  protected boolean saveAs() throws IOException {
    String newParentDir = null;
    String newName = null;

    /*
      JFileChooser fc = new JFileChooser();
      fc.setDialogTitle("Save sketch folder as...");
      if (isReadOnly() || isUntitled()) {
        // default to the sketchbook folder
        fc.setCurrentDirectory(new File(Preferences.get("sketchbook.path")));
      } else {
        // default to the parent folder of where this was
        fc.setCurrentDirectory(folder.getParentFile());
      }
      // can't do this, will try to save into itself by default
      //fc.setSelectedFile(folder);
      int result = fc.showSaveDialog(editor);
      if (result == JFileChooser.APPROVE_OPTION) {
        File selection = fc.getSelectedFile();
        newParentDir = selection.getParent();
        newName = selection.getName();
      }
    */

    // get new name for folder
    FileDialog fd = new FileDialog(editor,
                                   "Save sketch folder as...",
                                   FileDialog.SAVE);
    if (isReadOnly() || isUntitled()) {
      // default to the sketchbook folder
      fd.setDirectory(Preferences.get("sketchbook.path"));
    } else {
      // default to the parent folder of where this was
      fd.setDirectory(folder.getParent());
    }
    String oldName = folder.getName();
    fd.setFile(oldName);

    fd.setVisible(true);
    newParentDir = fd.getDirectory();
    newName = fd.getFile();

    // user canceled selection
    if (newName == null) return false;
    newName = Sketch.checkName(newName);

    File newFolder = new File(newParentDir, newName);
//    String newPath = newFolder.getAbsolutePath();
//    String oldPath = folder.getAbsolutePath();

//    if (newPath.equals(oldPath)) {
//      return false;  // Can't save a sketch over itself
//    }

    // make sure there doesn't exist a tab with that name already
    // but ignore this situation for the first tab, since it's probably being
    // resaved (with the same name) to another location/folder.
    for (int i = 1; i < codeCount; i++) {
      if (newName.equalsIgnoreCase(code[i].getPrettyName())) {
        Base.showMessage("Nope",
                         "You can't save the sketch as \"" + newName + "\"\n" +
                         "because the sketch already has a tab with that name.");
        return false;
      }
    }

    // check if the paths are identical
    if (newFolder.equals(folder)) {
      // just use "save" here instead, because the user will have received a
      // message (from the operating system) about "do you want to replace?"
      return save();
    }

    // check to see if the user is trying to save this sketch inside itself
    try {
      String newPath = newFolder.getCanonicalPath() + File.separator;
      String oldPath = folder.getCanonicalPath() + File.separator;

      if (newPath.indexOf(oldPath) == 0) {
        Base.showWarning("How very Borges of you",
                         "You cannot save the sketch into a folder\n" +
                         "inside itself. This would go on forever.", null);
        return false;
      }
    } catch (IOException e) { }

    // if the new folder already exists, then need to remove
    // its contents before copying everything over
    // (user will have already been warned)
    if (newFolder.exists()) {
      Base.removeDir(newFolder);
    }
    // in fact, you can't do this on windows because the file dialog
    // will instead put you inside the folder, but it happens on osx a lot.

    // now make a fresh copy of the folder
    newFolder.mkdirs();

    // grab the contents of the current tab before saving
    // first get the contents of the editor text area
    if (current.isModified()) {
      current.setProgram(editor.getText());
    }

    // save the other tabs to their new location
    for (int i = 1; i < codeCount; i++) {
      File newFile = new File(newFolder, code[i].getFileName());
      code[i].saveAs(newFile);
    }

    // re-copy the data folder (this may take a while.. add progress bar?)
    if (dataFolder.exists()) {
      File newDataFolder = new File(newFolder, "data");
      Base.copyDir(dataFolder, newDataFolder);
    }

    // re-copy the code folder
    if (codeFolder.exists()) {
      File newCodeFolder = new File(newFolder, "code");
      Base.copyDir(codeFolder, newCodeFolder);
    }

    // copy custom applet.html file if one exists
    // http://dev.processing.org/bugs/show_bug.cgi?id=485
    File customHtml = new File(folder, "applet.html");
    if (customHtml.exists()) {
      File newHtml = new File(newFolder, "applet.html");
      Base.copyFile(customHtml, newHtml);
    }

    // save the main tab with its new name
    File newFile = new File(newFolder, newName + ".pde");
    code[0].saveAs(newFile);

    editor.handleOpenUnchecked(newFile.getPath(),
                               currentIndex,
                               editor.getSelectionStart(),
                               editor.getSelectionStop(),
                               editor.getScrollPosition());

    // Name changed, rebuild the sketch menus
    //editor.sketchbook.rebuildMenusAsync();
    editor.base.rebuildSketchbookMenus();

    // Make sure that it's not an untitled sketch
    setUntitled(false);

    // let Editor know that the save was successful
    return true;
  }


  /**
   * Prompt the user for a new file to the sketch, then call the
   * other addFile() function to actually add it.
   */
  public void handleAddFile() {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    // if read-only, give an error
    if (isReadOnly()) {
      // if the files are read-only, need to first do a "save as".
      Base.showMessage("Sketch is Read-Only",
                       "Some files are marked \"read-only\", so you'll\n" +
                       "need to re-save the sketch in another location,\n" +
                       "and try again.");
      return;
    }

    // get a dialog, select a file to add to the sketch
    String prompt =
      "Select an image or other data file to copy to your sketch";
    //FileDialog fd = new FileDialog(new Frame(), prompt, FileDialog.LOAD);
    FileDialog fd = new FileDialog(editor, prompt, FileDialog.LOAD);
    fd.setVisible(true);

    String directory = fd.getDirectory();
    String filename = fd.getFile();
    if (filename == null) return;

    // copy the file into the folder. if people would rather
    // it move instead of copy, they can do it by hand
    File sourceFile = new File(directory, filename);

    // now do the work of adding the file
    boolean result = addFile(sourceFile);

    if (result) {
      editor.statusNotice("One file added to the sketch.");
    }
  }


  /**
   * Add a file to the sketch.
   * <p/>
   * .pde or .java files will be added to the sketch folder. <br/>
   * .jar, .class, .dll, .jnilib, and .so files will all
   * be added to the "code" folder. <br/>
   * All other files will be added to the "data" folder.
   * <p/>
   * If they don't exist already, the "code" or "data" folder
   * will be created.
   * <p/>
   * @return true if successful.
   */
  public boolean addFile(File sourceFile) {
    String filename = sourceFile.getName();
    File destFile = null;
    String codeExtension = null;
    boolean replacement = false;

    // if the file appears to be code related, drop it
    // into the code folder, instead of the data folder
    if (filename.toLowerCase().endsWith(".o") ||
        filename.toLowerCase().endsWith(".a") ||
        filename.toLowerCase().endsWith(".so")) {

      //if (!codeFolder.exists()) codeFolder.mkdirs();
      prepareCodeFolder();
      destFile = new File(codeFolder, filename);

    } else {
      for (String extension : getExtensions()) {
        String lower = filename.toLowerCase();
        if (lower.endsWith("." + extension)) {
          destFile = new File(this.folder, filename);
          codeExtension = extension;
        }
      }
      if (codeExtension == null) {
        prepareDataFolder();
        destFile = new File(dataFolder, filename);
      }
    }

    // check whether this file already exists
    if (destFile.exists()) {
      Object[] options = { "OK", "Cancel" };
      String prompt = "Replace the existing version of " + filename + "?";
      int result = JOptionPane.showOptionDialog(editor,
                                                prompt,
                                                "Replace",
                                                JOptionPane.YES_NO_OPTION,
                                                JOptionPane.QUESTION_MESSAGE,
                                                null,
                                                options,
                                                options[0]);
      if (result == JOptionPane.YES_OPTION) {
        replacement = true;
      } else {
        return false;
      }
    }

    // If it's a replacement, delete the old file first,
    // otherwise case changes will not be preserved.
    // http://dev.processing.org/bugs/show_bug.cgi?id=969
    if (replacement) {
      boolean muchSuccess = destFile.delete();
      if (!muchSuccess) {
        Base.showWarning("Error adding file",
                         "Could not delete the existing '" +
                         filename + "' file.", null);
        return false;
      }
    }

    // make sure they aren't the same file
    if ((codeExtension == null) && sourceFile.equals(destFile)) {
      Base.showWarning("You can't fool me",
                       "This file has already been copied to the\n" +
                       "location from which where you're trying to add it.\n" +
                       "I ain't not doin nuthin'.", null);
      return false;
    }

    // in case the user is "adding" the code in an attempt
    // to update the sketch's tabs
    if (!sourceFile.equals(destFile)) {
      try {
        Base.copyFile(sourceFile, destFile);

      } catch (IOException e) {
        Base.showWarning("Error adding file",
                         "Could not add '" + filename + "' to the sketch.", e);
        return false;
      }
    }

    if (codeExtension != null) {
      SketchCode newCode = new SketchCode(destFile, codeExtension);

      if (replacement) {
        replaceCode(newCode);

      } else {
        insertCode(newCode);
        sortCode();
      }
      setCurrentCode(filename);
      editor.header.repaint();
      if (editor.untitled) {  // TODO probably not necessary? problematic?
        // Mark the new code as modified so that the sketch is saved
        current.setModified(true);
      }

    } else {
      if (editor.untitled) {  // TODO probably not necessary? problematic?
        // If a file has been added, mark the main code as modified so
        // that the sketch is properly saved.
        code[0].setModified(true);
      }
    }
    return true;
  }


  /**
   * Add import statements to the current tab for all of packages inside
   * the specified jar file.
   */
  public void importLibrary(String jarPath) {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    String list[] = Compiler.headerListFromIncludePath(jarPath);

    // import statements into the main sketch file (code[0])
    // if the current code is a .java file, insert into current
    //if (current.flavor == PDE) {
    if (hasDefaultExtension(current)) {
      setCurrentCode(0);
    }
    // could also scan the text in the file to see if each import
    // statement is already in there, but if the user has the import
    // commented out, then this will be a problem.
    StringBuffer buffer = new StringBuffer();
    for (int i = 0; i < list.length; i++) {
      buffer.append("#include <");
      buffer.append(list[i]);
      buffer.append(">\n");
    }
    buffer.append('\n');
    buffer.append(editor.getText());
    editor.setText(buffer.toString());
    editor.setSelection(0, 0);  // scroll to start
    setModified(true);
  }


  /**
   * Change what file is currently being edited. Changes the current tab index.
   * <OL>
   * <LI> store the String for the text of the current file.
   * <LI> retrieve the String for the text of the new file.
   * <LI> change the text that's visible in the text area
   * </OL>
   */
  public void setCurrentCode(int which) {
    // if current is null, then this is the first setCurrent(0)
    if ((currentIndex == which) && (current != null)) {
      return;
    }

    // get the text currently being edited
    if (current != null) {
      current.setState(editor.getText(),
                       editor.getSelectionStart(),
                       editor.getSelectionStop(),
                       editor.getScrollPosition());
    }

    current = code[which];
    currentIndex = which;

    editor.setCode(current);
    editor.header.rebuild();
  }


  /**
   * Internal helper function to set the current tab based on a name.
   * @param findName the file name (not pretty name) to be shown
   */
  protected void setCurrentCode(String findName) {
    for (int i = 0; i < codeCount; i++) {
      if (findName.equals(code[i].getFileName()) ||
          findName.equals(code[i].getPrettyName())) {
        setCurrentCode(i);
        return;
      }
    }
  }


  /**
   * Cleanup temporary files used during a build/run.
   */
  protected void cleanup() {
    // if the java runtime is holding onto any files in the build dir, we
    // won't be able to delete them, so we need to force a gc here
    System.gc();

    // note that we can't remove the builddir itself, otherwise
    // the next time we start up, internal runs using Runner won't
    // work because the build dir won't exist at startup, so the classloader
    // will ignore the fact that that dir is in the CLASSPATH in run.sh
    Base.removeDescendants(tempBuildFolder);
  }


  /**
   * Preprocess, Compile, and Run the current code.
   * <P>
   * There are three main parts to this process:
   * <PRE>
   *   (0. if not java, then use another 'engine'.. i.e. python)
   *
   *    1. do the p5 language preprocessing
   *       this creates a working .java file in a specific location
   *       better yet, just takes a chunk of java code and returns a
   *       new/better string editor can take care of saving this to a
   *       file location
   *
   *    2. compile the code from that location
   *       catching errors along the way
   *       placing it in a ready classpath, or .. ?
   *
   *    3. run the code
   *       needs to communicate location for window
   *       and maybe setup presentation space as well
   *       run externally if a code folder exists,
   *       or if more than one file is in the project
   *
   *    X. afterwards, some of these steps need a cleanup function
   * </PRE>
   */
  protected String compile(boolean verbose)
    throws RunnerException {
    
    String name;
  
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    current.setProgram(editor.getText());

    // TODO record history here
    //current.history.record(program, SketchHistory.RUN);

    // if an external editor is being used, need to grab the
    // latest version of the code from the file.
    if (Preferences.getBoolean("editor.external")) {
      // history gets screwed by the open..
      //String historySaved = history.lastRecorded;
      //handleOpen(sketch);
      //history.lastRecorded = historySaved;

      // set current to null so that the tab gets updated
      // http://dev.processing.org/bugs/show_bug.cgi?id=515
      current = null;
      // nuke previous files and settings, just get things loaded
      load();
    }

    // in case there were any boogers left behind
    // do this here instead of after exiting, since the exit
    // can happen so many different ways.. and this will be
    // better connected to the dataFolder stuff below.
    cleanup();

    // handle preprocessing the main file's code
    name = build(tempBuildFolder.getAbsolutePath(), verbose);
    size(tempBuildFolder.getAbsolutePath(), name);
    
    return name;
  }


  /**
   * Build all the code for this sketch.
   *
   * In an advanced program, the returned class name could be different,
   * which is why the className is set based on the return value.
   * A compilation error will burp up a RunnerException.
   *
   * Setting purty to 'true' will cause exception line numbers to be incorrect.
   * Unless you know the code compiles, you should first run the preprocessor
   * with purty set to false to make sure there are no errors, then once
   * successful, re-export with purty set to true.
   *
   * @param buildPath Location to copy all the .java files
   * @return null if compilation failed, main class name if not
   */
  public String preprocess(String buildPath) throws RunnerException {
    return preprocess(buildPath, new PdePreprocessor());
  }

  public String preprocess(String buildPath, PdePreprocessor preprocessor) throws RunnerException {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    String[] codeFolderPackages = null;
    classPath = buildPath;

//    // figure out the contents of the code folder to see if there
//    // are files that need to be added to the imports
//    if (codeFolder.exists()) {
//      libraryPath = codeFolder.getAbsolutePath();
//
//      // get a list of .jar files in the "code" folder
//      // (class files in subfolders should also be picked up)
//      String codeFolderClassPath =
//        Compiler.contentsToClassPath(codeFolder);
//      // append the jar files in the code folder to the class path
//      classPath += File.pathSeparator + codeFolderClassPath;
//      // get list of packages found in those jars
//      codeFolderPackages =
//        Compiler.packageListFromClassPath(codeFolderClassPath);
//
//    } else {
//      libraryPath = "";
//    }

    // 1. concatenate all .pde files to the 'main' pde
    //    store line number for starting point of each code bit

    // Unfortunately, the header has to be written on a single line, because
    // there's no way to determine how long it will be until the code has
    // already been preprocessed. The header will vary in length based on
    // the programming mode (STATIC, ACTIVE, or JAVA), which is determined
    // by the preprocessor. So the preprocOffset for the primary class remains
    // zero, even though it'd be nice to have a legitimate offset, and be able
    // to remove the 'pretty' boolean for preproc.write().

    StringBuffer bigCode = new StringBuffer();
    int bigCount = 0;
    for (SketchCode sc : code) {
      if (sc.isExtension("pde")) {
        sc.setPreprocOffset(bigCount);
        bigCode.append(sc.getProgram());
        bigCode.append('\n');
        bigCount += sc.getLineCount();
//        if (sc != code[0]) {
//          sc.setPreprocName(null);  // don't compile me
//        }
      }
    }

    /*
    String program = code[0].getProgram();
    StringBuffer bigCode = new StringBuffer(program);
    int bigCount = code[0].getLineCount();
    bigCode.append('\n');

    for (int i = 1; i < codeCount; i++) {
      if (code[i].isExtension("pde")) {
        code[i].setPreprocOffset(bigCount);
        bigCode.append(code[i].getProgram());
        bigCode.append('\n');
        bigCount += code[i].getLineCount();
        code[i].setPreprocName(null);  // don't compile me
      }
    }
    */

    // Note that the headerOffset isn't applied until compile and run, because
    // it only applies to the code after it's been written to the .java file.
    int headerOffset = 0;
    //PdePreprocessor preprocessor = new PdePreprocessor();
    try {
      headerOffset = preprocessor.writePrefix(bigCode.toString(),
                                              buildPath,
                                              name,
                                              codeFolderPackages);
    } catch (FileNotFoundException fnfe) {
      fnfe.printStackTrace();
      String msg = "Build folder disappeared or could not be written";
      throw new RunnerException(msg);
    }

    // 2. run preproc on that code using the sugg class name
    //    to create a single .java file and write to buildpath

    String primaryClassName = null;

    try {
      // if (i != 0) preproc will fail if a pde file is not
      // java mode, since that's required
      String className = preprocessor.write();

      if (className == null) {
        throw new RunnerException("Could not find main class");
        // this situation might be perfectly fine,
        // (i.e. if the file is empty)
        //System.out.println("No class found in " + code[i].name);
        //System.out.println("(any code in that file will be ignored)");
        //System.out.println();

//      } else {
//        code[0].setPreprocName(className + ".java");
      }

      // store this for the compiler and the runtime
      primaryClassName = className + ".cpp";

    } catch (FileNotFoundException fnfe) {
      fnfe.printStackTrace();
      String msg = "Build folder disappeared or could not be written";
      throw new RunnerException(msg);
    } catch (RunnerException pe) {
      // RunnerExceptions are caught here and re-thrown, so that they don't
      // get lost in the more general "Exception" handler below.
      throw pe;

    } catch (Exception ex) {
      // TODO better method for handling this?
      System.err.println("Uncaught exception type:" + ex.getClass());
      ex.printStackTrace();
      throw new RunnerException(ex.toString());
    }

    // grab the imports from the code just preproc'd

    importedLibraries = new ArrayList<File>();

    for (String item : preprocessor.getExtraImports()) {
      File libFolder = (File) Base.importToLibraryTable.get(item);

      if (libFolder != null && !importedLibraries.contains(libFolder)) {
        importedLibraries.add(libFolder);
        classPath += Compiler.contentsToClassPath(libFolder);
        libraryPath += File.pathSeparator + libFolder.getAbsolutePath();
      }
    }

    // 3. then loop over the code[] and save each .java file

    for (SketchCode sc : code) {
      if (sc.isExtension("c") || sc.isExtension("cpp") || sc.isExtension("h")) {
        // no pre-processing services necessary for java files
        // just write the the contents of 'program' to a .java file
        // into the build directory. uses byte stream and reader/writer
        // shtuff so that unicode bunk is properly handled
        String filename = sc.getFileName(); //code[i].name + ".java";
        try {
          Base.saveFile(sc.getProgram(), new File(buildPath, filename));
        } catch (IOException e) {
          e.printStackTrace();
          throw new RunnerException("Problem moving " + filename +
                                    " to the build folder");
        }
//        sc.setPreprocName(filename);

      } else if (sc.isExtension("pde")) {
        // The compiler and runner will need this to have a proper offset
        sc.addPreprocOffset(headerOffset);
      }
    }
    return primaryClassName;
  }


  /**
   * Preprocess and compile all the code for this sketch.
   *
   * In an advanced program, the returned class name could be different,
   * which is why the className is set based on the return value.
   * A compilation error will burp up a RunnerException.
   *
   * @return null if compilation failed, main class name if not
   */
  public String build(String buildPath, boolean verbose)
    throws RunnerException {
    
    // run the preprocessor
    String primaryClassName = preprocess(buildPath);

    // compile the program. errors will happen as a RunnerException
    // that will bubble up to whomever called build().
    Compiler compiler = new Compiler();
    if (compiler.compile(this, buildPath, primaryClassName, verbose)) {
      return primaryClassName;
    }
    return null;
  }


  protected boolean exportApplet(boolean verbose) throws Exception {
    return exportApplet(tempBuildFolder.getAbsolutePath(), verbose);
  }


  /**
   * Handle export to applet.
   */
  public boolean exportApplet(String appletPath, boolean verbose)
    throws RunnerException, IOException {
    
    // Make sure the user didn't hide the sketch folder
    ensureExistence();

    current.setProgram(editor.getText());

    // Reload the code when an external editor is being used
    if (Preferences.getBoolean("editor.external")) {
      current = null;
      // nuke previous files and settings
      load();
    }

    File appletFolder = new File(appletPath);
    // Nuke the old applet folder because it can cause trouble
    if (Preferences.getBoolean("export.delete_target_folder")) {
      Base.removeDir(appletFolder);
    }
    // Create a fresh applet folder (needed before preproc is run below)
    appletFolder.mkdirs();

    // build the sketch
    String foundName = build(appletFolder.getPath(), false);
    // (already reported) error during export, exit this function
    if (foundName == null) return false;

//    // If name != exportSketchName, then that's weirdness
//    // BUG unfortunately, that can also be a bug in the preproc :(
//    if (!name.equals(foundName)) {
//      Base.showWarning("Error during export",
//                       "Sketch name is " + name + " but the sketch\n" +
//                       "name in the code was " + foundName, null);
//      return false;
//    }

    size(appletFolder.getPath(), foundName);
    upload(appletFolder.getPath(), foundName, verbose);

    return true;
  }


  protected void size(String buildPath, String suggestedClassName)
    throws RunnerException {
    long size = 0;
    long maxsize = Integer.parseInt(Base.getBoardPreferences().get("upload.maximum_size"));
    Sizer sizer = new Sizer(buildPath, suggestedClassName);
      try {
      size = sizer.computeSize();
      System.out.println("Binary sketch size: " + size + " bytes (of a " +
        maxsize + " byte maximum)");      
    } catch (RunnerException e) {
      System.err.println("Couldn't determine program size: " + e.getMessage());
    }

    if (size > maxsize)
      throw new RunnerException(
        "Sketch too big; see http://www.arduino.cc/en/Guide/Troubleshooting#size for tips on reducing it.");
  }


  protected String upload(String buildPath, String suggestedClassName, boolean verbose)
    throws RunnerException {

    Uploader uploader;

    // download the program
    //
    uploader = new AvrdudeUploader();
    boolean success = uploader.uploadUsingPreferences(buildPath,
                                                      suggestedClassName,
                                                      verbose);

    return success ? suggestedClassName : null;
    }

  /**
   * Replace all commented portions of a given String as spaces.
   * Utility function used here and in the preprocessor.
   */
  static public String scrubComments(String what) {
    char p[] = what.toCharArray();

    int index = 0;
    while (index < p.length) {
      // for any double slash comments, ignore until the end of the line
      if ((p[index] == '/') &&
          (index < p.length - 1) &&
          (p[index+1] == '/')) {
        p[index++] = ' ';
        p[index++] = ' ';
        while ((index < p.length) &&
               (p[index] != '\n')) {
          p[index++] = ' ';
        }

        // check to see if this is the start of a new multiline comment.
        // if it is, then make sure it's actually terminated somewhere.
      } else if ((p[index] == '/') &&
                 (index < p.length - 1) &&
                 (p[index+1] == '*')) {
        p[index++] = ' ';
        p[index++] = ' ';
        boolean endOfRainbow = false;
        while (index < p.length - 1) {
          if ((p[index] == '*') && (p[index+1] == '/')) {
            p[index++] = ' ';
            p[index++] = ' ';
            endOfRainbow = true;
            break;

          } else {
            index++;
          }
        }
        if (!endOfRainbow) {
          throw new RuntimeException("Missing the */ from the end of a " +
                                     "/* comment */");
        }
      } else {  // any old character, move along
        index++;
      }
    }
    return new String(p);
  }


  public boolean exportApplicationPrompt() throws IOException, RunnerException {
    return false;
  }

  
  /**
   * Export to application via GUI. 
   */
  protected boolean exportApplication() throws IOException, RunnerException {
        return false;
      }


  /**
   * Export to application without GUI.
   */
  public boolean exportApplication(String destPath,
                                   int exportPlatform) throws IOException, RunnerException {
      return false;
    }


  protected void addManifest(ZipOutputStream zos) throws IOException {
    ZipEntry entry = new ZipEntry("META-INF/MANIFEST.MF");
    zos.putNextEntry(entry);

    String contents =
      "Manifest-Version: 1.0\n" +
      "Created-By: Processing " + Base.VERSION_NAME + "\n" +
      "Main-Class: " + name + "\n";  // TODO not package friendly
    zos.write(contents.getBytes());
    zos.closeEntry();
  }


  /**
   * Read from a file with a bunch of attribute/value pairs
   * that are separated by = and ignore comments with #.
   */
  protected HashMap<String,String> readSettings(File inputFile) {
    HashMap<String,String> outgoing = new HashMap<String,String>();
    if (!inputFile.exists()) return outgoing;  // return empty hash

    String lines[] = PApplet.loadStrings(inputFile);
    for (int i = 0; i < lines.length; i++) {
      int hash = lines[i].indexOf('#');
      String line = (hash == -1) ?
        lines[i].trim() : lines[i].substring(0, hash).trim();
      if (line.length() == 0) continue;

      int equals = line.indexOf('=');
      if (equals == -1) {
        System.err.println("ignoring illegal line in " + inputFile);
        System.err.println("  " + line);
        continue;
      }
      String attr = line.substring(0, equals).trim();
      String valu = line.substring(equals + 1).trim();
      outgoing.put(attr, valu);
    }
    return outgoing;
  }


  /**
   * Slurps up .class files from a colon (or semicolon on windows)
   * separated list of paths and adds them to a ZipOutputStream.
   */
  protected void packClassPathIntoZipFile(String path,
                                          ZipOutputStream zos,
                                          HashMap<String,Object> zipFileContents)
    throws IOException {
    String[] pieces = PApplet.split(path, File.pathSeparatorChar);

    for (int i = 0; i < pieces.length; i++) {
      if (pieces[i].length() == 0) continue;

      // is it a jar file or directory?
      if (pieces[i].toLowerCase().endsWith(".jar") ||
          pieces[i].toLowerCase().endsWith(".zip")) {
        try {
          ZipFile file = new ZipFile(pieces[i]);
          Enumeration<?> entries = file.entries();
          while (entries.hasMoreElements()) {
            ZipEntry entry = (ZipEntry) entries.nextElement();
            if (entry.isDirectory()) {
              // actually 'continue's for all dir entries

            } else {
              String entryName = entry.getName();
              // ignore contents of the META-INF folders
              if (entryName.indexOf("META-INF") == 0) continue;

              // don't allow duplicate entries
              if (zipFileContents.get(entryName) != null) continue;
              zipFileContents.put(entryName, new Object());

              ZipEntry entree = new ZipEntry(entryName);

              zos.putNextEntry(entree);
              byte buffer[] = new byte[(int) entry.getSize()];
              InputStream is = file.getInputStream(entry);

              int offset = 0;
              int remaining = buffer.length;
              while (remaining > 0) {
                int count = is.read(buffer, offset, remaining);
                offset += count;
                remaining -= count;
              }

              zos.write(buffer);
              zos.flush();
              zos.closeEntry();
            }
          }
        } catch (IOException e) {
          System.err.println("Error in file " + pieces[i]);
          e.printStackTrace();
        }
      } else {  // not a .jar or .zip, prolly a directory
        File dir = new File(pieces[i]);
        // but must be a dir, since it's one of several paths
        // just need to check if it exists
        if (dir.exists()) {
          packClassPathIntoZipFileRecursive(dir, null, zos);
        }
      }
    }
  }


  /**
   * Continue the process of magical exporting. This function
   * can be called recursively to walk through folders looking
   * for more goodies that will be added to the ZipOutputStream.
   */
  static protected void packClassPathIntoZipFileRecursive(File dir,
                                                          String sofar,
                                                          ZipOutputStream zos)
    throws IOException {
    String files[] = dir.list();
    for (int i = 0; i < files.length; i++) {
      // ignore . .. and .DS_Store
      if (files[i].charAt(0) == '.') continue;

      File sub = new File(dir, files[i]);
      String nowfar = (sofar == null) ?
        files[i] : (sofar + "/" + files[i]);

      if (sub.isDirectory()) {
        packClassPathIntoZipFileRecursive(sub, nowfar, zos);

      } else {
        // don't add .jar and .zip files, since they only work
        // inside the root, and they're unpacked
        if (!files[i].toLowerCase().endsWith(".jar") &&
            !files[i].toLowerCase().endsWith(".zip") &&
            files[i].charAt(0) != '.') {
          ZipEntry entry = new ZipEntry(nowfar);
          zos.putNextEntry(entry);
          zos.write(Base.loadBytesRaw(sub));
          zos.closeEntry();
        }
      }
    }
  }


  /**
   * Make sure the sketch hasn't been moved or deleted by some
   * nefarious user. If they did, try to re-create it and save.
   * Only checks to see if the main folder is still around,
   * but not its contents.
   */
  protected void ensureExistence() {
    if (folder.exists()) return;

    Base.showWarning("Sketch Disappeared",
                     "The sketch folder has disappeared.\n " +
                     "Will attempt to re-save in the same location,\n" +
                     "but anything besides the code will be lost.", null);
    try {
      folder.mkdirs();
      modified = true;

      for (int i = 0; i < codeCount; i++) {
        code[i].save();  // this will force a save
      }
      calcModified();

    } catch (Exception e) {
      Base.showWarning("Could not re-save sketch",
                       "Could not properly re-save the sketch. " +
                       "You may be in trouble at this point,\n" +
                       "and it might be time to copy and paste " +
                       "your code to another text editor.", e);
    }
  }


  /**
   * Returns true if this is a read-only sketch. Used for the
   * examples directory, or when sketches are loaded from read-only
   * volumes or folders without appropriate permissions.
   */
  public boolean isReadOnly() {
    String apath = folder.getAbsolutePath();
    if (apath.startsWith(Base.getExamplesPath()) ||
        apath.startsWith(Base.getLibrariesPath()) ||
        apath.startsWith(Base.getSketchbookLibrariesPath())) {
      return true;

      // canWrite() doesn't work on directories
      //} else if (!folder.canWrite()) {
    } else {
      // check to see if each modified code file can be written to
      for (int i = 0; i < codeCount; i++) {
        if (code[i].isModified() &&
            code[i].fileReadOnly() &&
            code[i].fileExists()) {
          //System.err.println("found a read-only file " + code[i].file);
          return true;
        }
      }
      //return true;
    }
    return false;
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  // Breaking out extension types in order to clean up the code, and make it
  // easier for other environments (like Arduino) to incorporate changes.


  /**
   * True if the specified extension should be hidden when shown on a tab.
   * For Processing, this is true for .pde files. (Broken out for subclasses.)
   */
  public boolean hideExtension(String what) {
    return what.equals(getDefaultExtension());
  }


  /**
   * True if the specified code has the default file extension.
   */
  public boolean hasDefaultExtension(SketchCode code) {
    return code.getExtension().equals(getDefaultExtension());
  }


  /**
   * True if the specified extension is the default file extension.
   */
  public boolean isDefaultExtension(String what) {
    return what.equals(getDefaultExtension());
  }


  /**
   * Check this extension (no dots, please) against the list of valid
   * extensions.
   */
  public boolean validExtension(String what) {
    String[] ext = getExtensions();
    for (int i = 0; i < ext.length; i++) {
      if (ext[i].equals(what)) return true;
    }
    return false;
  }


  /**
   * Returns the default extension for this editor setup.
   */
  public String getDefaultExtension() {
    return "pde";
  }


  /**
   * Returns a String[] array of proper extensions.
   */
  public String[] getExtensions() {
    return new String[] { "pde", "c", "cpp", "h" };
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  // Additional accessors added in 0136 because of package work.
  // These will also be helpful for tool developers.


  /**
   * Returns the name of this sketch. (The pretty name of the main tab.)
   */
  public String getName() {
    return name;
  }


  /**
   * Returns a file object for the primary .pde of this sketch.
   */
  public File getPrimaryFile() {
    return primaryFile;
  }


  /**
   * Returns path to the main .pde file for this sketch.
   */
  public String getMainFilePath() {
    return primaryFile.getAbsolutePath();
    //return code[0].file.getAbsolutePath();
  }


  /**
   * Returns the sketch folder.
   */
  public File getFolder() {
    return folder;
  }


  /**
   * Returns the location of the sketch's data folder. (It may not exist yet.)
   */
  public File getDataFolder() {
    return dataFolder;
  }


  /**
   * Create the data folder if it does not exist already. As a convenience,
   * it also returns the data folder, since it's likely about to be used.
   */
  public File prepareDataFolder() {
    if (!dataFolder.exists()) {
      dataFolder.mkdirs();
    }
    return dataFolder;
  }


  /**
   * Returns the location of the sketch's code folder. (It may not exist yet.)
   */
  public File getCodeFolder() {
    return codeFolder;
  }


  /**
   * Create the code folder if it does not exist already. As a convenience,
   * it also returns the code folder, since it's likely about to be used.
   */
  public File prepareCodeFolder() {
    if (!codeFolder.exists()) {
      codeFolder.mkdirs();
    }
    return codeFolder;
  }


  public ArrayList<File> getImportedLibraries() {
    return importedLibraries;
  }


  public String getClassPath() {
    return classPath;
  }


  public String getLibraryPath() {
    return libraryPath;
  }


  public SketchCode[] getCode() {
    return code;
  }


  public int getCodeCount() {
    return codeCount;
  }


  public SketchCode getCode(int index) {
    return code[index];
  }


  public int getCodeIndex(SketchCode who) {
    for (int i = 0; i < codeCount; i++) {
      if (who == code[i]) {
        return i;
      }
    }
    return -1;
  }


  public SketchCode getCurrentCode() {
    return current;
  }


  public void setUntitled(boolean u) {
    editor.untitled = u;
  }


  public boolean isUntitled() {
    return editor.untitled;
  }


  public String getAppletClassName2() {
    return appletClassName;
  }


  // .................................................................


  /**
   * Convert to sanitized name and alert the user
   * if changes were made.
   */
  static public String checkName(String origName) {
    String newName = sanitizeName(origName);

    if (!newName.equals(origName)) {
      String msg =
        "The sketch name had to be modified. Sketch names can only consist\n" +
        "of ASCII characters and numbers (but cannot start with a number).\n" +
        "They should also be less less than 64 characters long.";
      System.out.println(msg);
    }
    return newName;
  }


  /**
   * Return true if the name is valid for a Processing sketch.
   */
  static public boolean isSanitaryName(String name) {
    return sanitizeName(name).equals(name);
  }


  /**
   * Produce a sanitized name that fits our standards for likely to work.
   * <p/>
   * Java classes have a wider range of names that are technically allowed
   * (supposedly any Unicode name) than what we support. The reason for
   * going more narrow is to avoid situations with text encodings and
   * converting during the process of moving files between operating
   * systems, i.e. uploading from a Windows machine to a Linux server,
   * or reading a FAT32 partition in OS X and using a thumb drive.
   * <p/>
   * This helper function replaces everything but A-Z, a-z, and 0-9 with
   * underscores. Also disallows starting the sketch name with a digit.
   */
  static public String sanitizeName(String origName) {
    char c[] = origName.toCharArray();
    StringBuffer buffer = new StringBuffer();

    // can't lead with a digit, so start with an underscore
    if ((c[0] >= '0') && (c[0] <= '9')) {
      buffer.append('_');
    }
    for (int i = 0; i < c.length; i++) {
      if (((c[i] >= '0') && (c[i] <= '9')) ||
          ((c[i] >= 'a') && (c[i] <= 'z')) ||
          ((c[i] >= 'A') && (c[i] <= 'Z'))) {
        buffer.append(c[i]);

      } else {
        buffer.append('_');
      }
    }
    // let's not be ridiculous about the length of filenames.
    // in fact, Mac OS 9 can handle 255 chars, though it can't really
    // deal with filenames longer than 31 chars in the Finder.
    // but limiting to that for sketches would mean setting the
    // upper-bound on the character limit here to 25 characters
    // (to handle the base name + ".class")
    if (buffer.length() > 63) {
      buffer.setLength(63);
    }
    return buffer.toString();
  }
}
