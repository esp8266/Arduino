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
import static processing.app.helpers.filefilters.OnlyDirs.ONLY_DIRS;

import java.io.File;
import java.util.Collection;
import java.util.LinkedHashMap;
import java.util.Map;

import processing.app.I18n;

public class LegacyTargetPackage implements TargetPackage {

  private String id;
  private Map<String, TargetPlatform> platforms;

  public LegacyTargetPackage(String _id, File _folder) throws TargetPlatformException {
    id = _id;
    platforms = new LinkedHashMap<String, TargetPlatform>();

    File[] folders = _folder.listFiles(ONLY_DIRS);
    if (folders == null)
      return;

    for (File subFolder : folders) {
      if (!subFolder.exists() || !subFolder.canRead())
        continue;
      String arch = subFolder.getName();
      try {
        TargetPlatform platform = new LegacyTargetPlatform(arch, subFolder, this);
        platforms.put(arch, platform);
      } catch (TargetPlatformException e) {
        System.err.println(e.getMessage());
      }
    }

    if (platforms.size() == 0) {
      throw new TargetPlatformException(I18n
          .format(_("No valid hardware definitions found in folder {0}."),
                  _folder.getName()));
    }
  }

  @Override
  public Map<String, TargetPlatform> getPlatforms() {
    return platforms;
  }

  @Override
  public Collection<TargetPlatform> platforms() {
    return platforms.values();
  }

  @Override
  public TargetPlatform get(String platform) {
    return platforms.get(platform);
  }

  @Override
  public boolean hasPlatform(TargetPlatform platform) {
    return platforms.containsKey(platform.getId());
  }

  @Override
  public String getId() {
    return id;
  }
}
