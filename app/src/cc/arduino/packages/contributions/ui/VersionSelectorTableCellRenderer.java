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

import java.awt.Component;

import javax.swing.ComboBoxModel;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumn;

import cc.arduino.packages.contributions.ui.ContributionIndexTableModel.ContributedPlatformReleases;

public class VersionSelectorTableCellRenderer implements TableCellRenderer {

  private JPanel panel;
  private JComboBox versionComboBox;
  private JLabel versionLabel;
  private JButton button;
  private int minCellWidth;

  public VersionSelectorTableCellRenderer() {
    panel = new JPanel();

    versionComboBox = new JComboBox();
    versionComboBox.setOpaque(false);
    panel.add(versionComboBox);
    versionComboBox.setVisible(false);

    versionLabel = new JLabel();
    versionLabel.setOpaque(false);
    panel.add(versionLabel);

    button = new JButton("Install");
    minCellWidth = button.getPreferredSize().width;
    button.setText("Upgrade");
    int buttonWidth = button.getPreferredSize().width;
    if (minCellWidth < buttonWidth)
      minCellWidth = buttonWidth;

    panel.add(button);
  }

  public Component getTableCellRendererComponent(JTable table, Object value,
                                                 boolean isSelected,
                                                 boolean hasFocus, int row,
                                                 int col) {
    ComboBoxModel comboModel = new DefaultComboBoxModel(
        new String[] { (String) value });
    versionComboBox.setModel(comboModel);

    versionLabel.setText((String) value);

    ContributionIndexTableModel model = (ContributionIndexTableModel) table
        .getModel();
    ContributedPlatformReleases releases = model.getReleases(row);

    boolean installed = releases.getInstalled() != null;
    if (installed) {
      if (releases.getInstalled() != releases.getSelected()) {
        button.setText("Upgrade");
        button.setVisible(true);
      } else {
        button.setVisible(false);
      }
    } else {
      button.setText("Install");
      button.setVisible(true);
    }

    int labelWidth = versionComboBox.getPreferredSize().width;
    if (minCellWidth < labelWidth)
      minCellWidth = labelWidth;
    TableColumn column = table.getColumnModel().getColumn(col);
    if (column.getMinWidth() < minCellWidth + 20) {
      column.setMinWidth(minCellWidth + 20);
      column.setMaxWidth(minCellWidth + 20);
    }

    if (isSelected) {
      panel.setBackground(table.getSelectionBackground());
      panel.setForeground(table.getSelectionForeground());
    } else {
      panel.setBackground(table.getBackground());
      panel.setForeground(table.getForeground());
    }
    return panel;
  }
}
