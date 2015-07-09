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

import java.io.File;
import java.util.Arrays;
import java.util.List;

import cc.arduino.contributions.libraries.ContributedLibraryReference;

public class LegacyUserLibrary extends UserLibrary {

  private String name;

  public static LegacyUserLibrary create(File libFolder) {
    // construct an old style library
    LegacyUserLibrary res = new LegacyUserLibrary();
    res.setInstalledFolder(libFolder);
    res.setInstalled(true);
    res.layout = LibraryLayout.FLAT;
    res.name = libFolder.getName();
    return res;
  }

  @Override
  public String getName() {
    return name;
  }

  @Override
  public List<String> getArchitectures() {
    return Arrays.asList("*");
  }

  @Override
  public String getAuthor() {
    return null;
  }

  @Override
  public String getParagraph() {
    return null;
  }

  @Override
  public String getSentence() {
    return null;
  }

  @Override
  public String getWebsite() {
    return null;
  }

  @Override
  public String getCategory() {
    return "Uncategorized";
  }

  @Override
  public String getLicense() {
    return null;
  }

  @Override
  public String getVersion() {
    return null;
  }

  @Override
  public String getMaintainer() {
    return null;
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
  public List<ContributedLibraryReference> getRequires() {
    return null;
  }

  @Override
  public String toString() {
    return "LegacyLibrary:" + name + "\n";
  }

}
