/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

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

package cc.arduino.packages;

import processing.app.helpers.PreferencesMap;

import java.util.List;

public interface Discovery {

  /**
   * Set discovery preferences
   *
   * @param options
   */
  public void setPreferences(PreferencesMap options);

  /**
   * Start discovery service
   *
   * @throws Exception
   */
  public void start() throws Exception;

  /**
   * Stop discovery service
   */
  public void stop() throws Exception;

  /**
   * Return the list of discovered ports.
   *
   * @return
   */
  public List<BoardPort> discovery();

}
