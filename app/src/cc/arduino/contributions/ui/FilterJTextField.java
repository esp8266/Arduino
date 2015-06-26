/*
 * This file is part of Arduino.
 *
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */

package cc.arduino.contributions.ui;

import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import java.awt.*;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;

@SuppressWarnings("serial")
public class FilterJTextField extends JTextField {
  private final String filterHint;

  private boolean showingHint;

  public FilterJTextField(String hint) {
    super(hint);
    filterHint = hint;

    showingHint = true;
    updateStyle();

    addFocusListener(new FocusListener() {
      public void focusLost(FocusEvent focusEvent) {
        if (getText().isEmpty()) {
          showingHint = true;
        }
        updateStyle();
      }

      public void focusGained(FocusEvent focusEvent) {
        if (showingHint) {
          showingHint = false;
          setText("");
        }
        updateStyle();
      }
    });

    getDocument().addDocumentListener(new DocumentListener() {
      public void removeUpdate(DocumentEvent e) {
        applyFilter();
      }

      public void insertUpdate(DocumentEvent e) {
        applyFilter();
      }

      public void changedUpdate(DocumentEvent e) {
        applyFilter();
      }
    });
  }

  private String lastFilter = "";

  private void applyFilter() {
    String filter = showingHint ? "" : getText();
    filter = filter.toLowerCase();

    // Replace anything but 0-9, a-z, or : with a space
    filter = filter.replaceAll("[^\\x30-\\x39^\\x61-\\x7a^\\x3a]", " ");

    // Fire event only if the filter is changed
    if (filter.equals(lastFilter))
      return;

    lastFilter = filter;
    onFilter(filter.split(" "));
  }

  protected void onFilter(String[] strings) {
    // Empty
  }

  public void updateStyle() {
    if (showingHint) {
      setText(filterHint);
      setForeground(Color.gray);
      setFont(getFont().deriveFont(Font.ITALIC));
    } else {
      setForeground(UIManager.getColor("TextField.foreground"));
      setFont(getFont().deriveFont(Font.PLAIN));
    }
  }
}
