/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2006-08 Ben Fry and Casey Reas

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

package processing.app.tools;

import processing.app.*;
import processing.core.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.text.*;


/**
 * Color selector tool for the Tools menu.
 * <p/>
 * Using the keyboard shortcuts, you can copy/paste the values for the
 * colors and paste them into your program. We didn't do any sort of
 * auto-insert of colorMode() or fill() or stroke() code cuz we couldn't
 * decide on a good way to do this.. your contributions welcome).
 */
public class ColorSelector implements Tool, DocumentListener {

  Editor editor;
  JFrame frame;

  int hue, saturation, brightness;  // range 360, 100, 100
  int red, green, blue;   // range 256, 256, 256

  ColorRange range;
  ColorSlider slider;

  JTextField hueField, saturationField, brightnessField;
  JTextField redField, greenField, blueField;

  JTextField hexField;

  JPanel colorPanel;


  public String getMenuTitle() {
    return "Color Selector";
  }
  
  
  public void init(Editor editor) {
    this.editor = editor;

    frame = new JFrame("Color Selector");
    frame.getContentPane().setLayout(new BorderLayout());

    Box box = Box.createHorizontalBox();
    box.setBorder(new EmptyBorder(12, 12, 12, 12));

    range = new ColorRange();
    range.init();
    Box rangeBox = new Box(BoxLayout.Y_AXIS);
    rangeBox.setAlignmentY(0);
    rangeBox.setBorder(BorderFactory.createBevelBorder(BevelBorder.LOWERED));
    rangeBox.add(range);
    box.add(rangeBox);
    box.add(Box.createHorizontalStrut(10));

    slider = new ColorSlider();
    slider.init();
    Box sliderBox = new Box(BoxLayout.Y_AXIS);
    sliderBox.setAlignmentY(0);
    sliderBox.setBorder(BorderFactory.createBevelBorder(BevelBorder.LOWERED));
    sliderBox.add(slider);
    box.add(sliderBox);
    box.add(Box.createHorizontalStrut(10));

    box.add(createColorFields());
    box.add(Box.createHorizontalStrut(10));

    frame.getContentPane().add(box, BorderLayout.CENTER);
    frame.pack();
    frame.setResizable(false);

    // these don't help either.. they fix the component size but
    // leave a gap where the component is located
    //range.setSize(256, 256);
    //slider.setSize(256, 20);

    Dimension size = frame.getSize();
    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    frame.setLocation((screen.width - size.width) / 2,
                      (screen.height - size.height) / 2);

    frame.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
    frame.addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
          frame.setVisible(false);
        }
      });
    Base.registerWindowCloseKeys(frame.getRootPane(), new ActionListener() {
        public void actionPerformed(ActionEvent actionEvent) {
          frame.setVisible(false);
        }
      });
    
    Base.setIcon(frame);

    hueField.getDocument().addDocumentListener(this);
    saturationField.getDocument().addDocumentListener(this);
    brightnessField.getDocument().addDocumentListener(this);
    redField.getDocument().addDocumentListener(this);
    greenField.getDocument().addDocumentListener(this);
    blueField.getDocument().addDocumentListener(this);
    hexField.getDocument().addDocumentListener(this);

    hexField.setText("FFFFFF");
  }


  public void run() {
    frame.setVisible(true);
    // You've got to be f--ing kidding me.. why did the following line 
    // get deprecated for the pile of s-- that follows it?
    //frame.setCursor(Cursor.CROSSHAIR_CURSOR);
    frame.setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
  }


  public void changedUpdate(DocumentEvent e) {
    //System.out.println("changed");
  }

  public void removeUpdate(DocumentEvent e) {
    //System.out.println("remove");
  }


  boolean updating;

  public void insertUpdate(DocumentEvent e) {
    if (updating) return;  // don't update forever recursively
    updating = true;

    Document doc = e.getDocument();
    if (doc == hueField.getDocument()) {
      hue = bounded(hue, hueField, 359);
      updateRGB();
      updateHex();

    } else if (doc == saturationField.getDocument()) {
      saturation = bounded(saturation, saturationField, 99);
      updateRGB();
      updateHex();

    } else if (doc == brightnessField.getDocument()) {
      brightness = bounded(brightness, brightnessField, 99);
      updateRGB();
      updateHex();

    } else if (doc == redField.getDocument()) {
      red = bounded(red, redField, 255);
      updateHSB();
      updateHex();

    } else if (doc == greenField.getDocument()) {
      green = bounded(green, greenField, 255);
      updateHSB();
      updateHex();

    } else if (doc == blueField.getDocument()) {
      blue = bounded(blue, blueField, 255);
      updateHSB();
      updateHex();

    } else if (doc == hexField.getDocument()) {
      String str = hexField.getText();
      while (str.length() < 6) {
        str += "0";
      }
      if (str.length() > 6) {
        str = str.substring(0, 6);
      }
      updateRGB2(Integer.parseInt(str, 16));
      updateHSB();
    }
    range.redraw();
    slider.redraw();
    colorPanel.repaint();
    updating = false;
  }


  /**
   * Set the RGB values based on the current HSB values.
   */
  protected void updateRGB() {
    int rgb = Color.HSBtoRGB((float)hue / 359f,
                             (float)saturation / 99f,
                             (float)brightness / 99f);
    updateRGB2(rgb);
  }


  /**
   * Set the RGB values based on a calculated ARGB int.
   * Used by both updateRGB() to set the color from the HSB values,
   * and by updateHex(), to unpack the hex colors and assign them.
   */
  protected void updateRGB2(int rgb) {
    red = (rgb >> 16) & 0xff;
    green = (rgb >> 8) & 0xff;
    blue = rgb & 0xff;

    redField.setText(String.valueOf(red));
    greenField.setText(String.valueOf(green));
    blueField.setText(String.valueOf(blue));
  }


  /**
   * Set the HSB values based on the current RGB values.
   */
  protected void updateHSB() {
    float hsb[] = new float[3];
    Color.RGBtoHSB(red, green, blue, hsb);

    hue = (int) (hsb[0] * 359.0f);
    saturation = (int) (hsb[1] * 99.0f);
    brightness = (int) (hsb[2] * 99.0f);

    hueField.setText(String.valueOf(hue));
    saturationField.setText(String.valueOf(saturation));
    brightnessField.setText(String.valueOf(brightness));
  }


  protected void updateHex() {
    hexField.setText(PApplet.hex(red, 2) +
                     PApplet.hex(green, 2) +
                     PApplet.hex(blue, 2));
  }


  /**
   * Get the bounded value for a specific range. If the value is outside
   * the max, you can't edit right away, so just act as if it's already
   * been bounded and return the bounded value, then fire an event to set
   * it to the value that was just returned.
   */
  protected int bounded(int current, final JTextField field, final int max) {
    String text = field.getText();
    if (text.length() == 0) {
      return 0;
    }
    try {
      int value = Integer.parseInt(text);
      if (value > max) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
              field.setText(String.valueOf(max));
            }
          });
        return max;
      }
      return value;

    } catch (NumberFormatException e) {
      return current;  // should not be reachable
    }
  }


  protected Container createColorFields() {
    Box box = Box.createVerticalBox();
    box.setAlignmentY(0);

    colorPanel = new JPanel() {
        public void paintComponent(Graphics g) {
          g.setColor(new Color(red, green, blue));
          Dimension size = getSize();
          g.fillRect(0, 0, size.width, size.height);
        }
      };
    colorPanel.setBorder(BorderFactory.createBevelBorder(BevelBorder.LOWERED));
    Dimension dim = new Dimension(60, 40);
    colorPanel.setMinimumSize(dim);
    //colorPanel.setMaximumSize(dim);
    //colorPanel.setPreferredSize(dim);
    box.add(colorPanel);
    box.add(Box.createVerticalStrut(10));

    Box row;

    row = Box.createHorizontalBox();
    row.add(createFixedLabel("H:"));
    row.add(hueField = new NumberField(4, false));
    row.add(new JLabel(" \u00B0"));  // degree symbol
    row.add(Box.createHorizontalGlue());
    box.add(row);
    box.add(Box.createVerticalStrut(5));

    row = Box.createHorizontalBox();
    row.add(createFixedLabel("S:"));
    row.add(saturationField = new NumberField(4, false));
    row.add(new JLabel(" %"));
    row.add(Box.createHorizontalGlue());
    box.add(row);
    box.add(Box.createVerticalStrut(5));

    row = Box.createHorizontalBox();
    row.add(createFixedLabel("B:"));
    row.add(brightnessField = new NumberField(4, false));
    row.add(new JLabel(" %"));
    row.add(Box.createHorizontalGlue());
    box.add(row);
    box.add(Box.createVerticalStrut(10));

    //

    row = Box.createHorizontalBox();
    row.add(createFixedLabel("R:"));
    row.add(redField = new NumberField(4, false));
    row.add(Box.createHorizontalGlue());
    box.add(row);
    box.add(Box.createVerticalStrut(5));

    row = Box.createHorizontalBox();
    row.add(createFixedLabel("G:"));
    row.add(greenField = new NumberField(4, false));
    row.add(Box.createHorizontalGlue());
    box.add(row);
    box.add(Box.createVerticalStrut(5));

    row = Box.createHorizontalBox();
    row.add(createFixedLabel("B:"));
    row.add(blueField = new NumberField(4, false));
    row.add(Box.createHorizontalGlue());
    box.add(row);
    box.add(Box.createVerticalStrut(10));

    //

    row = Box.createHorizontalBox();
    row.add(createFixedLabel("#"));
    row.add(hexField = new NumberField(5, true));
    row.add(Box.createHorizontalGlue());
    box.add(row);
    box.add(Box.createVerticalStrut(10));

    box.add(Box.createVerticalGlue());
    return box;
  }


  int labelH;

  /**
   * return a label of a fixed width
   */
  protected JLabel createFixedLabel(String title) {
    JLabel label = new JLabel(title);
    if (labelH == 0) {
      labelH = label.getPreferredSize().height;
    }
    Dimension dim = new Dimension(20, labelH);
    label.setPreferredSize(dim);
    label.setMinimumSize(dim);
    label.setMaximumSize(dim);
    return label;
  }


  public class ColorRange extends PApplet {

    static final int WIDE = 256;
    static final int HIGH = 256;

    int lastX, lastY;


    public void setup() {
      size(WIDE, HIGH, P3D);
      noLoop();

      colorMode(HSB, 360, 256, 256);
      noFill();
      rectMode(CENTER);
    }

    public void draw() {
      if ((g == null) || (g.pixels == null)) return;
      if ((width != WIDE) || (height < HIGH)) {
        //System.out.println("bad size " + width + " " + height);
        return;
      }

      int index = 0;
      for (int j = 0; j < 256; j++) {
        for (int i = 0; i < 256; i++) {
          g.pixels[index++] = color(hue, i, 255 - j);
        }
      }

      stroke((brightness > 50) ? 0 : 255);
      rect(lastX, lastY, 9, 9);
    }

    public void mousePressed() {
      updateMouse();
    }

    public void mouseDragged() {
      updateMouse();
    }

    public void updateMouse() {
      if ((mouseX >= 0) && (mouseX < 256) &&
          (mouseY >= 0) && (mouseY < 256)) {
        int nsaturation = (int) (100 * (mouseX / 255.0f));
        int nbrightness = 100 - ((int) (100 * (mouseY / 255.0f)));
        saturationField.setText(String.valueOf(nsaturation));
        brightnessField.setText(String.valueOf(nbrightness));

        lastX = mouseX;
        lastY = mouseY;
      }
    }

    public Dimension getPreferredSize() {
      return new Dimension(WIDE, HIGH);
    }

    public Dimension getMinimumSize() {
      return new Dimension(WIDE, HIGH);
    }

    public Dimension getMaximumSize() {
      return new Dimension(WIDE, HIGH);
    }
    
    public void keyPressed() {
      if (key == ESC) {
        ColorSelector.this.frame.setVisible(false);
        // don't quit out of processing
        // http://dev.processing.org/bugs/show_bug.cgi?id=1006
        key = 0;
  }
    }
  }


  public class ColorSlider extends PApplet {

    static final int WIDE = 20;
    static final int HIGH = 256;

    public void setup() {
      size(WIDE, HIGH, P3D);
      colorMode(HSB, 255, 100, 100);
      noLoop();
    }

    public void draw() {
      if ((g == null) || (g.pixels == null)) return;
      if ((width != WIDE) || (height < HIGH)) {
        //System.out.println("bad size " + width + " " + height);
        return;
      }

      int index = 0;
      int sel = 255 - (int) (255 * (hue / 359f));
      for (int j = 0; j < 256; j++) {
        int c = color(255 - j, 100, 100);
        if (j == sel) c = 0xFF000000;
        for (int i = 0; i < WIDE; i++) {
          g.pixels[index++] = c;
        }
      }
    }

    public void mousePressed() {
      updateMouse();
    }

    public void mouseDragged() {
      updateMouse();
    }

    public void updateMouse() {
      if ((mouseX >= 0) && (mouseX < 256) &&
          (mouseY >= 0) && (mouseY < 256)) {
        int nhue = 359 - (int) (359 * (mouseY / 255.0f));
        hueField.setText(String.valueOf(nhue));
      }
    }

    public Dimension getPreferredSize() {
      return new Dimension(WIDE, HIGH);
    }

    public Dimension getMinimumSize() {
      return new Dimension(WIDE, HIGH);
    }

    public Dimension getMaximumSize() {
      return new Dimension(WIDE, HIGH);
    }
    
    public void keyPressed() {
      if (key == ESC) {
        ColorSelector.this.frame.setVisible(false);
        // don't quit out of processing
        // http://dev.processing.org/bugs/show_bug.cgi?id=1006
        key = 0;
  }
    }
  }


  /**
   * Extension of JTextField that only allows numbers
   */
  class NumberField extends JTextField {

    public boolean allowHex;

    public NumberField(int cols, boolean allowHex) {
      super(cols);
      this.allowHex = allowHex;
    }

    protected Document createDefaultModel() {
      return new NumberDocument(this);
    }

    public Dimension getPreferredSize() {
      if (!allowHex) {
        return new Dimension(45, super.getPreferredSize().height);
      }
      return super.getPreferredSize();
    }

    public Dimension getMinimumSize() {
      return getPreferredSize();
    }

    public Dimension getMaximumSize() {
      return getPreferredSize();
    }
  }


  /**
   * Document model to go with JTextField that only allows numbers.
   */
  class NumberDocument extends PlainDocument {

    NumberField parentField;

    public NumberDocument(NumberField parentField) {
      this.parentField = parentField;
      //System.out.println("setting parent to " + parentSelector);
    }

    public void insertString(int offs, String str, AttributeSet a)
      throws BadLocationException {

      if (str == null) return;

      char chars[] = str.toCharArray();
      int charCount = 0;
      // remove any non-digit chars
      for (int i = 0; i < chars.length; i++) {
        boolean ok = Character.isDigit(chars[i]);
        if (parentField.allowHex) {
          if ((chars[i] >= 'A') && (chars[i] <= 'F')) ok = true;
          if ((chars[i] >= 'a') && (chars[i] <= 'f')) ok = true;
        }
        if (ok) {
          if (charCount != i) {  // shift if necessary
            chars[charCount] = chars[i];
          }
          charCount++;
        }
      }
      super.insertString(offs, new String(chars, 0, charCount), a);
      // can't call any sort of methods on the enclosing class here
      // seems to have something to do with how Document objects are set up
    }
  }
}
