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
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.List;

import javax.swing.AbstractCellEditor;
import javax.swing.ComboBoxModel;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.table.TableCellEditor;

import cc.arduino.packages.contributions.ui.ContributionIndexTableModel.ContributedPlatformReleases;

@SuppressWarnings("serial")
public class VersionSelectorTableCellEditor extends AbstractCellEditor
    implements TableCellEditor {

  private JPanel panel;
  private JComboBox combo;
  private JButton button;

  private int currRow, currCol;
  private ContributionIndexTableModel currModel = null;

  private ItemListener itemListener = new ItemListener() {
    @Override
    public void itemStateChanged(ItemEvent e) {
      if (currModel != null) {
        currModel.setValueAt(getCellEditorValue(), currRow, currCol);
        updateButtons();
      }
    }
  };

  private Listener listener = null;

  public VersionSelectorTableCellEditor() {
    panel = new JPanel();

    combo = new JComboBox();
    combo.setOpaque(false);
    combo.addItemListener(itemListener);
    panel.add(combo);

    button = new JButton("Install");
    button.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        if (listener == null)
          return;
        listener.onInstallEvent(currRow);
      }
    });
    panel.add(button);
  }

  @Override
  public Component getTableCellEditorComponent(JTable table, Object value,
                                               boolean isSelected, int row,
                                               int col) {
    currCol = col;
    currRow = row;
    currModel = (ContributionIndexTableModel) table.getModel();
    List<String> values = currModel.getReleasesVersions(row);

    ComboBoxModel model = new DefaultComboBoxModel(values.toArray());
    model.setSelectedItem(currModel.getSelectedRelease(row).getVersion());
    combo.setModel(model);

    updateButtons();
    if (table.getRowSelectionAllowed()) {
      panel.setBackground(table.getSelectionBackground());
      panel.setForeground(table.getSelectionForeground());
    } else {
      panel.setBackground(table.getBackground());
      panel.setForeground(table.getForeground());
    }
    return panel;
  }

  private void updateButtons() {
    ContributedPlatformReleases releases = currModel.getReleases(currRow);
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
  }

  @Override
  public Object getCellEditorValue() {
    return combo.getSelectedItem();
  }

  public static interface Listener {
    public void onInstallEvent(int row);
  }

  public void setListener(Listener newListener) {
    listener = newListener;
  }

}
