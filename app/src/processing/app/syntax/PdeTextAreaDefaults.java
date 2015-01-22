/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  PdeTextAreaDefaults - grabs font/color settings for the editor
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-06 Ben Fry and Casey Reas
  Copyright (c) 2001-03 Massachusetts Institute of Technology

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
import processing.app.helpers.OSUtils;


public class PdeTextAreaDefaults extends TextAreaDefaults {

  public PdeTextAreaDefaults() {

    inputHandler = new DefaultInputHandler();
    //inputHandler.addDefaultKeyBindings();  // 0122

    // use option on mac for text edit controls that are ctrl on windows/linux
    String mod = OSUtils.isMacOS() ? "A" : "C";

    // right now, ctrl-up/down is select up/down, but mod should be
    // used instead, because the mac expects it to be option(alt)

    inputHandler.addKeyBinding("BACK_SPACE", InputHandler.BACKSPACE);
    // for 0122, shift-backspace is delete, for 0176, it's now a preference, 
    // to prevent holy warriors from attacking me for it.
    if (Preferences.getBoolean("editor.keys.shift_backspace_is_delete")) {
      inputHandler.addKeyBinding("S+BACK_SPACE", InputHandler.DELETE);
    } else {
      inputHandler.addKeyBinding("S+BACK_SPACE", InputHandler.BACKSPACE);
    }
    
    inputHandler.addKeyBinding("DELETE", InputHandler.DELETE);
    inputHandler.addKeyBinding("S+DELETE", InputHandler.DELETE);

    // the following two were changing for 0122 for better mac/pc compatability
    inputHandler.addKeyBinding(mod+"+BACK_SPACE", InputHandler.BACKSPACE_WORD);
    inputHandler.addKeyBinding(mod+"+DELETE", InputHandler.DELETE_WORD);

    // handled by listener, don't bother here
    //inputHandler.addKeyBinding("ENTER", InputHandler.INSERT_BREAK);
    //inputHandler.addKeyBinding("TAB", InputHandler.INSERT_TAB);

    inputHandler.addKeyBinding("INSERT", InputHandler.OVERWRITE);
    
    // http://dev.processing.org/bugs/show_bug.cgi?id=162
    // added for 0176, though the bindings do not appear relevant for osx 
    if (Preferences.getBoolean("editor.keys.alternative_cut_copy_paste")) {
      inputHandler.addKeyBinding("C+INSERT", InputHandler.CLIPBOARD_COPY);
      inputHandler.addKeyBinding("S+INSERT", InputHandler.CLIPBOARD_PASTE);
      inputHandler.addKeyBinding("S+DELETE", InputHandler.CLIPBOARD_CUT);
    }

    // disabling for 0122, not sure what this does
    //inputHandler.addKeyBinding("C+\\", InputHandler.TOGGLE_RECT);

    // for 0122, these have been changed for better compatibility
    // HOME and END now mean the beginning/end of the document
    // for 0176 changed this to a preference so that the Mac OS X people
    // can get the "normal" behavior as well if they prefer.
    if (Preferences.getBoolean("editor.keys.home_and_end_travel_far")) {
      inputHandler.addKeyBinding("HOME", InputHandler.DOCUMENT_HOME);
      inputHandler.addKeyBinding("END", InputHandler.DOCUMENT_END);
      inputHandler.addKeyBinding("S+HOME", InputHandler.SELECT_DOC_HOME);
      inputHandler.addKeyBinding("S+END", InputHandler.SELECT_DOC_END);
    } else {
      // for 0123 added the proper windows defaults
      inputHandler.addKeyBinding("HOME", InputHandler.HOME);
      inputHandler.addKeyBinding("END", InputHandler.END);
      inputHandler.addKeyBinding("S+HOME", InputHandler.SELECT_HOME);
      inputHandler.addKeyBinding("S+END", InputHandler.SELECT_END);
      inputHandler.addKeyBinding("C+HOME", InputHandler.DOCUMENT_HOME);
      inputHandler.addKeyBinding("C+END", InputHandler.DOCUMENT_END);
      inputHandler.addKeyBinding("CS+HOME", InputHandler.SELECT_DOC_HOME);
      inputHandler.addKeyBinding("CS+END", InputHandler.SELECT_DOC_END);
    }

    if (OSUtils.isMacOS()) {
      inputHandler.addKeyBinding("M+LEFT", InputHandler.HOME);
      inputHandler.addKeyBinding("M+RIGHT", InputHandler.END);
      inputHandler.addKeyBinding("MS+LEFT", InputHandler.SELECT_HOME); // 0122
      inputHandler.addKeyBinding("MS+RIGHT", InputHandler.SELECT_END);  // 0122
    } else {
      inputHandler.addKeyBinding("C+LEFT", InputHandler.HOME);  // 0122
      inputHandler.addKeyBinding("C+RIGHT", InputHandler.END);  // 0122
      inputHandler.addKeyBinding("CS+HOME", InputHandler.SELECT_HOME); // 0122
      inputHandler.addKeyBinding("CS+END", InputHandler.SELECT_END);  // 0122
    }

    inputHandler.addKeyBinding("PAGE_UP", InputHandler.PREV_PAGE);
    inputHandler.addKeyBinding("PAGE_DOWN", InputHandler.NEXT_PAGE);
    inputHandler.addKeyBinding("S+PAGE_UP", InputHandler.SELECT_PREV_PAGE);
    inputHandler.addKeyBinding("S+PAGE_DOWN", InputHandler.SELECT_NEXT_PAGE);

    inputHandler.addKeyBinding("LEFT", InputHandler.PREV_CHAR);
    inputHandler.addKeyBinding("S+LEFT", InputHandler.SELECT_PREV_CHAR);
    inputHandler.addKeyBinding(mod + "+LEFT", InputHandler.PREV_WORD);
    inputHandler.addKeyBinding(mod + "S+LEFT", InputHandler.SELECT_PREV_WORD);
    inputHandler.addKeyBinding("RIGHT", InputHandler.NEXT_CHAR);
    inputHandler.addKeyBinding("S+RIGHT", InputHandler.SELECT_NEXT_CHAR);
    inputHandler.addKeyBinding(mod + "+RIGHT", InputHandler.NEXT_WORD);
    inputHandler.addKeyBinding(mod + "S+RIGHT", InputHandler.SELECT_NEXT_WORD);

    inputHandler.addKeyBinding("UP", InputHandler.PREV_LINE);
    inputHandler.addKeyBinding(mod + "+UP", InputHandler.PREV_LINE);  // p5
    inputHandler.addKeyBinding("S+UP", InputHandler.SELECT_PREV_LINE);
    inputHandler.addKeyBinding("DOWN", InputHandler.NEXT_LINE);
    inputHandler.addKeyBinding(mod + "+DOWN", InputHandler.NEXT_LINE);  // p5
    inputHandler.addKeyBinding("S+DOWN", InputHandler.SELECT_NEXT_LINE);

    inputHandler.addKeyBinding("MS+UP", InputHandler.SELECT_DOC_HOME);
    inputHandler.addKeyBinding("CS+UP", InputHandler.SELECT_DOC_HOME);
    inputHandler.addKeyBinding("MS+DOWN", InputHandler.SELECT_DOC_END);
    inputHandler.addKeyBinding("CS+DOWN", InputHandler.SELECT_DOC_END);

    inputHandler.addKeyBinding(mod + "+ENTER", InputHandler.REPEAT);

    document = new SyntaxDocument();
    editable = true;
    electricScroll = 3;

    cols = 80;
    rows = 15;


    // moved from SyntaxUtilities
    //DEFAULTS.styles = SyntaxUtilities.getDefaultSyntaxStyles();

    styles = new SyntaxStyle[Token.ID_COUNT];

    // comments
    styles[Token.COMMENT1] = Theme.getStyle("comment1");
    styles[Token.COMMENT2] = Theme.getStyle("comment2");

    // abstract, final, private
    styles[Token.KEYWORD1] = Theme.getStyle("keyword1");

    // beginShape, point, line
    styles[Token.KEYWORD2] = Theme.getStyle("keyword2");

    // byte, char, short, color
    styles[Token.KEYWORD3] = Theme.getStyle("keyword3");

    // constants: null, true, this, RGB, TWO_PI
    styles[Token.LITERAL1] = Theme.getStyle("literal1");

    // p5 built in variables: mouseX, width, pixels
    styles[Token.LITERAL2] = Theme.getStyle("literal2");

    // ??
    styles[Token.LABEL] = Theme.getStyle("label");

    // http://arduino.cc/
    styles[Token.URL] = Theme.getStyle("url");

    // + - = /
    styles[Token.OPERATOR] = Theme.getStyle("operator");

    // area that's not in use by the text (replaced with tildes)
    styles[Token.INVALID] = Theme.getStyle("invalid");


    // moved from TextAreaPainter

    font = Preferences.getFont("editor.font");

    fgcolor = Theme.getColor("editor.fgcolor");
    bgcolor = Theme.getColor("editor.bgcolor");

    caretVisible = true;
    caretBlinks = Preferences.getBoolean("editor.caret.blink");
    caretColor = Theme.getColor("editor.caret.color");

    selectionColor = Theme.getColor("editor.selection.color");

    lineHighlight =
      Theme.getBoolean("editor.linehighlight");
    lineHighlightColor =
      Theme.getColor("editor.linehighlight.color");

    bracketHighlight =
      Theme.getBoolean("editor.brackethighlight");
    bracketHighlightColor =
      Theme.getColor("editor.brackethighlight.color");

    eolMarkers = Theme.getBoolean("editor.eolmarkers");
    eolMarkerColor = Theme.getColor("editor.eolmarkers.color");

    paintInvalid = Theme.getBoolean("editor.invalid");
  }
}
