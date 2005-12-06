/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2005 Ben Fry and Casey Reas

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

import processing.app.syntax.*;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;


/**
 * Li'l status bar fella that shows the line number.
 */
public class EditorLineStatus extends JComponent {
  JEditTextArea textarea;
  int start = -1, stop;

  Image resize;

  Color foreground;
  Color background;
  Font font;
  int high;

  String text = "";


  public EditorLineStatus(JEditTextArea textarea) {
    this.textarea = textarea;
    textarea.editorLineStatus = this;

	// hardcoding new blue color scheme for consistency with images,
	// see EditorStatus.java for details.
    //background = Preferences.getColor("linestatus.bgcolor");
	background = new Color(0x04, 0x4F, 0x6F);
    font = Preferences.getFont("linestatus.font");
    foreground = Preferences.getColor("linestatus.color");
    high = Preferences.getInteger("linestatus.height");

    if (Base.isMacOS()) {
      resize = Base.getImage("resize.gif", this);
    }
    //linestatus.bgcolor = #000000
    //linestatus.font    = SansSerif,plain,10
    //linestatus.color   = #FFFFFF
  }


  public void set(int newStart, int newStop) {
    if ((newStart == start) && (newStop == stop)) return;

    start = newStart;
    stop = newStop;

    /*
    if (start == stop) {
      text = "Line " + (start + 1);
    } else {
      text = "Lines " + (start + 1) + " to " + (stop + 1);
    }
    */
    if (start == stop) {
      text = String.valueOf(start+1);
    } else {
      text = (start+1) + " - " + (stop+1);
    }

    repaint();
  }


  public void paintComponent(Graphics g) {
    g.setColor(background);
    Dimension size = getSize();
    g.fillRect(0, 0, size.width, size.height);

    g.setFont(font);
    g.setColor(foreground);
    int baseline = (high + g.getFontMetrics().getAscent()) / 2;
    g.drawString(text, 6, baseline);

    if (Base.isMacOS()) {
      g.drawImage(resize, size.width - 20, 0, this);
    }
  }


  public Dimension getPreferredSize() {
    return new Dimension(300, high);
  }

  public Dimension getMinimumSize() {
    return getPreferredSize();
  }

  public Dimension getMaximumSize() {
    return new Dimension(3000, high);
  }
}
