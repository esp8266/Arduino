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

import cc.arduino.contributions.libraries.ContributedLibrary;
import org.apache.commons.compress.utils.IOUtils;
import org.fife.ui.rsyntaxtextarea.TokenMap;
import org.fife.ui.rsyntaxtextarea.TokenTypes;
import processing.app.Base;
import processing.app.BaseNoGui;
import processing.app.legacy.PApplet;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Pattern;


public class PdeKeywords {

  private static final Map<String, Integer> KNOWN_TOKEN_TYPES = new HashMap<String, Integer>();
  private static final Pattern ALPHA = Pattern.compile("\\w");

  static {
    KNOWN_TOKEN_TYPES.put("RESERVED_WORD", TokenTypes.RESERVED_WORD);
    KNOWN_TOKEN_TYPES.put("RESERVED_WORD_2", TokenTypes.RESERVED_WORD_2);
    KNOWN_TOKEN_TYPES.put("VARIABLE", TokenTypes.VARIABLE);
    KNOWN_TOKEN_TYPES.put("OPERATOR", TokenTypes.OPERATOR);
    KNOWN_TOKEN_TYPES.put("DATA_TYPE", TokenTypes.DATA_TYPE);
    KNOWN_TOKEN_TYPES.put("LITERAL_BOOLEAN", TokenTypes.LITERAL_BOOLEAN);
    KNOWN_TOKEN_TYPES.put("LITERAL_CHAR", TokenTypes.LITERAL_CHAR);
  }

  // lookup table for the TokenMarker subclass, handles coloring
  private final TokenMap keywordTokenType;
  private final Map<String, String> keywordOldToken;
  private final Map<String, String> keywordTokenTypeAsString;

  // lookup table that maps keywords to their html reference pages
  private final Map<String, String> keywordToReference;

  public PdeKeywords() {
    this.keywordTokenType = new TokenMap();
    this.keywordOldToken = new HashMap<String, String>();
    this.keywordTokenTypeAsString = new HashMap<String, String>();
    this.keywordToReference = new HashMap<String, String>();
  }

  /**
   * Handles loading of keywords file.
   * <p/>
   * Uses getKeywords()  method because that's part of the
   * TokenMarker classes.
   * <p/>
   * It is recommended that a # sign be used for comments
   * inside keywords.txt.
   */
  public void reload() {
    try {
      parseKeywordsTxt(new File(BaseNoGui.getContentFile("lib"), "keywords.txt"));
      for (ContributedLibrary lib : Base.getLibraries()) {
        File keywords = new File(lib.getInstalledFolder(), "keywords.txt");
        if (keywords.exists()) {
          parseKeywordsTxt(keywords);
        }
      }
    } catch (Exception e) {
      Base.showError("Problem loading keywords", "Could not load keywords.txt,\nplease re-install Arduino.", e);
      System.exit(1);
    }
  }

  private void parseKeywordsTxt(File input) throws Exception {
    BufferedReader reader = null;
    try {
      reader = new BufferedReader(new InputStreamReader(new FileInputStream(input)));

      String line;
      while ((line = reader.readLine()) != null) {
        //System.out.println("line is " + line);
        // in case there's any garbage on the line
        line = line.trim();
        if (line.length() == 0 || line.startsWith("#")) {
          continue;
        }

        String pieces[] = PApplet.split(line, '\t');

        String keyword = pieces[0].trim();

        if (pieces.length >= 2) {
          keywordOldToken.put(keyword, pieces[1]);
        }

        if (pieces.length >= 3) {
          parseHTMLReferenceFileName(pieces[2], keyword);
        }
        if (pieces.length >= 4) {
          parseRSyntaxTextAreaTokenType(pieces[3], keyword);
        }
      }

      fillMissingTokenType();
    } finally {
      IOUtils.closeQuietly(reader);
    }

  }

  private void fillMissingTokenType() {
    for (Map.Entry<String, String> oldTokenEntry : keywordOldToken.entrySet()) {
      String keyword = oldTokenEntry.getKey();
      if (!keywordTokenTypeAsString.containsKey(keyword)) {
        if ("KEYWORD1".equals(oldTokenEntry.getValue())) {
          parseRSyntaxTextAreaTokenType("DATA_TYPE", keyword);
        } else {
          parseRSyntaxTextAreaTokenType("FUNCTION", keyword);
        }
      }
    }
  }

  private void parseRSyntaxTextAreaTokenType(String tokenTypeAsString, String keyword) {
    if (!ALPHA.matcher(keyword).find()) {
      return;
    }

    if (KNOWN_TOKEN_TYPES.containsKey(tokenTypeAsString)) {
      keywordTokenType.put(keyword, KNOWN_TOKEN_TYPES.get(tokenTypeAsString));
      keywordTokenTypeAsString.put(keyword, tokenTypeAsString);
    } else {
      keywordTokenType.put(keyword, TokenTypes.FUNCTION);
      keywordTokenTypeAsString.put(keyword, "FUNCTION");
    }
  }

  private void parseHTMLReferenceFileName(String piece, String keyword) {
    String htmlFilename = piece.trim();
    if (htmlFilename.length() > 0) {
      keywordToReference.put(keyword, htmlFilename);
    }
  }

  public String getReference(String keyword) {
    return keywordToReference.get(keyword);
  }

  public String getTokenTypeAsString(String keyword) {
    return keywordTokenTypeAsString.get(keyword);
  }

  public int getTokenType(char[] array, int start, int end) {
    return keywordTokenType.get(array, start, end);
  }
}
