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

import org.apache.commons.compress.utils.IOUtils;
import org.fife.ui.rsyntaxtextarea.*;
import org.fife.ui.rsyntaxtextarea.Theme;
import org.fife.ui.rsyntaxtextarea.Token;
import org.fife.ui.rsyntaxtextarea.focusabletip.FocusableTip;
import org.fife.ui.rtextarea.RTextArea;
import org.fife.ui.rtextarea.RTextAreaUI;
import org.fife.ui.rtextarea.RUndoManager;
import processing.app.*;

import javax.swing.*;
import javax.swing.event.EventListenerList;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.Segment;
import javax.swing.undo.UndoManager;
import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.logging.Logger;

/**
 * Arduino Sketch code editor based on RSyntaxTextArea (http://fifesoft.com/rsyntaxtextarea)
 *
 * @author Ricardo JL Rufino (ricardo@criativasoft.com.br)
 * @date 20/04/2015
 * @since 1.6.4
 */
public class SketchTextArea extends RSyntaxTextArea {

  private final static Logger LOG = Logger.getLogger(SketchTextArea.class.getName());

  /**
   * The last docTooltip displayed.
   */
  private FocusableTip docTooltip;

  private EditorListener editorListener;

  private final PdeKeywords pdeKeywords;

  public SketchTextArea(PdeKeywords pdeKeywords) throws IOException {
    this.pdeKeywords = pdeKeywords;
    installFeatures();
  }

  protected void installFeatures() throws IOException {
    setTheme(PreferencesData.get("editor.syntax_theme", "default"));

    setLinkGenerator(new DocLinkGenerator(pdeKeywords));

    fixControlTab();

    setSyntaxEditingStyle(SYNTAX_STYLE_CPLUSPLUS);
  }

  public void setTheme(String name) throws IOException {
    FileInputStream defaultXmlInputStream = null;
    try {
      defaultXmlInputStream = new FileInputStream(new File(BaseNoGui.getContentFile("lib"), "theme/syntax/" + name + ".xml"));
      Theme theme = Theme.load(defaultXmlInputStream);
      theme.apply(this);
    } finally {
      IOUtils.closeQuietly(defaultXmlInputStream);
    }

    setForeground(processing.app.Theme.getColor("editor.fgcolor"));
    setBackground(processing.app.Theme.getColor("editor.bgcolor"));
    setCurrentLineHighlightColor(processing.app.Theme.getColor("editor.linehighlight.color"));
    setCaretColor(processing.app.Theme.getColor("editor.caret.color"));
    setSelectedTextColor(null);
    setUseSelectedTextColor(false);
    setSelectionColor(processing.app.Theme.getColor("editor.selection.color"));
    setMatchedBracketBorderColor(processing.app.Theme.getColor("editor.brackethighlight.color"));
    setHyperlinkForeground((Color) processing.app.Theme.getStyledFont("url", getFont()).get("color"));

    setSyntaxTheme(TokenTypes.DATA_TYPE, "data_type");
    setSyntaxTheme(TokenTypes.FUNCTION, "function");
    setSyntaxTheme(TokenTypes.RESERVED_WORD, "reserved_word");
    setSyntaxTheme(TokenTypes.RESERVED_WORD_2, "reserved_word_2");
    setSyntaxTheme(TokenTypes.VARIABLE, "variable");
    setSyntaxTheme(TokenTypes.OPERATOR, "operator");
    setSyntaxTheme(TokenTypes.COMMENT_DOCUMENTATION, "comment1");
    setSyntaxTheme(TokenTypes.COMMENT_EOL, "comment1");
    setSyntaxTheme(TokenTypes.COMMENT_KEYWORD, "comment1");
    setSyntaxTheme(TokenTypes.COMMENT_MARKUP, "comment1");
    setSyntaxTheme(TokenTypes.LITERAL_CHAR, "literal_char");
    setSyntaxTheme(TokenTypes.LITERAL_STRING_DOUBLE_QUOTE, "literal_string_double_quote");
  }

  private void setSyntaxTheme(int tokenType, String id) {
    Style style = getSyntaxScheme().getStyle(tokenType);

    Map<String, Object> styledFont = processing.app.Theme.getStyledFont(id, style.font);
    style.foreground = (Color) styledFont.get("color");
    style.font = (Font) styledFont.get("font");

    getSyntaxScheme().setStyle(tokenType, style);
  }

  // Removing the default focus traversal keys
  // This is because the DefaultKeyboardFocusManager handles the keypress and consumes the event
  protected void fixControlTab() {
    removeCTRLTabFromFocusTraversal();

    removeCTRLSHIFTTabFromFocusTraversal();
  }

  private void removeCTRLSHIFTTabFromFocusTraversal() {
    KeyStroke ctrlShiftTab = KeyStroke.getKeyStroke("ctrl shift TAB");
    Set<AWTKeyStroke> backwardKeys = new HashSet<AWTKeyStroke>(this.getFocusTraversalKeys(KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS));
    backwardKeys.remove(ctrlShiftTab);
  }

