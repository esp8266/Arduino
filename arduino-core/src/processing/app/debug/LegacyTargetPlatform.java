/*
 TargetPlatform - Represents a hardware platform
 Part of the Arduino project - http://www.arduino.cc/

 Copyright (c) 2009-2014 Arduino

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

import processing.app.BaseNoGui;
import processing.app.I18n;
import processing.app.helpers.PreferencesMap;

import java.io.File;
import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;

import static processing.app.I18n._;
import static processing.app.I18n.format;

public class LegacyTargetPlatform implements TargetPlatform {

  private String id;
  private File folder;

  private TargetPackage containerPackage;
  protected PreferencesMap preferences = new PreferencesMap();

  private Map<String, TargetBoard> boards = new LinkedHashMap<String, TargetBoard>();
  private TargetBoard defaultBoard;

  /**
   * Contains preferences for every defined programmer
   */
  private Map<String, PreferencesMap> programmers = new LinkedHashMap<String, PreferencesMap>();

  /**
   * Contains labels for top level menus
   */
  private PreferencesMap customMenus = new PreferencesMap();

  public LegacyTargetPlatform(String _name, File _folder, TargetPackage parent)
      throws TargetPlatformException {

    id = _name;
    folder = _folder;
    containerPackage = parent;

    // If there is no boards.txt, this is not a valid 1.5 hardware folder
    File boardsFile = new File(folder, "boards.txt");
    if (!boardsFile.exists() || !boardsFile.canRead())
      throw new TargetPlatformException(
          format(_("Could not find boards.txt in {0}. Is it pre-1.5?"),
                 folder.getAbsolutePath()));

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
      Set<String> boardIds = boardsPreferences.keySet();
      for (String boardId : boardIds) {
        PreferencesMap preferences = boardsPreferences.get(boardId);
        TargetBoard board = new LegacyTargetBoard(boardId, preferences, this);
        boards.put(boardId, board);

        // Pick the first board as default
        if (defaultBoard == null)
          defaultBoard = board;
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

    // Allow overriding values in platform.txt. This allows changing
    // platform.txt (e.g. to use a system-wide toolchain), without
    // having to modify platform.txt (which, when running from git,
    // prevents files being marked as changed).
    File localPlatformsFile = new File(folder, "platform.local.txt");
    try {
      if (localPlatformsFile.exists() && localPlatformsFile.canRead()) {
        preferences.load(localPlatformsFile);
      }
    } catch (IOException e) {
      throw new TargetPlatformException(
          format(_("Error loading {0}"), localPlatformsFile.getAbsolutePath()), e);
    }

    if (!preferences.containsKey("rewriting") || !"disabled".equals(preferences.get("rewriting"))) {
      try {
        rewriteKeysOfOldPlatformsTxtAndWarnAboutIt();
      } catch (IOException e) {
        throw new TargetPlatformException(e);
      }
    }

    File progFile = new File(folder, "programmers.txt");
    try {
      if (progFile.exists() && progFile.canRead()) {
        PreferencesMap prefs = new PreferencesMap();
        prefs.load(progFile);
        programmers = prefs.firstLevelMap();
      }
    } catch (IOException e) {
      throw new TargetPlatformException(format(_("Error loading {0}"),
                                               progFile.getAbsolutePath()), e);
    }
  }

  private void rewriteKeysOfOldPlatformsTxtAndWarnAboutIt() throws IOException {
    File platformRewrite = new File(BaseNoGui.getHardwareFolder(), "platform.keys.rewrite.txt");
    PreferencesMap platformRewriteProps = new PreferencesMap(platformRewrite);

    PreferencesMap oldProps = platformRewriteProps.subTree("old");
    PreferencesMap newProps = platformRewriteProps.subTree("new");

    String platformName = preferences.get("name");
    if (platformName == null) {
      platformName = folder.getAbsolutePath();
    }

    for (Map.Entry<String, String> entry : oldProps.entrySet()) {
      String preferencesKey = entry.getKey().substring(entry.getKey().indexOf(".") + 1);
      if (preferences.containsKey(preferencesKey) && entry.getValue().equals(preferences.get(preferencesKey))) {
        System.err.println(I18n.format(_("Warning: platform.txt from core '{0}' contains deprecated {1}, automatically converted to {2}. Consider upgrading this core."), platformName, preferencesKey + "=" + entry.getValue(), preferencesKey + "=" + newProps.get(entry.getKey())));
        preferences.put(preferencesKey, newProps.get(entry.getKey()));
      }
    }

    PreferencesMap addedProps = platformRewriteProps.subTree("added");
    for (Map.Entry<String, String> entry : addedProps.entrySet()) {
      String keyToAdd = entry.getKey();
      String[] keyToAddParts = keyToAdd.split("\\.");
      String keyToAddFirstLevel = keyToAddParts[0];
      String keyToAddSecondLevel = keyToAddParts[0] + "." + keyToAddParts[1];
      if (!preferences.subTree(keyToAddFirstLevel).isEmpty() && !preferences.subTree(keyToAddSecondLevel).isEmpty() && !preferences.containsKey(keyToAdd)) {
        System.err.println(I18n.format(_("Warning: platform.txt from core '{0}' misses property {1}, automatically set to {2}. Consider upgrading this core."), platformName, keyToAdd, entry.getValue()));
        preferences.put(keyToAdd, entry.getValue());
      }
    }
  }

  @Override
  public String getId() {
    return id;
  }

  @Override
  public File getFolder() {
    return folder;
  }

  @Override
  public Map<String, TargetBoard> getBoards() {
    return boards;
  }

  @Override
  public PreferencesMap getCustomMenus() {
    return customMenus;
  }

  @Override
  public Set<String> getCustomMenuIds() {
    return customMenus.keySet();
  }

  @Override
  public Map<String, PreferencesMap> getProgrammers() {
    return programmers;
  }

  @Override
  public PreferencesMap getProgrammer(String programmer) {
    return getProgrammers().get(programmer);
  }

  @Override
  public PreferencesMap getTool(String tool) {
    return getPreferences().subTree("tools").subTree(tool);
  }

  @Override
  public PreferencesMap getPreferences() {
    return preferences;
  }

  @Override
  public TargetBoard getBoard(String boardId) {
    if (boards.containsKey(boardId)) {
      return boards.get(boardId);
    }
    return defaultBoard;
  }

  @Override
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
