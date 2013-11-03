/*
 * TextAreaLineNumbers.java - Show line numbers for the open file in the editor
 * Copyright (C) 2013 Cayci Gorlitsky
 *
 * You may use and modify this package for any purpose. Redistribution is
 * permitted, in both source and binary form, provided that this notice
 * remains intact in all source distributions of this package.
 */

package processing.app.syntax;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Rectangle;

import javax.swing.border.MatteBorder;

public class TextAreaLineNumbers extends TextAreaPainter {
  
  private final int LEFT_INDENT = 6;
  private final int RIGHT_INDENT = 6;
  private final int RIGHT_BORDER_WIDTH = 1;
  private final int PADDING_WIDTH = LEFT_INDENT + RIGHT_INDENT + RIGHT_BORDER_WIDTH;
  
  private final int MIN_WIDTH;
  private final int DIGIT_WIDTH;
  private final int MIN_NUM_DIGITS = 2;
  
  private int currStartNum = 0;
  private int currEndNum = 0;
  private int currNumDigits = MIN_NUM_DIGITS;
  
  
  
  public TextAreaLineNumbers(JEditTextArea textArea, TextAreaDefaults defaults) {
    super(textArea, defaults);
    DIGIT_WIDTH = getFontMetrics(getFont()).stringWidth("0");
    MIN_WIDTH = DIGIT_WIDTH * MIN_NUM_DIGITS + PADDING_WIDTH;
    setEnabled(false);
    setBorder(new MatteBorder(0, 0, 0, RIGHT_BORDER_WIDTH, new Color(240, 240, 240)));
  }
  
  public void updateLineNumbers(int startNum, int endNum) {
    if (currStartNum == startNum && currEndNum == endNum) {
      return;
    }
    currStartNum = startNum;
    currEndNum = endNum;
    
    invalidate();
    repaint();
  }
  
  @Override
  public void paint(Graphics gfx) {
    super.paint(gfx);
    getBorder().paintBorder(this, gfx, 0, 0, getSize().width, getSize().height);
  }

  @Override
  protected void paintLine(Graphics gfx, TokenMarker tokenMarker,
                           int line, int x)
  {
    currentLineIndex = line;
    gfx.setFont(getFont());
    gfx.setColor(Color.GRAY);
    int y = textArea.lineToY(line);
    int startX = getBounds().x + getBounds().width;
    if (line >= 0 && line < textArea.getLineCount()) {
      String lineNumberString = String.valueOf(line+1);
      int lineStartX = startX - RIGHT_BORDER_WIDTH - RIGHT_INDENT - fm.stringWidth(lineNumberString);
      gfx.drawString(lineNumberString,lineStartX,y + fm.getHeight());
    }
  }
  
  public void updateWidthForNumDigits(int numDigits) {
    if (currNumDigits == numDigits) {
      return;
    }
    currNumDigits = numDigits;
    
    if (isVisible()) {
      updateBounds();
      invalidate();
      repaint();
    }
  }

  public void setDisplayLineNumbers(boolean displayLineNumbers) {
    setVisible(displayLineNumbers);
    if (displayLineNumbers) {
      updateBounds();
    } else {
      setBounds(new Rectangle(0, getHeight()));
    }
    invalidate();
    repaint();
  }
  
  private void updateBounds() {
    if (isVisible()) {
      setBounds(new Rectangle(Math.max(MIN_WIDTH, DIGIT_WIDTH * currNumDigits + PADDING_WIDTH), getHeight()));
      textArea.validate();
    }
  }
}
