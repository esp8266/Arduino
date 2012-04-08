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
import java.util.Locale.*;
import java.text.MessageFormat;

public class I18n {
  // start using current locale but still allow using the dropdown list later
  private static ResourceBundle i18n = ResourceBundle.getBundle("processing.app.Resources");
  public static Locale locale;

  static protected void init (String language) {
    // there might be a null pointer exception ... most likely will never happen but the jvm gets mad
    try {
      if (language == null || language.trim().length() == 0) locale = Locale.getDefault();
      else locale = new Locale(language);
      i18n = ResourceBundle.getBundle("processing.app.Resources", locale);
    } catch (java.lang.NullPointerException e) {
    }
  }

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
