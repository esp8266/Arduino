/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 * JEditTextArea.java - jEdit's text component
 * Copyright (C) 1999 Slava Pestov
 *
 * You may use and modify this package for any purpose. Redistribution is
 * permitted, in both source and binary form, provided that this notice
 * remains intact in all source distributions of this package.
 */

package processing.app.syntax;

import processing.app.*;

import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.undo.*;
import javax.swing.*;

import java.awt.datatransfer.*;
import java.awt.event.*;
import java.awt.*;
import java.util.Enumeration;
import java.util.Vector;
import java.awt.im.InputMethodRequests;

import processing.app.syntax.im.InputMethodSupport;

/**
 * jEdit's text area component. It is more suited for editing program
 * source code than JEditorPane, because it drops the unnecessary features
 * (images, variable-width lines, and so on) and adds a whole bunch of
 * useful goodies such as:
 * <ul>
 * <li>More flexible key binding scheme
 * <li>Supports macro recorders
 * <li>Rectangular selection
 * <li>Bracket highlighting
 * <li>Syntax highlighting
 * <li>Command repetition
 * <li>Block caret can be enabled
 * </ul>
 * It is also faster and doesn't have as many problems. It can be used
 * in other applications; the only other part of jEdit it depends on is
 * the syntax package.<p>
 *
 * To use it in your app, treat it like any other component, for example:
 * <pre>JEditTextArea ta = new JEditTextArea();
 * ta.setTokenMarker(new JavaTokenMarker());
 * ta.setText("public class Test {\n"
 *     + "    public static void main(String[] args) {\n"
 *     + "        System.out.println(\"Hello World\");\n"
 *     + "    }\n"
 *     + "}");</pre>
 *
 * @author Slava Pestov
 * @version $Id: JEditTextArea.java 6123 2010-02-16 21:43:44Z fry $
 */
public class JEditTextArea extends JComponent
{
  /**
   * Adding components with this name to the text area will place
   * them left of the horizontal scroll bar. In jEdit, the status
   * bar is added this way.
   */
  public static String LEFT_OF_SCROLLBAR = "los";

  /**
   * Creates a new JEditTextArea with the default settings.
   */
  /*
  public JEditTextArea()
  {
    this(TextAreaDefaults.getDefaults());
  }
  */

  /**
   * Creates a new JEditTextArea with the specified settings.
   * @param defaults The default settings
   */
  public JEditTextArea(TextAreaDefaults defaults)
  {
    // Enable the necessary events
    enableEvents(AWTEvent.KEY_EVENT_MASK);

    // Initialize some misc. stuff
    painter = new TextAreaPainter(this,defaults);
    editorLineNumbers = new TextAreaLineNumbers(this,defaults);
    documentHandler = new DocumentHandler();
    eventListenerList = new EventListenerList();
    caretEvent = new MutableCaretEvent();
    lineSegment = new Segment();
    bracketLine = bracketPosition = -1;
    blink = true;

    // Initialize the GUI
    setLayout(new ScrollLayout());
    add(LEFT, editorLineNumbers);
    add(CENTER, painter);
    add(RIGHT, vertical = new JScrollBar(JScrollBar.VERTICAL));
    add(BOTTOM, horizontal = new JScrollBar(JScrollBar.HORIZONTAL));

    // Add some event listeners
    vertical.addAdjustmentListener(new AdjustHandler());
    horizontal.addAdjustmentListener(new AdjustHandler());
    painter.addComponentListener(new ComponentHandler());
    painter.addMouseListener(new MouseHandler());
    painter.addMouseMotionListener(new DragHandler());
    addFocusListener(new FocusHandler());
    // send tab keys through to the text area
    // http://dev.processing.org/bugs/show_bug.cgi?id=1267
    setFocusTraversalKeysEnabled(false);

    // Load the defaults
    setInputHandler(defaults.inputHandler);
    setDocument(defaults.document);
    editable = defaults.editable;
    caretVisible = defaults.caretVisible;
    caretBlinks = defaults.caretBlinks;
    electricScroll = defaults.electricScroll;

    // We don't seem to get the initial focus event?
    focusedComponent = this;

    addMouseWheelListener(new MouseWheelListener() {
        public void mouseWheelMoved(MouseWheelEvent e) {
          if (!scrollBarsInitialized) return;
          int amt = e.getWheelRotation();
          vertical.setValue(vertical.getValue() + amt * 3);
        }
      });
  }

  /**
  * Inline Input Method Support for Japanese.
  */
  private InputMethodSupport inputMethodSupport = null;
  public InputMethodRequests getInputMethodRequests() {
		if (inputMethodSupport == null) {
			inputMethodSupport = new InputMethodSupport(this);
		}
		return inputMethodSupport;
  }

  /**
   * Get current position of the vertical scroll bar. [fry]
   */
  public int getScrollPosition() {
    return vertical.getValue();
  }


  /**
   * Set position of the vertical scroll bar. [fry]
   */
  public void setScrollPosition(int what) {
    vertical.setValue(what);
  }


  /**
   * Returns if this component can be traversed by pressing
   * the Tab key. This returns false.
   */
//  public final boolean isManagingFocus() {
//    return true;
//  }

  /**
   * Returns the object responsible for painting this text area.
   */
  public final TextAreaPainter getPainter() {
    return painter;
  }

  /**
   * Returns the input handler.
   */
  public final InputHandler getInputHandler() {
    return inputHandler;
  }

  /**
   * Sets the input handler.
   * @param inputHandler The new input handler
   */
  public void setInputHandler(InputHandler inputHandler) {
    this.inputHandler = inputHandler;
  }

  /**
   * Returns true if the caret is blinking, false otherwise.
   */
  public final boolean isCaretBlinkEnabled() {
    return caretBlinks;
  }

  /**
   * Toggles caret blinking.
   * @param caretBlinks True if the caret should blink, false otherwise
   */
  public void setCaretBlinkEnabled(boolean caretBlinks) {
    this.caretBlinks = caretBlinks;
    if(!caretBlinks)
      blink = false;

    painter.invalidateSelectedLines();
  }

  /**
   * Returns true if the caret is visible, false otherwise.
   */
  public final boolean isCaretVisible() {
    return (!caretBlinks || blink) && caretVisible;
  }

  /**
   * Sets if the caret should be visible.
   * @param caretVisible True if the caret should be visible, false
   * otherwise
   */
  public void setCaretVisible(boolean caretVisible) {
    this.caretVisible = caretVisible;
    blink = true;

    painter.invalidateSelectedLines();
  }

  /**
   * Blinks the caret.
   */
  public final void blinkCaret() {
    if (caretBlinks)  {
      blink = !blink;
      painter.invalidateSelectedLines();
    } else {
      blink = true;
    }
  }

  /**
   * Returns the number of lines from the top and button of the
   * text area that are always visible.
   */
  public final int getElectricScroll() {
    return electricScroll;
  }

  /**
   * Sets the number of lines from the top and bottom of the text
   * area that are always visible
   * @param electricScroll The number of lines always visible from
   * the top or bottom
   */
  public final void setElectricScroll(int electricScroll) {
    this.electricScroll = electricScroll;
  }


