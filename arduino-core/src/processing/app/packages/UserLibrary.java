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
package processing.app.packages;

import cc.arduino.contributions.libraries.ContributedLibrary;
import cc.arduino.contributions.libraries.ContributedLibraryReference;
import processing.app.helpers.FileUtils;
import processing.app.helpers.PreferencesMap;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

public class UserLibrary extends ContributedLibrary {

  private String name;
  private String version;
  private String author;
  private String maintainer;
  private String sentence;
  private String paragraph;
  private String website;
  private String category;
  private String license;
  private List<String> architectures;
  private List<String> types;
  private List<String> declaredTypes;

  private static final List<String> MANDATORY_PROPERTIES = Arrays
    .asList("name", "version", "author", "maintainer",
      "sentence", "paragraph", "url");

  private static final List<String> CATEGORIES = Arrays.asList(
    "Display", "Communication", "Signal Input/Output", "Sensors",
    "Device Control", "Timing", "Data Storage", "Data Processing", "Other",
    "Uncategorized");

  public static UserLibrary create(File libFolder) throws IOException {
    // Parse metadata
    File propertiesFile = new File(libFolder, "library.properties");
    PreferencesMap properties = new PreferencesMap();
    properties.load(propertiesFile);

    // Library sanity checks
    // ---------------------

    // Compatibility with 1.5 rev.1 libraries:
    // "email" field changed to "maintainer"
    if (!properties.containsKey("maintainer") && properties.containsKey("email")) {
      properties.put("maintainer", properties.get("email"));
    }

    // Compatibility with 1.5 rev.1 libraries:
    // "arch" folder no longer supported
    File archFolder = new File(libFolder, "arch");
    if (archFolder.isDirectory())
      throw new IOException("'arch' folder is no longer supported! See http://goo.gl/gfFJzU for more information");

    // Check mandatory properties
    for (String p : MANDATORY_PROPERTIES)
      if (!properties.containsKey(p))
        throw new IOException("Missing '" + p + "' from library");

    // Check layout
    LibraryLayout layout;
    File srcFolder = new File(libFolder, "src");

    if (srcFolder.exists() && srcFolder.isDirectory()) {
      // Layout with a single "src" folder and recursive compilation
      layout = LibraryLayout.RECURSIVE;

      File utilFolder = new File(libFolder, "utility");
      if (utilFolder.exists() && utilFolder.isDirectory()) {
        throw new IOException("Library can't use both 'src' and 'utility' folders.");
      }
    } else {
      // Layout with source code on library's root and "utility" folders
      layout = LibraryLayout.FLAT;
    }

    // Warn if root folder contains development leftovers
    File[] files = libFolder.listFiles();
    if (files == null) {
      throw new IOException("Unable to list files of library in " + libFolder);
    }
    for (File file : files) {
      if (file.isDirectory() && FileUtils.isSCCSOrHiddenFile(file)) {
        if (!FileUtils.isSCCSFolder(file) && FileUtils.isHiddenFile(file)) {
          System.out.println("WARNING: Spurious " + file.getName() + " folder in '" + properties.get("name") + "' library");
        }
      }
    }

    // Extract metadata info
    String architectures = properties.get("architectures");
    if (architectures == null)
      architectures = "*"; // defaults to "any"
    List<String> archs = new ArrayList<String>();
    for (String arch : architectures.split(","))
      archs.add(arch.trim());

    String category = properties.get("category");
    if (category == null)
      category = "Uncategorized";
    if (!CATEGORIES.contains(category)) {
      System.out.println("WARNING: Category '" + category + "' in library " + properties.get("name") + " is not valid. Setting to 'Uncategorized'");
      category = "Uncategorized";
    }

    String license = properties.get("license");
    if (license == null) {
      license = "Unspecified";
    }

    String types = properties.get("types");
    if (types == null) {
      types = "Contributed";
    }
    List<String> typesList = new LinkedList<String>();
    for (String type : types.split(",")) {
      typesList.add(type.trim());
    }

    UserLibrary res = new UserLibrary();
    res.setInstalledFolder(libFolder);
    res.setInstalled(true);
    res.name = properties.get("name").trim();
    res.version = properties.get("version").trim();
    res.author = properties.get("author").trim();
    res.maintainer = properties.get("maintainer").trim();
    res.sentence = properties.get("sentence").trim();
    res.paragraph = properties.get("paragraph").trim();
    res.website = properties.get("url").trim();
    res.category = category.trim();
    res.license = license.trim();
    res.architectures = archs;
    res.layout = layout;
    res.declaredTypes = typesList;
    return res;
  }

  @Override
  public String getName() {
    return name;
  }

  @Override
  public List<String> getArchitectures() {
    return architectures;
  }

  @Override
  public String getAuthor() {
    return author;
  }

  @Override
  public String getParagraph() {
    return paragraph;
  }

  @Override
  public String getSentence() {
    return sentence;
  }

  @Override
  public String getWebsite() {
    return website;
  }

  @Override
  public String getCategory() {
    return category;
  }

  @Override
  public List<String> getTypes() {
    return types;
  }

  public void setTypes(List<String> types) {
    this.types = types;
  }

  @Override
  public String getLicense() {
    return license;
  }

  public static List<String> getCategories() {
    return CATEGORIES;
  }

  @Override
  public void setCategory(String category) {
    this.category = category;
  }

  @Override
  public String getVersion() {
    return version;
  }

  @Override
  public String getMaintainer() {
    return maintainer;
  }

  @Override
  public String getChecksum() {
    return null;
  }

  @Override
  public long getSize() {
    return 0;
  }

  @Override
  public String getUrl() {
    return null;
  }

  @Override
  public String getArchiveFileName() {
    return null;
  }

  @Override
  public List<ContributedLibraryReference> getRequires() {
    return null;
  }

  public List<String> getDeclaredTypes() {
    return declaredTypes;
  }

  protected enum LibraryLayout {
    FLAT, RECURSIVE
  }

  protected LibraryLayout layout;

  public File getSrcFolder() {
    switch (layout) {
      case FLAT:
        return getInstalledFolder();
      case RECURSIVE:
        return new File(getInstalledFolder(), "src");
      default:
        return null; // Keep compiler happy :-(
    }
  }

  public boolean useRecursion() {
    return (layout == LibraryLayout.RECURSIVE);
  }

  @Override
  public String toString() {
    String res = "Library: " + name + "\n";
    res += "         (version=" + version + ")\n";
    res += "         (author=" + author + ")\n";
    res += "         (maintainer=" + maintainer + ")\n";
    res += "         (sentence=" + sentence + ")\n";
    res += "         (paragraph=" + paragraph + ")\n";
    res += "         (url=" + website + ")\n";
    res += "         (architectures=" + architectures + ")\n";
    return res;
  }

}
