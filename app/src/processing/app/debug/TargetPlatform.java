/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 TargetPlatform - Represents a hardware platform
 Part of the Arduino project - http://www.arduino.cc/

 Copyright (c) 2009 David A. Mellis

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

import static processing.app.I18n._;
import static processing.app.I18n.format;

import java.io.File;
import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;

import processing.app.helpers.PreferencesMap;

public class TargetPlatform {

  private String id;
  private File folder;
  private TargetPackage containerPackage;

  /**
   * Contains preferences for every defined board
   */
  private Map<String, TargetBoard> boards = new LinkedHashMap<String, TargetBoard>();
  private TargetBoard defaultBoard;

  /**
   * Contains preferences for every defined programmer
   */
  private Map<String, PreferencesMap> programmers = new LinkedHashMap<String, PreferencesMap>();

  /**
   * Contains preferences for platform
   */
  private PreferencesMap preferences = new PreferencesMap();

  /**
   * Contains labels for top level menus
   */
  private PreferencesMap customMenus = new PreferencesMap();

  public TargetPlatform(String _name, File _folder, TargetPackage parent)
      throws TargetPlatformException {

    id = _name;
    folder = _folder;
    containerPackage = parent;

    // If there is no boards.txt, this is not a valid 1.5 hardware folder
    File boardsFile = new File(folder, "boards.txt");
    if (!boardsFile.exists() || !boardsFile.canRead())
      throw new TargetPlatformException(
          format(_("Could not find boards.txt in {0}. Is it pre-1.5?"),
                 boardsFile.getAbsolutePath()));

    // Load boards
    try {
      Map<String, PreferencesMap> boardsPreferences = new PreferencesMap(
          boardsFile).firstLevelMap();

      // Create custom menus for this platform
      PreferencesMap menus = boardsPreferences.get("menu");
      if (menus != null)
        customMenus = menus.topLevelMap();
      boardsPreferences.remove("menu");

      // Create boards
      Set<String> boardIDs = boardsPreferences.keySet();
      for (String id : boardIDs) {
        PreferencesMap preferences = boardsPreferences.get(id);
        TargetBoard board = new TargetBoard(id, preferences, this);
        boards.put(id, board);
      }
      if (!boardIDs.isEmpty()) {
        PreferencesMap preferences = boardsPreferences.get(boardIDs.iterator().next());
        defaultBoard = new TargetBoard(id, preferences, this);
      }
    } catch (IOException e) {
      throw new TargetPlatformException(format(_("Error loading {0}"),
                                               boardsFile.getAbsolutePath()), e);
    }

    File platformsFile = new File(folder, "platform.txt");
    try {
      if (platformsFile.exists() && platformsFile.canRead()) {
        preferences.load(platformsFile);
      }
    } catch (IOException e) {
      throw new TargetPlatformException(
          format(_("Error loading {0}"), platformsFile.getAbsolutePath()), e);
    }

    File progFile = new File(folder, "programmers.txt");
    try {
      if (progFile.exists() && progFile.canRead()) {
        PreferencesMap prefs = new PreferencesMap();
        prefs.load(progFile);
        programmers = prefs.firstLevelMap();
      }
    } catch (IOException e) {
      throw new TargetPlatformException(format(_("Error loading {0}"), progFile
          .getAbsolutePath()), e);
    }
  }

  public String getId() {
    return id;
  }

  public File getFolder() {
    return folder;
  }

  public Map<String, TargetBoard> getBoards() {
    return boards;
  }

  public PreferencesMap getCustomMenus() {
    return customMenus;
  }

  public Set<String> getCustomMenuIds() {
    return customMenus.keySet();
  }

  public Map<String, PreferencesMap> getProgrammers() {
    return programmers;
  }

  public PreferencesMap getProgrammer(String programmer) {
    return getProgrammers().get(programmer);
  }

  public PreferencesMap getTool(String tool) {
    return getPreferences().subTree("tools").subTree(tool);
  }

  public PreferencesMap getPreferences() {
    return preferences;
  }

  public TargetBoard getBoard(String boardId) {
    if (boards.containsKey(boardId)) {
      return boards.get(boardId);
    }
    return defaultBoard;
  }

  public TargetPackage getContainerPackage() {
    return containerPackage;
  }

  @Override
  public String toString() {
    String res = "TargetPlatform: name=" + id + " boards={\n";
    for (String boardId : boards.keySet())
      res += "  " + boardId + " = " + boards.get(boardId) + "\n";
    return res + "}";
  }
}
