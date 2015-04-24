/*
 * This file is part of Arduino.
 *
 * Copyright 2015 Ricardo JL Rufino (ricardo@criativasoft.com.br)
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */

package processing.app.syntax;

import java.util.HashMap;
import java.util.Set;

import org.fife.ui.rsyntaxtextarea.TokenMap;
import org.fife.ui.rsyntaxtextarea.TokenTypes;
import org.fife.ui.rsyntaxtextarea.modes.CPlusPlusTokenMaker;

import processing.app.Base;

/**
 * Controls the syntax highlighting of {@link SketchTextArea} based on the {@link PdeKeywords}
 * @author Ricardo JL Rufino (ricardo@criativasoft.com.br)
 * @date 20/04/2015
 * @since 1.6.4 
 */
public class SketchTokenMaker extends CPlusPlusTokenMaker {
   
  static TokenMap extraTokens;

  public SketchTokenMaker() {
      extraTokens = getKeywords();
  }

  @Override
  public void addToken(char[] array, int start, int end, int tokenType, int startOffset, boolean hyperlink) {
      // This assumes all of your extra tokens would normally be scanned as IDENTIFIER.
      if (tokenType == TokenTypes.IDENTIFIER || tokenType == TokenTypes.DATA_TYPE) {
          int newType = extraTokens.get(array, start, end);
          if (newType>-1) {
              tokenType = newType;
          }
      }
      super.addToken(array, start, end, tokenType, startOffset, hyperlink);
  }

  public static void addKeyword(String keyword, int type) {
      extraTokens.put(keyword, type);
  }

  public void clear() {
      extraTokens = new TokenMap();
  }
  
  
  /**
   * Handles loading of keywords file.
   * <P>
   * It is recommended that a # sign be used for comments
   * inside keywords.txt.
   */
  static public TokenMap getKeywords() {
    if (extraTokens == null) {
      try {
        extraTokens = new TokenMap(false);
        
        extraTokens.put("setup", TokenTypes.RESERVED_WORD);
        extraTokens.put("loop", TokenTypes.RESERVED_WORD);
        
        extraTokens.put("HIGH", TokenTypes.RESERVED_WORD_2);
        extraTokens.put("LOW", TokenTypes.RESERVED_WORD_2);
        extraTokens.put("OUTPUT", TokenTypes.RESERVED_WORD_2);
        extraTokens.put("INPUT", TokenTypes.RESERVED_WORD_2);
        extraTokens.put("INPUT_PULLUP", TokenTypes.RESERVED_WORD_2);

        extraTokens.put("CHANGE", TokenTypes.RESERVED_WORD_2);
        extraTokens.put("FALLING", TokenTypes.RESERVED_WORD_2);
        extraTokens.put("RISING", TokenTypes.RESERVED_WORD_2);

        extraTokens.put("PI", TokenTypes.LITERAL_NUMBER_FLOAT);
        extraTokens.put("HALF_PI", TokenTypes.LITERAL_NUMBER_FLOAT);
        extraTokens.put("TWO_PI", TokenTypes.LITERAL_NUMBER_FLOAT);
        extraTokens.put("DEG_TO_RAD", TokenTypes.LITERAL_NUMBER_FLOAT);
        extraTokens.put("RAD_TO_DEG", TokenTypes.LITERAL_NUMBER_FLOAT);
        extraTokens.put("EULER", TokenTypes.LITERAL_NUMBER_FLOAT);

        // Print.
        extraTokens.put("DEC", TokenTypes.RESERVED_WORD_2);
        extraTokens.put("HEX", TokenTypes.RESERVED_WORD_2);
        extraTokens.put("OCT", TokenTypes.RESERVED_WORD_2);
        extraTokens.put("BIN", TokenTypes.RESERVED_WORD_2);

        extraTokens.put("true", TokenTypes.LITERAL_BOOLEAN);
        extraTokens.put("false", TokenTypes.LITERAL_BOOLEAN);

        // Related IO
        extraTokens.put("pinMode", TokenTypes.FUNCTION);
        extraTokens.put("digitalWrite", TokenTypes.FUNCTION);
        extraTokens.put("digitalRead", TokenTypes.FUNCTION);
        extraTokens.put("analogRead", TokenTypes.FUNCTION);
        extraTokens.put("analogReference", TokenTypes.FUNCTION);
        extraTokens.put("analogWrite", TokenTypes.FUNCTION);
        
        // Others.
        extraTokens.put("DIGITAL", TokenTypes.RESERVED_WORD_2);
        extraTokens.put("ANALOG", TokenTypes.RESERVED_WORD_2);    
        
        // force load references.
        PdeKeywords.reload();
      
        
        HashMap<String, Integer> keywords = PdeKeywords.get();
        Set<String> keys = keywords.keySet();
        for (String key : keys) {
          extraTokens.put(key, keywords.get(key));
        }
        
      } catch (Exception e) {
        Base.showError("Problem loading keywords",
                          "Could not load keywords.txt,\n" +
                          "please re-install Arduino.", e);
        System.exit(1);
      }
    }
    return extraTokens;
  }
  
}
