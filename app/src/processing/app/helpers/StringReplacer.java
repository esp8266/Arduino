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

 $Id$
 */
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
