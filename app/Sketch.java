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

import processing.app.preproc.*;
//import processing.core.*;

import java.awt.*;
import java.io.*;
import java.util.*;
import java.util.zip.*;

import javax.swing.JOptionPane;

import com.oroinc.text.regex.*;


/**
 * Stores information about files in the current sketch
 */
public class Sketch {
  static File tempBuildFolder;

  Editor editor;

  /**
   * Name of sketch, which is the name of main file
   * (without .pde or .java extension)
   */
  String name;

  /**
   * Name of 'main' file, used by load(), such as sketch_04040.pde
   */
  String mainFilename;

  /**
   * true if any of the files have been modified.
   */
  boolean modified;

  public File folder;
  public File dataFolder;
  public File codeFolder;

  static final int PDE = 0;
  static final int JAVA = 1;

  public SketchCode current;
  int codeCount;
  SketchCode code[];

  int hiddenCount;
  SketchCode hidden[];

  Hashtable zipFileContents;

  // all these set each time build() is called
  String mainClassName;
  String classPath;
  String libraryPath;
  boolean externalRuntime;
  Vector importedLibraries; // vec of File objects

  /**
   * path is location of the main .pde file, because this is also
   * simplest to use when opening the file from the finder/explorer.
   */
  public Sketch(Editor editor, String path) throws IOException {
    this.editor = editor;

    File mainFile = new File(path);
    //System.out.println("main file is " + mainFile);

    mainFilename = mainFile.getName();
    //System.out.println("main file is " + mainFilename);

    // get the name of the sketch by chopping .pde or .java
    // off of the main file name
    if (mainFilename.endsWith(".pde")) {
      name = mainFilename.substring(0, mainFilename.length() - 4);
    } else if (mainFilename.endsWith(".java")) {
      name = mainFilename.substring(0, mainFilename.length() - 5);
    }

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
  public void load() {
    codeFolder = new File(folder, "code");
    dataFolder = new File(folder, "data");

    // get list of files in the sketch folder
    String list[] = folder.list();

    for (int i = 0; i < list.length; i++) {
      if (list[i].endsWith(".pde")) codeCount++;
      else if (list[i].endsWith(".java")) codeCount++;
      else if (list[i].endsWith(".pde.x")) hiddenCount++;
      else if (list[i].endsWith(".java.x")) hiddenCount++;
    }

    code = new SketchCode[codeCount];
    hidden = new SketchCode[hiddenCount];

    int codeCounter = 0;
    int hiddenCounter = 0;

    for (int i = 0; i < list.length; i++) {
      if (list[i].endsWith(".pde")) {
        code[codeCounter++] =
          new SketchCode(list[i].substring(0, list[i].length() - 4),
                      new File(folder, list[i]),
                      PDE);

      } else if (list[i].endsWith(".java")) {
        code[codeCounter++] =
          new SketchCode(list[i].substring(0, list[i].length() - 5),
                      new File(folder, list[i]),
                      JAVA);

      } else if (list[i].endsWith(".pde.x")) {
        hidden[hiddenCounter++] =
          new SketchCode(list[i].substring(0, list[i].length() - 6),
                      new File(folder, list[i]),
                      PDE);

      } else if (list[i].endsWith(".java.x")) {
        hidden[hiddenCounter++] =
          new SketchCode(list[i].substring(0, list[i].length() - 7),
                      new File(folder, list[i]),
                      JAVA);
      }
    }

    // remove any entries that didn't load properly
    int index = 0;
    while (index < codeCount) {
      if ((code[index] == null) ||
          (code[index].program == null)) {
        for (int i = index+1; i < codeCount; i++) {
          code[i-1] = code[i];
        }
        codeCount--;

      } else {
        index++;
      }
    }

    // move the main class to the first tab
    // start at 1, if it's at zero, don't bother
    for (int i = 1; i < codeCount; i++) {
      if (code[i].file.getName().equals(mainFilename)) {
        SketchCode temp = code[0];
        code[0] = code[i];
        code[i] = temp;
        break;
      }
    }

    // sort the entries at the top
    sortCode();

    // set the main file to be the current tab
    setCurrent(0);
  }


  protected void insertCode(SketchCode newCode) {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    // add file to the code/codeCount list, resort the list
    if (codeCount == code.length) {
      SketchCode temp[] = new SketchCode[codeCount+1];
      System.arraycopy(code, 0, temp, 0, codeCount);
      code = temp;
    }
    code[codeCount++] = newCode;
  }


  protected void sortCode() {
    // cheap-ass sort of the rest of the files
    // it's a dumb, slow sort, but there shouldn't be more than ~5 files
    for (int i = 1; i < codeCount; i++) {
      int who = i;
      for (int j = i + 1; j < codeCount; j++) {
        if (code[j].name.compareTo(code[who].name) < 0) {
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

  public void newCode() {
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


  public void renameCode() {
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

    // ask for new name of file (internal to window)
    // TODO maybe just popup a text area?
    renamingCode = true;
    String prompt = (current == code[0]) ?
      "New name for sketch:" : "New name for file:";
    String oldName =
      (current.flavor == PDE) ? current.name : current.name + ".java";
    editor.status.edit(prompt, oldName);
  }


  /**
   * This is called upon return from entering a new file name.
   * (that is, from either newCode or renameCode after the prompt)
   * This code is almost identical for both the newCode and renameCode
   * cases, so they're kept merged except for right in the middle
   * where they diverge.
   */
  public void nameCode(String newName) {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    // if renaming to the same thing as before, just ignore.
    // also ignoring case here, because i don't want to write
    // a bunch of special stuff for each platform
    // (osx is case insensitive but preserving, windows insensitive,
    // *nix is sensitive and preserving.. argh)
    if (renamingCode && newName.equalsIgnoreCase(current.name)) {
      // exit quietly for the 'rename' case.
      // if it's a 'new' then an error will occur down below
      return;
    }

    // don't allow blank names
    if (newName.trim().equals("")) {
      return;
    }

    if (newName.trim().equals(".java") ||
        newName.trim().equals(".pde")) {
      return;
    }

    String newFilename = null;
    int newFlavor = 0;

    // separate into newName (no extension) and newFilename (with ext)
    // add .pde to file if it has no extension
    if (newName.endsWith(".pde")) {
      newFilename = newName;
      newName = newName.substring(0, newName.length() - 4);
      newFlavor = PDE;

    } else if (newName.endsWith(".java")) {
      // don't show this error if creating a new tab
      if (renamingCode && (code[0] == current)) {
        Base.showWarning("Problem with rename",
                         "The main .pde file cannot be .java file.\n" +
                         "(It may be time for your to graduate to a\n" +
                         "\"real\" programming environment)", null);
        return;
      }

      newFilename = newName;
      newName = newName.substring(0, newName.length() - 5);
      newFlavor = JAVA;

    } else {
      newFilename = newName + ".pde";
      newFlavor = PDE;
    }

    // dots are allowed for the .pde and .java, but not in the name
    // make sure the user didn't name things poo.time.pde
    // or something like that (nothing against poo time)
    if (newName.indexOf('.') != -1) {
      newName = Sketchbook.sanitizedName(newName);
      newFilename = newName + ((newFlavor == PDE) ? ".pde" : ".java");
    }

    // create the new file, new SketchCode object and load it
    File newFile = new File(folder, newFilename);
    if (newFile.exists()) {  // yay! users will try anything
      Base.showMessage("Nope",
                       "A file named \"" + newFile + "\" already exists\n" +
                       "in \"" + folder.getAbsolutePath() + "\"");
      return;
    }

    File newFileHidden = new File(folder, newFilename + ".x");
    if (newFileHidden.exists()) {
      // don't let them get away with it if they try to create something
      // with the same name as something hidden
      Base.showMessage("No Way",
                       "A hidden tab with the same name already exists.\n" +
                       "Use \"Unhide\" to bring it back.");
      return;
    }

    if (renamingCode) {
      if (current == code[0]) {
        // get the new folder name/location
        File newFolder = new File(folder.getParentFile(), newName);
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
        if (current.modified) {
          current.program = editor.getText();
          try {
            // save this new SketchCode
            current.save();
          } catch (Exception e) {
            Base.showWarning("Error", "Could not rename the sketch. (0)", e);
            return;
          }
        }

        if (!current.file.renameTo(newFile)) {
          Base.showWarning("Error",
                           "Could not rename \"" + current.file.getName() +
                           "\" to \"" + newFile.getName() + "\"", null);
          return;
        }

        // save each of the other tabs because this is gonna be re-opened
        try {
          for (int i = 1; i < codeCount; i++) {
            //if (code[i].modified) code[i].save();
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

        File mainFile = new File(newFolder, newName + ".pde");
        mainFilename = mainFile.getAbsolutePath();

        // having saved everything and renamed the folder and the main .pde,
        // use the editor to re-open the sketch to re-init state
        // (unfortunately this will kill positions for carets etc)
        editor.handleOpenUnchecked(mainFilename);

        /*
          // backtrack and don't rename the sketch folder
          success = newFolder.renameTo(folder);
          if (!success) {
            String msg =
              "Started renaming sketch and then ran into\n" +
              "nasty trouble. Try to salvage with Copy & Paste\n" +
              "or attempt a \"Save As\" to see if that works.";
            Base.showWarning("Serious Error", msg, null);
          }
          return;
        }
        */

        /*
        // set the sketch name... used by the pde and whatnot.
        // the name is only set in the sketch constructor,
        // so it's important here
        name = newName;

        code[0].name = newName;
        code[0].file = mainFile;
        code[0].program = editor.getText();
        code[0].save();

        folder = newFolder;

        // get the changes into the sketchbook menu
        editor.sketchbook.rebuildMenus();

        // reload the sketch
        load();
        */

      } else {
        if (!current.file.renameTo(newFile)) {
          Base.showWarning("Error",
                           "Could not rename \"" + current.file.getName() +
                           "\" to \"" + newFile.getName() + "\"", null);
          return;
        }

        // just reopen the class itself
        current.name = newName;
        current.file = newFile;
        current.flavor = newFlavor;
      }

    } else {  // creating a new file
      try {
        newFile.createNewFile();  // TODO returns a boolean
      } catch (IOException e) {
        Base.showWarning("Error",
                         "Could not create the file \"" + newFile + "\"\n" +
                         "in \"" + folder.getAbsolutePath() + "\"", e);
        return;
      }
      SketchCode newCode = new SketchCode(newName, newFile, newFlavor);
      insertCode(newCode);
    }

    // sort the entries
    sortCode();

    // set the new guy as current
    setCurrent(newName);

    // update the tabs
    //editor.header.repaint();
    editor.header.rebuild();

    // force the update on the mac?
    Toolkit.getDefaultToolkit().sync();
    //editor.header.getToolkit().sync();
  }


  /**
   * Remove a piece of code from the sketch and from the disk.
   */
  public void deleteCode() {
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
    String prompt = (current == code[0]) ?
      "Are you sure you want to delete this sketch?" :
      "Are you sure you want to delete \"" + current.name + "\"?";
    int result = JOptionPane.showOptionDialog(editor,
                                              prompt,
                                              "Delete",
                                              JOptionPane.YES_NO_OPTION,
                                              JOptionPane.QUESTION_MESSAGE,
                                              null,
                                              options,
                                              options[0]);
    if (result == JOptionPane.YES_OPTION) {
      if (current == code[0]) {
        // need to unset all the modified flags, otherwise tries
        // to do a save on the handleNew()

        // delete the entire sketch
        Base.removeDir(folder);

        // get the changes into the sketchbook menu
        //sketchbook.rebuildMenus();

        // make a new sketch, and i think this will rebuild the sketch menu
        editor.handleNewUnchecked();

      } else {
        // delete the file
        if (!current.file.delete()) {
          Base.showMessage("Couldn't do it",
                           "Could not delete \"" + current.name + "\".");
          return;
        }

        // remove code from the list
        removeCode(current);

        // just set current tab to the main tab
        setCurrent(0);

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
        return;
      }
    }
    System.err.println("removeCode: internal error.. could not find code");
  }


  public void hideCode() {
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

    // don't allow hide of the main code
    // TODO maybe gray out the menu on setCurrent(0)
    if (current == code[0]) {
      Base.showMessage("Can't do that",
                       "You cannot hide the main " +
                       ".pde file from a sketch\n");
      return;
    }

    // rename the file
    File newFile = new File(current.file.getAbsolutePath() + ".x");
    if (!current.file.renameTo(newFile)) {
      Base.showWarning("Error",
                       "Could not hide " +
                       "\"" + current.file.getName() + "\".", null);
      return;
    }
    current.file = newFile;

    // move it to the hidden list
    if (hiddenCount == hidden.length) {
      SketchCode temp[] = new SketchCode[hiddenCount+1];
      System.arraycopy(hidden, 0, temp, 0, hiddenCount);
      hidden = temp;
    }
    hidden[hiddenCount++] = current;

    // remove it from the main list
    removeCode(current);

    // update the tabs
    setCurrent(0);
    editor.header.repaint();
    //editor.header.rebuild();
  }


  public void unhideCode(String what) {
    SketchCode unhideCode = null;

    for (int i = 0; i < hiddenCount; i++) {
      if (hidden[i].name.equals(what)) {
        //unhideIndex = i;
        unhideCode = hidden[i];

        // remove from the 'hidden' list
        for (int j = i; j < hiddenCount-1; j++) {
          hidden[j] = hidden[j+1];
        }
        hiddenCount--;
        break;
      }
    }
    //if (unhideIndex == -1) {
    if (unhideCode == null) {
      System.err.println("internal error: could find " + what + " to unhide.");
      return;
    }
    if (!unhideCode.file.exists()) {
      Base.showMessage("Can't unhide",
                       "The file \"" + what + "\" no longer exists.");
      //System.out.println(unhideCode.file);
      return;
    }
    String unhidePath = unhideCode.file.getAbsolutePath();
    File unhideFile =
      new File(unhidePath.substring(0, unhidePath.length() - 2));

    if (!unhideCode.file.renameTo(unhideFile)) {
      Base.showMessage("Can't unhide",
                       "The file \"" + what + "\" could not be" +
                       "renamed and unhidden.");
      return;
    }
    unhideCode.file = unhideFile;
    insertCode(unhideCode);
    sortCode();
    setCurrent(unhideCode.name);
    editor.header.repaint();
  }


  /**
   * Sets the modified value for the code in the frontmost tab.
   */
  public void setModified() {
    current.modified = true;
    calcModified();
  }


  public void calcModified() {
    modified = false;
    for (int i = 0; i < codeCount; i++) {
      if (code[i].modified) {
        modified = true;
        break;
      }
    }
    editor.header.repaint();
  }


  /**
   * Save all code in the current sketch.
   */
  public boolean save() throws IOException {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    // first get the contents of the editor text area
    if (current.modified) {
      current.program = editor.getText();
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
      if (code[i].modified) code[i].save();
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
  public boolean saveAs() throws IOException {
    // get new name for folder
    FileDialog fd = new FileDialog(editor,
                                   "Save sketch folder as...",
                                   FileDialog.SAVE);
    if (isReadOnly()) {
      // default to the sketchbook folder
      fd.setDirectory(Preferences.get("sketchbook.path"));
    } else {
      // default to the parent folder of where this was
      fd.setDirectory(folder.getParent());
    }
    fd.setFile(folder.getName());

    fd.show();
    String newParentDir = fd.getDirectory();
    String newName = fd.getFile();

    // user cancelled selection
    if (newName == null) return false;
    newName = Sketchbook.sanitizeName(newName);

    // make sure there doesn't exist a tab with that name already
    // (but allow it if it's just the main tab resaving itself.. oops)
    File codeAlready = new File(folder, newName + ".pde");
    if (codeAlready.exists() && (!newName.equals(name))) {
      Base.showMessage("Nope",
                       "You can't save the sketch as \"" + newName + "\"\n" +
                       "because the sketch already has a tab with that name.");
      return false;
    }

    // make sure there doesn't exist a tab with that name already
    File hiddenAlready = new File(folder, newName + ".pde.x");
    if (hiddenAlready.exists()) {
      Base.showMessage("Nope",
                       "You can't save the sketch as \"" + newName + "\"\n" +
                       "because the sketch already has a " +
                       "hidden tab with that name.");
      return false;
    }

    // new sketch folder
    File newFolder = new File(newParentDir, newName);

    // make sure the paths aren't the same
    if (newFolder.equals(folder)) {
      Base.showWarning("You can't fool me",
                       "The new sketch name and location are the same as\n" +
                       "the old. I ain't not doin nuthin' not now.", null);
      return false;
    }

    // check to see if the user is trying to save this sketch
    // inside the same sketch
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
    if (current.modified) {
      current.program = editor.getText();
    }

    // save the other tabs to their new location
    for (int i = 1; i < codeCount; i++) {
      File newFile = new File(newFolder, code[i].file.getName());
      code[i].saveAs(newFile);
    }

    // save the hidden code to its new location
    for (int i = 0; i < hiddenCount; i++) {
      File newFile = new File(newFolder, hidden[i].file.getName());
      hidden[i].saveAs(newFile);
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

    // save the main tab with its new name
    File newFile = new File(newFolder, newName + ".pde");
    code[0].saveAs(newFile);

    editor.handleOpenUnchecked(newFile.getPath());

    /*
    // copy the entire contents of the sketch folder
    Base.copyDir(folder, newFolder);

    // change the references to the dir location in SketchCode files
    for (int i = 0; i < codeCount; i++) {
      code[i].file = new File(newFolder, code[i].file.getName());
    }
    for (int i = 0; i < hiddenCount; i++) {
      hidden[i].file = new File(newFolder, hidden[i].file.getName());
    }

    // remove the old sketch file from the new dir
    code[0].file.delete();
    // name for the new main .pde file
    code[0].file = new File(newFolder, newName + ".pde");
    code[0].name = newName;
    // write the contents to the renamed file
    // (this may be resaved if the code is modified)
    code[0].modified = true;
    //code[0].save();
    //System.out.println("modified is " + modified);

    // change the other paths
    String oldName = name;
    name = newName;
    File oldFolder = folder;
    folder = newFolder;
    dataFolder = new File(folder, "data");
    codeFolder = new File(folder, "code");

    // remove the 'applet', 'application', 'library' folders
    // from the copied version.
    // otherwise their .class and .jar files can cause conflicts.
    Base.removeDir(new File(folder, "applet"));
    Base.removeDir(new File(folder, "application"));
    //Base.removeDir(new File(folder, "library"));

    // do a "save"
    // this will take care of the unsaved changes in each of the tabs
    save();

    // get the changes into the sketchbook menu
    //sketchbook.rebuildMenu();
    // done inside Editor instead

    // update the tabs for the name change
    editor.header.repaint();
    */

    // let Editor know that the save was successful
    return true;
  }


  /**
   * Prompt the user for a new file to the sketch.
   * This could be .class or .jar files for the code folder,
   * .pde or .java files for the project,
   * or .dll, .jnilib, or .so files for the code folder
   */
  public void addFile() {
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
    fd.show();

    String directory = fd.getDirectory();
    String filename = fd.getFile();
    if (filename == null) return;

    // copy the file into the folder. if people would rather
    // it move instead of copy, they can do it by hand
    File sourceFile = new File(directory, filename);

    File destFile = null;
    boolean addingCode = false;

    // if the file appears to be code related, drop it
    // into the code folder, instead of the data folder
    if (filename.toLowerCase().endsWith(".class") ||
        filename.toLowerCase().endsWith(".jar") ||
        filename.toLowerCase().endsWith(".dll") ||
        filename.toLowerCase().endsWith(".jnilib") ||
        filename.toLowerCase().endsWith(".so")) {
      //File codeFolder = new File(this.folder, "code");
      if (!codeFolder.exists()) codeFolder.mkdirs();
      destFile = new File(codeFolder, filename);

    } else if (filename.toLowerCase().endsWith(".pde") ||
               filename.toLowerCase().endsWith(".java")) {
      destFile = new File(this.folder, filename);
      addingCode = true;

    } else {
      //File dataFolder = new File(this.folder, "data");
      if (!dataFolder.exists()) dataFolder.mkdirs();
      destFile = new File(dataFolder, filename);
    }

    // make sure they aren't the same file
    if (!addingCode && sourceFile.equals(destFile)) {
      Base.showWarning("You can't fool me",
                          "This file has already been copied to the\n" +
                          "location where you're trying to add it.\n" +
                          "I ain't not doin nuthin'.", null);
      return;
    }

    // in case the user is "adding" the code in an attempt
    // to update the sketch's tabs
    if (!sourceFile.equals(destFile)) {
      try {
        Base.copyFile(sourceFile, destFile);
      } catch (IOException e) {
        Base.showWarning("Error adding file",
                            "Could not add '" + filename +
                            "' to the sketch.", e);
      }
    }

    // make the tabs update after this guy is added
    if (addingCode) {
      String newName = destFile.getName();
      int newFlavor = -1;
      if (newName.toLowerCase().endsWith(".pde")) {
        newName = newName.substring(0, newName.length() - 4);
        newFlavor = PDE;
      } else {
        newName = newName.substring(0, newName.length() - 5);
        newFlavor = JAVA;
      }

      // see also "nameCode" for identical situation
      SketchCode newCode = new SketchCode(newName, destFile, newFlavor);
      insertCode(newCode);
      sortCode();
      setCurrent(newName);
      editor.header.repaint();
    }
  }


  public void importLibrary(String jarPath) {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    String list[] = Compiler.packageListFromClassPath(jarPath);

    // import statements into the main sketch file (code[0])
    // if the current code is a .java file, insert into current
    if (current.flavor == PDE) {
      setCurrent(0);
    }
    // could also scan the text in the file to see if each import
    // statement is already in there, but if the user has the import
    // commented out, then this will be a problem.
    StringBuffer buffer = new StringBuffer();
    for (int i = 0; i < list.length; i++) {
      buffer.append("import ");
      buffer.append(list[i]);
      buffer.append(".*;\n");
    }
    buffer.append('\n');
    buffer.append(editor.getText());
    editor.setText(buffer.toString(), 0, 0);  // scroll to start
    setModified();
  }


  /**
   * Change what file is currently being edited.
   * <OL>
   * <LI> store the String for the text of the current file.
   * <LI> retrieve the String for the text of the new file.
   * <LI> change the text that's visible in the text area
   * </OL>
   */
  public void setCurrent(int which) {
    if (current == code[which]) {
      //System.out.println("already current, ignoring");
      return;
    }

    // get the text currently being edited
    if (current != null) {
      current.program = editor.getText();
      current.selectionStart = editor.textarea.getSelectionStart();
      current.selectionStop = editor.textarea.getSelectionEnd();
      current.scrollPosition = editor.textarea.getScrollPosition();
    }

    current = code[which];
    editor.setCode(current);
    //editor.setDocument(current.document,
    //                 current.selectionStart, current.selectionStop,
    //                 current.scrollPosition, current.undo);

    // set to the text for this file
    // 'true' means to wipe out the undo buffer
    // (so they don't undo back to the other file.. whups!)
    /*
    editor.setText(current.program,
                   current.selectionStart, current.selectionStop,
                   current.undo);
    */

    // set stored caret and scroll positions
    //editor.textarea.setScrollPosition(current.scrollPosition);
    //editor.textarea.select(current.selectionStart, current.selectionStop);
    //editor.textarea.setSelectionStart(current.selectionStart);
    //editor.textarea.setSelectionEnd(current.selectionStop);

    editor.header.rebuild();
  }


  /**
   * Internal helper function to set the current tab
   * based on a name (used by codeNew and codeRename).
   */
  protected void setCurrent(String findName) {
    for (int i = 0; i < codeCount; i++) {
      if (findName.equals(code[i].name)) {
        setCurrent(i);
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
  public boolean handleRun() throws RunnerException {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    current.program = editor.getText();

    // TODO record history here
    //current.history.record(program, SketchHistory.RUN);

    // if an external editor is being used, need to grab the
    // latest version of the code from the file.
    if (Preferences.getBoolean("editor.external")) {
      // history gets screwed by the open..
      //String historySaved = history.lastRecorded;
      //handleOpen(sketch);
      //history.lastRecorded = historySaved;

      // nuke previous files and settings, just get things loaded
      load();
    }

    // in case there were any boogers left behind
    // do this here instead of after exiting, since the exit
    // can happen so many different ways.. and this will be
    // better connected to the dataFolder stuff below.
    cleanup();

    // make up a temporary class name to suggest.
    // name will only be used if the code is not in ADVANCED mode.
    String suggestedClassName =
      ("Temporary_" + String.valueOf((int) (Math.random() * 10000)) +
       "_" + String.valueOf((int) (Math.random() * 10000)));

    // handle preprocessing the main file's code
    //mainClassName = build(TEMP_BUILD_PATH, suggestedClassName);
    mainClassName =
      build(tempBuildFolder.getAbsolutePath(), suggestedClassName);
    // externalPaths is magically set by build()

    if (!externalRuntime) {  // only if not running externally already
      // copy contents of data dir into lib/build
      if (dataFolder.exists()) {
        // just drop the files in the build folder (pre-68)
        //Base.copyDir(dataDir, buildDir);
        // drop the files into a 'data' subfolder of the build dir
        try {
          Base.copyDir(dataFolder, new File(tempBuildFolder, "data"));
        } catch (IOException e) {
          e.printStackTrace();
          throw new RunnerException("Problem copying files from data folder");
        }
      }
    }
    return (mainClassName != null);
  }


  /**
   * Build all the code for this sketch.
   *
   * In an advanced program, the returned classname could be different,
   * which is why the className is set based on the return value.
   * A compilation error will burp up a RunnerException.
   *
   * @return null if compilation failed, main class name if not
   */
  protected String build(String buildPath, String suggestedClassName)
    throws RunnerException {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    String codeFolderPackages[] = null;

    String javaClassPath = System.getProperty("java.class.path");
    // remove quotes if any.. this is an annoying thing on windows
    if (javaClassPath.startsWith("\"") && javaClassPath.endsWith("\"")) {
      javaClassPath = javaClassPath.substring(1, javaClassPath.length() - 1);
    }

    classPath = buildPath +
      File.pathSeparator + Sketchbook.librariesClassPath +
      File.pathSeparator + javaClassPath;
    //System.out.println("cp = " + classPath);

    // figure out the contents of the code folder to see if there
    // are files that need to be added to the imports
    //File codeFolder = new File(folder, "code");
    if (codeFolder.exists()) {
      externalRuntime = true;

      //classPath += File.pathSeparator +
      //Compiler.contentsToClassPath(codeFolder);
      classPath =
        Compiler.contentsToClassPath(codeFolder) +
        File.pathSeparator + classPath;

      //codeFolderPackages = Compiler.packageListFromClassPath(classPath);
      //codeFolderPackages = Compiler.packageListFromClassPath(codeFolder);
      libraryPath = codeFolder.getAbsolutePath();

      // get a list of .jar files in the "code" folder
      // (class files in subfolders should also be picked up)
      String codeFolderClassPath =
        Compiler.contentsToClassPath(codeFolder);
      // get list of packages found in those jars
      codeFolderPackages =
        Compiler.packageListFromClassPath(codeFolderClassPath);
      //PApplet.println(libraryPath);
      //PApplet.println("packages:");
      //PApplet.printarr(codeFolderPackages);

    } else {
      // since using the special classloader,
      // run externally whenever there are extra classes defined
      //externalRuntime = (codeCount > 1);
      // this no longer appears to be true.. so scrapping for 0088

      // check to see if multiple files that include a .java file
      externalRuntime = false;
      for (int i = 0; i < codeCount; i++) {
        if (code[i].flavor == JAVA) {
          externalRuntime = true;
          break;
        }
      }

      //codeFolderPackages = null;
      libraryPath = "";
    }

    // if 'data' folder is large, set to external runtime
    if (dataFolder.exists() &&
        Base.calcFolderSize(dataFolder) > 768 * 1024) {  // if > 768k
      externalRuntime = true;
    }


    // 1. concatenate all .pde files to the 'main' pde
    //    store line number for starting point of each code bit

    StringBuffer bigCode = new StringBuffer(code[0].program);
    int bigCount = countLines(code[0].program);

    for (int i = 1; i < codeCount; i++) {
      if (code[i].flavor == PDE) {
        code[i].preprocOffset = ++bigCount;
        bigCode.append('\n');
        bigCode.append(code[i].program);
        bigCount += countLines(code[i].program);
        code[i].preprocName = null;  // don't compile me
      }
    }

    // since using the special classloader,
    // run externally whenever there are extra classes defined
    if ((bigCode.indexOf(" class ") != -1) ||
        (bigCode.indexOf("\nclass ") != -1)) {
      externalRuntime = true;
    }

    // if running in opengl mode, this is gonna be external
    //if (Preferences.get("renderer").equals("opengl")) {
    //externalRuntime = true;
    //}

    // 2. run preproc on that code using the sugg class name
    //    to create a single .java file and write to buildpath

    String primaryClassName = null;

    PdePreprocessor preprocessor = new PdePreprocessor();
    try {
      // if (i != 0) preproc will fail if a pde file is not
      // java mode, since that's required
      String className =
        preprocessor.write(bigCode.toString(), buildPath,
                           suggestedClassName, codeFolderPackages);
      if (className == null) {
        throw new RunnerException("Could not find main class");
        // this situation might be perfectly fine,
        // (i.e. if the file is empty)
        //System.out.println("No class found in " + code[i].name);
        //System.out.println("(any code in that file will be ignored)");
        //System.out.println();

      } else {
        code[0].preprocName = className + ".java";
      }

      // store this for the compiler and the runtime
      primaryClassName = className;
      //System.out.println("primary class " + primaryClassName);

      // check if the 'main' file is in java mode
      if ((PdePreprocessor.programType == PdePreprocessor.JAVA) ||
          (preprocessor.extraImports.length != 0)) {
        externalRuntime = true; // we in advanced mode now, boy
      }

    } catch (antlr.RecognitionException re) {
      // this even returns a column
      int errorFile = 0;
      int errorLine = re.getLine() - 1;
      for (int i = 1; i < codeCount; i++) {
        if ((code[i].flavor == PDE) &&
            (code[i].preprocOffset < errorLine)) {
          errorFile = i;
        }
      }
      errorLine -= code[errorFile].preprocOffset;

      throw new RunnerException(re.getMessage(), errorFile,
                             errorLine, re.getColumn());

    } catch (antlr.TokenStreamRecognitionException tsre) {
      // while this seems to store line and column internally,
      // there doesn't seem to be a method to grab it..
      // so instead it's done using a regexp
      PatternMatcher matcher = new Perl5Matcher();
      PatternCompiler compiler = new Perl5Compiler();
      // line 3:1: unexpected char: 0xA0
      String mess = "^line (\\d+):(\\d+):\\s";

      Pattern pattern = null;
      try {
        pattern = compiler.compile(mess);
      } catch (MalformedPatternException e) {
        Base.showWarning("Internal Problem",
                         "An internal error occurred while trying\n" +
                         "to compile the sketch. Please report\n" +
                         "this online at http://processing.org/bugs", e);
      }

      PatternMatcherInput input =
        new PatternMatcherInput(tsre.toString());
      if (matcher.contains(input, pattern)) {
        MatchResult result = matcher.getMatch();

        int errorLine = Integer.parseInt(result.group(1).toString()) - 1;
        int errorColumn = Integer.parseInt(result.group(2).toString());
        int errorFile = 0;
        for (int i = 1; i < codeCount; i++) {
          if ((code[i].flavor == PDE) &&
              (code[i].preprocOffset < errorLine)) {
            errorFile = i;
          }
        }
        errorLine -= code[errorFile].preprocOffset;

        throw new RunnerException(tsre.getMessage(),
                               errorFile, errorLine, errorColumn);

      } else {
        // this is bad, defaults to the main class.. hrm.
        throw new RunnerException(tsre.toString(), 0, -1, -1);
      }

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

    importedLibraries = new Vector();
    String imports[] = preprocessor.extraImports;
    for (int i = 0; i < imports.length; i++) {
      // remove things up to the last dot
      String entry = imports[i].substring(0, imports[i].lastIndexOf('.'));
      //System.out.println("found package " + entry);
      File libFolder = (File) Sketchbook.importToLibraryTable.get(entry);

      if (libFolder == null) {
        //throw new RunnerException("Could not find library for " + entry);
        continue;
      }

      importedLibraries.add(libFolder);
      libraryPath += File.pathSeparator + libFolder.getAbsolutePath();

      /*
      String list[] = libFolder.list();
      if (list != null) {
        for (int j = 0; j < list.length; j++) {
          // this might have a dll/jnilib/so packed,
          // so add it to the library path
          if (list[j].toLowerCase().endsWith(".jar")) {
            libraryPath += File.pathSeparator +
              libFolder.getAbsolutePath() + File.separator + list[j];
          }
        }
      }
      */
    }


    // 3. then loop over the code[] and save each .java file

    for (int i = 0; i < codeCount; i++) {
      if (code[i].flavor == JAVA) {
        // no pre-processing services necessary for java files
        // just write the the contents of 'program' to a .java file
        // into the build directory. uses byte stream and reader/writer
        // shtuff so that unicode bunk is properly handled
        String filename = code[i].name + ".java";
        try {
          Base.saveFile(code[i].program, new File(buildPath, filename));
        } catch (IOException e) {
          e.printStackTrace();
          throw new RunnerException("Problem moving " + filename +
                                 " to the build folder");
        }
        code[i].preprocName = filename;
      }
    }

    // compile the program. errors will happen as a RunnerException
    // that will bubble up to whomever called build().
    //
    Compiler compiler = new Compiler();
    boolean success = compiler.compile(this, buildPath);
    //System.out.println("success = " + success + " ... " + primaryClassName);
    return success ? primaryClassName : null;
  }


  protected int countLines(String what) {
    char c[] = what.toCharArray();
    int count = 0;
    for (int i = 0; i < c.length; i++) {
      if (c[i] == '\n') count++;
    }
    return count;
  }


  /**
   * Initiate export to applet.
   * <PRE>
   * +-------------------------------------------------------+
   * +                                                       +
   * + Export to:  [ Applet (for the web)   + ]    [  OK  ]  +
   * +                                                       +
   * + > Advanced                                            +
   * +                                                       +
   * + - - - - - - - - - - - - - - - - - - - - - - - - - - - +
   * +   Version: [ Java 1.1   + ]                           +
   * +                                                       +
   * +   Recommended version of Java when exporting applets. +
   * + - - - - - - - - - - - - - - - - - - - - - - - - - - - +
   * +   Version: [ Java 1.3   + ]                           +
   * +                                                       +
   * +   Java 1.3 is not recommended for applets,            +
   * +   unless you are using features that require it.      +
   * +   Using a version of Java other than 1.1 will require +
   * +   your Windows users to install the Java Plug-In,     +
   * +   and your Macintosh users to be running OS X.        +
   * + - - - - - - - - - - - - - - - - - - - - - - - - - - - +
   * +   Version: [ Java 1.4   + ]                           +
   * +                                                       +
   * +   identical message as 1.3 above...                   +
   * +                                                       +
   * +-------------------------------------------------------+
   * </PRE>
   */
  public boolean exportApplet() throws Exception {
    return true;
  }


  /**
   * Export to application.
   * <PRE>
   * +-------------------------------------------------------+
   * +                                                       +
   * + Export to:  [ Application            + ]    [  OK  ]  +
   * +                                                       +
   * + > Advanced                                            +
   * + - - - - - - - - - - - - - - - - - - - - - - - - - - - +
   * +   Version: [ Java 1.1   + ]                           +
   * +                                                       +
   * +   Not much point to using Java 1.1 for applications.  +
   * +   To run applications, all users will have to         +
   * +   install Java, in which case they'll most likely     +
   * +   have version 1.3 or later.                          +
   * + - - - - - - - - - - - - - - - - - - - - - - - - - - - +
   * +   Version: [ Java 1.3   + ]                           +
   * +                                                       +
   * +   Java 1.3 is the recommended setting for exporting   +
   * +   applications. Applications will run on any Windows  +
   * +   or Unix machine with Java installed. Mac OS X has   +
   * +   Java installed with the operation system, so there  +
   * +   is no additional installation will be required.     +
   * +                                                       +
   * + - - - - - - - - - - - - - - - - - - - - - - - - - - - +
   * +                                                       +
   * +   Platform: [ Mac OS X   + ]    <-- defaults to current platform
   * +                                                       +
   * +   Exports the application as a double-clickable       +
   * +   .app package, compatible with Mac OS X.             +
   * + - - - - - - - - - - - - - - - - - - - - - - - - - - - +
   * +   Platform: [ Windows    + ]                          +
   * +                                                       +
   * +   Exports the application as a double-clickable       +
   * +   .exe and a handful of supporting files.             +
   * + - - - - - - - - - - - - - - - - - - - - - - - - - - - +
   * +   Platform: [ jar file   + ]                          +
   * +                                                       +
   * +   A jar file can be used on any platform that has     +
   * +   Java installed. Simply doube-click the jar (or type +
   * +   "java -jar sketch.jar" at a command prompt) to run  +
   * +   the application. It is the least fancy method for   +
   * +   exporting.                                          +
   * +                                                       +
   * +-------------------------------------------------------+
   * </PRE>
   */
  public boolean exportApplication() {
    return true;
  }


  public void addManifest(ZipOutputStream zos) throws IOException {
    ZipEntry entry = new ZipEntry("META-INF/MANIFEST.MF");
    zos.putNextEntry(entry);

    String contents =
      "Manifest-Version: 1.0\n" +
      "Created-By: Processing " + Base.VERSION_NAME + "\n" +
      "Main-Class: " + name + "\n";  // TODO not package friendly
    zos.write(contents.getBytes());
    zos.closeEntry();

    /*
      for (int i = 0; i < bagelClasses.length; i++) {
        if (!bagelClasses[i].endsWith(".class")) continue;
        entry = new ZipEntry(bagelClasses[i]);
        zos.putNextEntry(entry);
        zos.write(Base.grabFile(new File(exportDir + bagelClasses[i])));
        zos.closeEntry();
      }
    */
  }


  /**
   * Slurps up .class files from a colon (or semicolon on windows)
   * separated list of paths and adds them to a ZipOutputStream.
   */
  public void packClassPathIntoZipFile(String path,
                                       ZipOutputStream zos)
    throws IOException {
    String pieces[] = Base.split(path, File.pathSeparatorChar);

    for (int i = 0; i < pieces.length; i++) {
      if (pieces[i].length() == 0) continue;
      //System.out.println("checking piece " + pieces[i]);

      // is it a jar file or directory?
      if (pieces[i].toLowerCase().endsWith(".jar") ||
          pieces[i].toLowerCase().endsWith(".zip")) {
        try {
          ZipFile file = new ZipFile(pieces[i]);
          Enumeration entries = file.entries();
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
  static public void packClassPathIntoZipFileRecursive(File dir,
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
          zos.write(Base.grabFile(sub));
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
      for (int i = 0; i < hiddenCount; i++) {
        hidden[i].save();  // this will force a save
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
    if (apath.startsWith(Sketchbook.examplesPath) ||
        apath.startsWith(Sketchbook.librariesPath)) {
      return true;

      // canWrite() doesn't work on directories
      //} else if (!folder.canWrite()) {
    } else {
      // check to see if each modified code file can be written to
      for (int i = 0; i < codeCount; i++) {
        if (code[i].modified &&
            !code[i].file.canWrite() &&
            code[i].file.exists()) {
          //System.err.println("found a read-only file " + code[i].file);
          return true;
        }
      }
      //return true;
    }
    return false;
  }


  /**
   * Returns path to the main .pde file for this sketch.
   */
  public String getMainFilePath() {
    return code[0].file.getAbsolutePath();
  }
}
