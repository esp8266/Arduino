/*
 * This file is part of Arduino.
 *
 * Copyright 2014 Arduino LLC (http://www.arduino.cc/)
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */
package cc.arduino.contributions.libraries;

import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.module.mrbean.MrBeanModule;
import processing.app.BaseNoGui;
import processing.app.I18n;
import processing.app.helpers.FileUtils;
import processing.app.helpers.filefilters.OnlyDirs;
import processing.app.packages.LegacyUserLibrary;
import processing.app.packages.LibraryList;
import processing.app.packages.UserLibrary;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.List;

import static processing.app.I18n._;

public class LibrariesIndexer {

  private LibrariesIndex index;
  private final LibraryList installedLibraries = new LibraryList();
  private final LibraryList installedLibrariesWithDuplicates = new LibraryList();
  private List<File> librariesFolders;
  private final File indexFile;
  private final File stagingFolder;
  private File sketchbookLibrariesFolder;

  public LibrariesIndexer(File preferencesFolder) {
    indexFile = new File(preferencesFolder, "library_index.json");
    stagingFolder = new File(preferencesFolder, "staging" + File.separator +
            "libraries");
  }

  public void parseIndex() throws IOException {
    parseIndex(indexFile);
    // TODO: resolve libraries inner references
  }

  private void parseIndex(File indexFile) throws IOException {
    InputStream indexIn = null;
    try {
      indexIn = new FileInputStream(indexFile);
      ObjectMapper mapper = new ObjectMapper();
      mapper.registerModule(new MrBeanModule());
      mapper.configure(DeserializationFeature.ACCEPT_SINGLE_VALUE_AS_ARRAY, true);
      mapper.configure(DeserializationFeature.EAGER_DESERIALIZER_FETCH, true);
      mapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
      index = mapper.readValue(indexIn, LibrariesIndex.class);

      for (ContributedLibrary library : index.getLibraries()) {
        if (library.getCategory() == null || "".equals(library.getCategory())) {
          library.setCategory("Uncategorized");
        }
      }
    } finally {
      if (indexIn != null) {
        indexIn.close();
      }
    }
  }

  public void setLibrariesFolders(List<File> _librariesFolders) {
    librariesFolders = _librariesFolders;
    rescanLibraries();
  }

  public void rescanLibraries() {
    // Clear all installed flags
    installedLibraries.clear();
    installedLibrariesWithDuplicates.clear();
    for (ContributedLibrary lib : index.getLibraries())
      lib.setInstalled(false);

    // Rescan libraries
    for (File folder : librariesFolders)
      scanInstalledLibraries(folder, folder.equals(sketchbookLibrariesFolder));
  }

  private void scanInstalledLibraries(File folder, boolean isSketchbook) {
    File list[] = folder.listFiles(OnlyDirs.ONLY_DIRS);
    // if a bad folder or something like that, this might come back null
    if (list == null)
      return;

    for (File subfolder : list) {
      if (!BaseNoGui.isSanitaryName(subfolder.getName())) {
        String mess = I18n.format(_("The library \"{0}\" cannot be used.\n"
                        + "Library names must contain only basic letters and numbers.\n"
                        + "(ASCII only and no spaces, and it cannot start with a number)"),
                subfolder.getName());
        BaseNoGui.showMessage(_("Ignoring bad library name"), mess);
        continue;
      }

      try {
        scanLibrary(subfolder, isSketchbook);
      } catch (IOException e) {
        System.out.println(I18n.format(_("Invalid library found in {0}: {1}"), subfolder, e.getMessage()));
      }
    }
  }

  private void scanLibrary(File folder, boolean isSketchbook) throws IOException {
    boolean readOnly = !FileUtils.isSubDirectory(sketchbookLibrariesFolder, folder);

    // A library is considered "legacy" if it doesn't contains
    // a file called "library.properties"
    File check = new File(folder, "library.properties");
    if (!check.exists() || !check.isFile()) {
      // Create a legacy library and exit
      LegacyUserLibrary lib = LegacyUserLibrary.create(folder);
      lib.setReadOnly(readOnly);
      installedLibraries.addOrReplace(lib);
      if (isSketchbook) {
        installedLibrariesWithDuplicates.add(lib);
      } else {
        installedLibrariesWithDuplicates.addOrReplace(lib);
      }
      return;
    }

    // Create a regular library
    UserLibrary lib = UserLibrary.create(folder);
    lib.setReadOnly(readOnly);
    installedLibraries.addOrReplace(lib);
    if (isSketchbook) {
      installedLibrariesWithDuplicates.add(lib);
    } else {
      installedLibrariesWithDuplicates.addOrReplace(lib);
    }

    // Check if we can find the same library in the index
    // and mark it as installed
    ContributedLibrary foundLib = index.find(lib.getName(), lib.getParsedVersion());
    if (foundLib != null) {
      foundLib.setInstalled(true);
      foundLib.setInstalledFolder(folder);
      foundLib.setReadOnly(readOnly);
      lib.setTypes(foundLib.getTypes());
    }

    if (lib.isReadOnly() && lib.getTypes() == null && !lib.getDeclaredTypes().isEmpty()) {
      lib.setTypes(lib.getDeclaredTypes());
    }

    if (lib.getTypes() == null) {
      lib.setTypes(Arrays.asList("Contributed"));
    }
  }

  public LibrariesIndex getIndex() {
    return index;
  }

  public LibraryList getInstalledLibraries() {
    return installedLibraries;
  }

  // Same as getInstalledLibraries(), but allow duplicates between
  // builtin+package libraries and sketchbook installed libraries.
  // However, do not report duplicates among builtin and packages, to
  // allow any package to override builtin libraries without being
  // reported as duplicates.
  public LibraryList getInstalledLibrariesWithDuplicates() {
    return installedLibrariesWithDuplicates;
  }

  public File getStagingFolder() {
    return stagingFolder;
  }

  /**
   * Set the sketchbook library folder. <br />
   * New libraries will be installed here. <br />
   * Libraries not found on this folder will be marked as read-only.
   *
   * @param folder
   */
  public void setSketchbookLibrariesFolder(File folder) {
    this.sketchbookLibrariesFolder = folder;
  }

  public File getSketchbookLibrariesFolder() {
    return sketchbookLibrariesFolder;
  }

  public File getIndexFile() {
    return indexFile;
  }
}
