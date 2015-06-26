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

import cc.arduino.contributions.DownloadableContributionBuiltInAtTheBottomComparator;
import cc.arduino.contributions.filters.InstalledPredicate;
import cc.arduino.contributions.packages.ContributedPackage;
import cc.arduino.contributions.packages.ContributedPlatform;
import cc.arduino.contributions.packages.ContributionsIndexer;
import cc.arduino.contributions.ui.FilteredAbstractTableModel;
import com.google.common.base.Predicate;
import com.google.common.base.Predicates;
import com.google.common.collect.Collections2;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

@SuppressWarnings("serial")
public class ContributionIndexTableModel extends FilteredAbstractTableModel<ContributedPlatform> {

  public final static int DESCRIPTION_COL = 0;

  public static class ContributedPlatformReleases {
    public final ContributedPackage packager;
    public final String arch;
    public final List<ContributedPlatform> releases;
    public final List<String> versions;
    public ContributedPlatform selected = null;

    public ContributedPlatformReleases(ContributedPlatform platform) {
      this.packager = platform.getParentPackage();
      this.arch = platform.getArchitecture();
      this.releases = new LinkedList<ContributedPlatform>();
      this.versions = new LinkedList<String>();
      add(platform);
    }

    public boolean shouldContain(ContributedPlatform platform) {
      if (platform.getParentPackage() != packager)
        return false;
      return platform.getArchitecture().equals(arch);
    }

    public void add(ContributedPlatform platform) {
      releases.add(platform);
      String version = platform.getParsedVersion();
      if (version != null) {
        versions.add(version);
      }
      selected = getLatest();
    }

    public ContributedPlatform getInstalled() {
      List<ContributedPlatform> installedReleases = new LinkedList<ContributedPlatform>(Collections2.filter(releases, new InstalledPredicate()));
      Collections.sort(installedReleases, new DownloadableContributionBuiltInAtTheBottomComparator());

      if (installedReleases.isEmpty()) {
        return null;
      }

      return installedReleases.get(0);
    }

    public ContributedPlatform getLatest() {
      return getLatestOf(releases);
    }

    public ContributedPlatform getSelected() {
      return selected;
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

  private ContributionsIndexer indexer;

  public void setIndexer(ContributionsIndexer indexer) {
    this.indexer = indexer;
  }

  public void updateIndexFilter(String filters[], Predicate<ContributedPlatform>... additionalFilters) {
    contributions.clear();
    Predicate<ContributedPlatform> filter = Predicates.and(additionalFilters);
    for (ContributedPackage pack : indexer.getPackages()) {
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
    if (row >= contributions.size()) {
      return null;
    }
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