  /**
   * Updates the state of the scroll bars. This should be called
   * if the number of lines in the document changes, or when the
   * size of the text are changes.
   */
  public void updateScrollBars() {
    if (vertical != null && visibleLines != 0) {
      vertical.setValues(firstLine,visibleLines,0,getLineCount());
      vertical.setUnitIncrement(2);
      vertical.setBlockIncrement(visibleLines);
    }

    //if (horizontal != null && width != 0) {
    if ((horizontal != null) && (painter.getWidth() != 0)) {
      //int value = horizontal.getValue();
      //System.out.println("updateScrollBars");
      //int width = painter.getWidth();
      int lineCount = getLineCount();
      int maxLineLength = 0;
      for (int i = 0; i < lineCount; i++) {
        int lineLength = getLineLength(i);
        if (lineLength > maxLineLength) {
          maxLineLength = lineLength;
        }
      }
      int charWidth = painter.getFontMetrics().charWidth('w');
      int width = maxLineLength * charWidth;
      int painterWidth = painter.getWidth();
      //System.out.println("max line len " + maxLineLength);
      //System.out.println("width " + width);
      //System.out.println("text area width " + painter.getWidth());

      // this was the default, but it's enormous
      //horizontal.setValues(-horizontalOffset,width,0,width * 5);

      // something more reasonable, though this is a bad solution
      //horizontal.setValues(-horizontalOffset,width,0,width * 2);

      // in general.. time to start looking at that other syntax pkg
      // since most code should fit the window horizontally, just use
      // the default settings for the width, this is a nicer solution
      // until a better update mechanism can be implemented [fry]

      //horizontal.setValues(0, width, 0, width);
                           //0, width - horizontalOffset);
      // works, from pre-75 versions of p5
      //horizontal.setValues(-horizontalOffset, width, 0, width);

      // gets weird when writing to the end of lines
      //horizontal.setValues(value, painterWidth, 0, width);

      // seems to work, implemented for 0075
      horizontal.setValues(-horizontalOffset, painterWidth, 0, width);

      //horizontal.setUnitIncrement(painter.getFontMetrics().charWidth('w'));
      horizontal.setUnitIncrement(charWidth);
      horizontal.setBlockIncrement(width / 2);
    }
    updateLineNumbers();
  }

  private void updateLineNumbers() {
    if (editorLineNumbers != null) {
      editorLineNumbers.updateLineNumbers(getFirstLine() + 1, Math.min(getFirstLine() + getVisibleLines() + 1, getLineCount()));
      editorLineNumbers.updateWidthForNumDigits(String.valueOf(getLineCount()).length());
    }
  }

  /**
   * Returns the line displayed at the text area's origin.
   */
  public final int getFirstLine() {
    return firstLine;
  }

  /**
   * Sets the line displayed at the text area's origin without
   * updating the scroll bars.
   */
  public void setFirstLine(int firstLine) {
    if (firstLine == this.firstLine) return;

    this.firstLine = firstLine;
    if (firstLine != vertical.getValue()) {
      updateScrollBars();
    }
    repaintEditor();
  }

  /**
   * Returns the number of lines visible in this text area.
   */
  public final int getVisibleLines() {
    return visibleLines;
  }

  /**
   * Recalculates the number of visible lines. This should not
   * be called directly.
   */
  public final void recalculateVisibleLines() {
    if (painter == null) return;

    int height = painter.getHeight();
    int lineHeight = painter.getFontMetrics().getHeight();
    visibleLines = height / lineHeight;
    updateScrollBars();
  }

  /**
   * Returns the horizontal offset of drawn lines.
   */
  public final int getHorizontalOffset() {
    return horizontalOffset;
  }

  /**
   * Sets the horizontal offset of drawn lines. This can be used to
   * implement horizontal scrolling.
   * @param horizontalOffset offset The new horizontal offset
   */
  public void setHorizontalOffset(int horizontalOffset)
  {
    if(horizontalOffset == this.horizontalOffset)
      return;
    this.horizontalOffset = horizontalOffset;
    if(horizontalOffset != horizontal.getValue())
      updateScrollBars();
    repaintEditor();
  }

  /**
   * A fast way of changing both the first line and horizontal
   * offset.
   * @param firstLine The new first line
   * @param horizontalOffset The new horizontal offset
   * @return True if any of the values were changed, false otherwise
   */
  public boolean setOrigin(int firstLine, int horizontalOffset)
  {
    boolean changed = false;
    //int oldFirstLine = this.firstLine;

    if(horizontalOffset != this.horizontalOffset)
      {
        this.horizontalOffset = horizontalOffset;
        changed = true;
      }

    if(firstLine != this.firstLine)
      {
        this.firstLine = firstLine;
        changed = true;
      }

    if(changed)
      {
        updateScrollBars();
        repaintEditor();
      }

    return changed;
  }

  private void repaintEditor() {
    painter.repaint();
    updateLineNumbers();
  }
  
  /**
   * Ensures that the caret is visible by scrolling the text area if
   * necessary.
   * @return True if scrolling was actually performed, false if the
   * caret was already visible
   */
  public boolean scrollToCaret()
  {
    int line = getCaretLine();
    int lineStart = getLineStartOffset(line);
    int offset = Math.max(0,Math.min(getLineLength(line) - 1,
                                     getCaretPosition() - lineStart));

    return scrollTo(line,offset);
  }

  /**
   * Ensures that the specified line and offset is visible by scrolling
   * the text area if necessary.
   * @param line The line to scroll to
   * @param offset The offset in the line to scroll to
   * @return True if scrolling was actually performed, false if the
   * line and offset was already visible
   */
  public boolean scrollTo(int line, int offset)
  {
    // visibleLines == 0 before the component is realized
    // we can't do any proper scrolling then, so we have
    // this hack...
    if (visibleLines == 0) {
      setFirstLine(Math.max(0,line - electricScroll));
      return true;
    }

    int newFirstLine = firstLine;
    int newHorizontalOffset = horizontalOffset;

    if(line < firstLine + electricScroll) {
      newFirstLine = Math.max(0,line - electricScroll);

    } else if(line + electricScroll >= firstLine + visibleLines) {
      newFirstLine = (line - visibleLines) + electricScroll + 1;
      if(newFirstLine + visibleLines >= getLineCount())
        newFirstLine = getLineCount() - visibleLines;
      if(newFirstLine < 0)
        newFirstLine = 0;
    }

    int x = _offsetToX(line,offset);
    int width = painter.getFontMetrics().charWidth('w');

    if(x < 0) {
      newHorizontalOffset = Math.min(0,horizontalOffset - x + width + 5);
    } else if(x + width >= painter.getWidth()) {
      newHorizontalOffset = horizontalOffset +
        (painter.getWidth() - x) - width - 5;
    }

    return setOrigin(newFirstLine,newHorizontalOffset);
  }

  /**
   * Converts a line index to a y co-ordinate.
   * @param line The line
   */
  public int lineToY(int line)
  {
    FontMetrics fm = painter.getFontMetrics();
    return (line - firstLine) * fm.getHeight()
      - (fm.getLeading() + fm.getMaxDescent());
  }

  /**
   * Converts a y co-ordinate to a line index.
   * @param y The y co-ordinate
   */
  public int yToLine(int y)
  {
    FontMetrics fm = painter.getFontMetrics();
    int height = fm.getHeight();
    return Math.max(0,Math.min(getLineCount() - 1,
                               y / height + firstLine));
  }

  /**
   * Converts an offset in a line into an x co-ordinate. This is a
   * slow version that can be used any time.
   * @param line The line
   * @param offset The offset, from the start of the line
   */
  public final int offsetToX(int line, int offset)
  {
    // don't use cached tokens
    painter.currentLineTokens = null;
    return _offsetToX(line,offset);
  }

