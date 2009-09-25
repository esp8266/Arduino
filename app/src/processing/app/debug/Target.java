/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Target - represents a target platform
  Part of the Arduino project - http://arduino.berlios.de/

  Copyright (c) 2005
  David A. Mellis

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  $Id: Target.java 85 2006-01-12 23:24:12Z mellis $
*/

package processing.app.debug;
import java.io.*;
import java.util.*;

/**
 * Represents a target platform (e.g. Wiring board, Arduino board).
 */
public class Target {
  String path;
  List sources = new ArrayList();
  List objects = new ArrayList();

  /**
   * Create a Target.
   * @param path the directory containing config, source, and object files for
   * the target platform.
   */
  public Target(String base, String target) throws IOException {
    path = base + File.separator + target;
    String[] files = (new File(path)).list();
    
    if (files == null)
      throw new IOException("Target platform: \"" + target + "\" not found.\n" +
                            "Make sure that \"build.target\" in the \n" +
                            "preferences file points to a subdirectory of \n" +
                            base);
    
    for (int i = 0; i < files.length; i++) {
      if (files[i].endsWith(".S") || files[i].endsWith(".c") || files[i].endsWith(".cpp"))
        sources.add(files[i]);
      if (files[i].endsWith(".o"))
        objects.add(files[i]);
    }
  }
  
  public String getPath() { return path; }
  
  /**
   * The source files in the library for the target platform.
   * @return A read-only collection of strings containing the name of each source file.
   */
  public Collection getSourceFilenames() {
    return Collections.unmodifiableList(sources);
  }
  
  /**
   * The object files in the library for the target platform.
   * @return A read-only collection of strings containing the name of each object file.
   */
  public Collection getObjectFilenames() {
    return Collections.unmodifiableList(objects);
  }
}
