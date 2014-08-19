/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-08 Ben Fry and Casey Reas
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
import processing.app.helpers.OSUtils;
import processing.app.tools.MenuScroller;
import static processing.app.I18n._;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;


/**
 * Sketch tabs at the top of the editor window.
 */
@SuppressWarnings("serial")
public class EditorHeader extends JComponent {
  static Color backgroundColor;
  static Color textColor[] = new Color[2];

  Editor editor;

  int tabLeft[];
  int tabRight[];

  Font font;
  FontMetrics metrics;
  int fontAscent;

  JMenu menu;
  JPopupMenu popup;

  int menuLeft;
  int menuRight;

  //

  static final String STATUS[] = { "unsel", "sel" };
  static final int UNSELECTED = 0;
  static final int SELECTED = 1;

  static final String WHERE[] = { "left", "mid", "right", "menu" };
  static final int LEFT = 0;
  static final int MIDDLE = 1;
  static final int RIGHT = 2;
  static final int MENU = 3;

  static final int PIECE_WIDTH = 4;

  static Image[][] pieces;

  //

  Image offscreen;
  int sizeW, sizeH;
  int imageW, imageH;


  public EditorHeader(Editor eddie) {
    this.editor = eddie; // weird name for listener

    if (pieces == null) {
      pieces = new Image[STATUS.length][WHERE.length];
      for (int i = 0; i < STATUS.length; i++) {
        for (int j = 0; j < WHERE.length; j++) {
          String path = "tab-" + STATUS[i] + "-" + WHERE[j] + ".gif";
          pieces[i][j] = Base.getThemeImage(path, this);
        }
      }
    }

    if (backgroundColor == null) {
      backgroundColor =
        Theme.getColor("header.bgcolor");
      textColor[SELECTED] =
        Theme.getColor("header.text.selected.color");
      textColor[UNSELECTED] =
        Theme.getColor("header.text.unselected.color");
    }

    addMouseListener(new MouseAdapter() {
        public void mousePressed(MouseEvent e) {
          int x = e.getX();
          int y = e.getY();

          if ((x > menuLeft) && (x < menuRight)) {
            popup.show(EditorHeader.this, x, y);

          } else {
            Sketch sketch = editor.getSketch();
            for (int i = 0; i < sketch.getCodeCount(); i++) {
              if ((x > tabLeft[i]) && (x < tabRight[i])) {
                sketch.setCurrentCode(i);
                repaint();
              }
            }
          }
        }
      });
  }


  public void paintComponent(Graphics screen) {
    if (screen == null) return;

    Sketch sketch = editor.getSketch();
    if (sketch == null) return;  // ??

    Dimension size = getSize();
    if ((size.width != sizeW) || (size.height != sizeH)) {
      // component has been resized

      if ((size.width > imageW) || (size.height > imageH)) {
        // nix the image and recreate, it's too small
        offscreen = null;

      } else {
        // who cares, just resize
        sizeW = size.width;
        sizeH = size.height;
      }
    }

    if (offscreen == null) {
      sizeW = size.width;
      sizeH = size.height;
      imageW = sizeW;
      imageH = sizeH;
      offscreen = createImage(imageW, imageH);
    }

    Graphics g = offscreen.getGraphics();
    if (font == null) {
      font = Theme.getFont("header.text.font");
    }
    g.setFont(font);  // need to set this each time through
    metrics = g.getFontMetrics();
    fontAscent = metrics.getAscent();
    //}

    //Graphics2D g2 = (Graphics2D) g;
    //g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
    //                    RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

    // set the background for the offscreen
    g.setColor(backgroundColor);
    g.fillRect(0, 0, imageW, imageH);

    int codeCount = sketch.getCodeCount();
    if ((tabLeft == null) || (tabLeft.length < codeCount)) {
      tabLeft = new int[codeCount];
      tabRight = new int[codeCount];
    }

    int x = 6; // offset from left edge of the component
    for (int i = 0; i < sketch.getCodeCount(); i++) {
      SketchCode code = sketch.getCode(i);

      String codeName = code.isExtension(sketch.getHiddenExtensions()) ?
        code.getPrettyName() : code.getFileName();

      // if modified, add the li'l glyph next to the name
      String text = "  " + codeName + (code.isModified() ? " \u00A7" : "  ");

      Graphics2D g2 = (Graphics2D) g;
      int textWidth = (int)
        font.getStringBounds(text, g2.getFontRenderContext()).getWidth();

      int pieceCount = 2 + (textWidth / PIECE_WIDTH);
      int pieceWidth = pieceCount * PIECE_WIDTH;

      int state = (code == sketch.getCurrentCode()) ? SELECTED : UNSELECTED;
      g.drawImage(pieces[state][LEFT], x, 0, null);
      x += PIECE_WIDTH;

      int contentLeft = x;
      tabLeft[i] = x;
      for (int j = 0; j < pieceCount; j++) {
        g.drawImage(pieces[state][MIDDLE], x, 0, null);
        x += PIECE_WIDTH;
      }
      tabRight[i] = x;
      int textLeft = contentLeft + (pieceWidth - textWidth) / 2;

      g.setColor(textColor[state]);
      int baseline = (sizeH + fontAscent) / 2;
      //g.drawString(sketch.code[i].name, textLeft, baseline);
      g.drawString(text, textLeft, baseline);

      g.drawImage(pieces[state][RIGHT], x, 0, null);
      x += PIECE_WIDTH - 1;  // overlap by 1 pixel
    }

    menuLeft = sizeW - (16 + pieces[0][MENU].getWidth(this));
    menuRight = sizeW - 16;
    // draw the dropdown menu target
    g.drawImage(pieces[popup.isVisible() ? SELECTED : UNSELECTED][MENU],
                menuLeft, 0, null);

    screen.drawImage(offscreen, 0, 0, null);
  }