  /**
   * Converts an offset in a line into an x co-ordinate. This is a
   * fast version that should only be used if no changes were made
   * to the text since the last repaint.
   * @param line The line
   * @param offset The offset, from the start of the line
   */
  public int _offsetToX(int line, int offset)
  {
    TokenMarker tokenMarker = getTokenMarker();

    /* Use painter's cached info for speed */
    FontMetrics fm = painter.getFontMetrics();

    getLineText(line,lineSegment);

    int segmentOffset = lineSegment.offset;
    int x = horizontalOffset;

    /* If syntax coloring is disabled, do simple translation */
    if(tokenMarker == null)
      {
        lineSegment.count = offset;
        return x + Utilities.getTabbedTextWidth(lineSegment,
                                                fm,x,painter,0);
      }
    /* If syntax coloring is enabled, we have to do this because
     * tokens can vary in width */
    else
      {
        Token tokens;
        if(painter.currentLineIndex == line
           && painter.currentLineTokens != null)
          tokens = painter.currentLineTokens;
        else
          {
            painter.currentLineIndex = line;
            tokens = painter.currentLineTokens
              = tokenMarker.markTokens(lineSegment,line);
          }

        //Toolkit toolkit = painter.getToolkit();
        Font defaultFont = painter.getFont();
        SyntaxStyle[] styles = painter.getStyles();

        for(;;)
          {
            byte id = tokens.id;
            if(id == Token.END)
              {
                return x;
              }

            if(id == Token.NULL)
              fm = painter.getFontMetrics();
            else
              fm = styles[id].getFontMetrics(defaultFont, this);

            int length = tokens.length;

            if(offset + segmentOffset < lineSegment.offset + length)
              {
                lineSegment.count = offset - (lineSegment.offset - segmentOffset);
                return x + Utilities.getTabbedTextWidth(
                                                        lineSegment,fm,x,painter,0);
              }
            else
              {
                lineSegment.count = length;
                x += Utilities.getTabbedTextWidth(
                                                  lineSegment,fm,x,painter,0);
                lineSegment.offset += length;
              }
            tokens = tokens.next;
          }
      }
  }

  /**
   * Converts an x co-ordinate to an offset within a line.
   * @param line The line
   * @param x The x co-ordinate
   */
  public int xToOffset(int line, int x)
  {
    TokenMarker tokenMarker = getTokenMarker();

    /* Use painter's cached info for speed */
    FontMetrics fm = painter.getFontMetrics();

    getLineText(line,lineSegment);

    char[] segmentArray = lineSegment.array;
    int segmentOffset = lineSegment.offset;
    int segmentCount = lineSegment.count;

    int width = horizontalOffset;

    if(tokenMarker == null)
      {
        for(int i = 0; i < segmentCount; i++)
          {
            char c = segmentArray[i + segmentOffset];
            int charWidth;
            if(c == '\t')
              charWidth = (int)painter.nextTabStop(width,i)
                - width;
            else
              charWidth = fm.charWidth(c);

            if(painter.isBlockCaretEnabled())
              {
                if(x - charWidth <= width)
                  return i;
              }
            else
              {
                if(x - charWidth / 2 <= width)
                  return i;
              }

            width += charWidth;
          }

        return segmentCount;
      }
    else
      {
        Token tokens;
        if(painter.currentLineIndex == line && painter
           .currentLineTokens != null)
          tokens = painter.currentLineTokens;
        else
          {
            painter.currentLineIndex = line;
            tokens = painter.currentLineTokens
              = tokenMarker.markTokens(lineSegment,line);
          }

        int offset = 0;
        //Toolkit toolkit = painter.getToolkit();
        Font defaultFont = painter.getFont();
        SyntaxStyle[] styles = painter.getStyles();

        for(;;)
          {
            byte id = tokens.id;
            if(id == Token.END)
              return offset;

            if(id == Token.NULL)
              fm = painter.getFontMetrics();
            else
              fm = styles[id].getFontMetrics(defaultFont, this);

            int length = tokens.length;

            for(int i = 0; i < length; i++)
              {
                char c = segmentArray[segmentOffset + offset + i];
                int charWidth;
                if(c == '\t')
                  charWidth = (int)painter.nextTabStop(width,offset + i)
                    - width;
                else
                  charWidth = fm.charWidth(c);

                if(painter.isBlockCaretEnabled())
                  {
                    if(x - charWidth <= width)
                      return offset + i;
                  }
                else
                  {
                    if(x - charWidth / 2 <= width)
                      return offset + i;
                  }

                width += charWidth;
              }

            offset += length;
            tokens = tokens.next;
          }
      }
  }

  /**
   * Converts a point to an offset, from the start of the text.
   * @param x The x co-ordinate of the point
   * @param y The y co-ordinate of the point
   */
  public int xyToOffset(int x, int y)
  {
    int line = yToLine(y);
    int start = getLineStartOffset(line);
    return start + xToOffset(line,x);
  }

  /**
   * Returns the document this text area is editing.
   */
  public final SyntaxDocument getDocument()
  {
    return document;
  }

  /**
   * Sets the document this text area is editing.
   * @param document The document
   */
  public void setDocument(SyntaxDocument document) {
    if (this.document == document)
      return;
    if (this.document != null)
      this.document.removeDocumentListener(documentHandler);
    this.document = document;

    document.addDocumentListener(documentHandler);

    select(0, 0);
    updateScrollBars();
    repaintEditor();
  }


  /**
   * Set document with a twist, includes the old caret
   * and scroll positions, added for p5. [fry]
   */
  public void setDocument(SyntaxDocument document,
                          int start, int stop, int scroll) {
    if (this.document == document)
      return;
    if (this.document != null)
      this.document.removeDocumentListener(documentHandler);
    this.document = document;

    document.addDocumentListener(documentHandler);

    select(start, stop);
    updateScrollBars();
    setScrollPosition(scroll);
    repaintEditor();
  }


  /**
   * Returns the document's token marker. Equivalent to calling
   * <code>getDocument().getTokenMarker()</code>.
   */
  public final TokenMarker getTokenMarker()
  {
    return document.getTokenMarker();
  }

  /**
   * Sets the document's token marker. Equivalent to caling
   * <code>getDocument().setTokenMarker()</code>.
   * @param tokenMarker The token marker
   */
  public final void setTokenMarker(TokenMarker tokenMarker)
  {
    document.setTokenMarker(tokenMarker);
  }

  /**
   * Returns the length of the document. Equivalent to calling
   * <code>getDocument().getLength()</code>.
   */
  public final int getDocumentLength()
  {
    return document.getLength();
  }

  /**
   * Returns the number of lines in the document.
   */
  public final int getLineCount()
  {
    if (document != null) {
      return document.getDefaultRootElement().getElementCount();
    } else {
      return 0;
    }
  }

  /**
   * Returns the line containing the specified offset.
   * @param offset The offset
   */
  public final int getLineOfOffset(int offset)
  {
    return document.getDefaultRootElement().getElementIndex(offset);
  }

  /**
   * Returns the start offset of the specified line.
   * @param line The line
   * @return The start offset of the specified line, or -1 if the line is
   * invalid
   */
  public int getLineStartOffset(int line)
  {
    Element lineElement = document.getDefaultRootElement()
      .getElement(line);
    if(lineElement == null)
      return -1;
    else
      return lineElement.getStartOffset();
  }

  /**
   * Returns the end offset of the specified line.
   * @param line The line
   * @return The end offset of the specified line, or -1 if the line is
   * invalid.
   */
  public int getLineStopOffset(int line)
  {
    Element lineElement = document.getDefaultRootElement()
      .getElement(line);
    if(lineElement == null)
      return -1;
    else
      return lineElement.getEndOffset();
  }

  /**
   * Returns the end offset of the specified line, but not past the end of the text
   * @param line The line
   * @return The end offset of the specified line, safe to use for a selection, or -1 if the line is
   * invalid.
   */
  public int getSafeLineStopOffset(int line)
  {
    return Math.min(getLineStopOffset(line),getDocumentLength());
  }

  /**
   * Returns the length of the specified line.
   * @param line The line
   */
  public int getLineLength(int line)
  {
    Element lineElement = document.getDefaultRootElement()
      .getElement(line);
    if(lineElement == null)
      return -1;
    else
      return lineElement.getEndOffset()
        - lineElement.getStartOffset() - 1;
  }

