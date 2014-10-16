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
package cc.arduino.libraries.contributions;

import static processing.app.I18n._;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;

import processing.app.BaseNoGui;
import processing.app.I18n;
import processing.app.helpers.filefilters.OnlyDirs;
import processing.app.packages.LegacyUserLibrary;
import processing.app.packages.LibraryList;
import processing.app.packages.UserLibrary;

import com.fasterxml.jackson.core.JsonParseException;
import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.module.mrbean.MrBeanModule;

public class LibrariesIndexer {

  private LibrariesIndex index;
  private File indexFile;

  public LibrariesIndexer(File _settingsFolder) {
    indexFile = new File(_settingsFolder, "library_index.json");
  }

  public void parseIndex() throws JsonParseException, IOException {
    parseIndex(indexFile);
    System.out.println(index);

    // TODO: resolve libraries inner references
  }

  private void parseIndex(File indexFile) throws JsonParseException,
      IOException {
    InputStream indexIn = new FileInputStream(indexFile);
    ObjectMapper mapper = new ObjectMapper();
    mapper.registerModule(new MrBeanModule());
    mapper.configure(DeserializationFeature.ACCEPT_SINGLE_VALUE_AS_ARRAY, true);
    mapper.configure(DeserializationFeature.EAGER_DESERIALIZER_FETCH, true);
    mapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, true);
    index = mapper.readValue(indexIn, LibrariesIndex.class);
  }

  public LibraryList scanLibraries(List<File> folders) throws IOException {
    LibraryList res = new LibraryList();
    for (File folder : folders)
      res.addOrReplaceAll(scanLibraries(folder));
    return res;
  }

  private LibraryList scanLibraries(File folder) throws IOException {
    LibraryList res = new LibraryList();

    File list[] = folder.listFiles(new OnlyDirs());
    // if a bad folder or something like that, this might come back null
    if (list == null)
      return res;

    for (File subfolder : list) {
      if (!BaseNoGui.isSanitaryName(subfolder.getName())) {
        String mess = I18n
            .format(_("The library \"{0}\" cannot be used.\n"
                        + "Library names must contain only basic letters and numbers.\n"
                        + "(ASCII only and no spaces, and it cannot start with a number)"),
                    subfolder.getName());
        BaseNoGui.showMessage(_("Ignoring bad library name"), mess);
        continue;
      }

      try {
        ContributedLibrary lib = scanLibrary(subfolder);

        // (also replace previously found libs with the same name)
        if (lib != null)
          res.addOrReplace(lib);
      } catch (IOException e) {
        System.out.println(I18n.format(_("Invalid library found in {0}: {1}"),
                                       subfolder, e.getMessage()));
      }
    }
    return res;
  }

  private ContributedLibrary scanLibrary(File subfolder) throws IOException {
    // A library is considered non-Legacy if it contains
    // a file called "library.properties"
    File check = new File(subfolder, "library.properties");
    if (!check.exists() || !check.isFile())
      return LegacyUserLibrary.create(subfolder);

    ContributedLibrary lib = UserLibrary.create(subfolder);

    // Check if we can find the same library in the index
    // String libName = subfolder.getName(); // XXX: lib.getName()?
    // ContributedLibrary foundLib = index.find(libName, lib.getVersion());
    // if (foundLib != null) {
    // foundLib.setInstalled(true);
    // foundLib.setInstalledFolder(subfolder);
    // return foundLib;
    // }

    return lib;
  }

  public static void main(String[] args) throws JsonParseException, IOException {
    LibrariesIndexer indexer = new LibrariesIndexer(new File(
        "/home/megabug/.arduino15"));
    indexer.parseIndex();
    LibraryList libs = indexer.scanLibraries(new File(
        "/home/megabug/sketchbook/libraries"));
    for (ContributedLibrary lib : libs) {
      System.out.println(lib);
    }
  }
}
