package processing.app.helpers;

import java.util.List;

public class StringUtils {

  public static boolean stringContainsOneOf(String input, List<String> listOfStrings) {
    for (String string : listOfStrings) {
      if (input.contains(string)) {
        return true;
      }
    }
    return false;
  }

  /**
   * Tries to match <b>input</b> with <b>pattern</b>. The pattern can use the
   * "*" and "?" globs to match any-char-sequence and any-char respectively.
   *
   * @param input   The string to be checked
   * @param pattern The pattern to match
   * @return <b>true</b> if the <b>input</b> matches the <b>pattern</b>,
   *         <b>false</b> otherwise.
   */
  public static boolean wildcardMatch(String input, String pattern) {
    String regex = pattern.replace("?", ".?").replace("*", ".*?");
    return input.matches(regex);
  }

  /**
   * Returns the string without trailing whitespace characters
   * 
   * @param s
   * @return
   */
  public static String rtrim(String s) {
    int i = s.length() - 1;
    while (i >= 0 && Character.isWhitespace(s.charAt(i))) {
      i--;
    }
    return s.substring(0, i + 1);
  }
}