  /**
   * Returns the entire text of this text area.
   */
  public String getText()
  {
    try
      {
        return document.getText(0,document.getLength());
      }
    catch(BadLocationException bl)
      {
        bl.printStackTrace();
        return null;
      }
  }


  /**
   * Sets the entire text of this text area.
   */
  public void setText(String text)
  {
    try {
      document.beginCompoundEdit();
      document.remove(0,document.getLength());
      document.insertString(0,text,null);

    } catch (BadLocationException bl) {
      bl.printStackTrace();

    } finally {
      document.endCompoundEdit();
    }
  }


  /**
   * Returns the specified substring of the document.
   * @param start The start offset
   * @param len The length of the substring
   * @return The substring, or null if the offsets are invalid
   */
  public final String getText(int start, int len)
  {
    try
      {
        return document.getText(start,len);
      }
    catch(BadLocationException bl)
      {
        bl.printStackTrace();
        return null;
      }
  }

  /**
   * Copies the specified substring of the document into a segment.
   * If the offsets are invalid, the segment will contain a null string.
   * @param start The start offset
   * @param len The length of the substring
   * @param segment The segment
   */
  public final void getText(int start, int len, Segment segment)
  {
    try
      {
        document.getText(start,len,segment);
      }
    catch(BadLocationException bl)
      {
        bl.printStackTrace();
        segment.offset = segment.count = 0;
      }
  }

  /**
   * Returns the text on the specified line.
   * @param lineIndex The line
   * @return The text, or null if the line is invalid
   */
  public final String getLineText(int lineIndex)
  {
    int start = getLineStartOffset(lineIndex);
    return getText(start,getLineStopOffset(lineIndex) - start - 1);
  }

  /**
   * Copies the text on the specified line into a segment. If the line
   * is invalid, the segment will contain a null string.
   * @param lineIndex The line
   */
  public final void getLineText(int lineIndex, Segment segment)
  {
    int start = getLineStartOffset(lineIndex);
    getText(start,getLineStopOffset(lineIndex) - start - 1,segment);
  }

  /**
   * Returns the selection start offset.
   */
  public final int getSelectionStart()
  {
    return selectionStart;
  }

  /**
   * Returns the offset where the selection starts on the specified
   * line.
   */
  public int getSelectionStart(int line)
  {
    if(line == selectionStartLine)
      return selectionStart;
    else if(rectSelect)
      {
        Element map = document.getDefaultRootElement();
        int start = selectionStart - map.getElement(selectionStartLine)
          .getStartOffset();

        Element lineElement = map.getElement(line);
        int lineStart = lineElement.getStartOffset();
        int lineEnd = lineElement.getEndOffset() - 1;
        return Math.min(lineEnd,lineStart + start);
      }
    else
      return getLineStartOffset(line);
  }

  /**
   * Returns the selection start line.
   */
  public final int getSelectionStartLine()
  {
    return selectionStartLine;
  }

  /**
   * Sets the selection start. The new selection will be the new
   * selection start and the old selection end.
   * @param selectionStart The selection start
   * @see #select(int,int)
   */
  public final void setSelectionStart(int selectionStart)
  {
    select(selectionStart,selectionEnd);
  }

  /**
   * Returns the selection end offset.
   */
  public final int getSelectionStop()
  {
    return selectionEnd;
  }

  /**
   * Returns the offset where the selection ends on the specified
   * line.
   */
  public int getSelectionStop(int line)
  {
    if(line == selectionEndLine)
      return selectionEnd;
    else if(rectSelect)
      {
        Element map = document.getDefaultRootElement();
        int end = selectionEnd - map.getElement(selectionEndLine)
          .getStartOffset();

        Element lineElement = map.getElement(line);
        int lineStart = lineElement.getStartOffset();
        int lineEnd = lineElement.getEndOffset() - 1;
        return Math.min(lineEnd,lineStart + end);
      }
    else
      return getLineStopOffset(line) - 1;
  }

  /**
   * Returns the selection end line.
   */
  public final int getSelectionStopLine()
  {
    return selectionEndLine;
  }

  /**
   * Sets the selection end. The new selection will be the old
   * selection start and the bew selection end.
   * @param selectionEnd The selection end
   * @see #select(int,int)
   */
  public final void setSelectionEnd(int selectionEnd)
  {
    select(selectionStart,selectionEnd);
  }


  public final boolean isSelectionActive()
  {
    return(selectionStart != selectionEnd);
  }

  /**
   * Returns the caret position. This will either be the selection
   * start or the selection end, depending on which direction the
   * selection was made in.
   */
  public final int getCaretPosition()
  {
    return (biasLeft ? selectionStart : selectionEnd);
  }

  /**
   * Returns the caret line.
   */
  public final int getCaretLine()
  {
    return (biasLeft ? selectionStartLine : selectionEndLine);
  }

  /**
   * Returns the mark position. This will be the opposite selection
   * bound to the caret position.
   * @see #getCaretPosition()
   */
  public final int getMarkPosition()
  {
    return (biasLeft ? selectionEnd : selectionStart);
  }

  /**
   * Returns the mark line.
   */
  public final int getMarkLine()
  {
    return (biasLeft ? selectionEndLine : selectionStartLine);
  }

  /**
   * Sets the caret position. The new selection will consist of the
   * caret position only (hence no text will be selected)
   * @param caret The caret position
   * @see #select(int,int)
   */
  public final void setCaretPosition(int caret)
  {
    select(caret,caret);
  }

  /**
   * Selects all text in the document.
   */
  public final void selectAll()
  {
    select(0,getDocumentLength());
  }

  /**
   * Moves the mark to the caret position.
   */
  public final void selectNone()
  {
    select(getCaretPosition(),getCaretPosition());
  }

  /**
   * Selects from the start offset to the end offset. This is the
   * general selection method used by all other selecting methods.
   * The caret position will be start if start &lt; end, and end
   * if end &gt; start.
   * @param start The start offset
   * @param end The end offset
   */
  public void select(int start, int end)
  {
    int newStart, newEnd;
    boolean newBias;
    if(start <= end)
      {
        newStart = start;
        newEnd = end;
        newBias = false;
      }
    else
      {
        newStart = end;
        newEnd = start;
        newBias = true;
      }

    if(newStart < 0 || newEnd > getDocumentLength())
      {
        throw new IllegalArgumentException("Bounds out of"
                                           + " range: " + newStart + "," +
                                           newEnd + " [" + getDocumentLength() + "]");
      }

    // If the new position is the same as the old, we don't
    // do all this crap, however we still do the stuff at
    // the end (clearing magic position, scrolling)
    if(newStart != selectionStart || newEnd != selectionEnd
       || newBias != biasLeft)
      {
        int newStartLine = getLineOfOffset(newStart);
        int newEndLine = getLineOfOffset(newEnd);

        if(painter.isBracketHighlightEnabled())
          {
            if(bracketLine != -1)
              painter.invalidateLine(bracketLine);
            updateBracketHighlight(end);
            if(bracketLine != -1)
              painter.invalidateLine(bracketLine);
          }

        painter.invalidateLineRange(selectionStartLine,selectionEndLine);
        painter.invalidateLineRange(newStartLine,newEndLine);

        document.addUndoableEdit(new CaretUndo(selectionStart,selectionEnd));

        selectionStart = newStart;
        selectionEnd = newEnd;
        selectionStartLine = newStartLine;
        selectionEndLine = newEndLine;
        biasLeft = newBias;

        if (newStart != newEnd) {
          Clipboard unixclipboard = getToolkit().getSystemSelection();
          if (unixclipboard != null) {
            String selection = getSelectedText();
            if (selection != null) {
              unixclipboard.setContents(new StringSelection(selection), null);
            }
          }
        }

        fireCaretEvent();
      }

    // When the user is typing, etc, we don't want the caret
    // to blink
    blink = true;
    caretTimer.restart();

    // Disable rectangle select if selection start = selection end
    if(selectionStart == selectionEnd)
      rectSelect = false;

    // Clear the `magic' caret position used by up/down
    magicCaret = -1;

    scrollToCaret();

    // notify the line number feller
    if (editorLineStatus != null) {
      editorLineStatus.set(selectionStartLine, selectionEndLine);
      //System.out.println("why " + selectionStartLine + " " + selectionEndLine);
      //System.out.println(getLineOfOffset(start) + " " +
      //                 getLineOfOffset(end));
    }
  }
  
