/*
 TargetBoard - Represents a hardware board
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

import java.util.Set;

import processing.app.helpers.PreferencesMap;

public interface TargetBoard {

  /**
   * Get the name of the board.
   * 
   * @return
   */
  public String getName();

  /**
   * Get the identifier of the board
   * 
   * @return
   */
  public String getId();

  /**
   * Get the full preferences map of the board
   * 
   * @return
   */
  public PreferencesMap getPreferences();

  /**
   * Check if the board has a sub menu.
   * 
   * @param menuId
   *          The menu ID to check
   * @return
   */
  public boolean hasMenu(String menuId);

  /**
   * Returns the options available on a specific menu
   * 
   * @param menuId
   *          The menu ID
   * @return
   */
  public PreferencesMap getMenuLabels(String menuId);

  /**
   * Returns the label of the specified option in the specified menu
   * 
   * @param menuId
   *          The menu ID
   * @param selectionId
   *          The option ID
   * @return
   */
  public String getMenuLabel(String menuId, String selectionId);

  public Set<String> getMenuIds();

  /**
   * Returns the configuration parameters to override (as a PreferenceMap) when
   * the specified option in the specified menu is selected
   * 
   * @param menuId
   *          The menu ID
   * @param selectionId
   *          The option ID
   * @return
   */
  public PreferencesMap getMenuPreferences(String menuId, String selectionId);

  public TargetPlatform getContainerPlatform();

}
