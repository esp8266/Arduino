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

import cc.arduino.contributions.DownloadableContribution;

import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedList;
import java.util.List;

public abstract class ContributedPlatform extends DownloadableContribution {

  public abstract String getName();

  public abstract String getCategory();

  public abstract void setCategory(String category);

  public abstract String getArchitecture();

  public abstract String getChecksum();

  public abstract List<ContributedToolReference> getToolsDependencies();

  public abstract List<ContributedBoard> getBoards();

  public abstract ContributedHelp getHelp();

  private List<ContributedTool> resolvedTools = null;

  private ContributedPackage parentPackage;

  public List<ContributedTool> getResolvedTools() {
    if (resolvedTools == null) {
      return null;
    }
    return new LinkedList<ContributedTool>(resolvedTools);
  }

  public void resolveToolsDependencies(Collection<ContributedPackage> packages) {
    resolvedTools = new ArrayList<ContributedTool>();

    // If there are no dependencies return empty list
    if (getToolsDependencies() == null) {
      return;
    }

    // For each tool dependency
    for (ContributedToolReference dep : getToolsDependencies()) {
      // Search the referenced tool
      ContributedTool tool = dep.resolve(packages);
      if (tool == null) {
        System.err.println("Index error: could not find referenced tool " + dep);
      }
      resolvedTools.add(tool);
    }
  }

  public ContributedPackage getParentPackage() {
    return parentPackage;
  }

  public void setParentPackage(ContributedPackage parentPackage) {
    this.parentPackage = parentPackage;
  }

  @Override
  public String toString() {
    return getParsedVersion();
  }
}
