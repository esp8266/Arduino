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
package cc.arduino.ui;

import static cc.arduino.packages.contributions.ui.ContributionIndexTableModel.DESCRIPTION_COL;
import static processing.app.I18n._;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.ScrollPaneConstants;
import javax.swing.SwingUtilities;
import javax.swing.border.EmptyBorder;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;

import processing.app.Theme;

public abstract class InstallerJDialog extends JDialog {

  // Toolbar on top of the window:
  // - Categories drop-down menu
  protected JLabel categoryLabel;
  protected JComboBox categoryChooser;
  protected Component categoryStrut1;
  protected Component categoryStrut2;
  protected Component categoryStrut3;
  // - Search text-field
  protected FilterJTextField filterField;
  // Currently selected category and filters
  protected String category;
  protected String[] filters;

  // Real contribution table
  protected JTable contribTable;
  // Model behind the table
  protected FilteredAbstractTableModel contribModel;
  // Default table model listener
  protected TableModelListener tableModelListener = new TableModelListener() {
    @Override
    public void tableChanged(final TableModelEvent event) {
      SwingUtilities.invokeLater(new Runnable() {
        @Override
        public void run() {
          updateCellsHeight(event);
        }
      });
    }
  };

  abstract protected FilteredAbstractTableModel createContribModel();

  abstract protected InstallerTableCell createCellRenderer();

  abstract protected InstallerTableCell createCellEditor();

  // Bottom:
  // - Progress bar
  protected ProgressJProgressBar progressBar;
  protected Box progressBox;
  protected Box updateBox;

  protected InstallerTableCell cellEditor;

  public InstallerJDialog(Frame parent, String title,
                          ModalityType applicationModal) {
    super(parent, title, applicationModal);

    setResizable(true);

    Container pane = getContentPane();
    pane.setLayout(new BorderLayout());

    {
      categoryStrut1 = Box.createHorizontalStrut(5);
      categoryStrut2 = Box.createHorizontalStrut(5);
      categoryStrut3 = Box.createHorizontalStrut(5);

      categoryLabel = new JLabel(_("Category:"));

      categoryChooser = new JComboBox();
      categoryChooser.setMaximumRowCount(20);
      categoryChooser.setEnabled(false);

      filterField = new FilterJTextField(_("Filter your search...")) {
        @Override
        protected void onFilter(String[] _filters) {
          filters = _filters;
          cellEditor.stopCellEditing();
          contribModel.updateIndexFilter(category, filters);
        }
      };

      JPanel panel = new JPanel();
      panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
      panel.add(categoryStrut1);
      panel.add(categoryLabel);
      panel.add(categoryStrut2);
      panel.add(categoryChooser);
      panel.add(categoryStrut3);
      panel.add(filterField);
      panel.setBorder(new EmptyBorder(7, 7, 7, 7));
      pane.add(panel, BorderLayout.NORTH);
    }

    contribModel = createContribModel();
    contribTable = new JTable(contribModel);
    contribTable.setTableHeader(null);
    contribTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    contribTable.setColumnSelectionAllowed(false);
    contribTable.setDragEnabled(false);
    contribTable.setIntercellSpacing(new Dimension(0, 1));
    contribTable.setShowVerticalLines(false);
    contribTable
        .setSelectionBackground(Theme.getColor("status.notice.bgcolor"));

    {
      TableColumnModel tcm = contribTable.getColumnModel();
      TableColumn col = tcm.getColumn(DESCRIPTION_COL);
      col.setCellRenderer(createCellRenderer());
      cellEditor = createCellEditor();
      col.setCellEditor(cellEditor);
      col.setResizable(true);
    }

    {
      JScrollPane s = new JScrollPane();
      s.setViewportView(contribTable);
      s.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
      s.setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
      pane.add(s, BorderLayout.CENTER);
    }

    pane.add(Box.createHorizontalStrut(10), BorderLayout.WEST);
    pane.add(Box.createHorizontalStrut(10), BorderLayout.EAST);

    progressBar = new ProgressJProgressBar();
    progressBar.setStringPainted(true);
    progressBar.setString(" ");
    progressBar.setVisible(true);

    {
      JButton cancelButton = new JButton(_("Cancel"));
      cancelButton.addActionListener(new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent arg0) {
          onCancelPressed();
        }
      });

      JButton updateButton = new JButton(_("Update list"));
      updateButton.addActionListener(new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent arg0) {
          onUpdatePressed();
        }
      });

      progressBox = Box.createHorizontalBox();
      progressBox.add(progressBar);
      progressBox.add(Box.createHorizontalStrut(5));
      progressBox.add(cancelButton);

      updateBox = Box.createHorizontalBox();
      updateBox.add(Box.createHorizontalGlue());
      updateBox.add(updateButton);
    }

    {
      JPanel progressPanel = new JPanel();
      progressPanel.setBorder(new EmptyBorder(7, 7, 7, 7));
      progressPanel.setLayout(new BoxLayout(progressPanel, BoxLayout.Y_AXIS));
      progressPanel.add(progressBox);
      progressPanel.add(updateBox);
      pane.add(progressPanel, BorderLayout.SOUTH);
    }
    setProgressVisible(false);

    setMinimumSize(new Dimension(600, 450));
  }

  public void setProgressVisible(boolean visible) {
    progressBox.setVisible(visible);

    filterField.setEnabled(!visible);
    categoryChooser.setEnabled(!visible);
    contribTable.setEnabled(!visible);
    updateBox.setVisible(!visible);
    updateBox.setEnabled(!visible);
    cellEditor.setEnabled(!visible);

    if (visible && contribTable.isEditing()) {
      TableCellEditor editor = contribTable.getCellEditor();
      if (editor != null)
        editor.stopCellEditing();
    }
  }

  private void updateCellsHeight(TableModelEvent e) {
    int first = e.getFirstRow();
    int last = Math.min(e.getLastRow(), contribTable.getRowCount() - 1);
    for (int row = first; row <= last; row++) {
      TableCellRenderer editor = createCellRenderer();
      Component comp = contribTable.prepareRenderer(editor, row, 0);
      int height = comp.getPreferredSize().height;
      contribTable.setRowHeight(row, height);
    }
  }

  protected ActionListener categoryChooserActionListener = new ActionListener() {
    @Override
    public void actionPerformed(ActionEvent arg0) {
      String selected = (String) categoryChooser.getSelectedItem();
      if (category == null || !category.equals(selected)) {
        category = selected;
        cellEditor.stopCellEditing();
        contribModel.updateIndexFilter(category, filters);
      }
    }
  };

  /**
   * Action performed when the Cancel button is pressed.
   */
  protected void onCancelPressed() {
    // Empty
  }

  /**
   * Action performed when the "Update List" button is pressed.
   */
  protected void onUpdatePressed() {
    // Empty
  }

}
