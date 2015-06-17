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

package cc.arduino.contributions.packages;

import processing.app.debug.TargetPackage;
import processing.app.debug.TargetPlatform;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

public class ContributedTargetPackage implements TargetPackage {

  private final String id;
  private final Map<String, TargetPlatform> platforms;

  public ContributedTargetPackage(String _id) {
    id = _id;
    platforms = new HashMap<String, TargetPlatform>();
  }

  void addPlatform(TargetPlatform p) {
    platforms.put(p.getId(), p);
  }

  boolean hasPlatforms() {
    return platforms.size() > 0;
  }

  @Override
  public String getId() {
    return id;
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
  public String toString() {
    return "TargetPackage: " + getId();
  }
}