  private void removeCTRLTabFromFocusTraversal() {
    KeyStroke ctrlTab = KeyStroke.getKeyStroke("ctrl TAB");
    Set<AWTKeyStroke> forwardKeys = new HashSet<AWTKeyStroke>(this.getFocusTraversalKeys(KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS));
    forwardKeys.remove(ctrlTab);
    this.setFocusTraversalKeys(KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS, forwardKeys);
  }


  @Override
  public void select(int selectionStart, int selectionEnd) {
    super.select(selectionStart, selectionEnd);
  }

  public boolean isSelectionActive() {
    return this.getSelectedText() != null;
  }

  public void setSelectedText(String text) {

    int old = getTextMode();
    setTextMode(OVERWRITE_MODE);
    replaceSelection(text);
    setTextMode(old);

  }

  public void processKeyEvent(KeyEvent evt) {

    // this had to be added because the menu key events weren't making it up to the frame.

    switch (evt.getID()) {
      case KeyEvent.KEY_TYPED:
        if (editorListener != null) editorListener.keyTyped(evt);
        break;
      case KeyEvent.KEY_PRESSED:
        if (editorListener != null) editorListener.keyPressed(evt);
        break;
      case KeyEvent.KEY_RELEASED:
        // inputHandler.keyReleased(evt);
        break;
    }

    if (!evt.isConsumed()) {
      super.processKeyEvent(evt);
    }
  }

  public void switchDocument(Document document, UndoManager newUndo) {

    // HACK: Dont discard changes on curret UndoManager.
    // BUG: https://github.com/bobbylight/RSyntaxTextArea/issues/84
    setUndoManager(null); // bypass reset current undo manager...

    super.setDocument(document);

    setUndoManager((RUndoManager) newUndo);

    // HACK: Complement previous hack (hide code folding on switch) | Drawback: Lose folding state
//  if(sketch.getCodeCount() > 1 && textarea.isCodeFoldingEnabled()){
//    textarea.setCodeFoldingEnabled(false);
//    textarea.setCodeFoldingEnabled(true);
//  }


  }

  @Override
  protected JPopupMenu createPopupMenu() {
    JPopupMenu menu = super.createPopupMenu();
    return menu;
  }

  @Override
  protected void configurePopupMenu(JPopupMenu popupMenu) {
    super.configurePopupMenu(popupMenu);
  }

  @Override
  protected RTAMouseListener createMouseListener() {
    return new SketchTextAreaMouseListener(this);
  }

  public void getTextLine(int line, Segment segment) {
    try {
      int offset = getLineStartOffset(line);
      int end = getLineEndOffset(line);
      getDocument().getText(offset, end - offset, segment);
    } catch (BadLocationException e) {
    }
  }

  public String getTextLine(int line) {
    try {
      int offset = getLineStartOffset(line);
      int end = getLineEndOffset(line);
      return getDocument().getText(offset, end - offset);
    } catch (BadLocationException e) {
      return null;
    }
  }


  public void setEditorListener(EditorListener editorListener) {
    this.editorListener = editorListener;
  }

  private static class DocLinkGenerator implements LinkGenerator {

    private final PdeKeywords pdeKeywords;

    public DocLinkGenerator(PdeKeywords pdeKeywords) {
      this.pdeKeywords = pdeKeywords;
    }

    @Override
    public LinkGeneratorResult isLinkAtOffset(RSyntaxTextArea textArea, final int offs) {

      final Token token = textArea.modelToToken(offs);

      final String reference = pdeKeywords.getReference(token.getLexeme());

      // LOG.fine("reference: " + reference + ", match: " + (token.getType() == TokenTypes.DATA_TYPE || token.getType() == TokenTypes.VARIABLE || token.getType() == TokenTypes.FUNCTION));

      if (token != null && (reference != null || (token.getType() == TokenTypes.DATA_TYPE || token.getType() == TokenTypes.VARIABLE || token.getType() == TokenTypes.FUNCTION))) {

        LinkGeneratorResult generatorResult = new LinkGeneratorResult() {

          @Override
          public int getSourceOffset() {
            return offs;
          }

          @Override
          public HyperlinkEvent execute() {

            LOG.fine("Open Reference: " + reference);

            Base.showReference("Reference/" + reference);

            return null;
          }
        };

        return generatorResult;
      }

      return null;
    }
  }


  /**
   * Handles http hyperlinks.
   * NOTE (@Ricardo JL Rufino): Workaround to enable hyperlinks by default: https://github.com/bobbylight/RSyntaxTextArea/issues/119
   */
  private class SketchTextAreaMouseListener extends RTextAreaMutableCaretEvent {

    private Insets insets;
    private boolean isScanningForLinks;
    private int hoveredOverLinkOffset = -1;

