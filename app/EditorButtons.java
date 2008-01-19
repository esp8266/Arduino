/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-05 Ben Fry and Casey Reas
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

import java.awt.*;
import java.awt.event.*;
import java.awt.font.*;
import java.awt.geom.*;
import javax.swing.*;
import javax.swing.event.*;


/**
 * run/stop/etc buttons for the ide
 */
public class EditorButtons extends JComponent implements MouseInputListener {

  static final String title[] = {
    "Verify", "Stop", "New", "Open", "Save", "Upload to I/O Board", "Serial Monitor"
  };

  static final int BUTTON_COUNT  = title.length;
  /// height, width of the toolbar buttons
  static final int BUTTON_WIDTH  = 27;
  static final int BUTTON_HEIGHT = 32;
  /// amount of space between groups of buttons on the toolbar
  static final int BUTTON_GAP = 15; 

  static final int RUN      = 0;
  static final int STOP     = 1;

  static final int NEW      = 2;
  static final int OPEN     = 3;
  static final int SAVE     = 4;
  static final int EXPORT   = 5;
  static final int SERIAL   = 6;

  static final int INACTIVE = 0;
  static final int ROLLOVER = 1;
  static final int ACTIVE   = 2;

  Editor editor;
  //boolean disableRun;
  //Label status;

  Image offscreen;
  int width, height;

  Color bgcolor;

  Image buttons;
  Image inactive[];
  Image rollover[];
  Image active[];
  int currentRollover;
  //int currentSelection;

  JPopupMenu popup;

  int buttonCount;
  int state[] = new int[BUTTON_COUNT];
  Image stateImage[];
  int which[]; // mapping indices to implementation

  int x1[], x2[];
  int y1, y2;

  String status;
  Font statusFont;
  Color statusColor;
  //int statusY;


  public EditorButtons(Editor editor) {
    this.editor = editor;
    buttons = Base.getImage("buttons.gif", this);

    buttonCount = 0;
    which = new int[BUTTON_COUNT];

    //which[buttonCount++] = NOTHING;
    which[buttonCount++] = RUN;
    which[buttonCount++] = STOP;
    which[buttonCount++] = NEW;
    which[buttonCount++] = OPEN;
    which[buttonCount++] = SAVE;
    which[buttonCount++] = EXPORT;
    which[buttonCount++] = SERIAL;

    currentRollover = -1;

	// hardcoding new blue color scheme for consistency with images,
	// see EditorStatus.java for details.		
    //bgcolor = Preferences.getColor("buttons.bgcolor");
	bgcolor = new Color(0x04, 0x4F, 0x6F);

    status = "";

    statusFont = Preferences.getFont("buttons.status.font");
    statusColor = Preferences.getColor("buttons.status.color");

    //statusY = (BUTTON_COUNT + 1) * BUTTON_HEIGHT;

    addMouseListener(this);
    addMouseMotionListener(this);
  }


