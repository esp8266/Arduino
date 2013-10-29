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
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Rectangle;

import javax.swing.JTextPane;
import javax.swing.border.MatteBorder;
import javax.swing.text.AttributeSet;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;

import processing.app.Preferences;

public class TextAreaLineNumbers extends JTextPane {
  
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
  
  public TextAreaLineNumbers(TextAreaDefaults defaults, int preferredHeight) {
    setBackground(defaults.bgcolor);
    setForeground(defaults.fgcolor);
    setOpaque(true);
    setEditable(false);
    setEnabled(false);
    setBorder(new MatteBorder(0, 0, 0, 1, new Color(240, 240, 240)));
    setTextFont(Preferences.getFont("editor.font")); 
    
    DIGIT_WIDTH = getFontMetrics(getFont()).stringWidth("0");
    MIN_WIDTH = DIGIT_WIDTH * MIN_NUM_DIGITS + PADDING_WIDTH;
    
    setPreferredSize(new Dimension(MIN_WIDTH, preferredHeight));
  }

  public void setTextFont(Font font) {
    setFont(font);
    SimpleAttributeSet attribs = new SimpleAttributeSet();  
    StyleConstants.setAlignment(attribs , StyleConstants.ALIGN_RIGHT);
    StyleConstants.setLeftIndent(attribs , 6);
    StyleConstants.setRightIndent(attribs , 6);
    StyleConstants.setFontSize(attribs, getFont().getSize());
    setParagraphAttributes(attribs,true);
  }
  
  public void updateLineNumbers(int startNum, int endNum) {
    if (currStartNum == startNum && currEndNum == endNum) {
      return;
    }
    currStartNum = startNum;
    currEndNum = endNum;
    
    StringBuilder sb = new StringBuilder();
    for (int i = startNum; i < endNum; i++) {
      sb.append(i).append("\n");
    }
    sb.append(endNum);
    setText(sb.toString());

    invalidate();
  }
  
  public void updateWidthForNumDigits(int numDigits) {
    if (currNumDigits == numDigits) {
      return;
    }
    currNumDigits = numDigits;
    
    updateBounds();
    invalidate();
  }

  public void setDisplayLineNumbers(boolean displayLineNumbers) {
    setVisible(displayLineNumbers);
    if (displayLineNumbers) {
      updateBounds();
    } else {
      setBounds(new Rectangle(0, getHeight()));
    }
    invalidate();
  }
  
  private void updateBounds() {
    setBounds(new Rectangle(Math.max(MIN_WIDTH, DIGIT_WIDTH * currNumDigits + PADDING_WIDTH), getHeight()));
  }
}
