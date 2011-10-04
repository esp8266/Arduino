/*
 * by Shigeru KANEMOTO at SWITCHSCIENCE.
 *
 * Extract strings to be translated by:
 *   % xgettext -L Java --from-code=utf-8 -k_ -d Resources_ja *.java
 * Extract and merge by:
 *   % xgettext -j -L Java --from-code=utf-8 -k_ -d Resources_ja *.java
 *
 * Edit "Resources_ja.po".
 * Convert to the properties file format by:
 *   % msgcat -p Resources_ja.po > Resources_ja.properties
 */

package processing.app;
import java.util.*;
import java.text.MessageFormat;

public class I18n {
  private static ResourceBundle i18n = ResourceBundle.getBundle("processing.app.Resources");

  public static String _(String s) {
    try {
      return i18n.getString(s);
    }
    catch (MissingResourceException e) {
      return s;
    }
  }

  public static String format(String fmt, Object ... args) {
    return MessageFormat.format(fmt, args);
  }
}
