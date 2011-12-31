package processing.app.helpers;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class StringReplacer {

  public static String[] formatAndSplit(String src, Map<String, String> dict)
      throws Exception {
    // Do a replace with dictionary
    src = StringReplacer.replaceFromMapping(src, dict);

    // Split the resulting string in arguments
    return quotedSplit(src, '"', false);
  }

  public static String[] quotedSplit(String src, char escapeChar,
      boolean acceptEmptyArguments) throws Exception {
    String quote = "" + escapeChar;
    List<String> res = new ArrayList<String>();
    String escapedArg = null;
    boolean escaping = false;
    for (String i : src.split(" ")) {
      if (!escaping) {
        if (!i.startsWith(quote)) {
          if (!i.trim().isEmpty() || acceptEmptyArguments)
            res.add(i);
          continue;
        }

        escaping = true;
        i = i.substring(1);
        escapedArg = "";
      }

      if (!i.endsWith(quote)) {
        escapedArg += i + " ";
        continue;
      }

      escapedArg += i.substring(0, i.length() - 1);
      if (!escapedArg.trim().isEmpty() || acceptEmptyArguments)
        res.add(escapedArg);
      escaping = false;
    }
    if (escaping)
      throw new Exception("Invalid quoting: no closing '" + escapeChar
          + "' char found.");
    return res.toArray(new String[0]);
  }

  public static String replaceFromMapping(String src, Map<String, String> map) {
    return replaceFromMapping(src, map, "{", "}");
  }

  public static String replaceFromMapping(String src, Map<String, String> map,
      String leftDelimiter, String rightDelimiter) {
    for (String k : map.keySet()) {
      String keyword = leftDelimiter + k + rightDelimiter;
      src = src.replace(keyword, map.get(k));
    }
    return src;
  }

}
