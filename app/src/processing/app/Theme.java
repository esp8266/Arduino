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

import processing.app.helpers.OSUtils;
import processing.app.helpers.PreferencesHelper;
import processing.app.helpers.PreferencesMap;

import javax.swing.text.StyleContext;
import java.awt.*;
import java.awt.font.TextAttribute;
import java.io.File;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Map;

import static processing.app.I18n._;

/**
 * Storage class for theme settings. This was separated from the Preferences
 * class for 1.0 so that the coloring wouldn't conflict with previous releases
 * and to make way for future ability to customize.
 */
public class Theme {

  /**
   * Copy of the defaults in case the user mangles a preference.
   */
  static PreferencesMap defaults;
  /**
   * Table of attributes/values for the theme.
   */
  static PreferencesMap table = new PreferencesMap();

  static protected void init() {
    try {
      table.load(new File(BaseNoGui.getContentFile("lib"), "theme/theme.txt"));
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

  /**
   * Returns the default font for text areas.
   *
   * @return The default font.
   */
  public static final Font getDefaultFont() {

    // Use StyleContext to get a composite font for better Asian language
    // support; see Sun bug S282887.
    StyleContext sc = StyleContext.getDefaultStyleContext();
    Font font = null;

    if (OSUtils.isMacOS()) {
      // Snow Leopard (1.6) uses Menlo as default monospaced font,
      // pre-Snow Leopard used Monaco.
      font = sc.getFont("Menlo", Font.PLAIN, 12);
      if (!"Menlo".equals(font.getFamily())) {
        font = sc.getFont("Monaco", Font.PLAIN, 12);
        if (!"Monaco".equals(font.getFamily())) { // Shouldn't happen
          font = sc.getFont("Monospaced", Font.PLAIN, 13);
        }
      }
    } else {
      // Consolas added in Vista, used by VS2010+.
      font = sc.getFont("Consolas", Font.PLAIN, 13);
      if (!"Consolas".equals(font.getFamily())) {
        font = sc.getFont("Monospaced", Font.PLAIN, 13);
      }
    }

    //System.out.println(font.getFamily() + ", " + font.getName());
    return font;
  }

  public static Map<String, Object> getStyledFont(String what, Font font) {
    String split[] = get("editor." + what + ".style").split(",");

    Color color = PreferencesHelper.parseColor(split[0]);

    String style = split[1];
    boolean bold = style.contains("bold");
    boolean italic = style.contains("italic");
    boolean underlined = style.contains("underlined");

    Font styledFont = new Font(font.getFamily(), (bold ? Font.BOLD : 0) | (italic ? Font.ITALIC : 0), font.getSize());
    if (underlined) {
      Map<TextAttribute, Object> attr = new Hashtable<TextAttribute, Object>();
      attr.put(TextAttribute.UNDERLINE, TextAttribute.UNDERLINE_ON);
      styledFont = styledFont.deriveFont(attr);
    }

    Map<String, Object> result = new HashMap<String, Object>();
    result.put("color", color);
    result.put("font", styledFont);

    return result;
  }

}
