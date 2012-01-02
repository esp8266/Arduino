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

 $Id$
 */
package processing.app.debug;

import java.io.File;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import processing.app.helpers.filefilters.OnlyDirs;

public class TargetPackage {

  String name;
  File folder;

  Map<String, TargetPlatform> platforms = new HashMap<String, TargetPlatform>();

  public TargetPackage(String _name, File _folder) {
    name = _name;
    folder = _folder;

    String[] platformsList = folder.list(new OnlyDirs());
    for (String platformName : platformsList) {
      File platformFolder = new File(folder, platformName);
      TargetPlatform platform = new TargetPlatform(platformName, platformFolder);
      platforms.put(platformName, platform);
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

  public String getName() {
    return name;
  }
}
