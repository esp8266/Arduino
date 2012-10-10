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

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import processing.app.helpers.PreferencesMap;
import processing.core.PApplet;

public class TargetPlatform {
  private String name;
  private File folder;
  private Map<String, PreferencesMap> boards;
  private List<String> boardsOrder;
  private Map<String, PreferencesMap> programmers;
  private PreferencesMap preferences;

  public TargetPlatform(String _name, File _folder) {
    System.out.println("TargetPlatform: constructor start, name: " + _name);
    name = _name;
    folder = _folder;
    boards = new HashMap<String, PreferencesMap>();
    boardsOrder = new ArrayList<String>();
    programmers = new HashMap<String, PreferencesMap>();
    preferences = new PreferencesMap();

    try {
      File boardsFile = new File(_folder, "boards.txt");
      if (boardsFile.exists()) {
        PreferencesMap boardPreferences = new PreferencesMap();
        boardPreferences.load(boardsFile);
        boards = boardPreferences.createFirstLevelMap();
        boardsOrder = readBoardsOrder(boardsFile);
      }
    } catch (Exception e) {
      System.err.println("Error loading boards from boards.txt: " + e);
    }

    try {
      File platformsFile = new File(_folder, "platform.txt");
      if (platformsFile.exists())
        preferences.load(platformsFile);
    } catch (Exception e) {
      System.err.println("Error loading platforms from platform.txt: " + e);
    }

    try {
      File programmersFile = new File(_folder, "programmers.txt");
      if (programmersFile.exists()) {
        PreferencesMap prefs = new PreferencesMap();
        prefs.load(programmersFile);
        programmers = prefs.createFirstLevelMap();
      }
    } catch (Exception e) {
      System.err
          .println("Error loading programmers from programmers.txt: " + e);
    }
  }

  /**
   * Loads the ordered list of boards as they appears on the boards.txt file
   * 
   * @param boardsFile
   * @return
   */
  private List<String> readBoardsOrder(File boardsFile) {
    String[] strings = PApplet.loadStrings(boardsFile);

    List<String> res = new ArrayList<String>();
    String latestBoard = "-";
    for (String s : strings) {
      int dot = s.indexOf('.');
      if (dot == -1)
        continue;
      String board = s.substring(0, dot);
      if (board.equals(latestBoard))
        continue;
      if (!boards.containsKey(board))
        continue;
      latestBoard = board;
      res.add(board);
    }
    return res;
  }

  public String getName() {
    return name;
  }

  public File getFolder() {
    return folder;
  }

  public Map<String, PreferencesMap> getBoards() {
    return boards;
  }
  
  public List<String> getOrderedBoards() {
    return boardsOrder;
  }

  public Map<String, PreferencesMap> getProgrammers() {
    return programmers;
  }

  public PreferencesMap getProgrammer(String programmer) {
    return getProgrammers().get(programmer);
  }
  
  public PreferencesMap getTool(String tool) {
    return getPreferences().createSubTree("tools").createSubTree(tool);
  }
  
  public PreferencesMap getPreferences() {
    return preferences;
  }
}
