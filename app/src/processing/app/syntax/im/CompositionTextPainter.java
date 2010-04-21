package processing.app.syntax.im;

import java.awt.Color;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.font.TextLayout;

import processing.app.syntax.JEditTextArea;
import processing.app.syntax.TextAreaPainter;

/**
 * Paint texts from input method. Text via input method are transmitted by 
 * AttributedCaharacterIterator. This class helps the PDE's TextAreaPainter  
 * to handle AttributedCaharacterIterator.
 * 
 * For practical purposes, paint to textarea is done by TextLayout class.
 * Because TextLayout class is easy to draw composing texts. (For example, 
 * draw underline composing texts, focus when select from candidates text.)
 *  
 * @author Takashi Maekawa (takachin@generative.info)
 */
public class CompositionTextPainter {
  private TextLayout composedTextLayout;
  private int composedBeginCaretPosition = 0;
  private JEditTextArea textArea;

  /**
   * Constructor for painter.
   * @param textarea textarea used by PDE.
   */
  public CompositionTextPainter(JEditTextArea textArea) {
    this.textArea = textArea;
    composedTextLayout = null;
  }

  /**
   * Check the painter has TextLayout.
   * If a user input via InputMethod, this result will return true.
   * @param textarea textarea used by PDE.
   */
  public boolean hasComposedTextLayout() {
    return (composedTextLayout != null);
  }
  
  /**
   * Set TextLayout to the painter.
   * TextLayout will be created and set by CompositionTextManager.
   * 
   * @see CompositionTextManager
   * @param textarea textarea used by PDE.
   */
  public void setComposedTextLayout(TextLayout composedTextLayout, int composedStartCaretPosition) {
    this.composedTextLayout = composedTextLayout;
    this.composedBeginCaretPosition = composedStartCaretPosition;
  }

  /**
   * Invalidate this TextLayout to set null.
   * If a user end input via InputMethod, this method will called from CompositionTextManager.endCompositionText
   */
  public void invalidateComposedTextLayout(int composedEndCaretPosition) {
    this.composedTextLayout = null;
    this.composedBeginCaretPosition = composedEndCaretPosition;
    //this.composedBeginCaretPosition = textArea.getCaretPosition();
  }
  
  /**
   * Draw text via input method with composed text information.
   * This method can draw texts with some underlines to illustrate converting characters. 
   * 
   * This method is workaround for TextAreaPainter.
   * Because, TextAreaPainter can't treat AttributedCharacterIterator directly.
   * AttributedCharacterIterator has very important information when composing text.
   * It has a map where are converted characters and committed characters.
   * Ideally, changing TextAreaPainter method can treat AttributedCharacterIterator is better. But it's very tough!!
   * So I choose to write some code as a workaround.
   * 
   * This draw method is proceeded with the following steps.
   * 1. Original TextAreaPainter draws characters. 
   * 2. This refillComposedArea method erase previous paint characters by textarea's background color.
   *    The refill area is only square that width and height defined by characters with input method.
   * 3. CompositionTextPainter.draw method paints composed text. It was actually drawn by TextLayout.
   * 
   * @param gfx set TextAreaPainter's Graphics object.
   * @param fillBackGroundColor set textarea's background.
   */
  public void draw(Graphics gfx, Color fillBackGroundColor) {
    assert(composedTextLayout != null);
    Point composedLoc = getCaretLocation();
    refillComposedArea(fillBackGroundColor, composedLoc.x, composedLoc.y);
    composedTextLayout.draw((Graphics2D) gfx, composedLoc.x, composedLoc.y);
  }

  /**
   * Fill color to erase characters drawn by original TextAreaPainter. 
   *  
   * @param fillColor fill color to erase characters drawn by original TextAreaPainter method.
   * @param x x-coordinate where to fill.
   * @param y y-coordinate where to fill.
   */
  private void refillComposedArea(Color fillColor, int x, int y) {
    Graphics gfx = textArea.getPainter().getGraphics();
    gfx.setColor(fillColor);
    FontMetrics fm = textArea.getPainter().getFontMetrics();
    int newY = y - (fm.getHeight() - CompositionTextManager.COMPOSING_UNDERBAR_HEIGHT);
    int paintHeight = fm.getHeight();
    int paintWidth = (int) composedTextLayout.getBounds().getWidth();
    gfx.fillRect(x, newY, paintWidth, paintHeight);
  }

  private Point getCaretLocation() {
    Point loc = new Point();
    TextAreaPainter painter = textArea.getPainter();
    FontMetrics fm = painter.getFontMetrics();
    int offsetY = fm.getHeight() - CompositionTextManager.COMPOSING_UNDERBAR_HEIGHT;
    int lineIndex = textArea.getCaretLine();
    loc.y = lineIndex * fm.getHeight() + offsetY;
    int offsetX = composedBeginCaretPosition - textArea.getLineStartOffset(lineIndex);
    loc.x = textArea.offsetToX(lineIndex, offsetX);
    return loc;
  }
}
