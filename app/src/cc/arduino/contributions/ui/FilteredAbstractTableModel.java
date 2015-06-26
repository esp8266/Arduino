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

package cc.arduino.contributions.ui;

import cc.arduino.contributions.DownloadableContribution;
import cc.arduino.contributions.VersionComparator;
import com.google.common.base.Predicate;

import javax.swing.table.AbstractTableModel;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;

public abstract class FilteredAbstractTableModel<T> extends AbstractTableModel {

  abstract public void updateIndexFilter(String[] filters, Predicate<T>... additionalFilters);

  protected static <T extends DownloadableContribution> T getLatestOf(List<T> contribs) {
    contribs = new LinkedList<T>(contribs);
    final VersionComparator versionComparator = new VersionComparator();
    Collections.sort(contribs, new Comparator<T>() {
      @Override
      public int compare(T contrib1, T contrib2) {
        return versionComparator.compare(contrib1.getParsedVersion(), contrib2.getParsedVersion());
      }
    });

    if (contribs.isEmpty()) {
      return null;
    }

    return contribs.get(contribs.size() - 1);
  }

}