  private boolean isWordCharacter( char ch, String noWordSep )
  {
		return Character.isLetterOrDigit(ch) || ch=='_' || noWordSep.indexOf(ch) != -1;
  }

	protected void setNewSelectionWord( int line, int offset )
	{
		if (getLineLength(line) == 0) {
			newSelectionStart = getLineStartOffset(line);
			newSelectionEnd = newSelectionStart;
			return;
		}
		
		String noWordSep = (String)document.getProperty("noWordSep");
		if(noWordSep == null)
			noWordSep = "";

		String lineText = getLineText(line);

		int wordStart = 0;
		int wordEnd = lineText.length();

		char ch = lineText.charAt(Math.max(0,offset - 1));

		// special case for whitespace (fry 0122, bug #348)
		// this is really nasty.. turns out that double-clicking any non-letter
		// or digit char gets lumped together.. sooo, this quickly gets messy,
		// because really it needs to check whether the chars are of the same
		// type.. so a double space or double - might be grouped together,
		// but what about a +=1? do + and - get grouped but not the 1? blech,
		// coming back to this later. it's not a difficult fix, just a
		// time-consuming one to track down all the proper cases.
		/*
		if (ch == ' ') {
			//System.out.println("yeehaa");

			for(int i = offset - 1; i >= 0; i--) {
				if (lineText.charAt(i) == ' ') {
					wordStart = i;
				} else {
					break;
				}
			}
			for(int i = offset; i < lineText.length(); i++) {
				if (lineText.charAt(i) == ' ') {
					wordEnd = i + 1;
				} else {
					break;
				}
			}

		} else {
		*/

		// If the user clicked on a non-letter char,
		// we select the surrounding non-letters
		boolean selectNoLetter = !isWordCharacter(ch,noWordSep);

		for(int i = offset - 1; i >= 0; i--) {
			ch = lineText.charAt(i);
			if (selectNoLetter ^ !isWordCharacter(ch,noWordSep)) {
				wordStart = i + 1;
				break;
			}
		}

		for(int i = offset; i < lineText.length(); i++) {
			ch = lineText.charAt(i);
			if(selectNoLetter ^ !isWordCharacter(ch,noWordSep)) {
				wordEnd = i;
				break;
			}
		}
		//}
		int lineStart = getLineStartOffset(line);
		
		newSelectionStart = lineStart + wordStart;
		newSelectionEnd = lineStart + wordEnd;
	}


  /**
   * Returns the selected text, or null if no selection is active.
   */
  public final String getSelectedText()
  {
    if(selectionStart == selectionEnd)
      return null;

    if(rectSelect)
      {
        // Return each row of the selection on a new line

        Element map = document.getDefaultRootElement();

        int start = selectionStart - map.getElement(selectionStartLine)
          .getStartOffset();
        int end = selectionEnd - map.getElement(selectionEndLine)
          .getStartOffset();

        // Certain rectangles satisfy this condition...
        if(end < start)
          {
            int tmp = end;
            end = start;
            start = tmp;
          }

        StringBuffer buf = new StringBuffer();
        Segment seg = new Segment();

        for(int i = selectionStartLine; i <= selectionEndLine; i++)
          {
            Element lineElement = map.getElement(i);
            int lineStart = lineElement.getStartOffset();
            int lineEnd = lineElement.getEndOffset() - 1;
            int lineLen = lineEnd - lineStart;

            lineStart = Math.min(lineStart + start,lineEnd);
            lineLen = Math.min(end - start,lineEnd - lineStart);

            getText(lineStart,lineLen,seg);
            buf.append(seg.array,seg.offset,seg.count);

            if(i != selectionEndLine)
              buf.append('\n');
          }

        return buf.toString();
      }
    else
      {
        return getText(selectionStart,
                       selectionEnd - selectionStart);
      }
  }

  /**
   * Replaces the selection with the specified text.
   * @param selectedText The replacement text for the selection
   */
  public void setSelectedText(String selectedText)
  {
    if(!editable)
      {
        throw new InternalError("Text component"
                                + " read only");
      }

    document.beginCompoundEdit();

    try
      {
        if(rectSelect)
          {
            Element map = document.getDefaultRootElement();

            int start = selectionStart - map.getElement(selectionStartLine)
              .getStartOffset();
            int end = selectionEnd - map.getElement(selectionEndLine)
              .getStartOffset();

                                // Certain rectangles satisfy this condition...
            if(end < start)
              {
                int tmp = end;
                end = start;
                start = tmp;
              }

            int lastNewline = 0;
            int currNewline = 0;

            for(int i = selectionStartLine; i <= selectionEndLine; i++)
              {
                Element lineElement = map.getElement(i);
                int lineStart = lineElement.getStartOffset();
                int lineEnd = lineElement.getEndOffset() - 1;
                int rectStart = Math.min(lineEnd,lineStart + start);

                document.remove(rectStart,Math.min(lineEnd - rectStart,
                                                   end - start));

                if(selectedText == null)
                  continue;

                currNewline = selectedText.indexOf('\n',lastNewline);
                if(currNewline == -1)
                  currNewline = selectedText.length();

                document.insertString(rectStart,selectedText
                                      .substring(lastNewline,currNewline),null);

                lastNewline = Math.min(selectedText.length(),
                                       currNewline + 1);
              }

            if(selectedText != null &&
               currNewline != selectedText.length())
              {
                int offset = map.getElement(selectionEndLine)
                  .getEndOffset() - 1;
                document.insertString(offset,"\n",null);
                document.insertString(offset + 1,selectedText
                                      .substring(currNewline + 1),null);
              }
          }
        else
          {
            document.remove(selectionStart,
                            selectionEnd - selectionStart);
            if(selectedText != null)
              {
                document.insertString(selectionStart,
                                      selectedText,null);
              }
          }
      }
    catch(BadLocationException bl)
      {
        bl.printStackTrace();
        throw new InternalError("Cannot replace"
                                + " selection");
      }
    // No matter what happends... stops us from leaving document
    // in a bad state
    finally
      {
        document.endCompoundEdit();
      }

    setCaretPosition(selectionEnd);
  }

  /**
   * Returns true if this text area is editable, false otherwise.
   */
  public final boolean isEditable()
  {
    return editable;
  }

  /**
   * Sets if this component is editable.
   * @param editable True if this text area should be editable,
   * false otherwise
   */
  public final void setEditable(boolean editable)
  {
    this.editable = editable;
  }

  /**
   * Returns the right click popup menu.
   */
  public final JPopupMenu getRightClickPopup()
  {
    return popup;
  }

  /**
   * Sets the right click popup menu.
   * @param popup The popup
   */
  //public final void setRightClickPopup(EditPopupMenu popup)
  public final void setRightClickPopup(JPopupMenu popup)
  {
    this.popup = popup;
  }


  /**
   * Returns the `magic' caret position. This can be used to preserve
   * the column position when moving up and down lines.
   */
  public final int getMagicCaretPosition()
  {
    return magicCaret;
  }

