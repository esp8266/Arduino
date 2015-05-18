/*
 TargetPackage - Represents a hardware package
 Part of the Arduino project - http://www.arduino.cc/

 Copyright (c) 2014 Cristian Maglie

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

import java.util.Collection;
import java.util.Map;

public interface TargetPackage {

  String getId();

  Map<String, TargetPlatform> getPlatforms();

  Collection<TargetPlatform> platforms();

  TargetPlatform get(String platform);

  boolean hasPlatform(TargetPlatform platform);
}
