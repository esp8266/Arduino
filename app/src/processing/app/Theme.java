/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-06 Ben Fry and Casey Reas
  Copyright (c) 2001-04 Massachusetts Institute of Technology

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

package processing.app;

import static processing.app.I18n._;

import java.awt.Color;
import java.awt.Font;
import java.awt.SystemColor;

import processing.app.helpers.PreferencesHelper;
import processing.app.helpers.PreferencesMap;
import processing.app.syntax.SyntaxStyle;

/**
 * Storage class for theme settings. This was separated from the Preferences
 * class for 1.0 so that the coloring wouldn't conflict with previous releases
 * and to make way for future ability to customize.
 */
public class Theme {

  /** Copy of the defaults in case the user mangles a preference. */
  static PreferencesMap defaults;
  /** Table of attributes/values for the theme. */
  static PreferencesMap table = new PreferencesMap();

  static protected void init() {
    try {
      table.load(Base.getLibStream("theme/theme.txt"));
    } catch (Exception te) {
      Base.showError(null, _("Could not read color theme settings.\n" +
                             "You'll need to reinstall Arduino."), te);
    }

    // other things that have to be set explicitly for the defaults
    setColor("run.window.bgcolor", SystemColor.control);

    // clone the hash table
    defaults = new PreferencesMap(table);
  }

  static public String get(String attribute) {
    return table.get(attribute);
  }

  static public String getDefault(String attribute) {
    return defaults.get(attribute);
  }

  static public void set(String attribute, String value) {
    table.put(attribute, value);
  }

  static public boolean getBoolean(String attribute) {
    return table.getBoolean(attribute);
  }

  static public void setBoolean(String attribute, boolean value) {
    table.putBoolean(attribute, value);
  }

  static public int getInteger(String attribute) {
    return Integer.parseInt(get(attribute));
  }

  static public void setInteger(String key, int value) {
    set(key, String.valueOf(value));
  }

  static public Color getColor(String name) {
    return PreferencesHelper.parseColor(get(name));
  }

  static public void setColor(String attr, Color color) {
    PreferencesHelper.putColor(table, attr, color);
  }

  static public Font getFont(String attr) {
    Font font = PreferencesHelper.getFont(table, attr);
    if (font == null) {
      String value = getDefault(attr);
      set(attr, value);
      font = PreferencesHelper.getFont(table, attr);
    }
    return font;
  }

  static public SyntaxStyle getStyle(String what) {
    String split[] = get("editor." + what + ".style").split(",");

    Color color = PreferencesHelper.parseColor(split[0]);

    String style = split[1];
    boolean bold = style.contains("bold");
    boolean italic = style.contains("italic");
    boolean underlined = style.contains("underlined");

    return new SyntaxStyle(color, italic, bold, underlined);
  }
}