  public void paintComponent(Graphics screen) {
    if (inactive == null) {
      inactive = new Image[BUTTON_COUNT];
      rollover = new Image[BUTTON_COUNT];
      active   = new Image[BUTTON_COUNT];

      int IMAGE_SIZE = 33;

      for (int i = 0; i < BUTTON_COUNT; i++) {
        inactive[i] = createImage(BUTTON_WIDTH, BUTTON_HEIGHT);
        Graphics g = inactive[i].getGraphics();
        g.drawImage(buttons, -(i*IMAGE_SIZE) - 3, -2*IMAGE_SIZE, null);

        rollover[i] = createImage(BUTTON_WIDTH, BUTTON_HEIGHT);
        g = rollover[i].getGraphics();
        g.drawImage(buttons, -(i*IMAGE_SIZE) - 3, -1*IMAGE_SIZE, null);

        active[i] = createImage(BUTTON_WIDTH, BUTTON_HEIGHT);
        g = active[i].getGraphics();
        g.drawImage(buttons, -(i*IMAGE_SIZE) - 3, -0*IMAGE_SIZE, null);
      }

      state = new int[buttonCount];
      stateImage = new Image[buttonCount];
      for (int i = 0; i < buttonCount; i++) {
        setState(i, INACTIVE, false);
      }
    }
    Dimension size = getSize();
    if ((offscreen == null) ||
        (size.width != width) || (size.height != height)) {
      offscreen = createImage(size.width, size.height);
      width = size.width;
      height = size.height;

      y1 = 0;
      y2 = BUTTON_HEIGHT;

      x1 = new int[buttonCount];
      x2 = new int[buttonCount];

      int offsetX = 3;
      for (int i = 0; i < buttonCount; i++) {
        x1[i] = offsetX;
        if (i == 2) x1[i] += BUTTON_GAP;
        x2[i] = x1[i] + BUTTON_WIDTH;
        offsetX = x2[i];
      }
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
    Graphics2D g2 = (Graphics2D) g;
    FontRenderContext frc = g2.getFontRenderContext();
    float statusW = (float) statusFont.getStringBounds(status, frc).getWidth();
    float statusX = (getSize().width - statusW) / 2;
    g2.drawString(status, statusX, statusY);
    */
    //int statusY = (BUTTON_HEIGHT + statusFont.getAscent()) / 2;
    int statusY = (BUTTON_HEIGHT + g.getFontMetrics().getAscent()) / 2;
    g.drawString(status, buttonCount * BUTTON_WIDTH + 2 * BUTTON_GAP, statusY);

    screen.drawImage(offscreen, 0, 0, null);
  }


  public void mouseMoved(MouseEvent e) {
    // mouse events before paint();
    if (state == null) return;

    if (state[OPEN] != INACTIVE) {
      // avoid flicker, since there will probably be an update event
      setState(OPEN, INACTIVE, false);
    }
    //System.out.println(e);
    //mouseMove(e);
    handleMouse(e.getX(), e.getY());
  }


  public void mouseDragged(MouseEvent e) { }


  public void handleMouse(int x, int y) {
    if (currentRollover != -1) {
      if ((x > x1[currentRollover]) && (y > y1) &&
          (x < x2[currentRollover]) && (y < y2)) {
        return;

      } else {
        setState(currentRollover, INACTIVE, true);
        messageClear(title[currentRollover]);
        currentRollover = -1;
      }
    }
    int sel = findSelection(x, y);
    if (sel == -1) return;

    if (state[sel] != ACTIVE) {
      //if (!(disableRun && ((sel == RUN) || (sel == STOP)))) {
      setState(sel, ROLLOVER, true);
      currentRollover = sel;
      //}
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
    //if (inactive == null) return;
    state[slot] = newState;
    switch (newState) {
    case INACTIVE:
      stateImage[slot] = inactive[which[slot]];
      break;
    case ACTIVE:
      stateImage[slot] = active[which[slot]];
      break;
    case ROLLOVER:
      stateImage[slot] = rollover[which[slot]];
      message(title[which[slot]]);
      break;
    }
    if (updateAfter) repaint(); // changed for swing from update();
  }


  public void mouseEntered(MouseEvent e) {
    //mouseMove(e);
    handleMouse(e.getX(), e.getY());
  }


  public void mouseExited(MouseEvent e) {
    // if the popup menu for is visible, don't register this,
    // because the popup being set visible will fire a mouseExited() event
    if ((popup != null) && popup.isVisible()) return;

    if (state[OPEN] != INACTIVE) {
      setState(OPEN, INACTIVE, true);
    }
    status = "";
    handleMouse(e.getX(), e.getY());
  }

  int wasDown = -1;


  public void mousePressed(MouseEvent e) {
    final int x = e.getX();
    final int y = e.getY();

    int sel = findSelection(x, y);
    ///if (sel == -1) return false;
    if (sel == -1) return;
    currentRollover = -1;
    //int currentSelection = sel;
    //if (!(disableRun && ((sel == RUN) || (sel == STOP)))) {
    // moving the handling of this over into the editor
    //setState(sel, ACTIVE, true);
    //}

    //if (currentSelection == OPEN) {
    //switch (currentSelection) {
    switch (sel) {
    case RUN:
      //if (!disableRun) {
      editor.handleRun(e.isShiftDown());
      //}
      break;

    case STOP:
      //if (!disableRun) {
      //setState(RUN, INACTIVE, true);
      //setInactive();
      editor.handleStop();
      //}
      break;

    case OPEN:
      if (popup == null) {
        //popup = new JPopupMenu();
        popup = editor.sketchbook.getPopupMenu();
        // no events properly being fired, so nevermind
        /*
        popup.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
              System.out.println("action " + e);
            }
          });
        popup.addComponentListener(new ComponentAdapter() {
            public void componentHidden(ComponentEvent e) {
              System.out.println("hidden " + e);
            }
          });
        */
        add(popup);
      }
      //activate(OPEN);
      //SwingUtilities.invokeLater(new Runnable() {
      //public void run() {
      popup.show(EditorButtons.this, x, y);
      //}});
      break;

    case NEW:
      editor.handleNew(e.isShiftDown());
      break;

    case SAVE:
      editor.handleSave(false);
      break;

    case EXPORT:
      editor.handleExport();
      break;

    case SERIAL:
      editor.handleSerial();
      break;
    }    
  }


  public void mouseClicked(MouseEvent e) { }


  public void mouseReleased(MouseEvent e) {
    /*
    switch (currentSelection) {
      case RUN:
        if (!disableRun) {
          editor.handleRun(e.isShiftDown());
        }
        break;

      case STOP:
        if (!disableRun) {
          setState(RUN, INACTIVE, true);
          editor.handleStop();
        }
        break;

      case OPEN:   setState(OPEN, INACTIVE, true); break;
      case NEW:    editor.handleNew(e.isShiftDown()); break;
      case SAVE:   editor.handleSave(); break;
      case EXPORT: editor.handleExport(); break;
      case SERIAL: editor.handleSerial(); break;
    }
    currentSelection = -1;
    */
  }


  //public void disableRun(boolean what) {
  //disableRun = what;
  //}


  /*
  public void run() {
    if (inactive == null) return;
    clear();
    setState(RUN, ACTIVE, true);
  }
  */


  public void running(boolean yesno) {
    setState(RUN, yesno ? ACTIVE : INACTIVE, true);
  }


  /**
   * Set a particular button to be active.
   */
  public void activate(int what) {
    if (inactive == null) return;
    setState(what, ACTIVE, true);
  }


  //public void clearRun() {
  //if (inactive == null) return;
  //setState(RUN, INACTIVE, true);
  //}


  /**
   * Clear all the state of all buttons.
   */
  public void clear() { // (int button) {
    if (inactive == null) return;

    // skip the run button, do the others
    for (int i = 1; i < buttonCount; i++) {
      setState(i, INACTIVE, false);
    }
    repaint(); // changed for swing from update();
  }


  public void message(String msg) {
    //status.setText(msg + "  ");  // don't mind the hack
    status = msg;
  }


  public void messageClear(String msg) {
    //if (status.getText().equals(msg + "  ")) status.setText(Editor.EMPTY);
    if (status.equals(msg)) status = "";
  }


  public Dimension getPreferredSize() {
    return new Dimension((BUTTON_COUNT + 1)*BUTTON_WIDTH, BUTTON_HEIGHT);
    //return new Dimension(BUTTON_WIDTH, (BUTTON_COUNT + 1)*BUTTON_HEIGHT);
  }


  public Dimension getMinimumSize() {
    return getPreferredSize();
  }


  public Dimension getMaximumSize() {
    return new Dimension(3000, BUTTON_HEIGHT);
  }
}
