/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 * TextAreaPainter.java - Paints the text area
 * Copyright (C) 1999 Slava Pestov
 *
 * You may use and modify this package for any purpose. Redistribution is
 * permitted, in both source and binary form, provided that this notice
 * remains intact in all source distributions of this package.
 */

package processing.app.syntax;

import processing.app.*;
import processing.app.syntax.im.CompositionTextPainter;

import javax.swing.ToolTipManager;
import javax.swing.text.*;
import javax.swing.JComponent;
import java.awt.event.MouseEvent;
import java.awt.*;
import java.awt.print.*;

/**
 * The text area repaint manager. It performs double buffering and paints
 * lines of text.
 * @author Slava Pestov
 */
public class TextAreaPainter extends JComponent
implements TabExpander, Printable
{
  /** True if inside printing, will handle disabling the highlight */
  boolean printing;
  /** Current setting for editor.antialias preference */
  boolean antialias;

  /** A specific painter composed by the InputMethod.*/
  protected CompositionTextPainter compositionTextPainter;

  /**
   * Creates a new repaint manager. This should be not be called
   * directly.
   */
  public TextAreaPainter(JEditTextArea textArea, TextAreaDefaults defaults)
  {
    this.textArea = textArea;

    setAutoscrolls(true);
    setDoubleBuffered(true);
    setOpaque(true);

    ToolTipManager.sharedInstance().registerComponent(this);

    currentLine = new Segment();
    currentLineIndex = -1;

    setCursor(Cursor.getPredefinedCursor(Cursor.TEXT_CURSOR));

    setFont(defaults.font);
    setForeground(defaults.fgcolor);
    setBackground(defaults.bgcolor);

    antialias = Preferences.getBoolean("editor.antialias");

    blockCaret = defaults.blockCaret;
    styles = defaults.styles;
    cols = defaults.cols;
    rows = defaults.rows;
    caretColor = defaults.caretColor;
    selectionColor = defaults.selectionColor;
    lineHighlightColor = defaults.lineHighlightColor;
    lineHighlight = defaults.lineHighlight;
    bracketHighlightColor = defaults.bracketHighlightColor;
    bracketHighlight = defaults.bracketHighlight;
    paintInvalid = defaults.paintInvalid;
    eolMarkerColor = defaults.eolMarkerColor;
    eolMarkers = defaults.eolMarkers;
  }

  /**
   * Get CompositionTextPainter. if CompositionTextPainter is not created, create it.
   */
   public CompositionTextPainter getCompositionTextpainter(){
     if(compositionTextPainter == null){
       compositionTextPainter = new CompositionTextPainter(textArea);
     }
     return compositionTextPainter;
   }

  /**
   * Returns if this component can be traversed by pressing the
   * Tab key. This returns false.
   */
//  public final boolean isManagingFocus()
//  {
//    return false;
//  }

  /**
   * Returns the syntax styles used to paint colorized text. Entry <i>n</i>
   * will be used to paint tokens with id = <i>n</i>.
   * @see processing.app.syntax.Token
   */
  public final SyntaxStyle[] getStyles()
  {
    return styles;
  }

  /**
   * Sets the syntax styles used to paint colorized text. Entry <i>n</i>
   * will be used to paint tokens with id = <i>n</i>.
   * @param styles The syntax styles
   * @see processing.app.syntax.Token
   */
  public final void setStyles(SyntaxStyle[] styles)
  {
    this.styles = styles;
    repaint();
  }

  /**
   * Returns the caret color.
   */
  public final Color getCaretColor()
  {
    return caretColor;
  }

  /**
   * Sets the caret color.
   * @param caretColor The caret color
   */
  public final void setCaretColor(Color caretColor)
  {
    this.caretColor = caretColor;
    invalidateSelectedLines();
  }

  /**
   * Returns the selection color.
   */
  public final Color getSelectionColor()
  {
    return selectionColor;
  }

  /**
   * Sets the selection color.
   * @param selectionColor The selection color
   */
  public final void setSelectionColor(Color selectionColor)
  {
    this.selectionColor = selectionColor;
    invalidateSelectedLines();
  }

  /**
   * Returns the line highlight color.
   */
  public final Color getLineHighlightColor()
  {
    return lineHighlightColor;
  }

  /**
   * Sets the line highlight color.
   * @param lineHighlightColor The line highlight color
   */
  public final void setLineHighlightColor(Color lineHighlightColor)
  {
    this.lineHighlightColor = lineHighlightColor;
    invalidateSelectedLines();
  }

  /**
   * Returns true if line highlight is enabled, false otherwise.
   */
  public final boolean isLineHighlightEnabled()
  {
    return lineHighlight;
  }

  /**
   * Enables or disables current line highlighting.
   * @param lineHighlight True if current line highlight
   * should be enabled, false otherwise
   */
  public final void setLineHighlightEnabled(boolean lineHighlight)
  {
    this.lineHighlight = lineHighlight;
    invalidateSelectedLines();
  }

  /**
   * Returns the bracket highlight color.
   */
  public final Color getBracketHighlightColor()
  {
    return bracketHighlightColor;
  }

  /**
   * Sets the bracket highlight color.
   * @param bracketHighlightColor The bracket highlight color
   */
  public final void setBracketHighlightColor(Color bracketHighlightColor)
  {
    this.bracketHighlightColor = bracketHighlightColor;
    invalidateLine(textArea.getBracketLine());
  }

  /**
   * Returns true if bracket highlighting is enabled, false otherwise.
   * When bracket highlighting is enabled, the bracket matching the
   * one before the caret (if any) is highlighted.
   */
  public final boolean isBracketHighlightEnabled()
  {
    return bracketHighlight;
  }

  /**
   * Enables or disables bracket highlighting.
   * When bracket highlighting is enabled, the bracket matching the
   * one before the caret (if any) is highlighted.
   * @param bracketHighlight True if bracket highlighting should be
   * enabled, false otherwise
   */
  public final void setBracketHighlightEnabled(boolean bracketHighlight)
  {
    this.bracketHighlight = bracketHighlight;
    invalidateLine(textArea.getBracketLine());
  }

  /**
   * Returns true if the caret should be drawn as a block, false otherwise.
   */
  public final boolean isBlockCaretEnabled()
  {
    return blockCaret;
  }

  /**
   * Sets if the caret should be drawn as a block, false otherwise.
   * @param blockCaret True if the caret should be drawn as a block,
   * false otherwise.
   */
  public final void setBlockCaretEnabled(boolean blockCaret)
  {
    this.blockCaret = blockCaret;
    invalidateSelectedLines();
  }

  /**
   * Returns the EOL marker color.
   */
  public final Color getEOLMarkerColor()
  {
    return eolMarkerColor;
  }

  /**
   * Sets the EOL marker color.
   * @param eolMarkerColor The EOL marker color
   */
  public final void setEOLMarkerColor(Color eolMarkerColor)
  {
    this.eolMarkerColor = eolMarkerColor;
    repaint();
  }

  /**
   * Returns true if EOL markers are drawn, false otherwise.
   */
  public final boolean getEOLMarkersPainted()
  {
    return eolMarkers;
  }

  /**
   * Sets if EOL markers are to be drawn.
   * @param eolMarkers True if EOL markers should be drawn, false otherwise
   */
  public final void setEOLMarkersPainted(boolean eolMarkers)
  {
    this.eolMarkers = eolMarkers;
    repaint();
  }

  /**
   * Returns true if invalid lines are painted as red tildes (~),
   * false otherwise.
   */
  public boolean getInvalidLinesPainted()
  {
    return paintInvalid;
  }

  /**
   * Sets if invalid lines are to be painted as red tildes.
   * @param paintInvalid True if invalid lines should be drawn, false otherwise
   */
  public void setInvalidLinesPainted(boolean paintInvalid)
  {
    this.paintInvalid = paintInvalid;
  }

  /**
   * Adds a custom highlight painter.
   * @param highlight The highlight
   */
  public void addCustomHighlight(Highlight highlight)
  {
    highlight.init(textArea,highlights);
    highlights = highlight;
  }

  /**
   * Highlight interface.
   */
  public interface Highlight
  {
    /**
     * Called after the highlight painter has been added.
     * @param textArea The text area
     * @param next The painter this one should delegate to
     */
    void init(JEditTextArea textArea, Highlight next);

    /**
     * This should paint the highlight and delgate to the
     * next highlight painter.
     * @param gfx The graphics context
     * @param line The line number
     * @param y The y co-ordinate of the line
     */
    void paintHighlight(Graphics gfx, int line, int y);

    /**
     * Returns the tool tip to display at the specified
     * location. If this highlighter doesn't know what to
     * display, it should delegate to the next highlight
     * painter.
     * @param evt The mouse event
     */
    String getToolTipText(MouseEvent evt);
  }

  /**
   * Returns the tool tip to display at the specified location.
   * @param evt The mouse event
   */
  public String getToolTipText(MouseEvent evt)
  {
    if(highlights != null)
      return highlights.getToolTipText(evt);
    else
      return null;
  }

  /**
   * Returns the font metrics used by this component.
   */
  public FontMetrics getFontMetrics()
  {
    return fm;
  }

  /**
   * Sets the font for this component. This is overridden to update the
   * cached font metrics and to recalculate which lines are visible.
   * @param font The font
   */
  public void setFont(Font font)
  {
    super.setFont(font);
    fm = super.getFontMetrics(font);
    textArea.recalculateVisibleLines();
  }

  /**
   * Repaints the text.
   * @param gfx The graphics context
   */
  public void paint(Graphics gfx)
  {
    Graphics2D g2 = (Graphics2D) gfx;
    g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
                        antialias ?
                        RenderingHints.VALUE_TEXT_ANTIALIAS_ON :
                        RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);

    tabSize = fm.charWidth(' ') * ((Integer)textArea.getDocument().getProperty(PlainDocument.tabSizeAttribute)).intValue();

    Rectangle clipRect = gfx.getClipBounds();

    gfx.setColor(getBackground());
    gfx.fillRect(clipRect.x,clipRect.y,clipRect.width,clipRect.height);

    // We don't use yToLine() here because that method doesn't
    // return lines past the end of the document
    int height = fm.getHeight();
    int firstLine = textArea.getFirstLine();
    int firstInvalid = firstLine + clipRect.y / height;
    // Because the clipRect's height is usually an even multiple
    // of the font height, we subtract 1 from it, otherwise one
    // too many lines will always be painted.
    int lastInvalid = firstLine + (clipRect.y + clipRect.height - 1) / height;

    try {
      TokenMarker tokenMarker = textArea.getDocument().getTokenMarker();
      int x = textArea.getHorizontalOffset();

      for (int line = firstInvalid; line <= lastInvalid; line++) {
        paintLine(gfx,tokenMarker,line,x);
      }

      if (tokenMarker != null && tokenMarker.isNextLineRequested()) {
        int h = clipRect.y + clipRect.height;
        repaint(0,h,getWidth(),getHeight() - h);
      }
    } catch (Exception e) {
      System.err.println("Error repainting line"
                         + " range {" + firstInvalid + ","
                         + lastInvalid + "}:");
      e.printStackTrace();
    }
  }


  public int print(Graphics g, PageFormat pageFormat, int pageIndex) {
    int lineHeight = fm.getHeight();
    int linesPerPage = (int) (pageFormat.getImageableHeight() / lineHeight);
    int lineCount = textArea.getLineCount();
    int lastPage = lineCount / linesPerPage;

    if (pageIndex > lastPage) {
      return NO_SUCH_PAGE;

    } else {
      Graphics2D g2d = (Graphics2D)g;
      TokenMarker tokenMarker = textArea.getDocument().getTokenMarker();
      int firstLine = pageIndex*linesPerPage;
      g2d.translate(Math.max(54, pageFormat.getImageableX()),
                    pageFormat.getImageableY() - firstLine*lineHeight);
      printing = true;
      for (int line = firstLine; line < firstLine + linesPerPage; line++) {
        paintLine(g2d, tokenMarker, line, 0);
      }
      printing = false;
      return PAGE_EXISTS;
    }
  }


  /**
   * Marks a line as needing a repaint.
   * @param line The line to invalidate
   */
  public final void invalidateLine(int line)
  {
    repaint(0,textArea.lineToY(line) + fm.getMaxDescent() + fm.getLeading(),
            getWidth(),fm.getHeight());
  }

  /**
   * Marks a range of lines as needing a repaint.
   * @param firstLine The first line to invalidate
   * @param lastLine The last line to invalidate
   */
  public final void invalidateLineRange(int firstLine, int lastLine)
  {
    repaint(0,textArea.lineToY(firstLine) +
            fm.getMaxDescent() + fm.getLeading(),
            getWidth(),(lastLine - firstLine + 1) * fm.getHeight());
  }

  /**
   * Repaints the lines containing the selection.
   */
  public final void invalidateSelectedLines()
  {
    invalidateLineRange(textArea.getSelectionStartLine(),
                        textArea.getSelectionStopLine());
  }

  /**
   * Implementation of TabExpander interface. Returns next tab stop after
   * a specified point.
   * @param x The x co-ordinate
   * @param tabOffset Ignored
   * @return The next tab stop after <i>x</i>
   */
  public float nextTabStop(float x, int tabOffset)
  {
    int offset = textArea.getHorizontalOffset();
    int ntabs = ((int)x - offset) / tabSize;
    return (ntabs + 1) * tabSize + offset;
  }

  /**
   * Returns the painter's preferred size.
   */
  public Dimension getPreferredSize()
  {
    Dimension dim = new Dimension();
    dim.width = fm.charWidth('w') * cols;
    dim.height = fm.getHeight() * rows;
    return dim;
  }


  /**
   * Returns the painter's minimum size.
   */
  public Dimension getMinimumSize()
  {
    Dimension dim = new Dimension();
    dim.width = fm.charWidth('w') * 10;
    dim.height = fm.getHeight() * 4;
    return dim;
  }

  // package-private members
  int currentLineIndex;
  Token currentLineTokens;
  Segment currentLine;

  /**
   * Accessor used by tools that want to hook in and grab the formatting.
   */
  public int getCurrentLineIndex() {
    return currentLineIndex;
  }

  /**
   * Accessor used by tools that want to hook in and grab the formatting.
   */
  public void setCurrentLineIndex(int what) {
    currentLineIndex = what;
  }

  /**
   * Accessor used by tools that want to hook in and grab the formatting.
   */
  public Token getCurrentLineTokens() {
    return currentLineTokens;
  }

  /**
   * Accessor used by tools that want to hook in and grab the formatting.
   */
  public void setCurrentLineTokens(Token tokens) {
    currentLineTokens = tokens;
  }

  /**
   * Accessor used by tools that want to hook in and grab the formatting.
   */
  public Segment getCurrentLine() {
    return currentLine;
  }


  // protected members
  protected JEditTextArea textArea;

  protected SyntaxStyle[] styles;
  protected Color caretColor;
  protected Color selectionColor;
  protected Color lineHighlightColor;
  protected Color bracketHighlightColor;
  protected Color eolMarkerColor;

  protected boolean blockCaret;
  protected boolean lineHighlight;
  protected boolean bracketHighlight;
  protected boolean paintInvalid;
  protected boolean eolMarkers;
  protected int cols;
  protected int rows;

  protected int tabSize;
  protected FontMetrics fm;

  protected Highlight highlights;

  protected void paintLine(Graphics gfx, TokenMarker tokenMarker,
                           int line, int x)
  {
    Font defaultFont = getFont();
    Color defaultColor = getForeground();

    currentLineIndex = line;
    int y = textArea.lineToY(line);

    if (line < 0 || line >= textArea.getLineCount()) {
      if (paintInvalid) {
        paintHighlight(gfx,line,y);
        styles[Token.INVALID].setGraphicsFlags(gfx,defaultFont);
        gfx.drawString("~",0,y + fm.getHeight());
      }
    } else if(tokenMarker == null) {
      paintPlainLine(gfx,line,defaultFont,defaultColor,x,y);
    } else {
      paintSyntaxLine(gfx,tokenMarker,line,defaultFont,
                      defaultColor,x,y);
    }
  }

  protected void paintPlainLine(Graphics gfx, int line, Font defaultFont,
                                Color defaultColor, int x, int y)
  {
    paintHighlight(gfx,line,y);
    textArea.getLineText(line,currentLine);

    gfx.setFont(defaultFont);
    gfx.setColor(defaultColor);

    y += fm.getHeight();
    x = Utilities.drawTabbedText(currentLine,x,y,gfx,this,0);
    /*
     * Draw characters via input method. 
     */
    if (compositionTextPainter != null && compositionTextPainter.hasComposedTextLayout()) {
      compositionTextPainter.draw(gfx, lineHighlightColor);
    }
    if (eolMarkers) {
      gfx.setColor(eolMarkerColor);
      gfx.drawString(".",x,y);
    }
  }

  protected void paintSyntaxLine(Graphics gfx, TokenMarker tokenMarker,
                                 int line, Font defaultFont,
                                 Color defaultColor, int x, int y)
  {
    textArea.getLineText(currentLineIndex,currentLine);
    currentLineTokens = tokenMarker.markTokens(currentLine,
                                               currentLineIndex);

    paintHighlight(gfx,line,y);

    gfx.setFont(defaultFont);
    gfx.setColor(defaultColor);
    y += fm.getHeight();
    x = SyntaxUtilities.paintSyntaxLine(currentLine,
                                        currentLineTokens,
                                        styles, this, gfx, x, y);
    /*
     * Draw characters via input method. 
     */
    if (compositionTextPainter != null && compositionTextPainter.hasComposedTextLayout()) {
      compositionTextPainter.draw(gfx, lineHighlightColor);
    }
    if (eolMarkers) {
      gfx.setColor(eolMarkerColor);
      gfx.drawString(".",x,y);
    }
  }

  protected void paintHighlight(Graphics gfx, int line, int y)
  {
    if (!printing) {
      if (line >= textArea.getSelectionStartLine()
          && line <= textArea.getSelectionStopLine())
        paintLineHighlight(gfx,line,y);

      if (highlights != null)
        highlights.paintHighlight(gfx,line,y);

      if (bracketHighlight && line == textArea.getBracketLine())
        paintBracketHighlight(gfx,line,y);

      if (line == textArea.getCaretLine())
        paintCaret(gfx,line,y);
    }
  }

  protected void paintLineHighlight(Graphics gfx, int line, int y)
  {
    int height = fm.getHeight();
    y += fm.getLeading() + fm.getMaxDescent();

    int selectionStart = textArea.getSelectionStart();
    int selectionEnd = textArea.getSelectionStop();

    if (selectionStart == selectionEnd) {
      if (lineHighlight) {
        gfx.setColor(lineHighlightColor);
        gfx.fillRect(0,y,getWidth(),height);
      }
    } else {
      gfx.setColor(selectionColor);

      int selectionStartLine = textArea.getSelectionStartLine();
      int selectionEndLine = textArea.getSelectionStopLine();
      int lineStart = textArea.getLineStartOffset(line);

      int x1, x2;
      if (textArea.isSelectionRectangular()) {
        int lineLen = textArea.getLineLength(line);
        x1 = textArea._offsetToX(line,Math.min(lineLen, selectionStart - textArea.getLineStartOffset(selectionStartLine)));
        x2 = textArea._offsetToX(line,Math.min(lineLen, selectionEnd - textArea.getLineStartOffset(selectionEndLine)));
        if (x1 == x2)
          x2++;
      } else if(selectionStartLine == selectionEndLine) {
        x1 = textArea._offsetToX(line, selectionStart - lineStart);
        x2 = textArea._offsetToX(line, selectionEnd - lineStart);
      } else if(line == selectionStartLine) {
        x1 = textArea._offsetToX(line, selectionStart - lineStart);
        x2 = getWidth();
      } else if(line == selectionEndLine) {
        //x1 = 0;
        // hack from stendahl to avoid doing weird side selection thing
        x1 = textArea._offsetToX(line, 0);
        // attempt at getting the gutter too, but doesn't seem to work
        //x1 = textArea._offsetToX(line, -textArea.getHorizontalOffset());
        x2 = textArea._offsetToX(line, selectionEnd - lineStart);
      } else {
        //x1 = 0;
        // hack from stendahl to avoid doing weird side selection thing
        x1 = textArea._offsetToX(line, 0);
        // attempt at getting the gutter too, but doesn't seem to work
        //x1 = textArea._offsetToX(line, -textArea.getHorizontalOffset());
        x2 = getWidth();
      }

      // "inlined" min/max()
      gfx.fillRect(x1 > x2 ? x2 : x1,y,x1 > x2 ?
                   (x1 - x2) : (x2 - x1),height);
    }

  }

  protected void paintBracketHighlight(Graphics gfx, int line, int y)
  {
    int position = textArea.getBracketPosition();
    if(position == -1)
      return;
    y += fm.getLeading() + fm.getMaxDescent();
    int x = textArea._offsetToX(line,position);
    gfx.setColor(bracketHighlightColor);
    // Hack!!! Since there is no fast way to get the character
    // from the bracket matching routine, we use ( since all
    // brackets probably have the same width anyway
    gfx.drawRect(x,y,fm.charWidth('(') - 1,
                 fm.getHeight() - 1);
  }

  protected void paintCaret(Graphics gfx, int line, int y)
  {
    //System.out.println("painting caret " + line + " " + y);
    if (textArea.isCaretVisible()) {
      //System.out.println("caret is visible");
      int offset =
        textArea.getCaretPosition() - textArea.getLineStartOffset(line);
      int caretX = textArea._offsetToX(line, offset);
      int caretWidth = ((blockCaret ||
                         textArea.isOverwriteEnabled()) ?
                        fm.charWidth('w') : 1);
      y += fm.getLeading() + fm.getMaxDescent();
      int height = fm.getHeight();

      //System.out.println("caretX, width = " + caretX + " " + caretWidth);

      gfx.setColor(caretColor);

      if (textArea.isOverwriteEnabled()) {
        gfx.fillRect(caretX,y + height - 1, caretWidth,1);

      } else {
        // some machines don't like the drawRect for the single
        // pixel caret.. this caused a lot of hell because on that
        // minority of machines, the caret wouldn't show up past
        // the first column. the fix is to use drawLine() in
        // those cases, as a workaround.
        if (caretWidth == 1) {
          gfx.drawLine(caretX, y, caretX, y + height - 1);
        } else {
          gfx.drawRect(caretX, y, caretWidth - 1, height - 1);
        }
        //gfx.drawRect(caretX, y, caretWidth, height - 1);
      }
    }
  }
}
