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
package cc.arduino.libraries.contributions.ui;

import cc.arduino.libraries.contributions.ContributedLibrary;
import cc.arduino.libraries.contributions.LibrariesIndexer;
import cc.arduino.packages.contributions.ContributedPackage;
import cc.arduino.packages.contributions.ContributedPlatform;
import cc.arduino.ui.FilteredAbstractTableModel;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import static cc.arduino.packages.contributions.VersionComparator.VERSION_COMPARATOR;

@SuppressWarnings("serial")
public class LibrariesIndexTableModel extends FilteredAbstractTableModel {

  public final static int DESCRIPTION_COL = 0;

  public static class ContributedLibraryReleases implements
      Comparable<ContributedLibraryReleases> {
    public ContributedPackage packager;
    public String name;
    public List<ContributedLibrary> releases = new ArrayList<ContributedLibrary>();
    public List<String> versions = new ArrayList<String>();
    public ContributedLibrary selected = null;

    public ContributedLibraryReleases(ContributedLibrary library) {
      name = library.getName();
      add(library);
    }

    public boolean shouldContain(ContributedLibrary lib) {
      if (!lib.getName().equals(name))
        return false;
      return true;
    }

    public void add(ContributedLibrary library) {
      releases.add(library);
      versions.add(library.getVersion());
      selected = getLatest();
    }

    public ContributedLibrary getInstalled() {
      for (ContributedLibrary lib : releases)
        if (lib.isInstalled())
          return lib;
      return null;
    }

    public ContributedLibrary getLatest() {
      ContributedLibrary latest = null;
      for (ContributedLibrary lib : releases) {
        if (latest == null)
          latest = lib;
        // TODO a better version compare

        if (VERSION_COMPARATOR.compare(lib.getVersion(), latest.getVersion()) > 0)
          latest = lib;
      }
      return latest;
    }

    public ContributedLibrary getSelected() {
      return selected;
    }

    public void selectVersion(String version) {
      for (ContributedLibrary lib : releases) {
        if (lib.getVersion().equals(version)) {
          selected = lib;
          return;
        }
      }
    }

    public void select(ContributedLibrary value) {
      for (ContributedLibrary plat : releases) {
        if (plat == value) {
          selected = plat;
          return;
        }
      }
    }

    @Override
    public int compareTo(ContributedLibraryReleases o) {
      return name.compareToIgnoreCase(o.name);
    }
  }

  private List<ContributedLibraryReleases> contributions = new ArrayList<ContributedLibraryReleases>();

  private String[] columnNames = { "Description" };

  private Class<?>[] columnTypes = { ContributedPlatform.class };

  private LibrariesIndexer indexer;

  public void setIndexer(LibrariesIndexer _index) {
    indexer = _index;
  }

  String selectedCategory = null;
  String selectedFilters[] = null;

  public void updateIndexFilter(String category, String filters[]) {
    selectedCategory = category;
    selectedFilters = filters;
    update();
  }

  /**
   * Check if <b>string</b> contains all the substrings in <b>set</b>. The
   * compare is case insensitive.
   * 
   * @param string
   * @param filters
   * @return <b>true<b> if all the strings in <b>set</b> are contained in
   *         <b>string</b>.
   */
  private boolean stringContainsAll(String string, String filters[]) {
    if (string == null) {
      return false;
    }

    if (filters == null) {
      return true;
    }

    for (String filter : filters) {
      if (!string.toLowerCase().contains(filter.toLowerCase())) {
        return false;
      }
    }

    return true;
  }

  private void addContribution(ContributedLibrary lib) {
    for (ContributedLibraryReleases contribution : contributions) {
      if (!contribution.shouldContain(lib))
        continue;
      contribution.add(lib);
      return;
    }

    contributions.add(new ContributedLibraryReleases(lib));
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
    ContributedLibraryReleases contribution = contributions.get(row);
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

  public ContributedLibraryReleases getReleases(int row) {
    return contributions.get(row);
  }

  public ContributedLibrary getSelectedRelease(int row) {
    return contributions.get(row).getSelected();
  }

  public void update() {
    updateContributions();
    fireTableDataChanged();
  }

  private void applyFilterToLibrary(ContributedLibrary lib) {
    if (selectedCategory != null && !selectedCategory.isEmpty()) {
      if (lib.getCategory() == null || !lib.getCategory().equals(selectedCategory)) {
        return;
      }
    }
    if (!stringContainsAll(lib.getName(), selectedFilters) && !stringContainsAll(lib.getParagraph(), selectedFilters) && !stringContainsAll(lib.getSentence(), selectedFilters)) {
      return;
    }
    addContribution(lib);
  }

  public void updateLibrary(ContributedLibrary lib) {
    // Find the row interested in the change
    int row = -1;
    for (ContributedLibraryReleases releases : contributions) {
      if (releases.shouldContain(lib))
        row = contributions.indexOf(releases);
    }

    updateContributions();

    // If the library is found in the list send update event
    // or insert event on the specific row...
    for (ContributedLibraryReleases releases : contributions) {
      if (releases.shouldContain(lib)) {
        if (row == -1) {
          row = contributions.indexOf(releases);
          fireTableRowsInserted(row, row);
        } else {
          fireTableRowsUpdated(row, row);
        }
        return;
      }
    }
    // ...otherwise send a row deleted event
    fireTableRowsDeleted(row, row);
  }

  private void updateContributions() {
    contributions.clear();
    for (ContributedLibrary l : indexer.getIndex().getLibraries())
      applyFilterToLibrary(l);
    for (ContributedLibrary l : indexer.getInstalledLibraries())
      applyFilterToLibrary(l);
    Collections.sort(contributions);
  }

}