    protected SketchTextAreaMouseListener(RTextArea textArea) {
      super(textArea);
      insets = new Insets(0, 0, 0, 0);
    }

    /**
     * Notifies all listeners that have registered interest for notification
     * on this event type.  The listener list is processed last to first.
     *
     * @param e The event to fire.
     * @see EventListenerList
     */
    private void fireHyperlinkUpdate(HyperlinkEvent e) {
      // Guaranteed to return a non-null array
      Object[] listeners = listenerList.getListenerList();
      // Process the listeners last to first, notifying
      // those that are interested in this event
      for (int i = listeners.length - 2; i >= 0; i -= 2) {
        if (listeners[i] == HyperlinkListener.class) {
          ((HyperlinkListener) listeners[i + 1]).hyperlinkUpdate(e);
        }
      }
    }

    private HyperlinkEvent createHyperlinkEvent(MouseEvent e) {
      HyperlinkEvent he = null;

      Token t = viewToToken(e.getPoint());
      if (t != null) {
        // Copy token, viewToModel() unfortunately modifies Token
        t = new TokenImpl(t);
      }

      if (t != null && t.isHyperlink()) {
        URL url = null;
        String desc = null;
        try {
          String temp = t.getLexeme();
          // URI's need "http://" prefix for web URL's to work.
          if (temp.startsWith("www.")) {
            temp = "http://" + temp;
          }
          url = new URL(temp);
        } catch (MalformedURLException mue) {
          desc = mue.getMessage();
        }
        he = new HyperlinkEvent(SketchTextArea.this, HyperlinkEvent.EventType.ACTIVATED, url, desc);
      }

      return he;
    }

    @Override
    public void mouseClicked(MouseEvent e) {
      if (getHyperlinksEnabled()) {
        HyperlinkEvent he = createHyperlinkEvent(e);
        if (he != null) {
          fireHyperlinkUpdate(he);
        }
      }
    }

    @Override
    public void mouseMoved(MouseEvent e) {

      super.mouseMoved(e);

      if (!getHyperlinksEnabled()) {
        return;
      }

//      LinkGenerator linkGenerator = getLinkGenerator();

      // GitHub issue RSyntaxTextArea/#25 - links identified at "edges" of editor
      // should not be activated if mouse is in margin insets.
      insets = getInsets(insets);
      if (insets != null) {
        int x = e.getX();
        int y = e.getY();
        if (x <= insets.left || y < insets.top) {
          if (isScanningForLinks) {
            stopScanningForLinks();
          }
          return;
        }
      }

      isScanningForLinks = true;
      Token t = viewToToken(e.getPoint());
      if (t != null) {
        // Copy token, viewToModel() unfortunately modifies Token
        t = new TokenImpl(t);
      }
      Cursor c2 = null;
      if (t != null && t.isHyperlink()) {
        if (hoveredOverLinkOffset == -1 ||
          hoveredOverLinkOffset != t.getOffset()) {
          hoveredOverLinkOffset = t.getOffset();
          repaint();
        }
        c2 = Cursor.getPredefinedCursor(Cursor.HAND_CURSOR);
      }
//      else if (t!=null && linkGenerator!=null) {
//        int offs = viewToModel(e.getPoint());
//        LinkGeneratorResult newResult = linkGenerator.
//            isLinkAtOffset(SketchTextArea.this, offs);
//        if (newResult!=null) {
//          // Repaint if we're at a new link now.
//          if (linkGeneratorResult==null ||
//              !equal(newResult, linkGeneratorResult)) {
//            repaint();
//          }
//          linkGeneratorResult = newResult;
//          hoveredOverLinkOffset = t.getOffset();
//          c2 = Cursor.getPredefinedCursor(Cursor.HAND_CURSOR);
//        }
//        else {
//          // Repaint if we've moved off of a link.
//          if (linkGeneratorResult!=null) {
//            repaint();
//          }
//          c2 = Cursor.getPredefinedCursor(Cursor.TEXT_CURSOR);
//          hoveredOverLinkOffset = -1;
//          linkGeneratorResult = null;
//        }
//      }
      else {
        c2 = Cursor.getPredefinedCursor(Cursor.TEXT_CURSOR);
        hoveredOverLinkOffset = -1;
        //  linkGeneratorResult = null;
      }
      if (getCursor() != c2) {
        setCursor(c2);
        // TODO: Repaint just the affected line(s).
        repaint(); // Link either left or went into.
      }
    }

    private void stopScanningForLinks() {
      if (isScanningForLinks) {
        Cursor c = getCursor();
        isScanningForLinks = false;
        if (c != null && c.getType() == Cursor.HAND_CURSOR) {
          setCursor(Cursor.getPredefinedCursor(Cursor.TEXT_CURSOR));
          repaint(); // TODO: Repaint just the affected line.
        }
      }
    }

  }

  @Override
  protected RTextAreaUI createRTextAreaUI() {
    return new SketchTextAreaUI(this);
  }
}
