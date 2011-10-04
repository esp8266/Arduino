/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-09 Ben Fry and Casey Reas
  Copyright (c) 2001-04 Massachusetts Institute of Technology

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
import static processing.app.I18n._;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;


/**
 * run/stop/etc buttons for the ide
 */
public class EditorToolbar extends JComponent implements MouseInputListener, KeyListener {

  /** Rollover titles for each button. */
  static final String title[] = {
    _("Verify"), _("Upload"), _("New"), _("Open"), _("Save"), _("Serial Monitor")
  };

  /** Titles for each button when the shift key is pressed. */ 
  static final String titleShift[] = {
    _("Verify"), _("Upload Using Programmer"), _("New Editor Window"), _("Open in Another Window"), _("Save"), _("Serial Monitor")
  };

  static final int BUTTON_COUNT  = title.length;
  /** Width of each toolbar button. */
  static final int BUTTON_WIDTH  = 27;
  /** Height of each toolbar button. */
  static final int BUTTON_HEIGHT = 32;
  /** The amount of space between groups of buttons on the toolbar. */
  static final int BUTTON_GAP    = 5;
  /** Size of the button image being chopped up. */
  static final int BUTTON_IMAGE_SIZE = 33;


  static final int RUN      = 0;
  static final int EXPORT   = 1;

  static final int NEW      = 2;
  static final int OPEN     = 3;
  static final int SAVE     = 4;

  static final int SERIAL   = 5;

  static final int INACTIVE = 0;
  static final int ROLLOVER = 1;
  static final int ACTIVE   = 2;

  Editor editor;

  Image offscreen;
  int width, height;

  Color bgcolor;

  static Image[][] buttonImages;
  int currentRollover;

  JPopupMenu popup;
  JMenu menu;

  int buttonCount;
  int[] state = new int[BUTTON_COUNT];
  Image[] stateImage;
  int which[]; // mapping indices to implementation

  int x1[], x2[];
  int y1, y2;

  Font statusFont;
  Color statusColor;

  boolean shiftPressed;

  public EditorToolbar(Editor editor, JMenu menu) {
    this.editor = editor;
    this.menu = menu;

    buttonCount = 0;
    which = new int[BUTTON_COUNT];

    //which[buttonCount++] = NOTHING;
    which[buttonCount++] = RUN;
    which[buttonCount++] = EXPORT;
    which[buttonCount++] = NEW;
    which[buttonCount++] = OPEN;
    which[buttonCount++] = SAVE;
    which[buttonCount++] = SERIAL;

    currentRollover = -1;

    bgcolor = Theme.getColor("buttons.bgcolor");
    statusFont = Theme.getFont("buttons.status.font");
    statusColor = Theme.getColor("buttons.status.color");

    addMouseListener(this);
    addMouseMotionListener(this);
  }

  protected void loadButtons() {
    Image allButtons = Base.getThemeImage("buttons.gif", this);
    buttonImages = new Image[BUTTON_COUNT][3];

      for (int i = 0; i < BUTTON_COUNT; i++) {
      for (int state = 0; state < 3; state++) {
        Image image = createImage(BUTTON_WIDTH, BUTTON_HEIGHT);
        Graphics g = image.getGraphics();
        g.drawImage(allButtons, 
                    -(i*BUTTON_IMAGE_SIZE) - 3, 
                    (-2 + state)*BUTTON_IMAGE_SIZE, null);
        buttonImages[i][state] = image;
      }
    }
  }

  @Override
  public void paintComponent(Graphics screen) {
    // this data is shared by all EditorToolbar instances
    if (buttonImages == null) {
      loadButtons();
    }

    // this happens once per instance of EditorToolbar
    if (stateImage == null) {
      state = new int[buttonCount];
      stateImage = new Image[buttonCount];
      for (int i = 0; i < buttonCount; i++) {
        setState(i, INACTIVE, false);
      }
      y1 = 0;
      y2 = BUTTON_HEIGHT;
      x1 = new int[buttonCount];
      x2 = new int[buttonCount];
    }

    Dimension size = getSize();
    if ((offscreen == null) ||
        (size.width != width) || (size.height != height)) {
      offscreen = createImage(size.width, size.height);
      width = size.width;
      height = size.height;

      int offsetX = 3;
      for (int i = 0; i < buttonCount; i++) {
        x1[i] = offsetX;
        if (i == 2 || i == 6) x1[i] += BUTTON_GAP;
        x2[i] = x1[i] + BUTTON_WIDTH;
        offsetX = x2[i];
      }
      
      // Serial button must be on the right
      x1[SERIAL] = width - BUTTON_WIDTH - 14;
      x2[SERIAL] = width - 14;
    }
    Graphics g = offscreen.getGraphics();
    g.setColor(bgcolor); //getBackground());
    g.fillRect(0, 0, width, height);

    for (int i = 0; i < buttonCount; i++) {
      g.drawImage(stateImage[i], x1[i], y1, null);
    }

    g.setColor(statusColor);
    g.setFont(statusFont);

    /*
    // if i ever find the guy who wrote the java2d api, i will hurt him.
     * 
     * whereas I love the Java2D API. --jdf. lol.
     * 
    Graphics2D g2 = (Graphics2D) g;
    FontRenderContext frc = g2.getFontRenderContext();
    float statusW = (float) statusFont.getStringBounds(status, frc).getWidth();
    float statusX = (getSize().width - statusW) / 2;
    g2.drawString(status, statusX, statusY);
    */
    if (currentRollover != -1) {
      int statusY = (BUTTON_HEIGHT + g.getFontMetrics().getAscent()) / 2;
      String status = shiftPressed ? titleShift[currentRollover] : title[currentRollover];
      if (currentRollover != SERIAL)
        g.drawString(status, (buttonCount-1) * BUTTON_WIDTH + 3 * BUTTON_GAP, statusY);
      else {
        int statusX = x1[SERIAL] - BUTTON_GAP;
        statusX -= g.getFontMetrics().stringWidth(status);
        g.drawString(status, statusX, statusY);
      }
    }

    screen.drawImage(offscreen, 0, 0, null);
    
    if (!isEnabled()) {
      screen.setColor(new Color(0,0,0,100));
      screen.fillRect(0, 0, getWidth(), getHeight());
  }
  }


