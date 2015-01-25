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

import java.text.MessageFormat;
import java.util.Locale;
import java.util.MissingResourceException;
import java.util.ResourceBundle;

public class I18n {
  // start using current locale but still allow using the dropdown list later
  private static ResourceBundle i18n;

  // prompt text stuff

  static String PROMPT_YES;
  static String PROMPT_NO;
  static String PROMPT_CANCEL;
  static String PROMPT_OK;
  static String PROMPT_BROWSE;

  static protected void init(String language) throws MissingResourceException {
    String[] languageParts = language.split("_");
    Locale locale = Locale.getDefault();
    // both language and country
    if (languageParts.length == 2) {
      locale = new Locale(languageParts[0], languageParts[1]);
      // just language
    } else if (languageParts.length == 1 && !"".equals(languageParts[0])) {
      locale = new Locale(languageParts[0]);
    }
    // there might be a null pointer exception ... most likely will never happen but the jvm gets mad
    Locale.setDefault(locale);
    i18n = ResourceBundle.getBundle("processing.app.i18n.Resources", Locale.getDefault());
    PROMPT_YES = _("Yes");
    PROMPT_NO = _("No");
    PROMPT_CANCEL = _("Cancel");
    PROMPT_OK = _("OK");
    PROMPT_BROWSE = _("Browse");
  }

  public static String _(String s) {
    String res;
    try {
      if (i18n == null)
        res = s;
      else
        res = i18n.getString(s);
    } catch (MissingResourceException e) {
      res = s;
    }

    // The single % is the arguments selector in .PO files.
    // We must put double %% inside the translations to avoid
    // getting .PO processing in the way.
    res = res.replace("%%", "%");

    return res;
  }

  public static String format(String fmt, Object... args) {
    // Single quote is used to escape curly bracket arguments.

    // - Prevents strings fixed at translation time to be fixed again
    fmt = fmt.replace("''", "'");
    // - Replace ' with the escaped version ''
    fmt = fmt.replace("'", "''");

    return MessageFormat.format(fmt, args);
  }

  /**
   * Does nothing.
   * <p/>
   * This method is an hack to extract words with gettext tool.
   */
  protected static void unusedStrings() {
    // These phrases are defined in the "platform.txt".
    _("Arduino AVR Boards");
    _("Arduino ARM (32-bits) Boards");

    // This word is defined in the "boards.txt".
    _("Processor");
  }
}
