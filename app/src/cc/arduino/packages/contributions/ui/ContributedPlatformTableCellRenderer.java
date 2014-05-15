/*
 * This file is part of Arduino.
 *
 * Copyright 2014 Arduino LLC (http://www.arduino.cc/)
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
package cc.arduino.packages.contributions.ui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Insets;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.JTextPane;
import javax.swing.border.EmptyBorder;
import javax.swing.table.TableCellRenderer;
import javax.swing.text.Document;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.StyleSheet;

import cc.arduino.packages.contributions.ContributedBoard;
import cc.arduino.packages.contributions.ContributedPlatform;

@SuppressWarnings("serial")
public class ContributedPlatformTableCellRenderer extends JPanel implements
    TableCellRenderer {

  private JTextPane description;

  public ContributedPlatformTableCellRenderer() {
    super();

    // Align contents to the left
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
    setAlignmentX(Component.LEFT_ALIGNMENT);

    description = new JTextPane();
    description.setInheritsPopupMenu(true);
    Insets margin = description.getMargin();
    margin.bottom = 0;
    description.setMargin(margin);
    description.setContentType("text/html");
    setTextStyle(description);
    description.setOpaque(false);
    description.setBorder(new EmptyBorder(4, 7, 7, 7));
    description.setHighlighter(null);
    add(description);
  }

  static void setTextStyle(JTextPane textPane) {
    Document doc = textPane.getDocument();
    if (doc instanceof HTMLDocument) {
      HTMLDocument html = (HTMLDocument) doc;
      StyleSheet stylesheet = html.getStyleSheet();
      stylesheet.addRule("body { margin: 0; padding: 0;"
          + "font-family: Verdana, Geneva, Arial, Helvetica, sans-serif;"
          + "font-size: 100%;" + "font-size: 0.95em; }");
    }
  }

  public Component getTableCellRendererComponent(JTable table, Object value,
                                                 boolean isSelected,
                                                 boolean hasFocus, int row,
                                                 int column) {
    ContributedPlatform contrib = (ContributedPlatform) value;

    String descriptionText = "<html><body><b>" + contrib.getName() + "</b>";
    String authorList = "arduino"; // contrib.getAuthorList();
    if (authorList != null && !authorList.isEmpty()) {
      descriptionText += " by <a href=\"www.arudino.cc\">Arduino</a>";
    }
    descriptionText += "<br /><br />";

    descriptionText += "Boards contributed in this package:<br />";
    for (ContributedBoard board : contrib.getBoards())
      descriptionText += "- " + board.getName() + "<br />";
    // descriptionText += "<br />Available version: <b>1.5.5</b><br />";
    // descriptionText += "Installed version: <b>1.5.4</b><br />";
    descriptionText += "</body></html>";
    description.setText(descriptionText);
    description.setBackground(Color.WHITE);
    description.setVisible(true);

    int h = getPreferredSize().height;
    if (table.getRowHeight(row) != h)
      table.setRowHeight(row, h);

    if (isSelected) {
      setBackground(table.getSelectionBackground());
      setForeground(table.getSelectionForeground());
    } else {
      setBackground(table.getBackground());
      setForeground(table.getForeground());
    }
    return this;
  }
}