  /**
   * Sets the `magic' caret position. This can be used to preserve
   * the column position when moving up and down lines.
   * @param magicCaret The magic caret position
   */
  public final void setMagicCaretPosition(int magicCaret)
  {
    this.magicCaret = magicCaret;
  }

  /**
   * Similar to <code>setSelectedText()</code>, but overstrikes the
   * appropriate number of characters if overwrite mode is enabled.
   * @param str The string
   * @see #setSelectedText(String)
   * @see #isOverwriteEnabled()
   */
  public void overwriteSetSelectedText(String str)
  {
    // Don't overstrike if there is a selection
    if(!overwrite || selectionStart != selectionEnd)
      {
        setSelectedText(str);
        return;
      }

    // Don't overstrike if we're on the end of
    // the line
    int caret = getCaretPosition();
    int caretLineEnd = getLineStopOffset(getCaretLine());
    if(caretLineEnd - caret <= str.length())
      {
        setSelectedText(str);
        return;
      }

    document.beginCompoundEdit();

    try
      {
        document.remove(caret,str.length());
        document.insertString(caret,str,null);
      }
    catch(BadLocationException bl)
      {
        bl.printStackTrace();
      }
    finally
      {
        document.endCompoundEdit();
      }
  }

  /**
   * Returns true if overwrite mode is enabled, false otherwise.
   */
  public final boolean isOverwriteEnabled()
  {
    return overwrite;
  }

  /**
   * Sets if overwrite mode should be enabled.
   * @param overwrite True if overwrite mode should be enabled,
   * false otherwise.
   */
  public final void setOverwriteEnabled(boolean overwrite)
  {
    this.overwrite = overwrite;
    painter.invalidateSelectedLines();
  }

  /**
   * Returns true if the selection is rectangular, false otherwise.
   */
  public final boolean isSelectionRectangular()
  {
    return rectSelect;
  }

  /**
   * Sets if the selection should be rectangular.
   * @param rectSelect True if the selection should be rectangular,
   * false otherwise.
   */
  public final void setSelectionRectangular(boolean rectSelect)
  {
    this.rectSelect = rectSelect;
    painter.invalidateSelectedLines();
  }

  /**
   * Returns the position of the highlighted bracket (the bracket
   * matching the one before the caret)
   */
  public final int getBracketPosition()
  {
    return bracketPosition;
  }

  /**
   * Returns the line of the highlighted bracket (the bracket
   * matching the one before the caret)
   */
  public final int getBracketLine()
  {
    return bracketLine;
  }

  /**
   * Adds a caret change listener to this text area.
   * @param listener The listener
   */
  public final void addCaretListener(CaretListener listener)
  {
    eventListenerList.add(CaretListener.class,listener);
  }

  /**
   * Removes a caret change listener from this text area.
   * @param listener The listener
   */
  public final void removeCaretListener(CaretListener listener)
  {
    eventListenerList.remove(CaretListener.class,listener);
  }

  /**
   * Deletes the selected text from the text area and places it
   * into the clipboard.
   */
  public void cut()
  {
    if(editable)
      {
        copy();
        setSelectedText("");
      }
  }

  /**
   * Places the selected text into the clipboard.
   */
  public void copy()
  {
    if(selectionStart != selectionEnd)
      {
        Clipboard clipboard = getToolkit().getSystemClipboard();

        String selection = getSelectedText();

        int repeatCount = inputHandler.getRepeatCount();
        StringBuffer buf = new StringBuffer();
        for(int i = 0; i < repeatCount; i++)
          buf.append(selection);

        Transferable t = new StringSelection(buf.toString());
        clipboard.setContents(t, null);

        Clipboard unixclipboard = getToolkit().getSystemSelection();
        if (unixclipboard != null) unixclipboard.setContents(t, null);
      }
  }

