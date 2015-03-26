/*
 * This file is part of Arduino.
 *
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
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
package cc.arduino.contributions.packages.ui;

import cc.arduino.contributions.packages.ContributedPackage;
import cc.arduino.contributions.packages.ContributedPlatform;
import cc.arduino.contributions.packages.ContributionsIndex;
import cc.arduino.contributions.ui.FilteredAbstractTableModel;
import com.google.common.base.Predicate;
import com.google.common.base.Predicates;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

import static processing.app.I18n._;

@SuppressWarnings("serial")
public class ContributionIndexTableModel extends FilteredAbstractTableModel<ContributedPlatform> {

  public final static int DESCRIPTION_COL = 0;

  public static class ContributedPlatformReleases {
    public ContributedPackage packager;
    public String arch;
    public List<ContributedPlatform> releases = new ArrayList<ContributedPlatform>();
    public List<String> versions = new ArrayList<String>();
    public ContributedPlatform selected = null;

    public ContributedPlatformReleases(ContributedPlatform platform) {
      packager = platform.getParentPackage();
      arch = platform.getArchitecture();
      add(platform);
    }

    public boolean shouldContain(ContributedPlatform platform) {
      if (platform.getParentPackage() != packager)
        return false;
      return platform.getArchitecture().equals(arch);
    }

    public void add(ContributedPlatform platform) {
      releases.add(platform);
      versions.add(platform.getVersion());
      selected = getLatest();
    }

    public ContributedPlatform getInstalled() {
      List<ContributedPlatform> installedPlatforms = new LinkedList<ContributedPlatform>();
      for (ContributedPlatform platform : releases) {
        if (platform.isInstalled()) {
          installedPlatforms.add(platform);
        }
      }

      if (installedPlatforms.size() > 1) {
        throw new IllegalStateException(_("More than one platform is currently installed! Only one can be installed at any given time"));
      }

      if (!installedPlatforms.isEmpty()) {
        return installedPlatforms.get(0);
      }

      return null;
    }

    public ContributedPlatform getLatest() {
      ContributedPlatform latest = null;
      for (ContributedPlatform plat : releases) {
        if (latest == null)
          latest = plat;
        // TODO a better version compare
        if (plat.getVersion().compareTo(latest.getVersion()) > 0)
          latest = plat;
      }
      return latest;
    }

    public ContributedPlatform getSelected() {
      return selected;
    }

    public void selectVersion(String version) {
      for (ContributedPlatform plat : releases) {
        if (plat.getVersion().equals(version)) {
          selected = plat;
          return;
        }
      }
    }

    public void select(ContributedPlatform value) {
      for (ContributedPlatform plat : releases) {
        if (plat == value) {
          selected = plat;
          return;
        }
      }
    }
  }

  private List<ContributedPlatformReleases> contributions = new ArrayList<ContributedPlatformReleases>();

  private String[] columnNames = {"Description"};

  private Class<?>[] columnTypes = {ContributedPlatform.class};

  private ContributionsIndex index;

  public void setIndex(ContributionsIndex _index) {
    index = _index;
  }

  public void updateIndexFilter(String filters[], Predicate<ContributedPlatform>... additionalFilters) {
    contributions.clear();
    Predicate<ContributedPlatform> filter = Predicates.and(additionalFilters);
    for (ContributedPackage pack : index.getPackages()) {
      for (ContributedPlatform platform : pack.getPlatforms()) {
        if (!filter.apply(platform)) {
          continue;
        }
        if (!stringContainsAll(platform.getName(), filters))
          continue;
        addContribution(platform);
      }
    }
    fireTableDataChanged();
  }

  /**
   * Check if <b>string</b> contains all the substrings in <b>set</b>. The
   * compare is case insensitive.
   *
   * @param string
   * @param set
   * @return <b>true<b> if all the strings in <b>set</b> are contained in
   * <b>string</b>.
   */
  private boolean stringContainsAll(String string, String set[]) {
    if (set == null)
      return true;
    for (String s : set) {
      if (!string.toLowerCase().contains(s.toLowerCase()))
        return false;
    }
    return true;
  }

  private void addContribution(ContributedPlatform platform) {
    for (ContributedPlatformReleases contribution : contributions) {
      if (!contribution.shouldContain(platform))
        continue;
      contribution.add(platform);
      return;
    }

    contributions.add(new ContributedPlatformReleases(platform));
  }

  @Override
  public int getColumnCount() {
    return columnNames.length;
  }

  @Override
  public int getRowCount() {
    return contributions.size();
  }

  @Override
  public String getColumnName(int column) {
    return columnNames[column];
  }

  @Override
  public Class<?> getColumnClass(int colum) {
    return columnTypes[colum];
  }

  @Override
  public void setValueAt(Object value, int row, int col) {
    if (col == DESCRIPTION_COL) {
      fireTableCellUpdated(row, col);
    }
  }

  @Override
  public Object getValueAt(int row, int col) {
    ContributedPlatformReleases contribution = contributions.get(row);
    if (col == DESCRIPTION_COL) {
      return contribution;// .getSelected();
    }
    return null;
  }

  @Override
  public boolean isCellEditable(int row, int col) {
    return col == DESCRIPTION_COL;
  }

  public List<String> getReleasesVersions(int row) {
    return contributions.get(row).versions;
  }

  public ContributedPlatformReleases getReleases(int row) {
    return contributions.get(row);
  }

  public ContributedPlatform getSelectedRelease(int row) {
    return contributions.get(row).getSelected();
  }

  public void update() {
    fireTableDataChanged();
  }

}