  /**
   * Called when a new sketch is opened.
   */
  public void rebuild() {
    //System.out.println("rebuilding editor header");
    rebuildMenu();
    repaint();
    Toolkit.getDefaultToolkit().sync();
  }


  public void rebuildMenu() {
    //System.out.println("rebuilding");
    if (menu != null) {
      menu.removeAll();

    } else {
      menu = new JMenu();
      MenuScroller.setScrollerFor(menu);
      popup = menu.getPopupMenu();
      add(popup);
      popup.setLightWeightPopupEnabled(true);

      /*
      popup.addPopupMenuListener(new PopupMenuListener() {
          public void popupMenuCanceled(PopupMenuEvent e) {
            // on redraw, the isVisible() will get checked.
            // actually, a repaint may be fired anyway, so this
            // may be redundant.
            repaint();
          }

          public void popupMenuWillBecomeInvisible(PopupMenuEvent e) { }
          public void popupMenuWillBecomeVisible(PopupMenuEvent e) { }
        });
      */
    }
    JMenuItem item;

    // maybe this shouldn't have a command key anyways..
    // since we're not trying to make this a full ide..
    //item = Editor.newJMenuItem("New", 'T');

    /*
    item = Editor.newJMenuItem("Previous", KeyEvent.VK_PAGE_UP);
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          System.out.println("prev");
        }
      });
    if (editor.sketch != null) {
      item.setEnabled(editor.sketch.codeCount > 1);
    }
    menu.add(item);

    item = Editor.newJMenuItem("Next", KeyEvent.VK_PAGE_DOWN);
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          System.out.println("ext");
        }
      });
    if (editor.sketch != null) {
      item.setEnabled(editor.sketch.codeCount > 1);
    }
    menu.add(item);

    menu.addSeparator();
    */

    //item = new JMenuItem("New Tab");
    item = Editor.newJMenuItemShift(_("New Tab"), 'N');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          editor.getSketch().handleNewCode();
        }
      });
    menu.add(item);

    item = new JMenuItem(_("Rename"));
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          editor.getSketch().handleRenameCode();
          /*
          // this is already being called by nameCode(), the second stage of rename
          if (editor.sketch.current == editor.sketch.code[0]) {
            editor.sketchbook.rebuildMenus();
          }
          */
        }
      });
    menu.add(item);

    item = new JMenuItem(_("Delete"));
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          editor.getSketch().handleDeleteCode();
        }
      });
    menu.add(item);

    menu.addSeparator();

    //  KeyEvent.VK_LEFT and VK_RIGHT will make Windows beep

    item = new JMenuItem(_("Previous Tab"));
    KeyStroke ctrlAltLeft = KeyStroke
        .getKeyStroke(KeyEvent.VK_LEFT, Editor.SHORTCUT_ALT_KEY_MASK);
    item.setAccelerator(ctrlAltLeft);
    item.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        editor.sketch.handlePrevCode();
      }
    });
    menu.add(item);

    item = new JMenuItem(_("Next Tab"));
    KeyStroke ctrlAltRight = KeyStroke
        .getKeyStroke(KeyEvent.VK_RIGHT, Editor.SHORTCUT_ALT_KEY_MASK);
    item.setAccelerator(ctrlAltRight);
    item.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        editor.sketch.handleNextCode();
      }
    });
    menu.add(item);

    Sketch sketch = editor.getSketch();
    if (sketch != null) {
      menu.addSeparator();

      ActionListener jumpListener = new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            editor.getSketch().setCurrentCode(e.getActionCommand());
          }
        };
      for (SketchCode code : sketch.getCodes()) {
        item = new JMenuItem(code.isExtension(sketch.getDefaultExtension()) ? 
                             code.getPrettyName() : code.getFileName());
        item.setActionCommand(code.getFileName());
        item.addActionListener(jumpListener);
        menu.add(item);
      }
    }
  }


  public void deselectMenu() {
    repaint();
  }


  public Dimension getPreferredSize() {
    return getMinimumSize();
  }


  public Dimension getMinimumSize() {
    if (OSUtils.isMacOS()) {
      return new Dimension(300, Preferences.GRID_SIZE);
    }
    return new Dimension(300, Preferences.GRID_SIZE - 1);
  }


  public Dimension getMaximumSize() {
    if (OSUtils.isMacOS()) {
      return new Dimension(3000, Preferences.GRID_SIZE);
    }
    return new Dimension(3000, Preferences.GRID_SIZE - 1);
  }
}
