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
package cc.arduino.packages.contributions;

import static processing.app.I18n._;
import static processing.app.I18n.format;

import java.io.File;
import java.util.Set;

import processing.app.debug.LegacyTargetPlatform;
import processing.app.debug.TargetPackage;
import processing.app.debug.TargetPlatformException;
import processing.app.helpers.PreferencesMap;

public class ContributedTargetPlatform extends LegacyTargetPlatform {

  public ContributedTargetPlatform(String _name, File _folder,
                                   TargetPackage parent,
                                   ContributionsIndex index)
      throws TargetPlatformException {
    super(_name, _folder, parent);

    // Populate tools
    PreferencesMap toolsPrefs = preferences.subTree("tools");
    Set<String> names = toolsPrefs.firstLevelMap().keySet();
    for (String name : names) {
      String version = toolsPrefs.get(name + ".version");
      if (version == null) {
        throw new TargetPlatformException(
            format(_("Tool {0} must define a version property ({1})"), //
                   name, "tools." + name + ".version"));
      }

      String packageName = getContainerPackage().getId();
      ContributedTool tool = index.findTool(packageName, name, version);
      if (tool == null) {
        throw new TargetPlatformException(
            format(_("Tool {0} not found in package {1}"),
                   name + ":" + version, packageName));
      }

      DownloadableContribution download = tool.getDownloadableContribution();
      if (!download.isInstalled()) {
        throw new TargetPlatformException(
            format(_("Tool {0} is required but it's not installed."), //
                   name + ":" + version));
      }
      preferences.put("tools." + name + ".path", //
                      download.getInstalledFolder().getAbsolutePath());
    }

  }
}
