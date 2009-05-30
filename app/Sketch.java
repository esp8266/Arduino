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
import processing.core.*;

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
  static final int CPP = 1;
  static final int C = 2;
  static final int HEADER = 3;
  
  static final String flavorExtensionsReal[] = new String[] { ".pde", ".cpp", ".c", ".h" };
  static final String flavorExtensionsShown[] = new String[] { "", ".cpp", ".c", ".h" };

  public SketchCode current;
  int currentIndex;
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
  public Vector importedLibraries; // vec of Library objects

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
    } else if (mainFilename.endsWith(".c")) {
      name = mainFilename.substring(0, mainFilename.length() - 2);
    } else if (mainFilename.endsWith(".h")) {
      name = mainFilename.substring(0, mainFilename.length() - 2);
    } else if (mainFilename.endsWith(".cpp")) {
      name = mainFilename.substring(0, mainFilename.length() - 4);
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
   *
   * Generally this is only done once, rather than
   * each time a change is made, because otherwise it gets to be
   * a nightmare to keep track of what files went where, because
   * not all the data will be saved to disk.
   *
   * This also gets called when the main sketch file is renamed,
   * because the sketch has to be reloaded from a different folder.
   *
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
      else if (list[i].endsWith(".c")) codeCount++;
      else if (list[i].endsWith(".h")) codeCount++;
      else if (list[i].endsWith(".cpp")) codeCount++;
      else if (list[i].endsWith(".pde.x")) hiddenCount++;
      else if (list[i].endsWith(".c.x")) hiddenCount++;
      else if (list[i].endsWith(".h.x")) hiddenCount++;
      else if (list[i].endsWith(".cpp.x")) hiddenCount++;
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

      } else if (list[i].endsWith(".c")) {
        code[codeCounter++] =
          new SketchCode(list[i].substring(0, list[i].length() - 2),
                      new File(folder, list[i]),
                      C);

      } else if (list[i].endsWith(".h")) {
        code[codeCounter++] =
          new SketchCode(list[i].substring(0, list[i].length() - 2),
                      new File(folder, list[i]),
                      HEADER);

      } else if (list[i].endsWith(".cpp")) {
        code[codeCounter++] =
          new SketchCode(list[i].substring(0, list[i].length() - 4),
                      new File(folder, list[i]),
                      CPP);

      } else if (list[i].endsWith(".pde.x")) {
        hidden[hiddenCounter++] =
          new SketchCode(list[i].substring(0, list[i].length() - 6),
                      new File(folder, list[i]),
                      PDE);

      } else if (list[i].endsWith(".c.x")) {
        hidden[hiddenCounter++] =
          new SketchCode(list[i].substring(0, list[i].length() - 4),
                      new File(folder, list[i]),
                      C);
      } else if (list[i].endsWith(".h.x")) {
        hidden[hiddenCounter++] =
          new SketchCode(list[i].substring(0, list[i].length() - 4),
                      new File(folder, list[i]),
                      HEADER);
      } else if (list[i].endsWith(".cpp.x")) {
        hidden[hiddenCounter++] =
          new SketchCode(list[i].substring(0, list[i].length() - 6),
                      new File(folder, list[i]),
                      CPP);
      }
    }

    // some of the hidden files may be bad too, so use hiddenCounter
    // added for rev 0121, fixes bug found by axel
    hiddenCount = hiddenCounter;

    // remove any entries that didn't load properly from codeCount
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
    String prompt = (currentIndex == 0) ?
      "New name for sketch:" : "New name for file:";
    String oldName = current.name + flavorExtensionsShown[current.flavor];
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

    if (newName.trim().equals(".c") ||
        newName.trim().equals(".h") ||
        newName.trim().equals(".pde") ||
        newName.trim().equals(".cpp")) {
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

    } else if (newName.endsWith(".c") || newName.endsWith(".cpp") ||
      newName.endsWith(".h")) {
      // don't show this error if creating a new tab
      if (renamingCode && (code[0] == current)) {
        Base.showWarning("Problem with rename",
                         "The main .pde file cannot be .c, .cpp, or .h file.\n" +
                         "(It may be time for your to graduate to a\n" +
                         "\"real\" programming environment)", null);
        return;
      }

      newFilename = newName;
      if(newName.endsWith(".c")) {
        newName = newName.substring(0, newName.length() - 2);
        newFlavor = C;
      } if(newName.endsWith(".h")) {
        newName = newName.substring(0, newName.length() - 2);
        newFlavor = HEADER;
      } else if(newName.endsWith(".cpp")) {
        newName = newName.substring(0, newName.length() - 4);
        newFlavor = CPP;
      }
    } else {
      newFilename = newName + ".pde";
      newFlavor = PDE;
    }

    // dots are allowed for the .pde and .java, but not in the name
    // make sure the user didn't name things poo.time.pde
    // or something like that (nothing against poo time)
    if (newName.indexOf('.') != -1) {
      newName = Sketchbook.sanitizedName(newName);
      newFilename = newName + ((newFlavor == PDE) ? ".pde" : ".cpp");
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
      if (currentIndex == 0) {
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
        editor.handleOpenUnchecked(mainFilename,
                                   currentIndex,
                                   editor.textarea.getSelectionStart(),
                                   editor.textarea.getSelectionEnd(),
                                   editor.textarea.getScrollPosition());

        // get the changes into the sketchbook menu
        // (re-enabled in 0115 to fix bug #332)
        editor.sketchbook.rebuildMenus();

      } else {  // else if something besides code[0]
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
    setCurrent(newName + flavorExtensionsShown[newFlavor]);

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
    String prompt = (currentIndex == 0) ?
      "Are you sure you want to delete this sketch?" :
      "Are you sure you want to delete \"" + current.name +
      flavorExtensionsShown[current.flavor] + "\"?";
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
    if (currentIndex == 0) {
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
  }


  public void unhideCode(String what) {
    SketchCode unhideCode = null;
    String name = what.substring(0,
      (what.indexOf(".") == -1 ? what.length() : what.indexOf(".")));
    String extension = what.indexOf(".") == -1 ? "" :
      what.substring(what.indexOf("."));
      
    for (int i = 0; i < hiddenCount; i++) {
      if (hidden[i].name.equals(name) &&
        Sketch.flavorExtensionsShown[hidden[i].flavor].equals(extension)) {
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
  public void setModified(boolean state) {
    current.modified = state;
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

    editor.handleOpenUnchecked(newFile.getPath(),
                               currentIndex,
                               editor.textarea.getSelectionStart(),
                               editor.textarea.getSelectionEnd(),
                               editor.textarea.getScrollPosition());

    // Name changed, rebuild the sketch menus
    editor.sketchbook.rebuildMenusAsync();

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

    // now do the work of adding the file
    addFile(sourceFile);
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
    boolean addingCode = false;

    // if the file appears to be code related, drop it
    // into the code folder, instead of the data folder
    if (filename.toLowerCase().endsWith(".o") /*||
        filename.toLowerCase().endsWith(".jar") ||
        filename.toLowerCase().endsWith(".dll") ||
        filename.toLowerCase().endsWith(".jnilib") ||
        filename.toLowerCase().endsWith(".so") */ ) {
      //File codeFolder = new File(this.folder, "code");
      if (!codeFolder.exists()) codeFolder.mkdirs();
      destFile = new File(codeFolder, filename);

    } else if (filename.toLowerCase().endsWith(".pde") ||
               filename.toLowerCase().endsWith(".c") ||
               filename.toLowerCase().endsWith(".h") ||
               filename.toLowerCase().endsWith(".cpp")) {
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
      return false;
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
        return false;
      }
    }

    // make the tabs update after this guy is added
    if (addingCode) {
      String newName = destFile.getName();
      int newFlavor = -1;
      if (newName.toLowerCase().endsWith(".pde")) {
        newName = newName.substring(0, newName.length() - 4);
        newFlavor = PDE;
      } else if (newName.toLowerCase().endsWith(".c")) {
        newName = newName.substring(0, newName.length() - 2);
        newFlavor = C;
      } else if (newName.toLowerCase().endsWith(".h")) {
        newName = newName.substring(0, newName.length() - 2);
        newFlavor = HEADER;
      } else { // ".cpp"
        newName = newName.substring(0, newName.length() - 4);
        newFlavor = CPP;
      }

      // see also "nameCode" for identical situation
      SketchCode newCode = new SketchCode(newName, destFile, newFlavor);
      insertCode(newCode);
      sortCode();
      setCurrent(newName);
      editor.header.repaint();
    }
    return true;
  }


  public void importLibrary(String jarPath) {
    //System.out.println(jarPath);
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    //String list[] = Compiler.packageListFromClassPath(jarPath);
    FileFilter onlyHFiles = new FileFilter() {
      public boolean accept(File file) {
        return (file.getName()).endsWith(".h");
      }
    };
    
    File list[] = new File(jarPath).listFiles(onlyHFiles);

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
      buffer.append("#include <");
      buffer.append(list[i].getName());
      buffer.append(">\n");
    }
    buffer.append('\n');
    buffer.append(editor.getText());
    editor.setText(buffer.toString(), 0, 0);  // scroll to start
    setModified(true);
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
    // if current is null, then this is the first setCurrent(0)
    if ((currentIndex == which) && (current != null)) {
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
    currentIndex = which;
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
    SketchCode unhideCode = null;
    String name = findName.substring(0,
      (findName.indexOf(".") == -1 ? findName.length() : findName.indexOf(".")));
    String extension = findName.indexOf(".") == -1 ? "" :
      findName.substring(findName.indexOf("."));

    for (int i = 0; i < codeCount; i++) {
      if (name.equals(code[i].name) &&
        Sketch.flavorExtensionsShown[code[i].flavor].equals(extension)) {
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
  public boolean handleRun(Target target) throws RunnerException {
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

    // make up a temporary class name to suggest.
    // name will only be used if the code is not in ADVANCED mode.
    String suggestedClassName =
      ("Temporary_" + String.valueOf((int) (Math.random() * 10000)) +
       "_" + String.valueOf((int) (Math.random() * 10000)));

    // handle preprocessing the main file's code
    //mainClassName = build(TEMP_BUILD_PATH, suggestedClassName);
    mainClassName =
      build(target, tempBuildFolder.getAbsolutePath(), suggestedClassName);
    size(tempBuildFolder.getAbsolutePath(), name);
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
  protected String build(Target target, String buildPath, String suggestedClassName)
    throws RunnerException {

    // build unbuilt buildable libraries
    // completely independent from sketch, so run all the time
    editor.prepareLibraries();

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
//      codeFolderPackages =
//        Compiler.packageListFromClassPath(codeFolderClassPath);
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
        if (code[i].flavor == C || code[i].flavor == CPP) {
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
/*    if ((bigCode.indexOf(" class ") != -1) ||
        (bigCode.indexOf("\nclass ") != -1)) {
      externalRuntime = true;
    }
*/
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
                           suggestedClassName, codeFolderPackages, target);
      if (className == null) {
        throw new RunnerException("Could not find main class");
        // this situation might be perfectly fine,
        // (i.e. if the file is empty)
        //System.out.println("No class found in " + code[i].name);
        //System.out.println("(any code in that file will be ignored)");
        //System.out.println();

      } else {
        code[0].preprocName = className + ".cpp";
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
      errorLine -= preprocessor.prototypeCount;
      errorLine -= preprocessor.headerCount;

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
                            "this online at " +
                            "https://developer.berlios.de/bugs/?group_id=3590",
                         e);
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
        errorLine -= preprocessor.prototypeCount;
        errorLine -= preprocessor.headerCount;

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
    try {
      LibraryManager libraryManager = new LibraryManager();
      Collection libraries = libraryManager.getAll();
      for (Iterator i = libraries.iterator(); i.hasNext(); ) {
        Library library = (Library) i.next();
        File[] headerFiles = library.getHeaderFiles();
        
        for (int j = 0; j < headerFiles.length; j++)
          for (int k = 0; k < imports.length; k++)
            if (headerFiles[j].getName().equals(imports[k]) &&
              !importedLibraries.contains(library)) {
              importedLibraries.add(library);
              //System.out.println("Adding library " + library.getName());
            }
      }
    } catch (IOException e) {
      System.err.println("Error finding libraries:");
      e.printStackTrace();
      throw new RunnerException(e.getMessage());
    }

    //for (int i = 0; i < imports.length; i++) {
      /*
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
      */
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
    //}


    // 3. then loop over the code[] and save each .java file

    for (int i = 0; i < codeCount; i++) {
      if (code[i].flavor == CPP || code[i].flavor == C || code[i].flavor == HEADER) {
        // no pre-processing services necessary for java files
        // just write the the contents of 'program' to a .java file
        // into the build directory. uses byte stream and reader/writer
        // shtuff so that unicode bunk is properly handled
        String filename = code[i].name + flavorExtensionsReal[code[i].flavor];
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
    // note: this has been changed to catch build exceptions, adjust
    // line number for number of included prototypes, and rethrow
    
    Compiler compiler = new Compiler();
    boolean success;
    try {
      success = compiler.compile(this, buildPath, target);
    } catch (RunnerException re) {
      throw new RunnerException(re.getMessage(),
                                re.file,
                                re.line - preprocessor.prototypeCount - preprocessor.headerCount,
                                re.column);
    } catch (Exception ex) {
      // TODO better method for handling this?
      throw new RunnerException(ex.toString());
    }
    //System.out.println("success = " + success + " ... " + primaryClassName);
    return success ? primaryClassName : null;
  }
  
  protected void size(String buildPath, String suggestedClassName)
    throws RunnerException {
    long size = 0;
    long maxsize = Preferences.getInteger("boards." + Preferences.get("board") + ".upload.maximum_size");
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

  protected String upload(String buildPath, String suggestedClassName)
    throws RunnerException {
    
    Uploader uploader;

    // download the program
    //
    uploader = new AvrdudeUploader();
    // macos9 now officially broken.. see PdeCompilerJavac
    //PdeCompiler compiler =
    //  ((PdeBase.platform == PdeBase.MACOS9) ?
    //   new PdeCompilerJavac(buildPath, className, this) :
    //   new PdeCompiler(buildPath, className, this));

    // run the compiler, and funnel errors to the leechErr
    // which is a wrapped around
    // (this will catch and parse errors during compilation
    // the messageStream will call message() for 'compiler')
    //MessageStream messageStream = new MessageStream(downloader);
    //PrintStream leechErr = new PrintStream(messageStream);
    //boolean result = compiler.compileJava(leechErr);
    //return compiler.compileJava(leechErr);
    boolean success =
      uploader.uploadUsingPreferences(buildPath, suggestedClassName);

    return success ? suggestedClassName : null;
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
  public boolean exportApplet(Target target) throws RunnerException {
    // make sure the user didn't hide the sketch folder
    ensureExistence();

    // fix for issue posted on the board. make sure that the code
    // is reloaded when exporting and an external editor is being used.
    if (Preferences.getBoolean("editor.external")) {
      // nuke previous files and settings
      load();
    } else {
      current.program = editor.getText();
    }

    zipFileContents = new Hashtable();

    // nuke the old applet folder because it can cause trouble
    File appletFolder = new File(folder, "applet");
    Base.removeDir(appletFolder);
    appletFolder.mkdirs();

    // build the sketch
    String foundName = build(target, appletFolder.getPath(), name);
    size(appletFolder.getPath(), name);
    foundName = upload(appletFolder.getPath(), name);
    // (already reported) error during export, exit this function
    if (foundName == null) return false;

    // if name != exportSketchName, then that's weirdness
    // BUG unfortunately, that can also be a bug in the preproc :(
    if (!name.equals(foundName)) {
      Base.showWarning("Error during export",
                       "Sketch name is " + name + " but the sketch\n" +
                       "name in the code was " + foundName, null);
      return false;
    }

/*    int wide = PApplet.DEFAULT_WIDTH;
    int high = PApplet.DEFAULT_HEIGHT;
    String renderer = "";

    PatternMatcher matcher = new Perl5Matcher();
    PatternCompiler compiler = new Perl5Compiler();

    // this matches against any uses of the size() function,
    // whether they contain numbers of variables or whatever.
    // this way, no warning is shown if size() isn't actually
    // used in the applet, which is the case especially for
    // beginners that are cutting/pasting from the reference.
    // modified for 83 to match size(XXX, ddd so that it'll
    // properly handle size(200, 200) and size(200, 200, P3D)
    String sizing =
      // match the renderer string as well
      "[\\s\\;]size\\s*\\(\\s*(\\S+)\\s*,\\s*(\\d+),?\\s*([^\\)]*)\\s*\\)";
      // match just the width and height
      //"[\\s\\;]size\\s*\\(\\s*(\\S+)\\s*,\\s*(\\d+)(.*)\\)";
    Pattern pattern = compiler.compile(sizing);

    // adds a space at the beginning, in case size() is the very
    // first thing in the program (very common), since the regexp
    // needs to check for things in front of it.
    PatternMatcherInput input =
      new PatternMatcherInput(" " + scrubComments(code[0].program));
    if (matcher.contains(input, pattern)) {
      MatchResult result = matcher.getMatch();

      try {
        wide = Integer.parseInt(result.group(1).toString());
        high = Integer.parseInt(result.group(2).toString());

        renderer = result.group(3).toString(); //.trim();

      } catch (NumberFormatException e) {
        // found a reference to size, but it didn't
        // seem to contain numbers
        final String message =
          "The size of this applet could not automatically be\n" +
          "determined from your code. You'll have to edit the\n" +
          "HTML file to set the size of the applet.";

        Base.showWarning("Could not find applet size", message, null);
      }
    }  // else no size() command found

    // originally tried to grab this with a regexp matcher,
    // but it wouldn't span over multiple lines for the match.
    // this could prolly be forced, but since that's the case
    // better just to parse by hand.
    StringBuffer dbuffer = new StringBuffer();
    String lines[] = PApplet.split(code[0].program, '\n');
    for (int i = 0; i < lines.length; i++) {
      if (lines[i].trim().startsWith("/**")) {  // this is our comment
*/        // some smartass put the whole thing on the same line
        //if (lines[j].indexOf("*/") != -1) break;

//        for (int j = i+1; j < lines.length; j++) {
//          if (lines[j].trim().endsWith("*/")) {
            // remove the */ from the end, and any extra *s
            // in case there's also content on this line
            // nah, don't bother.. make them use the three lines
//            break;
//          }

/*          int offset = 0;
          while ((offset < lines[j].length()) &&
                 ((lines[j].charAt(offset) == '*') ||
                  (lines[j].charAt(offset) == ' '))) {
            offset++;
          }
          // insert the return into the html to help w/ line breaks
          dbuffer.append(lines[j].substring(offset) + "\n");
        }
      }
    }
    String description = dbuffer.toString();

    StringBuffer sources = new StringBuffer();
    for (int i = 0; i < codeCount; i++) {
      sources.append("<a href=\"" + code[i].file.getName() + "\">" +
                     code[i].name + "</a> ");
    }

    File htmlOutputFile = new File(appletFolder, "index.html");
    FileOutputStream fos = new FileOutputStream(htmlOutputFile);
    PrintStream ps = new PrintStream(fos);

    // @@sketch@@, @@width@@, @@height@@, @@archive@@, @@source@@
    // and now @@description@@

    InputStream is = null;
    // if there is an applet.html file in the sketch folder, use that
    File customHtml = new File(folder, "applet.html");
    if (customHtml.exists()) {
      is = new FileInputStream(customHtml);
    }
    if (is == null) {
      is = Base.getStream("applet.html");
    }

    // copy the loading gif to the applet
    String LOADING_IMAGE = "loading.gif";
    File loadingImage = new File(folder, LOADING_IMAGE);
    if (!loadingImage.exists()) {
      loadingImage = new File("lib", LOADING_IMAGE);
    }
    Base.copyFile(loadingImage, new File(appletFolder, LOADING_IMAGE));
*/
    // copy the source files to the target, since we like
    // to encourage people to share their code
    for (int i = 0; i < codeCount; i++) {
      try {
        Base.copyFile(code[i].file,
                      new File(appletFolder, code[i].file.getName()));
      } catch (IOException e) {
        e.printStackTrace();
      }
    }

/*    // create new .jar file
    FileOutputStream zipOutputFile =
      new FileOutputStream(new File(appletFolder, name + ".jar"));
    ZipOutputStream zos = new ZipOutputStream(zipOutputFile);
    ZipEntry entry;
    archives.append(name + ".jar");

    // add the manifest file
    addManifest(zos);

    // add the contents of the code folder to the jar
    // unpacks all jar files, unless multi jar files selected in prefs
    if (codeFolder.exists()) {
      String includes = Compiler.contentsToClassPath(codeFolder);
      packClassPathIntoZipFile(includes, zos);
    }

    // add contents of 'library' folders to the jar file
    // if a file called 'export.txt' is in there, it contains
    // a list of the files that should be exported.
    // otherwise, all files are exported.
    Enumeration en = importedLibraries.elements();
    while (en.hasMoreElements()) {
      // in the list is a File object that points the
      // library sketch's "library" folder
      File libraryFolder = (File)en.nextElement();
      File exportSettings = new File(libraryFolder, "export.txt");
      String exportList[] = null;
      if (exportSettings.exists()) {
        String info[] = Base.loadStrings(exportSettings);
        for (int i = 0; i < info.length; i++) {
          if (info[i].startsWith("applet")) {
            int idx = info[i].indexOf('=');  // get applet= or applet =
            String commas = info[i].substring(idx+1).trim();
            exportList = PApplet.split(commas, ", ");
          }
        }
      } else {
        exportList = libraryFolder.list();
      }
      for (int i = 0; i < exportList.length; i++) {
        if (exportList[i].equals(".") ||
            exportList[i].equals("..")) continue;

        exportList[i] = PApplet.trim(exportList[i]);
        if (exportList[i].equals("")) continue;

        File exportFile = new File(libraryFolder, exportList[i]);
        if (!exportFile.exists()) {
          System.err.println("File " + exportList[i] + " does not exist");

        } else if (exportFile.isDirectory()) {
          System.err.println("Ignoring sub-folder \"" + exportList[i] + "\"");

        } else if (exportFile.getName().toLowerCase().endsWith(".zip") ||
                   exportFile.getName().toLowerCase().endsWith(".jar")) {
          if (separateJar) {
            String exportFilename = exportFile.getName();
            Base.copyFile(exportFile, new File(appletFolder, exportFilename));
            if (renderer.equals("OPENGL") &&
                exportFilename.indexOf("natives") != -1) {
              // don't add these to the archives list
            } else {
              archives.append("," + exportFilename);
            }
          } else {
          packClassPathIntoZipFile(exportFile.getAbsolutePath(), zos);
          }

        } else {  // just copy the file over.. prolly a .dll or something
          Base.copyFile(exportFile,
                           new File(appletFolder, exportFile.getName()));
        }
      }
    }
*/
/*    String bagelJar = "lib/core.jar";
    packClassPathIntoZipFile(bagelJar, zos);
    }

    // files to include from data directory
    // TODO this needs to be recursive
    if (dataFolder.exists()) {
      String dataFiles[] = dataFolder.list();
      for (int i = 0; i < dataFiles.length; i++) {
        // don't export hidden files
        // skipping dot prefix removes all: . .. .DS_Store
        if (dataFiles[i].charAt(0) == '.') continue;

        entry = new ZipEntry(dataFiles[i]);
        zos.putNextEntry(entry);
        zos.write(Base.grabFile(new File(dataFolder, dataFiles[i])));
        zos.closeEntry();
      }
    }

    // add the project's .class files to the jar
    // just grabs everything from the build directory
    // since there may be some inner classes
    // (add any .class files from the applet dir, then delete them)
    // TODO this needs to be recursive (for packages)
    String classfiles[] = appletFolder.list();
    for (int i = 0; i < classfiles.length; i++) {
      if (classfiles[i].endsWith(".class")) {
        entry = new ZipEntry(classfiles[i]);
        zos.putNextEntry(entry);
        zos.write(Base.grabFile(new File(appletFolder, classfiles[i])));
        zos.closeEntry();
      }
    }
*/
    String classfiles[] = appletFolder.list();
    // remove the .class files from the applet folder. if they're not
    // removed, the msjvm will complain about an illegal access error,
    // since the classes are outside the jar file.
    for (int i = 0; i < classfiles.length; i++) {
      if (classfiles[i].endsWith(".class")) {
        File deadguy = new File(appletFolder, classfiles[i]);
        if (!deadguy.delete()) {
          Base.showWarning("Could not delete",
                           classfiles[i] + " could not \n" +
                           "be deleted from the applet folder.  \n" +
                           "You'll need to remove it by hand.", null);
        }
      }
    }

    // close up the jar file
/*    zos.flush();
    zos.close();
*/
    if(Preferences.getBoolean("uploader.open_folder"))
      Base.openFolder(appletFolder);
    return true;
  }


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


/*  public void addManifest(ZipOutputStream zos) throws IOException {
    ZipEntry entry = new ZipEntry("META-INF/MANIFEST.MF");
    zos.putNextEntry(entry);

    String contents =
      "Manifest-Version: 1.0\n" +
      "Created-By: Processing " + Base.VERSION_NAME + "\n" +
      "Main-Class: " + name + "\n";  // TODO not package friendly
    zos.write(contents.getBytes());
    zos.closeEntry();
*/
    /*
      for (int i = 0; i < bagelClasses.length; i++) {
        if (!bagelClasses[i].endsWith(".class")) continue;
        entry = new ZipEntry(bagelClasses[i]);
        zos.putNextEntry(entry);
        zos.write(Base.grabFile(new File(exportDir + bagelClasses[i])));
        zos.closeEntry();
      }
    */
/*  }
*/

  /**
   * Slurps up .class files from a colon (or semicolon on windows)
   * separated list of paths and adds them to a ZipOutputStream.
   */
/*  public void packClassPathIntoZipFile(String path,
                                       ZipOutputStream zos)
    throws IOException {
    String pieces[] = PApplet.split(path, File.pathSeparatorChar);

    for (int i = 0; i < pieces.length; i++) {
      if (pieces[i].length() == 0) continue;

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
*/

  /**
   * Continue the process of magical exporting. This function
   * can be called recursively to walk through folders looking
   * for more goodies that will be added to the ZipOutputStream.
   */
/*  static public void packClassPathIntoZipFileRecursive(File dir,
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
*/

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


  public void prevCode() {
    int prev = currentIndex - 1;
    if (prev < 0) prev = codeCount-1;
    setCurrent(prev);
  }


  public void nextCode() {
    setCurrent((currentIndex + 1) % codeCount);
  }
}
