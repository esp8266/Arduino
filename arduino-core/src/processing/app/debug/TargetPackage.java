/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 TargetPackage - Represents a hardware package
 Part of the Arduino project - http://www.arduino.cc/

 Copyright (c) 2011 Cristian Maglie

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
 */
package processing.app.debug;

import static processing.app.I18n._;

import java.io.File;
import java.util.Collection;
import java.util.LinkedHashMap;
import java.util.Map;

import processing.app.I18n;
import processing.app.helpers.filefilters.OnlyDirs;

public class TargetPackage {

  private String id;

  Map<String, TargetPlatform> platforms = new LinkedHashMap<String, TargetPlatform>();

  public TargetPackage(String _id, File _folder) throws TargetPlatformException {
    id = _id;

    File[] folders = _folder.listFiles(new OnlyDirs());
    if (folders == null)
      return;

    for (File subFolder : folders) {
      if (!subFolder.exists() || !subFolder.canRead())
        continue;
      String arch = subFolder.getName();
      try {
        TargetPlatform platform = new TargetPlatform(arch, subFolder, this);
        platforms.put(arch, platform);
      } catch (TargetPlatformException e) {
        System.out.println(e.getMessage());
      }
    }

    if (platforms.size() == 0) {
      throw new TargetPlatformException(I18n
          .format(_("No valid hardware definitions found in folder {0}."),
                  _folder.getName()));
    }
  }

  public Map<String, TargetPlatform> getPlatforms() {
    return platforms;
  }

  public Collection<TargetPlatform> platforms() {
    return platforms.values();
  }

  public TargetPlatform get(String platform) {
    return platforms.get(platform);
  }

  public String getId() {
    return id;
  }
}