  /**
   * Inserts the clipboard contents into the text.
   */
  public void paste() {
    if (editable) {
      Clipboard clipboard = getToolkit().getSystemClipboard();
      try {
        // The MacOS MRJ doesn't convert \r to \n, so do it here
        String selection = ((String)clipboard.getContents(this).getTransferData(DataFlavor.stringFlavor)).replace('\r','\n');

        // particularly on macosx when pasting from safari,
        // replace unicode x00A0 (non-breaking space)
        // with just a plain space. [fry 030929]
        selection = selection.replace('\u00A0', ' ');

        int repeatCount = inputHandler.getRepeatCount();
        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < repeatCount; i++)
          buf.append(selection);
        selection = buf.toString();
        setSelectedText(selection);

      } catch(Exception e) {
        getToolkit().beep();
        System.err.println("Clipboard does not contain a string");
      }
    }
  }

  /**
   * Called by the AWT when this component is removed from it's parent.
   * This stops clears the currently focused component.
   */
  public void removeNotify()
  {
    super.removeNotify();
    if(focusedComponent == this)
      focusedComponent = null;
  }

  /**
   * Forwards key events directly to the input handler.
   * This is slightly faster than using a KeyListener
   * because some Swing overhead is avoided.
   */
  public EditorListener editorListener;

  /**
   * The component that tracks the current line number.
   */
  public EditorLineStatus editorLineStatus;


  public void processKeyEvent(KeyEvent evt) {
    // this had to be added in Processing 007X, because the menu key
    // events weren't making it up to the frame.
    super.processKeyEvent(evt);

    //System.out.println("jedittextarea: " + evt);
    //System.out.println();
    if (inputHandler == null) return;

    switch(evt.getID()) {
    case KeyEvent.KEY_TYPED:
      if ((editorListener == null) || !editorListener.keyTyped(evt)) {
        inputHandler.keyTyped(evt);
      }
      break;
    case KeyEvent.KEY_PRESSED:
      if ((editorListener == null) || !editorListener.keyPressed(evt)) {
        inputHandler.keyPressed(evt);
      }
      break;
    case KeyEvent.KEY_RELEASED:
      inputHandler.keyReleased(evt);
      break;
    }
  }

  // protected members
  protected static String LEFT = "left";
  protected static String CENTER = "center";
  protected static String RIGHT = "right";
  protected static String BOTTOM = "bottom";

  protected static JEditTextArea focusedComponent;
  protected static Timer caretTimer;

  protected TextAreaPainter painter;
  protected TextAreaLineNumbers editorLineNumbers;

  //protected EditPopupMenu popup;
  protected JPopupMenu popup;

  protected EventListenerList eventListenerList;
  protected MutableCaretEvent caretEvent;

  protected boolean caretBlinks;
  protected boolean caretVisible;
  protected boolean blink;

  protected boolean editable;

  protected int firstLine;
  protected int visibleLines;
  protected int electricScroll;

  protected int horizontalOffset;

  protected JScrollBar vertical;
  protected JScrollBar horizontal;
  protected boolean scrollBarsInitialized;

  protected InputHandler inputHandler;
  protected SyntaxDocument document;
  protected DocumentHandler documentHandler;

  protected Segment lineSegment;

  protected int selectionStart;
  protected int selectionStartLine;
  protected int selectionEnd;
  protected int selectionEndLine;
  protected boolean biasLeft;
  
  protected int newSelectionStart; // hack to get around lack of multiple returns in Java
  protected int newSelectionEnd;

  protected boolean selectWord;
  protected boolean selectLine;
  protected int selectionAncorStart;
  protected int selectionAncorEnd;

  protected int bracketPosition;
  protected int bracketLine;

  protected int magicCaret;
  protected boolean overwrite;
  protected boolean rectSelect;


  protected void fireCaretEvent()
  {
    Object[] listeners = eventListenerList.getListenerList();
    for(int i = listeners.length - 2; i >= 0; i--)
      {
        if(listeners[i] == CaretListener.class)
          {
            ((CaretListener)listeners[i+1]).caretUpdate(caretEvent);
          }
      }
  }

  protected void updateBracketHighlight(int newCaretPosition)
  {
    if(newCaretPosition == 0)
      {
        bracketPosition = bracketLine = -1;
        return;
      }

    try
      {
        int offset = TextUtilities.findMatchingBracket(
                                                       document,newCaretPosition - 1);
        if(offset != -1)
          {
            bracketLine = getLineOfOffset(offset);
            bracketPosition = offset - getLineStartOffset(bracketLine);
            return;
          }
      }
    catch(BadLocationException bl)
      {
        bl.printStackTrace();
      }

    bracketLine = bracketPosition = -1;
  }

  protected void documentChanged(DocumentEvent evt)
  {
    DocumentEvent.ElementChange ch =
      evt.getChange(document.getDefaultRootElement());

    int count;
    if(ch == null)
      count = 0;
    else
      count = ch.getChildrenAdded().length -
        ch.getChildrenRemoved().length;

    int line = getLineOfOffset(evt.getOffset());
    if(count == 0)
      {
        painter.invalidateLine(line);
      }
    // do magic stuff
    else if(line < firstLine)
      {
        setFirstLine(firstLine + count);
      }
    // end of magic stuff
    else
      {
        painter.invalidateLineRange(line,firstLine + visibleLines);
        updateScrollBars();
      }
  }

  class ScrollLayout implements LayoutManager
  {
    //final int LEFT_EXTRA = 5;

    public void addLayoutComponent(String name, Component comp)
    {
      if(name.equals(LEFT))
        left = comp;
      else if(name.equals(CENTER))
        center = comp;
      else if(name.equals(RIGHT))
        right = comp;
      else if(name.equals(BOTTOM))
        bottom = comp;
      else if(name.equals(LEFT_OF_SCROLLBAR))
        leftOfScrollBar.addElement(comp);
    }

    public void removeLayoutComponent(Component comp)
    {
      if(left == comp)
        left = null;
      if(center == comp)
        center = null;
      if(right == comp)
        right = null;
      if(bottom == comp)
        bottom = null;
      else
        leftOfScrollBar.removeElement(comp);
    }

    public Dimension preferredLayoutSize(Container parent)
    {
      Dimension dim = new Dimension();
      Insets insets = getInsets();
      dim.width = insets.left + insets.right;
      dim.height = insets.top + insets.bottom;

      Dimension centerPref = center.getPreferredSize();
      dim.width += centerPref.width;
      dim.height += centerPref.height;
      Dimension leftPref = left.getPreferredSize();
      dim.width += leftPref.width;
      Dimension rightPref = right.getPreferredSize();
      dim.width += rightPref.width;
      Dimension bottomPref = bottom.getPreferredSize();
      dim.height += bottomPref.height;

      return dim;
    }

    public Dimension minimumLayoutSize(Container parent)
    {
      Dimension dim = new Dimension();
      Insets insets = getInsets();
      dim.width = insets.left + insets.right;
      dim.height = insets.top + insets.bottom;

      Dimension centerPref = center.getMinimumSize();
      dim.width += centerPref.width;
      dim.height += centerPref.height;
      Dimension leftPref = left.getMinimumSize();
      dim.width += leftPref.width;
      Dimension rightPref = right.getMinimumSize();
      dim.width += rightPref.width;
      Dimension bottomPref = bottom.getMinimumSize();
      dim.height += bottomPref.height;

      dim.height += 5;

      return dim;
    }

    public void layoutContainer(Container parent)
    {
      Dimension size = parent.getSize();
      Insets insets = parent.getInsets();
      int itop = insets.top;
      int ileft = insets.left;
      int ibottom = insets.bottom;
      int iright = insets.right;

      int leftWidth = left.getSize().width;
      int rightWidth = right.getPreferredSize().width;
      int bottomHeight = bottom.getPreferredSize().height;
      int centerWidth = size.width - leftWidth - rightWidth - ileft - iright;
      int centerHeight = size.height - bottomHeight - itop - ibottom;

      left.setBounds(ileft,
                      itop,
                      leftWidth,
                      centerHeight);
      
      ileft += leftWidth;
      
      center.setBounds(ileft, // + LEFT_EXTRA,
                       itop,
                       centerWidth, // - LEFT_EXTRA,
                       centerHeight);

      right.setBounds(ileft + centerWidth,
                      itop,
                      rightWidth,
                      centerHeight);

      // Lay out all status components, in order
      Enumeration status = leftOfScrollBar.elements();
      while (status.hasMoreElements()) {
        Component comp = (Component)status.nextElement();
        Dimension dim = comp.getPreferredSize();
        comp.setBounds(ileft,
                       itop + centerHeight,
                       dim.width,
                       bottomHeight);
        ileft += dim.width;
      }

      bottom.setBounds(ileft,
                       itop + centerHeight,
                       size.width - rightWidth - ileft - iright,
                       bottomHeight);
    }

    // private members
    private Component left;
    private Component center;
    private Component right;
    private Component bottom;
    private Vector leftOfScrollBar = new Vector();
  }

  static class CaretBlinker implements ActionListener
  {
    public void actionPerformed(ActionEvent evt)
    {
      if(focusedComponent != null
         && focusedComponent.hasFocus())
        focusedComponent.blinkCaret();
    }
  }

  class MutableCaretEvent extends CaretEvent
  {
    MutableCaretEvent()
    {
      super(JEditTextArea.this);
    }

    public int getDot()
    {
      return getCaretPosition();
    }

    public int getMark()
    {
      return getMarkPosition();
    }
  }

