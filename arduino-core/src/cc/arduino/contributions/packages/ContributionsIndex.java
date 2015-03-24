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
package cc.arduino.contributions.packages;

import java.util.ArrayList;
import java.util.List;

public abstract class ContributionsIndex {

  public abstract List<ContributedPackage> getPackages();

  public ContributedPackage findPackage(String packageName) {
    for (ContributedPackage pack : getPackages()) {
      if (pack.getName().equals(packageName))
        return pack;
    }
    return null;
  }

  public ContributedTool findTool(String packageName, String name,
                                  String version) {
    ContributedPackage pack = findPackage(packageName);
    if (pack == null)
      return null;
    return pack.findTool(name, version);
  }

  private final List<String> categories = new ArrayList<String>();

  public List<String> getCategories() {
    return categories;
  }

  public void fillCategories() {
    categories.clear();
    for (ContributedPackage pack : getPackages()) {
      for (ContributedPlatform platform : pack.getPlatforms()) {
        if (!categories.contains(platform.getCategory()))
          categories.add(platform.getCategory());
      }
    }
  }

  public ContributedPackage getPackage(String packager) {
    for (ContributedPackage pack : getPackages())
      if (pack.getName().equals(packager))
        return pack;
    return null;
  }

  @Override
  public String toString() {
    String res = "";
    res += "Categories: ";
    for (String c : getCategories())
      res += "'" + c + "' ";
    res += "\n";
    for (ContributedPackage pack : getPackages())
      res += pack + "\n";
    return res;
  }
}