  public void mouseMoved(MouseEvent e) {
    if (!isEnabled())
      return;
    
    // mouse events before paint();
    if (state == null) return;

    if (state[OPEN] != INACTIVE) {
      // avoid flicker, since there will probably be an update event
      setState(OPEN, INACTIVE, false);
    }
    handleMouse(e);
  }


  public void mouseDragged(MouseEvent e) { }


  public void handleMouse(MouseEvent e) {
    int x = e.getX();
    int y = e.getY();

    if (currentRollover != -1) {
      if ((x > x1[currentRollover]) && (y > y1) &&
          (x < x2[currentRollover]) && (y < y2)) {
        return;

      } else {
        setState(currentRollover, INACTIVE, true);
        currentRollover = -1;
      }
    }
    int sel = findSelection(x, y);
    if (sel == -1) return;

    if (state[sel] != ACTIVE) {
      setState(sel, ROLLOVER, true);
      currentRollover = sel;
    }
  }


  private int findSelection(int x, int y) {
    // if app loads slowly and cursor is near the buttons
    // when it comes up, the app may not have time to load
    if ((x1 == null) || (x2 == null)) return -1;

    for (int i = 0; i < buttonCount; i++) {
      if ((y > y1) && (x > x1[i]) &&
          (y < y2) && (x < x2[i])) {
        //System.out.println("sel is " + i);
        return i;
      }
    }
    return -1;
  }


  private void setState(int slot, int newState, boolean updateAfter) {
    state[slot] = newState;
    stateImage[slot] = buttonImages[which[slot]][newState];
    if (updateAfter) {
      repaint();
    }
  }


  public void mouseEntered(MouseEvent e) {
    handleMouse(e);
  }


  public void mouseExited(MouseEvent e) {
    // if the popup menu for is visible, don't register this,
    // because the popup being set visible will fire a mouseExited() event
    if ((popup != null) && popup.isVisible()) return;

    if (state[OPEN] != INACTIVE) {
      setState(OPEN, INACTIVE, true);
    }
    handleMouse(e);
  }

  int wasDown = -1;


  public void mousePressed(MouseEvent e) {
    
    // jdf
    if (!isEnabled())
      return;
    
    final int x = e.getX();
    final int y = e.getY();

    int sel = findSelection(x, y);
    ///if (sel == -1) return false;
    if (sel == -1) return;
    currentRollover = -1;

    switch (sel) {
    case RUN:
      editor.handleRun(false);
      break;

//    case STOP:
//      editor.handleStop();
//      break;
//
    case OPEN:
      popup = menu.getPopupMenu();
      popup.show(EditorToolbar.this, x, y);
      break;

    case NEW:
      if (shiftPressed) {
        editor.base.handleNew();
      } else {
      editor.base.handleNewReplace();
      }
      break;

    case SAVE:
      editor.handleSave(false);
      break;

    case EXPORT:
      editor.handleExport(e.isShiftDown());
      break;

    case SERIAL:
      editor.handleSerial();
      break;
    }
  }


  public void mouseClicked(MouseEvent e) { }


  public void mouseReleased(MouseEvent e) { }


  /**
   * Set a particular button to be active.
   */
  public void activate(int what) {
    if (buttonImages != null) {
    setState(what, ACTIVE, true);
  }
  }


  /**
   * Set a particular button to be active.
   */
  public void deactivate(int what) {
    if (buttonImages != null) {
    setState(what, INACTIVE, true);
  }
  }


  public Dimension getPreferredSize() {
    return getMinimumSize();
  }


  public Dimension getMinimumSize() {
    return new Dimension((BUTTON_COUNT + 1)*BUTTON_WIDTH, BUTTON_HEIGHT);
  }


  public Dimension getMaximumSize() {
    return new Dimension(3000, BUTTON_HEIGHT);
  }


  public void keyPressed(KeyEvent e) {
    if (e.getKeyCode() == KeyEvent.VK_SHIFT) {
      shiftPressed = true;
      repaint();
}
  }


  public void keyReleased(KeyEvent e) {
    if (e.getKeyCode() == KeyEvent.VK_SHIFT) {
      shiftPressed = false;
      repaint();
    }
  }


  public void keyTyped(KeyEvent e) { }
}
