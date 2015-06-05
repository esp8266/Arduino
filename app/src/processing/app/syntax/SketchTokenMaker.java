/*
 * This file is part of Arduino.
 *
 * Copyright 2015 Ricardo JL Rufino (ricardo@criativasoft.com.br)
 * Copyright 2015 Arduino LLC
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

import org.fife.ui.rsyntaxtextarea.modes.CPlusPlusTokenMaker;

/**
 * Controls the syntax highlighting of {@link SketchTextArea} based on the {@link PdeKeywords}
 *
 * @author Ricardo JL Rufino (ricardo@criativasoft.com.br)
 * @date 20/04/2015
 * @since 1.6.4
 */
public class SketchTokenMaker extends CPlusPlusTokenMaker {

  private final PdeKeywords pdeKeywords;

  public SketchTokenMaker(PdeKeywords pdeKeywords) {
    this.pdeKeywords = pdeKeywords;
  }

  @Override
  public void addToken(char[] array, int start, int end, int tokenType, int startOffset, boolean hyperlink) {
    if (start > end) {
      super.addToken(array, start, end, tokenType, startOffset, hyperlink);
      return;
    }

    // This assumes all of your extra tokens would normally be scanned as IDENTIFIER.
    int newType = pdeKeywords.getTokenType(array, start, end);
    if (newType > -1) {
      tokenType = newType;
    }
    super.addToken(array, start, end, tokenType, startOffset, hyperlink);
  }

}