/*
#ifdef JDK14
  class WheelHandler implements MouseWheelListener {

    public void mouseWheelMoved(MouseWheelEvent e) {
      if (!scrollBarsInitialized) return;

      int amt = e.getWheelRotation();
      //System.out.println(amt);
      vertical.setValue(vertical.getValue() + amt * wheelMultiplier);
    }
  }
#endif
*/

  class AdjustHandler implements AdjustmentListener
  {
    public void adjustmentValueChanged(final AdjustmentEvent evt)
    {
      if(!scrollBarsInitialized)
        return;

      // If this is not done, mousePressed events accumilate
      // and the result is that scrolling doesn't stop after
      // the mouse is released
      SwingUtilities.invokeLater(new Runnable() {
          public void run()
          {
            if(evt.getAdjustable() == vertical)
              setFirstLine(vertical.getValue());
            else
              setHorizontalOffset(-horizontal.getValue());
          }
        });
    }
  }

  class ComponentHandler extends ComponentAdapter
  {
    public void componentResized(ComponentEvent evt)
    {
      recalculateVisibleLines();
      scrollBarsInitialized = true;
    }
  }

  class DocumentHandler implements DocumentListener
  {
    public void insertUpdate(DocumentEvent evt)
    {
      documentChanged(evt);

      int offset = evt.getOffset();
      int length = evt.getLength();

      int newStart;
      int newEnd;

      if (selectionStart > offset ||
          (selectionStart == selectionEnd && selectionStart == offset))
        newStart = selectionStart + length;
      else
        newStart = selectionStart;

      if(selectionEnd >= offset)
        newEnd = selectionEnd + length;
      else
        newEnd = selectionEnd;

      select(newStart,newEnd);
    }

    public void removeUpdate(DocumentEvent evt)
    {
      documentChanged(evt);

      int offset = evt.getOffset();
      int length = evt.getLength();

      int newStart;
      int newEnd;

      if(selectionStart > offset)
        {
          if(selectionStart > offset + length)
            newStart = selectionStart - length;
          else
            newStart = offset;
        }
      else
        newStart = selectionStart;

      if(selectionEnd > offset)
        {
          if(selectionEnd > offset + length)
            newEnd = selectionEnd - length;
          else
            newEnd = offset;
        }
      else
        newEnd = selectionEnd;

      select(newStart,newEnd);
    }

    public void changedUpdate(DocumentEvent evt)
    {
    }
  }

  class DragHandler implements MouseMotionListener
  {
    public void mouseDragged(MouseEvent evt)
    {
      if (popup != null && popup.isVisible()) return;

      if ( !selectWord && !selectLine ) {
        setSelectionRectangular((evt.getModifiers()
                                 & InputEvent.CTRL_MASK) != 0);
        select(getMarkPosition(),xyToOffset(evt.getX(),evt.getY()));
      } else {
        int line = yToLine(evt.getY());
        if ( selectWord ) {
          setNewSelectionWord( line, xToOffset(line,evt.getX()) );
        } else {
          newSelectionStart = getLineStartOffset(line);
          newSelectionEnd = getSafeLineStopOffset(line);
        }
        if ( newSelectionStart < selectionAncorStart ) {
          select(newSelectionStart,selectionAncorEnd);
        } else if ( newSelectionEnd > selectionAncorEnd ) {
          select(selectionAncorStart,newSelectionEnd);
        } else {
          select(newSelectionStart,newSelectionEnd);
        }
      }
    }

    final Cursor normalCursor = new Cursor(Cursor.DEFAULT_CURSOR);
    final Cursor handCursor = new Cursor(Cursor.HAND_CURSOR);
    
    public void mouseMoved(MouseEvent evt) {
      int line = yToLine(evt.getY());
      int offset = xToOffset(line, evt.getX());
      boolean wantHandCursor = checkClickedURL(getLineText(line), offset) != null;
      JComponent src = (JComponent) evt.getSource();
      if (wantHandCursor)
        src.setCursor(handCursor);
      else
        src.setCursor(normalCursor);
    }
  }

  class FocusHandler implements FocusListener
  {
    public void focusGained(FocusEvent evt)
    {
      //System.out.println("JEditTextArea: focusGained");
      setCaretVisible(true);
      focusedComponent = JEditTextArea.this;
    }

    public void focusLost(FocusEvent evt)
    {
      //System.out.println("JEditTextArea: focusLost");
      setCaretVisible(false);
      focusedComponent = null;
    }
  }

  public String checkClickedURL(String line, int offset) {
    String[] parse = SyntaxUtilities.parseCommentUrls(line);
    if (parse==null)
      return null;
    int start = parse[0].length();
    int stop = start + parse[1].length();
    if (offset<start|| offset>stop)
      return null;
    return parse[1];
  }

  class MouseHandler extends MouseAdapter
  {
    public void mousePressed(MouseEvent evt)
    {
      requestFocus();

      // Focus events not fired sometimes?
      setCaretVisible(true);
      focusedComponent = JEditTextArea.this;

      // isPopupTrigger wasn't working for danh on windows
      boolean trigger = (evt.getModifiers() & InputEvent.BUTTON3_MASK) != 0;
      // but it's required for macosx, since control-click does
      // the same thing as a right-mouse click
      if (!trigger && evt.isPopupTrigger()) trigger = true;

      if (trigger && (popup != null)) {
        popup.show(painter,evt.getX(),evt.getY());
        return;
      }

      // on Linux, middle button pastes selected text
      if ((evt.getModifiers() & InputEvent.BUTTON2_MASK) != 0) {
        Clipboard unixclipboard = getToolkit().getSystemSelection();
        if (unixclipboard != null) {
          Transferable t = unixclipboard.getContents(null);
          if (t != null && t.isDataFlavorSupported(DataFlavor.stringFlavor)) {
            try {
              String s = (String)t.getTransferData(DataFlavor.stringFlavor);
              s = s.replace('\u00A0', ' ');
              if (editable) setSelectedText(s);
            } catch (Exception e) {
              System.err.println(e);
              e.printStackTrace();
            }
          }
          return;
        }
      }

      int line = yToLine(evt.getY());
      int offset = xToOffset(line,evt.getX());
      int dot = getLineStartOffset(line) + offset;

      selectLine = false;
      selectWord = false;

      switch(evt.getClickCount()) {

      case 1:
        doSingleClick(evt,line,offset,dot);
        break;

      case 2:
        // It uses the bracket matching stuff, so
        // it can throw a BLE
        try {
          doDoubleClick(evt,line,offset,dot);
        } catch(BadLocationException bl) {
          bl.printStackTrace();
        }
        break;

      case 3:
        doTripleClick(evt,line,offset,dot);
        break;
      }
    }


    private void doSingleClick(MouseEvent evt, int line,
                               int offset, int dot) {
      // Check for click on urls
      String clickedURL = checkClickedURL(getLineText(line), offset);
      if (clickedURL != null) {
        Base.openURL(clickedURL);
        return;
      }
      
      if ((evt.getModifiers() & InputEvent.SHIFT_MASK) != 0) {
        rectSelect = (evt.getModifiers() & InputEvent.CTRL_MASK) != 0;
        select(getMarkPosition(),dot);
      } else {
        setCaretPosition(dot);
      }
    }


    private void doDoubleClick(MouseEvent evt, int line,
                               int offset, int dot) throws BadLocationException
    {
      // Ignore empty lines
      if (getLineLength(line) == 0)
        return;

      try {
        int bracket = TextUtilities.findMatchingBracket(document,
                                                        Math.max(0,dot - 1));
        if (bracket != -1) {
          int mark = getMarkPosition();
          // Hack
          if (bracket > mark) {
            bracket++;
            mark--;
          }
          select(mark,bracket);
          return;
        }
      } catch(BadLocationException bl) {
        bl.printStackTrace();
      }

      setNewSelectionWord( line, offset );
      select(newSelectionStart,newSelectionEnd);
      selectWord = true;
      selectionAncorStart = selectionStart;
      selectionAncorEnd = selectionEnd;

      /*
        String lineText = getLineText(line);
        String noWordSep = (String)document.getProperty("noWordSep");
        int wordStart = TextUtilities.findWordStart(lineText,offset,noWordSep);
        int wordEnd = TextUtilities.findWordEnd(lineText,offset,noWordSep);

        int lineStart = getLineStartOffset(line);
        select(lineStart + wordStart,lineStart + wordEnd);
      */
    }

    private void doTripleClick(MouseEvent evt, int line,
                               int offset, int dot)
    {
      selectLine = true;
      select(getLineStartOffset(line),getSafeLineStopOffset(line));
      selectionAncorStart = selectionStart;
      selectionAncorEnd = selectionEnd;
    }
  }

  class CaretUndo extends AbstractUndoableEdit
  {
    private int start;
    private int end;

    CaretUndo(int start, int end)
    {
      this.start = start;
      this.end = end;
    }

    public boolean isSignificant()
    {
      return false;
    }

    public String getPresentationName()
    {
      return "caret move";
    }

    public void undo() throws CannotUndoException
    {
      super.undo();

      select(start,end);
    }

    public void redo() throws CannotRedoException
    {
      super.redo();

      select(start,end);
    }

    public boolean addEdit(UndoableEdit edit)
    {
      if(edit instanceof CaretUndo)
        {
          CaretUndo cedit = (CaretUndo)edit;
          start = cedit.start;
          end = cedit.end;
          cedit.die();

          return true;
        }
      else
        return false;
    }
  }

  static
  {
    caretTimer = new Timer(500,new CaretBlinker());
    caretTimer.setInitialDelay(500);
    caretTimer.start();
  }

  public void setDisplayLineNumbers(boolean displayLineNumbers) {
    editorLineNumbers.setDisplayLineNumbers(displayLineNumbers);
  }
}
