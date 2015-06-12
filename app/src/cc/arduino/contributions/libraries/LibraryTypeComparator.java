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

package cc.arduino.contributions.libraries;

import java.util.Arrays;
import java.util.Comparator;
import java.util.List;

public class LibraryTypeComparator implements Comparator<String> {

  private final List<String> types;

  public LibraryTypeComparator() {
    this("Arduino", "Recommended", "Contributed");
  }

  public LibraryTypeComparator(String... types) {
    this.types = Arrays.asList(types);
  }

  @Override
  public int compare(String o1, String o2) {
    if (types.contains(o1) && types.contains(o2)) {
      return types.indexOf(o1) - types.indexOf(o2);
    } else if (types.contains(o1)) {
      return -1;
    } else if (types.contains(o2)) {
      return 1;
    }
    return o1.compareTo(o2);
  }

}
