/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 StringReplacer - Utility class for expression formatting
 Part of the Arduino project - http://www.arduino.cc/

 Copyright (c) 2011 Cristian Maglie

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
package processing.app.helpers;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class StringReplacer {

  public static String[] formatAndSplit(String src, Map<String, String> dict,
                                        boolean recursive) throws Exception {
    String res;

    // Recursive replace with a max depth of 10 levels.
    for (int i = 0; i < 10; i++) {
      // Do a replace with dictionary
      res = StringReplacer.replaceFromMapping(src, dict);
      if (!recursive)
        break;
      if (res.equals(src))
        break;
      src = res;
    }

    // Split the resulting string in arguments
    return quotedSplit(src, "\"'", false);
  }

  public static String[] quotedSplit(String src, String quoteChars,
                                     boolean acceptEmptyArguments)
      throws Exception {
    List<String> res = new ArrayList<String>();
    String escapedArg = null;
    String escapingChar = null;
    for (String i : src.split(" ")) {
      if (escapingChar == null) {
        // If the first char is not an escape char..
        String first = null;
        if (i.length() > 0)
          first = i.substring(0, 1);
        if (first == null || !quoteChars.contains(first)) {
          if (i.trim().length() != 0 || acceptEmptyArguments)
            res.add(i);
          continue;
        }

        escapingChar = first;
        i = i.substring(1);
        escapedArg = "";
      }

      if (!i.endsWith(escapingChar)) {
        escapedArg += i + " ";
        continue;
      }

      escapedArg += i.substring(0, i.length() - 1);
      if (escapedArg.trim().length() != 0 || acceptEmptyArguments)
        res.add(escapedArg);
      escapingChar = null;
    }
    if (escapingChar != null)
      throw new Exception("Invalid quoting: no closing [" + escapingChar +
          "] char found.");
    return res.toArray(new String[0]);
  }

  public static String replaceFromMapping(String src, Map<String, String> map) {
    return replaceFromMapping(src, map, "{", "}");
  }

  public static String replaceFromMapping(String src, Map<String, String> map,
                                          String leftDelimiter,
                                          String rightDelimiter) {
    for (String k : map.keySet()) {
      String keyword = leftDelimiter + k + rightDelimiter;
      src = src.replace(keyword, map.get(k));
    }
    return src;
  }

}
