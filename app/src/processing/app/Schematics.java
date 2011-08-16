/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 Part of the Wiring project - http://wiring.org.co

 Copyright (c) 2009-11 Hernando Barragan

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

package processing.app;

import java.awt.*;
import java.awt.event.*;
import java.io.*;

import javax.swing.*;

public class Schematics extends JFrame {
  private static final long serialVersionUID = 3254345343658939796L;

  // prompt text stuff
  Image image;

  int diagramX = 0, diagramY = 0;

  int x1, y1, x2, y2;

  static final String PROMPT_CLOSE = "Close";

  static final String PROMPT_RESETVIEW = "Reset";

  /**
   * Standardized width for buttons. Mac OS X 10.3 wants 70 as its default,
   * Windows XP needs 66, and my Ubuntu machine needs 80+, so 80 seems proper.
   */
  static public int BUTTON_WIDTH = 80;

  /**
   * Standardized button height. Mac OS X 10.3 (Java 1.4) wants 29, presumably
   * because it now includes the blue border, where it didn't in Java 1.3.
   * Windows XP only wants 23 (not sure what default Linux would be). Because of
   * the disparity, on Mac OS X, it will be set inside a static block.
   */
  static public int BUTTON_HEIGHT = 24;

  // indents and spacing standards. these probably need to be modified
  // per platform as well, since macosx is so huge, windows is smaller,
  // and linux is all over the map

  static final int GUI_BIG = 13;

  static final int GUI_BETWEEN = 10;

  static final int GUI_SMALL = 6;

  // gui elements

  int wide, high;

  JLabel label;

  JLabel labelBack;

  JButton resetButton;

  JButton closeButton;

  // the calling editor, so updates can be applied

  Editor editor;

  public Schematics(File path) {
    super("Schematics");

    image = Toolkit.getDefaultToolkit().createImage(path.getAbsolutePath());

    // dialog = new JFrame("Schematics");
    setResizable(true);
    // setBackground(Color.white);
    // dialog.setContentPane(new JLabel(new ImageIcon(image)));

    Container pain = getContentPane();

    pain.setBackground(Color.white);
    pain.setLayout(null);

    int top = GUI_BIG;
    int left = GUI_BIG;
    int right = 0;

    // to override bug on OSX setting the JFrame background color
    labelBack = new JLabel();

    label = new JLabel(new ImageIcon(image));
    // System.out.println(label.getPreferredSize());
    Dimension d = label.getPreferredSize();
    label.addMouseMotionListener(new MouseMotionListener() {
      public void mouseDragged(MouseEvent e) {
        // Base.openFolder(Base.getSettingsFolder());
        x2 = e.getX();
        y2 = e.getY();
        diagramX += x2 - x1;
        diagramY += y2 - y1;
        Dimension d = label.getPreferredSize();
        Dimension d1 = getSize();
        diagramX = Math.max(d1.width - (d.width + GUI_BIG), diagramX);
        diagramX = Math.min(GUI_BIG, diagramX);
        diagramY = Math
            .max(
                d1.height
                    - (d.height + GUI_BIG + GUI_BETWEEN + BUTTON_HEIGHT
                        + GUI_BIG + GUI_BIG), diagramY);
        diagramY = Math.min(GUI_BIG, diagramY);
        label.setBounds(diagramX, diagramY, d.width, d.height);
        // System.out.println("dragging");
      }

      public void mouseMoved(MouseEvent e) {
      }
    });

    label.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        x1 = e.getX();
        y1 = e.getY();
        // System.out.println("pressed at "+x1+" "+y1);
      }

      public void mouseEntered(MouseEvent e) {
      }

      public void mouseExited(MouseEvent e) {
      }
    });

    label.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
    pain.add(label);
    label.setBounds(left, top, d.width, d.height);

    right = Math.max(right, left + d.width);
    top += d.height; // + GUI_SMALL;

    resetButton = new JButton(PROMPT_RESETVIEW);
    resetButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        diagramX = GUI_BIG;
        diagramY = GUI_BIG;
        Dimension d = label.getPreferredSize();
        label.setBounds(diagramX, diagramY, d.width, d.height);
      }
    });

    pain.add(resetButton);
    BUTTON_HEIGHT = resetButton.getPreferredSize().height;

    int h = right - (BUTTON_WIDTH + GUI_SMALL + BUTTON_WIDTH);
    resetButton.setBounds(h, top, BUTTON_WIDTH, BUTTON_HEIGHT);
    h += BUTTON_WIDTH + GUI_SMALL;

    // h = right - (BUTTON_WIDTH + GUI_SMALL + BUTTON_WIDTH);
    // h += BUTTON_WIDTH + GUI_SMALL;

    closeButton = new JButton(PROMPT_CLOSE);
    closeButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        disposeFrame();
      }
    });
    pain.add(closeButton);
    closeButton.setBounds(h, top, BUTTON_WIDTH, BUTTON_HEIGHT);

    top += BUTTON_HEIGHT + GUI_BETWEEN;

    // finish up

    wide = right + GUI_BIG;
    high = top + GUI_SMALL;

    // closing the window is same as hitting close button

    addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent e) {
        disposeFrame();
      }
    });

    ActionListener disposer = new ActionListener() {
      public void actionPerformed(ActionEvent actionEvent) {
        disposeFrame();
      }
    };

    Base.registerWindowCloseKeys(getRootPane(), disposer);
    if (!Base.isMacOS())
      Base.setIcon(this);

    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    setLocation((screen.width - wide) / 2, (screen.height - high) / 2);

    pack(); // get insets
    Insets insets = getInsets();
    setSize(Math.min(screen.width, wide + insets.left + insets.right), Math
        .min(screen.height, high + insets.top + insets.bottom));

    labelBack.setOpaque(true);
    labelBack.setBackground(Color.white);
    labelBack.setBounds(0, 0, screen.width, screen.height);
    pain.add(labelBack);

    getContentPane().setBackground(Color.white);
    // handle window closing commands for ctrl/cmd-W or hitting ESC.
    addComponentListener(new ComponentAdapter() {
      public void componentResized(ComponentEvent e) {
        Dimension d = getSize();
        int top = d.height - (BUTTON_HEIGHT + GUI_BETWEEN + GUI_BIG + GUI_BIG);
        int left = GUI_BIG;
        int right = Math.max(0, d.width - (left + GUI_BIG));
        int h = right - (BUTTON_WIDTH + GUI_SMALL + BUTTON_WIDTH);
        resetButton.setBounds(h, top, BUTTON_WIDTH, BUTTON_HEIGHT);
        h += BUTTON_WIDTH + GUI_SMALL;
        closeButton.setBounds(h, top, BUTTON_WIDTH, BUTTON_HEIGHT);
      }
    });

    pain.addKeyListener(new KeyAdapter() {
      public void keyPressed(KeyEvent e) {
        // System.out.println(e);
        KeyStroke wc = Base.WINDOW_CLOSE_KEYSTROKE;
        if ((e.getKeyCode() == KeyEvent.VK_ESCAPE)
            || (KeyStroke.getKeyStrokeForEvent(e).equals(wc))) {
          disposeFrame();
        }
      }
    });
  }

  public Dimension getPreferredSize() {
    return new Dimension(wide, high);
  }

  // .................................................................

  /**
   * Close the window after an OK or Cancel.
   */
  protected void disposeFrame() {
    editor.toolbar.deactivate(EditorToolbar.SCHEMATICS);
    dispose();
  }

  protected void applyFrame() {

  }

  protected void showFrame(Editor editor) {
    this.editor = editor;
    setVisible(true);
  }

}
