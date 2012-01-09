/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  PdeKeywords - handles text coloring and links to html reference
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

package processing.app.syntax;

import processing.app.*;

import java.io.*;
import java.util.*;


public class PdeKeywords extends CTokenMarker {

  // lookup table for the TokenMarker subclass, handles coloring
  static KeywordMap keywordColoring;

  // lookup table that maps keywords to their html reference pages
  static Hashtable keywordToReference;


  public PdeKeywords() {
    super(false, getKeywords());
  }


  /**
   * Handles loading of keywords file.
   * <P>
   * Uses getKeywords()  method because that's part of the
   * TokenMarker classes.
   * <P>
   * It is recommended that a # sign be used for comments
   * inside keywords.txt.
   */
  static public KeywordMap getKeywords() {
    if (keywordColoring == null) {
      try {
        keywordColoring = new KeywordMap(false);
        keywordToReference = new Hashtable();
        getKeywords(Base.getLibStream("keywords.txt"));
        for (File lib : Base.getLibraries().values()) {
          File keywords = new File(lib, "keywords.txt");
          if (keywords.exists()) getKeywords(new FileInputStream(keywords));
        }
      } catch (Exception e) {
        Base.showError("Problem loading keywords",
                          "Could not load keywords.txt,\n" +
                          "please re-install Processing.", e);
        System.exit(1);
      }
    }
    return keywordColoring;
  }
  
  static private void getKeywords(InputStream input) throws Exception {
    InputStreamReader isr = new InputStreamReader(input);
    BufferedReader reader = new BufferedReader(isr);

    String line = null;
    while ((line = reader.readLine()) != null) {
      //System.out.println("line is " + line);
      // in case there's any garbage on the line
      //if (line.trim().length() == 0) continue;

      String pieces[] = processing.core.PApplet.split(line, '\t');
      if (pieces.length >= 2) {
        //int tab = line.indexOf('\t');
        // any line with no tab is ignored
        // meaning that a comment is any line without a tab
        //if (tab == -1) continue;

        String keyword = pieces[0].trim();
        //String keyword = line.substring(0, tab).trim();
        //String second = line.substring(tab + 1);
        //tab = second.indexOf('\t');
        //String coloring = second.substring(0, tab).trim();
        //String htmlFilename = second.substring(tab + 1).trim();
        String coloring = pieces[1].trim();

        if (coloring.length() > 0) {
          // text will be KEYWORD or LITERAL
          boolean isKey = (coloring.charAt(0) == 'K');
          // KEYWORD1 -> 0, KEYWORD2 -> 1, etc
          int num = coloring.charAt(coloring.length() - 1) - '1';
          byte id = (byte)
            ((isKey ? Token.KEYWORD1 : Token.LITERAL1) + num);
          //System.out.println("got " + (isKey ? "keyword" : "literal") +
          //                 (num+1) + " for " + keyword);
          keywordColoring.add(keyword, id);
        }
        if (pieces.length >= 3) {
          String htmlFilename = pieces[2].trim();
          if (htmlFilename.length() > 0) {
            keywordToReference.put(keyword, htmlFilename);
          }
        }
      }
    }
    reader.close();
  }


  static public String getReference(String keyword) {
    return (String) keywordToReference.get(keyword);
  }
}
