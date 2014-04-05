package processing.app.helpers;

import java.awt.Color;
import java.awt.Font;

public abstract class PreferencesHelper {

//  /**
//   * Create a Color with the value of the specified key. The format of the color
//   * should be an hexadecimal number of 6 digit, eventually prefixed with a '#'.
//   * 
//   * @param name
//   * @return A Color object or <b>null</b> if the key is not found or the format
//   *         is wrong
//   */
//  static public Color getColor(PreferencesMap prefs, String name) {
//    Color parsed = parseColor(prefs.get(name));
//    if (parsed != null)
//      return parsed;
//    return Color.GRAY; // set a default
//  }
//
//
//  static public void setColor(PreferencesMap prefs, String attr, Color what) {
//    putColor(prefs, attr, what);
//  }
//
//
//  static public Font getFontWithDefault(PreferencesMap prefs, PreferencesMap defaults, String attr) {
//    Font font = getFont(prefs, attr);
//    if (font == null) {
//      String value = defaults.get(attr);
//      prefs.put(attr, value);
//      font = getFont(prefs, attr);
//    }
//    return font;
//  }
//
//  static public SyntaxStyle getStyle(PreferencesMap prefs, String what) {
//    String str = prefs.get("editor." + what + ".style");
//
//    StringTokenizer st = new StringTokenizer(str, ",");
//
//    String s = st.nextToken();
//    if (s.indexOf("#") == 0) s = s.substring(1);
//    Color color = Color.DARK_GRAY;
//    try {
//      color = new Color(Integer.parseInt(s, 16));
//    } catch (Exception e) { }
//
//    s = st.nextToken();
//    boolean bold = (s.indexOf("bold") != -1);
//    boolean italic = (s.indexOf("italic") != -1);
//    boolean underlined = (s.indexOf("underlined") != -1);
//
//    return new SyntaxStyle(color, italic, bold, underlined);
//  }

  /**
   * Set the value of the specified key based on the Color passed as parameter.
   * 
   * @param attr
   * @param color
   */
  public static void putColor(PreferencesMap prefs, String attr, Color color) {
    prefs.put(attr, "#" + String.format("%06x", color.getRGB() & 0xffffff));
  }

  public static Color parseColor(String v) {
    try {
      if (v.indexOf("#") == 0)
        v = v.substring(1);
      return new Color(Integer.parseInt(v, 16));
    } catch (Exception e) {
      return null;
    }
  }

  public static Font getFont(PreferencesMap prefs, String key) {
    String value = prefs.get(key);
    if (value == null)
      return null;
    String[] split = value.split(",");
    if (split.length != 3)
      return null;

    String name = split[0];
    int style = Font.PLAIN;
    if (split[1].contains("bold"))
      style |= Font.BOLD;
    if (split[1].contains("italic"))
      style |= Font.ITALIC;
    int size;
    try {
      // ParseDouble handle numbers with decimals too
      size = (int) Double.parseDouble(split[2]);
    } catch (NumberFormatException e) {
      // for wrong formatted size pick the default
      size = 12;
    }
    return new Font(name, style, size);
  }
}
